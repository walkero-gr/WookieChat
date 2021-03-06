/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWURLGRABBER_H
#define MUICLASS_WINDOWURLGRABBER_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define WindowURLGrabberObject NEWOBJECT( appclasses[ CLASSID_WINDOWURLGRABBER ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWURLGRABBER_DUMMY =  0xFED00060,
MM_WINDOWURLGRABBER_EXTRACTURL,
MM_WINDOWURLGRABBER_DOUBLECLICK,
/* Attributes */
};

struct MP_WINDOWURLGRABBER_EXTRACTURL { STACKED ULONG MethodID; STACKED STRPTR Data; };
struct MP_WINDOWURLGRABBER_ADDURL     { STACKED ULONG MethodID; STACKED STRPTR URL; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowURLGrabber_InitClass( void );
void    MCC_WindowURLGrabber_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWURLGRABBER_H */

