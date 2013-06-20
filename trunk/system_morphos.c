/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** system_morphos.c
*/

/*************************************************************************/

#ifdef ENABLE_GEITDEBUG
#define DEBUG
#include "debug.h"
#include "debug.c"
#undef DEBUG
#undef REG
#undef REGARGS
#undef STDARGS
#undef FAR
#undef INLINE
#else
#undef debug
void debug() {}
#endif /* ENABLE_GEITDEBUG */
