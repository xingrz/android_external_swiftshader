#include "EmulatedReactor.hpp"

#include <cmath>
#include <functional>
#include <utility>

namespace rr {
namespace {

template <typename T>
struct UnderlyingType
{
	using Type = typename decltype(rr::Extract(std::declval<RValue<T>>(), 0))::rvalue_underlying_type;
};

template <typename T>
using UnderlyingTypeT = typename UnderlyingType<T>::Type;

// Call single arg function on a vector type
template <typename Func, typename T>
RValue<T> call4(Func func, const RValue<T>& x)
{
	T result;
	result = Insert(result, Call(func, Extract(x, 0)), 0);
	result = Insert(result, Call(func, Extract(x, 1)), 1);
	result = Insert(result, Call(func, Extract(x, 2)), 2);
	result = Insert(result, Call(func, Extract(x, 3)), 3);
	return result;
}

// Call two arg function on a vector type
template <typename Func, typename T>
RValue<T> call4(Func func, const RValue<T>& x, const RValue<T>& y)
{
	T result;
	result = Insert(result, Call(func, Extract(x, 0), Extract(y, 0)), 0);
	result = Insert(result, Call(func, Extract(x, 1), Extract(y, 1)), 1);
	result = Insert(result, Call(func, Extract(x, 2), Extract(y, 2)), 2);
	result = Insert(result, Call(func, Extract(x, 3), Extract(y, 3)), 3);
	return result;
}

template <typename T, typename EL = UnderlyingTypeT<T>>
void gather(T& out, RValue<Pointer<EL>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes)
{
	constexpr bool atomic = false;
	constexpr std::memory_order order = std::memory_order_relaxed;

	Pointer<Byte> baseBytePtr = base;

	out = T(0);
	for(int i = 0; i < 4; i++)
	{
		If(Extract(mask, i) != 0)
		{
			auto offset = Extract(offsets, i);
			auto el = Load(Pointer<EL>(&baseBytePtr[offset]), alignment, atomic, order);
			out = Insert(out, el, i);
		}
		Else If(zeroMaskedLanes)
		{
			out = Insert(out, EL(0), i);
		}
	}
}

template <typename T, typename EL = UnderlyingTypeT<T>>
void scatter(RValue<Pointer<EL>> base, RValue<T> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	constexpr bool atomic = false;
	constexpr std::memory_order order = std::memory_order_relaxed;

	Pointer<Byte> baseBytePtr = base;

	for(int i = 0; i < 4; i++)
	{
		If(Extract(mask, i) != 0)
		{
			auto offset = Extract(offsets, i);
			Store(Extract(val, i), Pointer<EL>(&baseBytePtr[offset]), alignment, atomic, order);
		}
	}
}

}  // anonymous namespace

namespace emulated {

RValue<Float4> Gather(RValue<Pointer<Float>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	Float4 result{};
	gather(result, base, offsets, mask, alignment, zeroMaskedLanes);
	return result;
}

RValue<Int4> Gather(RValue<Pointer<Int>> base, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment, bool zeroMaskedLanes /* = false */)
{
	Int4 result{};
	gather(result, base, offsets, mask, alignment, zeroMaskedLanes);
	return result;
}

void Scatter(RValue<Pointer<Float>> base, RValue<Float4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	scatter(base, val, offsets, mask, alignment);
}

void Scatter(RValue<Pointer<Int>> base, RValue<Int4> val, RValue<Int4> offsets, RValue<Int4> mask, unsigned int alignment)
{
	scatter<Int4>(base, val, offsets, mask, alignment);
}

RValue<Float> Exp2(RValue<Float> x)
{
	return Call(exp2f, x);
}

RValue<Float> Log2(RValue<Float> x)
{
	return Call(log2f, x);
}

RValue<Float4> Sin(RValue<Float4> x)
{
	return call4(sinf, x);
}

RValue<Float4> Cos(RValue<Float4> x)
{
	return call4(cosf, x);
}

RValue<Float4> Tan(RValue<Float4> x)
{
	return call4(tanf, x);
}

RValue<Float4> Asin(RValue<Float4> x)
{
	return call4(asinf, x);
}

RValue<Float4> Acos(RValue<Float4> x)
{
	return call4(acosf, x);
}

RValue<Float4> Atan(RValue<Float4> x)
{
	return call4(atanf, x);
}

RValue<Float4> Sinh(RValue<Float4> x)
{
	return call4(sinhf, x);
}

RValue<Float4> Cosh(RValue<Float4> x)
{
	return call4(coshf, x);
}

RValue<Float4> Tanh(RValue<Float4> x)
{
	return call4(tanhf, x);
}

RValue<Float4> Asinh(RValue<Float4> x)
{
	return call4(asinhf, x);
}

RValue<Float4> Acosh(RValue<Float4> x)
{
	return call4(acoshf, x);
}

RValue<Float4> Atanh(RValue<Float4> x)
{
	return call4(atanhf, x);
}

RValue<Float4> Atan2(RValue<Float4> x, RValue<Float4> y)
{
	return call4(atan2f, x, y);
}

RValue<Float4> Pow(RValue<Float4> x, RValue<Float4> y)
{
	return call4(powf, x, y);
}

RValue<Float4> Exp(RValue<Float4> x)
{
	return call4(expf, x);
}

RValue<Float4> Log(RValue<Float4> x)
{
	return call4(logf, x);
}

RValue<Float4> Exp2(RValue<Float4> x)
{
	return call4(exp2f, x);
}

RValue<Float4> Log2(RValue<Float4> x)
{
	return call4(log2f, x);
}

}  // namespace emulated
}  // namespace rr
