#ifndef GEN7_SHARED_UTIL_H
#define GEN7_SHARED_UTIL_H

namespace util {

template <typename T, unsigned B>
static inline T SignExtend( const T x )
{
	struct {
		T x:B;
	} s;

	return s.x = x;
}

} //namespace util

#endif //GEN7_SHARED_UTIL_H

