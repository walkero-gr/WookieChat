/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_CHATLOG_H
#define MUICLASS_CHATLOG_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define ChatLogObject NEWOBJECT( appclasses[ CLASSID_CHATLOG ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_CHATLOG_ =  0xFED00700,
MM_CHATLOG_SHOWLASTLINE,

MM_CHATLOG_PENSOBTAIN,
MM_CHATLOG_PENSRELEASE,
MM_CHATLOG_PENSUPDATE,

MM_CHATLOG_COMPOSELOGNAME,
MM_CHATLOG_OPENNEWFILE,
MM_CHATLOG_OPEN,
MM_CHATLOG_CLOSE,
MM_CHATLOG_WRITE,
MM_CHATLOG_ENTRYALLOC,
MM_CHATLOG_ENTRYFREE,

/* Attributes */
};

struct MP_CHATLOG_SHOWLASTLINE { ULONG MethodID; ULONG Force; };

struct MP_CHATLOG_COMPOSELOGNAME { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct Channel *Channel; };
struct MP_CHATLOG_OPENNEWFILE    { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct Channel *Channel; };
struct MP_CHATLOG_OPEN           { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct Channel *Channel; };
struct MP_CHATLOG_CLOSE          { STACKED ULONG MethodID; STACKED struct Channel *Channel; };
struct MP_CHATLOG_WRITE          { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatLogEntry *ChatLogEntry; };
struct MP_CHATLOG_ENTRYALLOC     { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED ULONG Pen; STACKED char *Message;  };
struct MP_CHATLOG_ENTRYFREE      { STACKED ULONG MethodID; STACKED struct ChatLogEntry *ChatLogEntry; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_ChatLog_InitClass( void );
void    MCC_ChatLog_DisposeClass( void );

/*************************************************************************/

struct ChatLogEntry {
	struct ChatLogEntry *cle_Succ;
	struct ChatLogEntry *cle_Pred;
	ULONG                cle_Pen;
	char                 cle_Message[1];
};

/*************************************************************************/

#endif /* MUICLASS_CHATLOG_H */

