/******************************************************************************

	File: STCollection.h

	Description:

	VM representation of Smalltalk abstract collection classes.

	N.B. Some of the classes here defined are well known to the VM, and must not
	be modified in the image. Note also that these classes may also have
	a representation in the assembler modules (so see istasm.inc)

******************************************************************************/

#ifndef _IST_STCOLLECTION_H_
#define _IST_STCOLLECTION_H_

#include "STObject.h"

class Collection //: public Object
{
public:
	enum { FixedSize = 0 };		// FixedSize does not include Header
};

class SequenceableCollection : public Collection
{
};

class ArrayedCollection : public SequenceableCollection
{
};

#endif	// EOF