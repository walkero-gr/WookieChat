/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_NETWORK_H
#define MUICLASS_NETWORK_H 1

/*************************************************************************/

#include <exec/types.h>
#include <dos/dos.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>


#include "system.h"
#include "muiclass.h"
#include "muiclass_serverlist.h"
#include "muiclass_channellist.h"
#include "muiclass_nicklist.h"

#define NetworkObject NEWOBJECT( appclasses[ CLASSID_NETWORK ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_NETWORK_ =  0xFED00b00,

MM_NETWORK_SERVERFIND,
MM_NETWORK_SERVERALLOC,
MM_NETWORK_SERVERFREE,

MM_NETWORK_SERVERSOCKETINIT,
MM_NETWORK_SERVERSOCKETCLOSE,

MM_NETWORK_SERVERCONNECTAUTO,
MM_NETWORK_SERVERCONNECT,
MM_NETWORK_SERVERDISCONNECT,
MM_NETWORK_SERVERLOGIN,
MM_NETWORK_SERVERAUTOJOIN,

MM_NETWORK_SERVERMESSAGERECEIVED,
MM_NETWORK_SERVERMESSAGESENDMSG,
MM_NETWORK_SERVERMESSAGESENDPROC,
MM_NETWORK_SERVERMESSAGEPROCESS,
MM_NETWORK_SERVERMESSAGEPARSEBEGIN,
MM_NETWORK_SERVERMESSAGEPARSEEND,
 
MM_NETWORK_SERVERRECEIVEDATA,
MM_NETWORK_SERVERSENDDATA,

MM_NETWORK_WAITSELECT,

MM_NETWORK_CHANNELFIND,
MM_NETWORK_CHANNELALLOC,
MM_NETWORK_CHANNELFREE,

MM_NETWORK_CHATNICKENTRYALLOC,
MM_NETWORK_CHATNICKENTRYFREE,

/* Attributes */
MA_NETWORK_OBJECTSETTINGS,
MA_NETWORK_OBJECTAUDIO,
MA_NETWORK_OBJECTCHATLOG,
};

struct MP_NETWORK_SERVERFIND              { STACKED ULONG MethodID; STACKED struct ServerEntry *ServerEntry; };
struct MP_NETWORK_SERVERALLOC             { STACKED ULONG MethodID; STACKED struct ServerEntry *ServerEntry; };
struct MP_NETWORK_SERVERFREE              { STACKED ULONG MethodID; STACKED struct Server *Server; };

struct MP_NETWORK_SERVERCONNECTAUTO       { STACKED ULONG MethodID; };
struct MP_NETWORK_SERVERSOCKETINIT        { STACKED ULONG MethodID; STACKED struct Server *Server; };
struct MP_NETWORK_SERVERSOCKETCLOSE       { STACKED ULONG MethodID; STACKED struct Server *Server; };

struct MP_NETWORK_SERVERCONNECT           { STACKED ULONG MethodID; STACKED struct Server *Server; };
struct MP_NETWORK_SERVERDISCONNECT        { STACKED ULONG MethodID; STACKED struct Server *Server; };

struct MP_NETWORK_SERVERLOGIN             { STACKED ULONG MethodID; STACKED struct Server *Server; };
struct MP_NETWORK_SERVERAUTOJOIN          { STACKED ULONG MethodID; STACKED struct Server *Server; };

struct MP_NETWORK_SERVERMESSAGERECEIVED   { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Message; };
struct MP_NETWORK_SERVERMESSAGESENDMSG    { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct Channel *Channel; STACKED char *Message; };
struct MP_NETWORK_SERVERMESSAGEPARSEBEGIN { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Message; };
struct MP_NETWORK_SERVERMESSAGEPARSEEND   { STACKED ULONG MethodID; STACKED struct ServerMessageParse *ServerMessageParse; };
struct MP_NETWORK_SERVERMESSAGEPROCESS    { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct ServerMessageParse *ServerMessageParse; };

struct MP_NETWORK_SERVERRECEIVEDATA       { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Data; STACKED ULONG Length; };
struct MP_NETWORK_SERVERSENDDATA          { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Data; STACKED ULONG Length; };

struct MP_NETWORK_WAITSELECT              { STACKED ULONG MethodID; STACKED ULONG *SignalMask; };

struct MP_NETWORK_CHANNELFIND             { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Name; };
struct MP_NETWORK_CHANNELALLOC            { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED char *Name; STACKED ULONG Flags; };
struct MP_NETWORK_CHANNELFREE             { STACKED ULONG MethodID; STACKED struct Server *Server; STACKED struct Channel *Channel; };

