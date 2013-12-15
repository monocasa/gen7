#ifndef STANDALONECPP_UCONTEXT_H
#define STANDALONECPP_UCONTEXT_H

#ifdef __amd64__
#include "amd64/ucontext.h"
#else //!__amd64__
#error No architecture specified 
#endif

extern "C" {

} //extern "C"

#endif //STANDALONECPP_UCONTEXT_H

