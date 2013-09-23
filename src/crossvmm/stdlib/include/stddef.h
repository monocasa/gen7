#ifndef __STDDEF_H
#define __STDDEF_H

typedef unsigned int wint_t;

#ifndef __cplusplus
typedef wint_t wchar_t;
#endif /*__cplusplus*/

typedef unsigned long long size_t;

typedef signed long long ptrdiff_t;

#define NULL (0)

#endif /*__STDDEF_H*/

