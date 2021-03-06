/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H 1

/*************************************************************************/

#include <exec/types.h>
#include <exec/ports.h>
#include <devices/timer.h>
#include <workbench/icon.h>
#include <workbench/startup.h>

#include <string.h>

#include "system.h"

/*************************************************************************/

extern char *STR_NUL;

/*
** intuition
*/

#define ShowRequesterError(message,args) ShowRequester( message, MSG_REQUESTER_OK_GADGETS, args )
ULONG ShowRequester     ( ULONG message, ULONG gadgets, IPTR *args );

/***************************************************************************/

/*
** exec functions
*/

/* /// libraries data
*/

struct LibraryData {
	char            *Name;
	int              Version;
	int              Flags;
	struct Library **LibBase;
#ifdef __amigaos4__
	APTR            *IBase;
#endif
};

#define LIBFLAGSF_OPTIONAL 1  /* no error message and no abort! Make sure you check library base
								before using a library marked as optional */

#ifndef __amigaos4__
#define LIBMACRO(name,version,flags,base,ibase) { (char *) name, version, flags, (struct Library **) &base }
#define LIBMACROEND  { NULL, 0, 0, NULL }
#else
#define LIBMACRO(name,version,flags,base,ibase) { (char *) name, version, flags, (struct Library **) &base, &ibase }
#define LIBMACROEND  { NULL, NULL, 0, NULL, NULL }
#endif

/* \\\ */

BOOL Libraries_Open( void );
void Libraries_Close( void );

/*************************************************************************/

/*
** DOS Functions
*/

void  Dos_ShowFailure( void );
ULONG Dos_GetSizeHandle( BPTR handle );

/*************************************************************************/

/*
** Icon
*/

struct DiskObject *Icon_GetPutDiskObject( struct DiskObject *diskobj );
BOOL               Icon_ToolTypeGetBool( struct DiskObject *o, STRPTR tooltype, BOOL defvalue );
long               Icon_ToolTypeGetInteger( struct DiskObject *o, STRPTR tooltype, long defvalue );
STRPTR             Icon_ToolTypeGetString( struct DiskObject *o, STRPTR tooltype, STRPTR deftooltype );

/*************************************************************************/

/*
** WBMessage
*/

extern struct WBStartup *wbmessage;

void WBMessage_Get  ( void );
void WBMessage_Reply( void );

/*************************************************************************/

/*
** SimpleArgumentParsing
*/

#define SIMPLEREADARGS_ARGNUM 32

struct SimpleReadArgsData {
	struct RDArgs *srad_rda;
	struct RDArgs *srad_rd;
	IPTR           srad_ArgArray[ SIMPLEREADARGS_ARGNUM ];
	char           srad_Buffer[1];
};

struct SimpleReadArgsData *SimpleReadArgsParse( char *templatestring, char *parsestring );
void                       SimpleReadArgsFree( struct SimpleReadArgsData *srad );

/*
** Timer Device
*/

struct Device_Timer {
	struct MsgPort        *MsgPort;
	struct timerequest    *IORequest;
	ULONG                  SignalMask;
	ULONG                  OpenCheck;
};

ULONG DeviceTimer_Open( struct Device_Timer *dt );
void DeviceTimer_Close( struct Device_Timer *dt );
void DeviceTimer_SendRequest( struct Device_Timer *dt );

/*
** exec lists
*/

APTR List_GetListFromNode( APTR snode );

/*************************************************************************/

#endif /* FUNCTIONS_H */

