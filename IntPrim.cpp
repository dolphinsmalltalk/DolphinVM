#include "Ist.h"

#ifndef _DEBUG
#pragma optimize("s", on)
#pragma auto_inline(off)
#endif

#include "ObjMem.h"
#include "Interprt.h"
#include "InterprtPrim.inl"

template <class P> __forceinline static Oop* primitiveIntegerCompare(Oop* const sp, const P &pred)
{
	// Normally it is better to jump on the failure case as the static prediction is that forward
	// jumps are not taken, but these primitives are normally only invoked when the special bytecode 
	// has triggered the fallback method (unless performed), which suggests the arg will not be a 
	// SmallInteger, so the 99% case is that the primitive should fail
	Oop arg = *sp;
	if (!ObjectMemoryIsIntegerObject(arg))
		return nullptr;

	Oop receiver = *(sp - 1);
	// We can perform the comparisons without shifting away the SmallInteger bit since it always 1
	*(sp - 1) = reinterpret_cast<Oop>(pred(receiver, arg) ? Pointers.True : Pointers.False);
	return sp - 1;
}

Oop* __fastcall Interpreter::primitiveLessThan(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::less<SMALLINTEGER>());
}

Oop* __fastcall Interpreter::primitiveGreaterThan(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::greater<SMALLINTEGER>());
}

Oop* __fastcall Interpreter::primitiveLessOrEqual(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::less_equal<SMALLINTEGER>());
}

Oop* __fastcall Interpreter::primitiveGreaterOrEqual(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::greater_equal<SMALLINTEGER>());
}

Oop* __fastcall Interpreter::primitiveEqual(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::equal_to<SMALLINTEGER>());
}

Oop* __fastcall Interpreter::primitiveNotEqual(Oop* const sp, unsigned)
{
	return primitiveIntegerCompare(sp, std::not_equal_to<SMALLINTEGER>());
}

//////////////////////////////////////////////////////////////////////////////;
// SmallInteger Bit Manipulation Primitives

template <class P> __forceinline static Oop* primitiveIntegerOp(Oop* const sp, const P &op)
{
	Oop arg = *sp;
	if (!ObjectMemoryIsIntegerObject(arg))
		return nullptr;

	Oop receiver = *(sp - 1);
	*(sp - 1) = op(receiver, arg);
	return sp - 1;
}

Oop* __fastcall Interpreter::primitiveBitAnd(Oop* const sp, unsigned)
{
	return primitiveIntegerOp(sp, std::bit_and<Oop>());
}

Oop* __fastcall Interpreter::primitiveBitOr(Oop* const sp, unsigned)
{
	return primitiveIntegerOp(sp, std::bit_or<Oop>());
}

Oop* __fastcall Interpreter::primitiveBitXor(Oop* const sp, unsigned)
{
	struct bit_xor {
		Oop operator() (const Oop& receiver, const Oop& arg) const {
			return receiver ^ (arg - 1);
		}
	};

	return primitiveIntegerOp(sp, bit_xor());
}

Oop* __fastcall Interpreter::primitiveAnyMask(Oop* const sp, unsigned)
{
	struct any_mask {
		bool operator() (const Oop& receiver, const Oop& arg) const {
			return (receiver & arg) != 1;
		}
	};

	return primitiveIntegerCompare(sp, any_mask());
}

Oop* __fastcall Interpreter::primitiveAllMask(Oop* const sp, unsigned)
{
	struct all_mask {
		bool operator() (const Oop& receiver, const Oop& arg) const {
			return (receiver & arg) == arg;
		}
	};

	return primitiveIntegerCompare(sp, all_mask());
}
