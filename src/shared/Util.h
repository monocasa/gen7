#ifndef GEN7_SHARED_UTIL_H
#define GEN7_SHARED_UTIL_H

namespace Util {

template <typename T, unsigned B>
static inline T SignExtend( const T x )
{
	struct {
		T x:B;
	} s;

	return s.x = x;
}

} //namespace Util

#endif //GEN7_SHARED_UTIL_H

