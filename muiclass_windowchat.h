/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWCHAT_H
#define MUICLASS_WINDOWCHAT_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"
#include "muiclass_network.h"


#define WindowChatObject NEWOBJECT( appclasses[ CLASSID_WINDOWCHAT ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWCHAT_DUMMY =  0xFED00010,
MM_WINDOWCHAT_MENUSELECT,
MM_WINDOWCHAT_VISUALCHANGE,

MM_WINDOWCHAT_MESSAGERECEIVED,

MM_WINDOWCHAT_CHANNELPART,
MM_WINDOWCHAT_CHANNELADD,
MM_WINDOWCHAT_CHANNELREMOVE,
MM_WINDOWCHAT_CHANNELCHANGE,
MM_WINDOWCHAT_CHANNELCHANGETOPIC,
MM_WINDOWCHAT_CHANNELNICKADD,
MM_WINDOWCHAT_CHANNELNICKREMOVE,
MM_WINDOWCHAT_CHANNELISVISIBLE,
/* Attributes */
MA_WINDOWCHAT_OBJECTSETTINGS,
MA_WINDOWCHAT_OBJECTNETWORK,
MA_WINDOWCHAT_OBJECTQUIT,
};

struct MP_WINDOWCHAT_MENUSELECT         { STACKED ULONG MethodID; STACKED ULONG MenuID; };

struct MP_WINDOWCHAT_MESSAGERECEIVED    { STACKED ULONG MethodID; STACKED struct ChatLogEntry *ChatLogEntry; };
struct MP_WINDOWCHAT_MESSAGEENTERED     { STACKED ULONG MethodID; };
struct MP_WINDOWCHAT_CHANNELREMOVE      { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_WINDOWCHAT_CHANNELADD         { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_WINDOWCHAT_CHANNELCHANGE      { STACKED ULONG MethodID; };
struct MP_WINDOWCHAT_CHANNELCHANGETOPIC { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_WINDOWCHAT_CHANNELNICKREMOVE  { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatNickEntry *ChatNickEntry; };
struct MP_WINDOWCHAT_CHANNELNICKADD     { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatNickEntry *ChatNickEntry; };
struct MP_WINDOWCHAT_CHANNELISVISIBLE   { STACKED ULONG MethodID; STACKED struct Channel *Channel; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowChat_InitClass( void );
void    MCC_WindowChat_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWCHAT_H */

