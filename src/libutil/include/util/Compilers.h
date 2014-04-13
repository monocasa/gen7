#ifndef GEN7_LIBUTIL_COMPILERS_H
#define GEN7_LIBUTIL_COMPILERS_H

#ifdef __GNUC__
#	define UNUSED(x) (void)x
#else
#	error Unknown compiler for UNUSED()
#endif 

#endif //GEN7_LIBUTIL_COMPILERS_H

