/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_network.c
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "includes.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowchat.h"
#include "muiclass_network.h"

/*************************************************************************/

enum{
WID_SETTINGS = 0,
GID_AUDIO,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct List            mcc_ServerList;
	fd_set                 mcc_ReadMaster;       // master file descriptor list
	fd_set                 mcc_WriteMaster;      // master file descriptor list
	int                    mcc_FDMax;            // maximum file descriptor number
	fd_set                 mcc_ReadFDS;          // master file descriptor list
	fd_set                 mcc_WriteFDS;         // master file descriptor list

};

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg )
{
	if( ( obj = (Object*) DoSuperNew( cl, obj, TAG_DONE ) ) ) {
		struct mccdata *mccdata = INST_DATA( cl, obj );
		
		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );

		NEWLIST( &mccdata->mcc_ServerList ); /* init list */

		/* init defaults */
		mccdata->mcc_FDMax = -1;

	}
	return( (ULONG) obj );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;

	while( (s = (APTR) mccdata->mcc_ServerList.lh_Head)->s_Succ ) {
		DoMethod( obj, MM_NETWORK_SERVERFREE, s );
	}
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static ULONG OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_NETWORK_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (APTR) tidata;
				break;
			case MA_NETWORK_OBJECTAUDIO:
				mccdata->mcc_ClassObjects[ GID_AUDIO    ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_ServerFind()
**
*/

/*************************************************************************/

static ULONG MM_ServerFind( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERFIND *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = msg->ServerEntry;
struct Server *s;

	for( s = (APTR) mccdata->mcc_ServerList.lh_Head ; s->s_Succ ; s = s->s_Succ ) {
		if( s->s_Port == se->se_Port ) { /* different port -> different server */
			if( !Stricmp( (CONST_STRPTR) s->s_Address, (CONST_STRPTR) se->se_Address ) ) {
				return( (IPTR) s );
			}
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ServerAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERALLOC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server       *s = NULL;
struct ServerEntry  *se;
struct Nick         *n;
struct NickEntry    *ne;
struct Channel      *c;
struct ChannelEntry *ce;

	debug("MM_ServerAlloc()\n");

	if( ( se = msg->ServerEntry ) ) {
		if( ( s = AllocVec( sizeof( struct Server ), MEMF_ANY|MEMF_CLEAR ) ) ) {
			NEWLIST( &s->s_ChannelList );
			NEWLIST( &s->s_NickList );
			AddTail( &mccdata->mcc_ServerList, (struct Node *) s );
			s->s_ServerSocket      = -1;
			s->ident_listen_socket = -1;
			/* we cannot link to server entry as it may be removed by user during
			** runtime, so we spy important data */
			strcpy( s->s_Name    , se->se_Name );
			strcpy( s->s_Address , se->se_Address );
			strcpy( s->s_Charset , se->se_Charset );
			strcpy( s->s_Password, se->se_Password );
			if( !( s->s_Port = se->se_Port ) ) {
				s->s_Port = 6667;
			}
			s->s_State = SVRSTATE_NOTCONNECTED;

			/* add a server channel */
			if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELALLOC, s, s->s_Name ) ) ) {
				c->c_Flags      |= CHANNELF_SERVER;
				c->c_ChatWindow  = msg->WindowChat;
				debug("now adding channel\n");
				DoMethod( c->c_ChatWindow, MM_WINDOWCHAT_CHANNELADD, c );
			}
			debug("c was %08lx\n", c);

			for( ce = (APTR) se->se_ChannelList.lh_Head ; ce->ce_Succ ; ce = ce->ce_Succ ) {
				if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELALLOC, s, ce->ce_Name ) ) ) {
					strcpy( c->c_Password, ce->ce_Password );
					c->c_ChatWindow = msg->WindowChat;
				}
			}
			for( ne = (APTR) se->se_ChannelList.lh_Head ; ne->ne_Succ ; ne = ne->ne_Succ ) {
				if( ( n = AllocVec( sizeof( struct Channel ), MEMF_ANY ) ) ) {
					AddTail( &s->s_NickList, (struct Node *) n );
					strcpy( n->n_Name    , ne->ne_Name     );
					strcpy( n->n_Password, ne->ne_Password );
				}
			}
		}
	}
	debug("MM_ServerAlloc() - done\n");

	return( (IPTR) s );
}
/* \\\ */
/* /// MM_ServerFree()
**
*/

