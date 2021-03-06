/******************************************************************************

	File: Finalize.cpp

	Description:

	Implementation of the ObjectMemory's finalization support

******************************************************************************/
#include "Ist.h"

#ifndef _DEBUG
	#pragma optimize("s", on)
	#pragma auto_inline(off)
#endif

#pragma code_seg(PROCESS_SEG)

#include "Interprt.h"

// Smalltalk classes
#include "STMemoryManager.h"

OopQueue<POTE> Interpreter::m_qForFinalize;
OopQueue<Oop> Interpreter::m_qBereavements;

///////////////////////////////////////////////////////////////////////////////

// Entry point from the normal count down mechanism - not used from GC as that queues many objects
// which could generate a large number of async. signals to the same semaphore
void ObjectMemory::finalize(OTE* ote)
{
	#if defined(_DEBUG)
	{
		if (abs(Interpreter::executionTrace) > 1)
		{
			tracelock lock(TRACESTREAM);
			TRACESTREAM<< L"Finalizing " << ote<< L"\n";
		}
	}
	#endif
		
	MemoryManager* memMan = memoryManager();
	Interpreter::queueForFinalization(ote, (unsigned)integerValueOf(memMan->m_hospiceHighWater));
}

void Interpreter::scheduleFinalization()
{
	// Signal the bereavement semaphore first, since it belongs to a very high priority
	// process, and we'll only need to do more work switching to it if we signal it
	// after the finalize semaphore
	//MemoryManager& memMan = memoryManager();
	if (!m_qBereavements.isEmpty())
	{
		SemaphoreOTE* bereavementsSemaphore = Pointers.BereavementSemaphore; // memMan.m_bereavements;
		if (!bereavementsSemaphore->isNil())
		{
			#ifdef _DEBUG
			{
				tracelock lock(TRACESTREAM);
				TRACESTREAM<< L"Signalling undertaker process" << std::endl;
			}
			#endif
			asynchronousSignal(bereavementsSemaphore );
		}
	}
	if (!m_qForFinalize.isEmpty())
	{
		SemaphoreOTE* finalizationSemaphore = Pointers.FinalizeSemaphore; //memMan.m_lastRequests;
		if (!finalizationSemaphore->isNil())
		{
			#ifdef _DEBUG
			{
				tracelock lock(TRACESTREAM);
				TRACESTREAM<< L"Signalling finalizer process" << std::endl;
			}
			#endif
			asynchronousSignal(finalizationSemaphore);
		}
	}

	MemoryManager* memMan = ObjectMemory::memoryManager();
	unsigned count = m_qForFinalize.Count();
	// Raise interrupt when at or above high water mark
	if (count >= (unsigned)integerValueOf(memMan->m_hospiceHighWater))
		queueInterrupt(VMI_HOSPICECRISIS, integerObjectOf(count));
}

///////////////////////////////////////////////////////////////////////////////

// Remove a request from the finalize queue and answer it (answer nil if none pending).
OTE* __fastcall Interpreter::dequeueForFinalization()
{
	return m_qForFinalize.Pop();
}


OTE* Interpreter::dequeueBereaved(VariantObject* out)
{
	OTE* answer;
	if (m_qBereavements.isEmpty())
		answer = Pointers.False;
	else
	{
		for (unsigned i = 0; i < OopsPerBereavementQEntry; i++)
		{
			ObjectMemory::countDown(out->m_fields[i]);
			out->m_fields[i] = m_qBereavements.Pop();
		}
		answer = Pointers.True;

		ASSERT(!ObjectMemoryIsIntegerObject(out->m_fields[0]));
		ASSERT(reinterpret_cast<OTE*>(out->m_fields[0])->isWeak());
		ASSERT(ObjectMemoryIsIntegerObject(out->m_fields[1]));
	}

	return answer;
}