struct MP_NETWORK_CHATNICKENTRYALLOC      { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED char *NickName; };
struct MP_NETWORK_CHATNICKENTRYFREE       { STACKED ULONG MethodID; STACKED struct Channel *Channel; STACKED struct ChatNickEntry *ChatNickEntry; };

/*************************************************************************/

#define SERVERBUFFER_SIZEOF 0xffff

struct Server {
	struct Server *s_Succ;
	struct Server *s_Pred;
/* copied over from serverentry */
	ULONG              s_Port;
	char               s_Name[ SERVERENTRY_NAME_SIZEOF   + 2 ];
	char               s_Address[ SERVERENTRY_ADDRESS_SIZEOF   + 2 ];
	char               s_Charset[ SERVERENTRY_CHARSET_SIZEOF   + 2 ];
	char               s_Password[ SERVERENTRY_PASSWORD_SIZEOF + 2 ];
	struct List        s_ChannelList;
	struct List        s_NickList;
	struct List        s_SendList;
/* network runtime data */
	ULONG              s_Flags;
	ULONG              s_State; /* this is the current state */
	ULONG              s_Retries;
	char               s_CurrentNick[ NICKENTRY_NICK_SIZEOF + 2 ];
	char               s_Buffer[ SERVERBUFFER_SIZEOF + 1 ];
	LONG 			   s_BufferFilled;
	//struct sockaddr_in s_ServerSocket;
	LONG               s_ServerSocket;
/* ident */
	long               s_IdentSocket;
	long               s_Ident_a_Socket;
	struct sockaddr_in s_IdentTest;
//	  long               ident_listen_socket;
//	  struct sockaddr_in ident_their_addr;
//	  char               ident_buffer[2000];

};

#define SERVERF_AUTOJOINSENT    1  /* mark if auto join has alredy been executed for server */

struct Channel {
	struct Channel    *c_Succ;
	struct Channel    *c_Pred;
	struct List        c_ChatLogList;
	struct List        c_ChatNickList;
	BPTR               c_ChatLogFile;
	char              *c_Topic;
	ULONG              c_Flags;
	char               c_Name[ CHANNELENTRY_CHANNEL_SIZEOF        + 2 ];
	char               c_Password[ CHANNELENTRY_PASSWORD_SIZEOF   + 2 ];
};

#define CHANNELF_SERVER          1  /* this is the servers channel */
#define CHANNELF_NAMESLIST       2  /* set after first NAMES command and cleared on last */
#define CHANNELF_MESSAGEOFTHEDAY 4  /* set after MOTD START arrives and clears on MOTD END */
#define CHANNELF_PUBLIC          8  /* set for public channel, clr for private */

struct Nick {
	struct Nick       *n_Succ;
	struct Nick       *n_Pred;
	char               n_Name[ NICKENTRY_NICK_SIZEOF       + 2 ];
	char               n_Password[ NICKENTRY_NICK_SIZEOF   + 2 ];
};

struct ChatNickEntry {
	struct ChatNickEntry *cne_Succ;
	struct ChatNickEntry *cne_Pred;
	char                  cne_Nick[1];
};

enum{
SVRSTATE_NOTCONNECTED = 0,
SVRSTATE_PRECONNECTED,
SVRSTATE_CONNECTED,
SVRSTATE_RETRY,
SVRSTATE_FAILED,
};

/*
** ServerMessageParse
*/

#define SMP_MESSAGEBUFFER_SIZEOF 0x200
#define SMP_STRINGSTORAGE_SIZEOF 0x1000

struct ServerMessageParse {
	ULONG                smp_Pen;
	ULONG                smp_ModeFlags;
	struct DateStamp     smp_DateStamp;
	char                 smp_MessageBuffer[ SMP_MESSAGEBUFFER_SIZEOF ];
	char                *smp_From;
	char                *smp_Command;
	char                *smp_Arguments;
	char                *smp_Message;
	char                *smp_Channel;     /* extracted from Arguments */
	char                *smp_Nick;        /* extracted from Arguments */
	char                *smp_FromNick;    /* extracted from Arguments */
	char                *smp_FromUserID;  /* extracted from Arguments */
	char                *smp_FromHost;    /* extracted from Arguments */
	char                *smp_Flags;       /* extracted from Arguments */
	char                *smp_Date;        /* extracted from Arguments */
	/* do not use this buffers in CMD functions. Use pointers above */
	char                 smp_StringStorage[ SMP_STRINGSTORAGE_SIZEOF ];
	char                 smp_Data[1];
};


/*
** Prototypes
*/

ULONG   MCC_Network_InitClass( void );
void    MCC_Network_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_NETWORK_H */