/*************************************************************************/

static ULONG MM_ServerFree( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERFREE *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server   *s = msg->Server;
struct Nick         *n;
struct Channel      *c;

	debug("server free '%s'\n", s->s_Name );

	/* dissnnect if needed */

	DoMethod( obj, MM_NETWORK_SERVERDISCONNECT, s );

	/* remove all channels */
	while( ( c = (APTR) s->s_ChannelList.lh_Head )->c_Succ ) {
		DoMethod( obj, MM_NETWORK_CHANNELFREE, s, c );
	}

	/* remove all nicks */
	while( ( n = (APTR) s->s_NickList.lh_Head )->n_Succ ) {
		/* remove from list */
		Remove( (struct Node *) n );
		/* free structure */
		FreeVec( n );
	}
	/* remove from list */
	Remove( (struct Node *) s );
	/* free structure */
	FreeVec( s );

	return( 0 );
}
/* \\\ */

/* /// MM_ServerConnectAuto()
**
*/

/*************************************************************************/

static ULONG MM_ServerConnectAuto( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERCONNECTAUTO *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct Server      *s;
Object *serverlist, *chatwindow;
ULONG i;

	serverlist = (APTR) LocalReadConfig( OID_SVR_LIST );

	for( i = 0 ; ; i++ ) {
		se = NULL;

		if( !( chatwindow = (Object*) DoMethod( _app(obj), MM_APPLICATION_CHATFINDACTIVE ) ) ) {
			chatwindow = (Object*) DoMethod( _app(obj), MM_APPLICATION_WINDOWNEWCHAT );
		}

		DoMethod( serverlist, MUIM_NList_GetEntry, i, &se );
		if( se ) {

			if( ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ) {

				if( ( s = (APTR) DoMethod( obj, MM_NETWORK_SERVERALLOC, se, chatwindow ) ) ) {

					DoMethod( obj, MM_NETWORK_SERVERCONNECT, s );
				}
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

/* /// MM_ServerSocketInit()
**
*/

static ULONG MM_ServerSocketInit( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSOCKETINIT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;
ULONG result;

	debug("MM_ServerSocketInit()\n");

	result = MSG_ERROR_NOERROR + 1; /* socket error */
	if( ( s = msg->Server ) ) {

		if( ( s->s_ServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) != INVALID_SOCKET ) {
			ULONG on = 1;
			result = MSG_ERROR_NOERROR + 2; /* invalid socket */

			if( setsockopt( s->s_ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char const*) &on, sizeof( ULONG ) ) != -1 ) {
				if( mccdata->mcc_FDMax < s->s_ServerSocket ) {
					mccdata->mcc_FDMax = s->s_ServerSocket;
				}

				if( IoctlSocket( s->s_ServerSocket, FIONBIO, (char*) &on) >= 0 ) {
					result = MSG_ERROR_NOERROR;
				}
			}
		}
    }
	debug("MM_ServerSocketInit() - Done %08lx\n", result );
	return( result );
}
/* \\\ */
/* /// MM_ServerSocketClose()
**
*/

static ULONG MM_ServerSocketClose( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSOCKETCLOSE *msg )
{
struct Server *s;

	if( ( s = msg->Server ) ) {
		if( s->s_ServerSocket != -1 ) { /* still open? */
			CloseSocket( s->s_ServerSocket );
			s->s_ServerSocket = -1;
		}
		s->s_State = SVRSTATE_NOTCONNECTED;
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerConnect()
**
*/

/*************************************************************************/

static ULONG MM_ServerConnect( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERCONNECT *msg )
{
struct Server *s = msg->Server;
ULONG result;

	debug("MM_ServerConnect()\n" );

	result = MSG_ERROR_NOERROR + 4; /* bsdsocket missing */
	if( !SocketBase ) {
		SocketBase = OpenLibrary( (_ub_cs) "bsdsocket.library", 0L );
#ifdef __amigaos4__
		ISocket = (struct SocketIFace*) GetInterface( (struct Library *) SocketBase, "main", 1, NULL );
#endif
    }

	if( SocketBase ) {
		struct hostent *he;
		result = MSG_ERROR_NOERROR + 5; /* host error */

		if( !( result = DoMethod( obj, MM_NETWORK_SERVERSOCKETINIT, s ) ) ) {
			debug("connecting to '%s' Port %ld\n",s->s_Address, s->s_Port );
			if( ( he = gethostbyname( (const UBYTE *) s->s_Address) ) ) {
				debug("%20lh\n", he );
				struct sockaddr_in addr;

				addr.sin_family = AF_INET;
				addr.sin_port   = htons( s->s_Port );
				addr.sin_addr   = *((struct in_addr *) he->h_addr);;

				memset( &( addr.sin_zero ), '\0', 8 );

				result = MSG_ERROR_NOERROR + 6; /* socket error */
				if( ( connect( s->s_ServerSocket, (struct sockaddr*) &addr, sizeof( struct sockaddr ) ) != SOCKET_ERROR ) ) {
					s->s_State = SVRSTATE_CONNECTED;
					result = MSG_ERROR_NOERROR;
				}
			}
			if( result )  {
				DoMethod( obj, MM_NETWORK_SERVERSOCKETCLOSE, s );
				s->s_Retry++;
				if( s->s_Retry <= 5 ) {
					s->s_State = SVRSTATE_RETRY;
				} else {
					s->s_State = SVRSTATE_FAILED;
				}
			}
		}
    }
	debug("MM_ServerConnect() - Done %08lx\n", result );
	return( result );
}
/* \\\ */
/* /// MM_ServerDisconnect()
**
*/

/*************************************************************************/

static ULONG MM_ServerDisconnect( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERDISCONNECT *msg )
{
struct Server *s = msg->Server;

	debug("disconnect '%s'\n", s->s_Name );

	if( SocketBase ) {
		DoMethod( obj, MM_NETWORK_SERVERSOCKETCLOSE, s );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerSendData()
**
*/

/*************************************************************************/

static ULONG MM_ServerSendData( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSENDDATA *msg )
{
struct Server *s = msg->Server;
ULONG result;

	result = MSG_ERROR_NOERROR + 10;

	if( s->s_State == SVRSTATE_CONNECTED ) {
		if( ( send( s->s_ServerSocket, (CONST UBYTE *) msg->Data, ( msg->Length ? msg->Length : strlen( msg->Data ) ), 0 ) != -1 ) ) {
			result = MSG_ERROR_NOERROR;
		}
	}
	return( result );
}
/* \\\ */

/* /// MM_ServerReceiveData()
**
*/

/*************************************************************************/

static ULONG MM_ServerReceiveData( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERRECEIVEDATA *msg )
{
struct Server *s = msg->Server;
LONG bytes;

	if( s->s_State == SVRSTATE_CONNECTED ) {

		memset( msg->Data, 0, msg->Length );

		bytes = recv( s->s_ServerSocket, (UBYTE *) msg->Data, msg->Length - 1, 0 );

		if( bytes > 0 ) {
			return( bytes );
		} else {
			DoMethod( obj, MM_NETWORK_SERVERDISCONNECT );
		}
	}
	return(0);
}
/* \\\ */
/* /// MM_WaitSelect()
**
*/

/*************************************************************************/

static ULONG MM_WaitSelect( struct IClass *cl, Object *obj, struct MP_NETWORK_WAITSELECT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG selectresult;
LONG waitsignals = *((ULONG*) msg->SignalMask );

	mccdata->mcc_ReadFDS  = mccdata->mcc_ReadMaster;
	mccdata->mcc_WriteFDS = mccdata->mcc_WriteMaster;

	selectresult  = WaitSelect( mccdata->mcc_FDMax + 1, &mccdata->mcc_ReadFDS, &mccdata->mcc_WriteFDS, NULL, NULL, msg->SignalMask );

	if( selectresult > 0 ) {
		struct Server *s;
		ULONG i;

		for( i = 0; i <= mccdata->mcc_FDMax; i++ ) {

			if( FD_ISSET( i, &mccdata->mcc_ReadFDS ) ) {

				for( s = (APTR) mccdata->mcc_ServerList.lh_Head ; s->s_Succ ; s = s->s_Succ ) {
					if( ( i == s->s_ServerSocket ) || ( i == s->ident_a_socket ) || ( i == s->ident_listen_socket ) ) {
						break;
					}
				}
				if( !s->s_Succ ) {
					s = (APTR) mccdata->mcc_ServerList.lh_Head;
				}
				if( s->s_Succ ) {
					
					UBYTE buffer[0x100];
					LONG length;
					debug("plan on reading\n");
					length = recv( i, buffer, 0x100, 0 );
					buffer[ 0x100 - 1 ] = 0x00;
					debug("Buffer '%s'\n", buffer );
				}
			}
			if( FD_ISSET( i, &mccdata->mcc_WriteFDS ) ) {

			}
		}

	} else {
//		  debug("wait select failed -> using normal wait to keep application alive\n");
		*((ULONG*) msg->SignalMask ) = Wait( waitsignals );
	}
	return( selectresult );
}
/* \\\ */

/* /// MM_ChannelFind()
**
*/

/*************************************************************************/

static ULONG MM_ChannelFind( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELFIND *msg )
{
struct Server   *s = msg->Server;
struct Channel      *c;

	for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
		if( !Stricmp( (CONST_STRPTR) c->c_Name, (CONST_STRPTR) msg->Name ) ) {
			return( (IPTR) c );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ChannelAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELALLOC *msg )
{
struct Server   *s = msg->Server;
struct Channel      *c;

	if( !( msg->Name ) || !(msg->Name[0]) || !( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELFIND, s, msg->Name ) ) ) {
		if( (msg->Name[0] ) ) {
			if( ( c = AllocVec( sizeof( struct Channel ), MEMF_ANY|MEMF_CLEAR ) ) ) {
				AddTail( &s->s_ChannelList, (struct Node *) c );
				if( msg->Name ) {
					strcpy( c->c_Name, msg->Name );
				}
			}
		}
	}
	return( (IPTR) c );
}
/* \\\ */
/* /// MM_ChannelFree()
**
*/

/*************************************************************************/

static ULONG MM_ChannelFree( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELFREE *msg )
{
struct Channel      *c  = msg->Channel;

	debug("channel free #1 '%s'\n", c->c_Name );

	if( c ) {
		if( c->c_ChatWindow ) {
	debug("channel free #2 '%s'\n", c->c_Name );
			DoMethod( c->c_ChatWindow, MM_WINDOWCHAT_CHANNELREMOVE, c );
	debug("channel free #3 '%s'\n", c->c_Name );
		}
	debug("channel free #4 '%s'\n", c->c_Name );
		Remove( (struct Node *) c );
	debug("channel free #5 '%s'\n", c->c_Name );
		FreeVec( c );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_Network_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Network_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                        : return( OM_New              ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                    : return( OM_Dispose          ( cl, obj, (APTR) msg ) );
		case OM_SET                        : return( OM_Set              ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_WAITSELECT         : return( MM_WaitSelect       ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_SERVERSOCKETINIT   : return( MM_ServerSocketInit ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERSOCKETCLOSE  : return( MM_ServerSocketClose( cl, obj, (APTR) msg ) );

		case MM_NETWORK_SERVERFIND         : return( MM_ServerFind       ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERALLOC        : return( MM_ServerAlloc      ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERFREE         : return( MM_ServerFree       ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERCONNECT      : return( MM_ServerConnect    ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERDISCONNECT   : return( MM_ServerDisconnect ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERCONNECTAUTO  : return( MM_ServerConnectAuto( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERRECEIVEDATA  : return( MM_ServerReceiveData( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERSENDDATA     : return( MM_ServerSendData   ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_CHANNELFIND        : return( MM_ChannelFind      ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELALLOC       : return( MM_ChannelAlloc     ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELFREE        : return( MM_ChannelFree      ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Network_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Network_InitClass( void )
{
	appclasses[ CLASSID_NETWORK ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Area, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_Network_Dispatcher) );
	return( appclasses[ CLASSID_NETWORK ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Network_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Network_DisposeClass( void )
{
	if( appclasses[ CLASSID_NETWORK ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_NETWORK ] );
		appclasses[ CLASSID_NETWORK ] = NULL;
    }
}
/* \\\ */




