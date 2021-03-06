/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_APPLICATION_H
#define MUICLASS_APPLICATION_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"
#include "muiclass_network.h"

/*
** Methods and attributes
*/

enum {
MM_APPLICATION_DUMMY =  0xFED00200,
MM_APPLICATION_STARTUP,
MM_APPLICATION_WINDOWFIND,
MM_APPLICATION_WINDOWDISPOSE,
MM_APPLICATION_WINDOWCHATNEW,
MM_APPLICATION_WINDOWCHATCLOSE,
MM_APPLICATION_WINDOWCHATGETNUM,

MM_APPLICATION_CHATSETACTIVE,
MM_APPLICATION_CHATREMOVE,
MM_APPLICATION_CHATFINDACTIVE,

MM_APPLICATION_MESSAGERECEIVED,
MM_APPLICATION_CHANNELADD,
MM_APPLICATION_CHANNELREMOVE,
MM_APPLICATION_CHANNELCHANGETOPIC,
MM_APPLICATION_CHANNELNICKADD,
MM_APPLICATION_CHANNELNICKREMOVE,

MM_APPLICATION_VISUALCHANGE,
MM_APPLICATION_CHANNELISVISIBLE,

/* Attributes */
MA_APPLICATION_CLASSID,
MA_APPLICATION_OBJECTNETWORK,
MA_APPLICATION_OBJECTWINDOWQUIT,
MA_APPLICATION_OBJECTWINDOWABOUT,
MA_APPLICATION_OBJECTWINDOWSETTINGS,
MA_APPLICATION_OBJECTWINDOWIGNORELIST,
MA_APPLICATION_OBJECTWINDOWURLGRABBER,
};

struct MP_APPLICATION_WINDOWFIND        { STACKED ULONG MethodID; STACKED Object *Window; };
struct MP_APPLICATION_WINDOWDISPOSE     { STACKED ULONG MethodID; STACKED Object *Window; };

struct MP_APPLICATION_WINDOWCHATNEW     { STACKED ULONG MethodID; };
struct MP_APPLICATION_WINDOWCHATCLOSE   { STACKED ULONG MethodID; STACKED Object *Window; };
struct MP_APPLICATION_WINDOWCHATGETNUM  { STACKED ULONG MethodID; };

struct MP_APPLICATION_CHATSETACTIVE     { STACKED ULONG MethodID; STACKED Object *Window; };
struct MP_APPLICATION_CHATREMOVE        { STACKED ULONG MethodID; STACKED Object *Window; };
struct MP_APPLICATION_CHATFINDACTIVE    { STACKED ULONG MethodID; };

struct MP_APPLICATION_MESSAGERECEIVED   { STACKED ULONG MethodID; STACKED struct Server *Server;  STACKED char *ChannelName; STACKED ULONG Pen; STACKED char *Message; };
struct MP_APPLICATION_CHANNELADD        { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_APPLICATION_CHANNELREMOVE     { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_APPLICATION_CHANNELNICKADD    { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatNickEntry *ChatNickEntry; };
struct MP_APPLICATION_CHANNELNICKREMOVE { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatNickEntry *ChatNickEntry; };
struct MP_APPLICATION_CHANNELISVISIBLE  { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED Object *IgnoreWindowChat; };


/*
** some defines
*/

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses 0x8042e9a7
#endif

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_Application_InitClass( void );
void    MCC_Application_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_APPLICATION_H */

