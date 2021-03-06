/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowchat.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_messageinput.h"
#include "muiclass_network.h"
#include "muiclass_settingscolor.h"
#include "muiclass_application.h"
#include "muiclass_windowchat.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowsettings.h"
#include "muiclass_chatuserlist.h"
#include "muiclass_chatchannellist.h"
#include "muiclass_chatlog.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_MENUSTRIP = 0,
GID_NETWORK,
WID_SETTINGS,
WID_QUIT,
MID_HIDE,
MID_HISTORYCLEAR,
MID_HISTORYCLEARALL,
MID_HISTORYSAVE,
MID_ABOUT,
MID_QUIT,
MID_CUT,
MID_COPY,
MID_PASTE,
MID_SETTINGS,
MID_SETTINGSMUI,
MID_MUTESOUND,
MID_DCCINCOMING,
MID_DCCOUTGOIING,
MID_URLGRABBER,
MID_IGNORELIST,
GID_CLOSETAB,
GID_TOPIC,
GID_MODET,
GID_MODEN,
GID_MODES,
GID_MODEI,
GID_MODEP,
GID_MODEM,
GID_MODEB,
GID_MODEK,
GID_MODEKEYWORD,
GID_MODEL,
GID_MODELIMIT,
GID_CHATUSERLIST,
GID_CHATCHANNELLIST,
GID_CHATLOG,
GID_CHATMESSAGE,
GID_LAST
};

#define WINDOWTITLE_SIZEOF 0x100
#define TOPIC_SIZEOF 0x400
#define KEYWORDLIMIT_SIZEOF 40
#define USERLIMIT_SIZEOF    40
#define MESSAGE_SIZEOF      490

#define FIRSTMENU_ITEM MID_HIDE
#define LASTMENU_ITEM MID_IGNORELIST
 
/*
** data used by this class
*/
#define COMMAND_SIZEOF 0x1000

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	char                   mcc_WindowTitle[ WINDOWTITLE_SIZEOF + 2 ];
	char                   mcc_CommandBuffer[ COMMAND_SIZEOF ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ] = {0};

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWCHAT_TITLE ),
			MUIA_Window_ID               , MAKE_ID('C','H','A','T'),
			MUIA_Window_Menustrip, MenustripObject,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_WINDOWCHAT_PROJECT_TITLE ),
								Child, objs[ MID_HIDE               ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_WINDOWCHAT_HIDE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_WINDOWCHAT_HIDE_KEY ), End,
								Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
								Child, objs[ MID_HISTORYCLEAR       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEAR_ITEM ), End,
								Child, objs[ MID_HISTORYCLEARALL    ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEARALL_ITEM ), End,
								Child, objs[ MID_HISTORYSAVE        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYSAVE_ITEM ), End,
								Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
								Child, objs[ MID_ABOUT              ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_ABOUT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_ABOUT_KEY ), End,
								Child, objs[ MID_QUIT               ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_QUIT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_QUIT_KEY ), End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_EDIT_TITLE ),
								Child, objs[ MID_CUT                ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_CUT_ITEM )  , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_CUT_KEY )  , End,
								Child, objs[ MID_COPY               ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_COPY_ITEM ) , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_COPY_KEY ) , End,
								Child, objs[ MID_PASTE              ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_PASTE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_PASTE_KEY ), End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_SETTINGS_TITLE ),
								Child, objs[ MID_SETTINGS           ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMAIN_ITEM ), End,
								Child, objs[ MID_SETTINGSMUI        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMUI_ITEM ), End,
								Child, objs[ MID_MUTESOUND          ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_MUTEALLSOUNDS_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_WINDOWS_TITLE ),
								Child, objs[ MID_DCCINCOMING        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCINCOMING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCINCOMING_KEY ), End,
								Child, objs[ MID_DCCOUTGOIING       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCOUTGOING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCOUTGOING_KEY ), End,
								Child, objs[ MID_URLGRABBER         ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_URLGRABBER_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_URLGRABBER_KEY ), End,
								Child, objs[ MID_IGNORELIST         ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_IGNORELIST_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_IGNORELIST_KEY ), End,
							End,
						End,
			WindowContents, VGroup,
						Child, HGroup,
							Child, objs[ GID_CLOSETAB    ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_CLOSETAB_GAD ),
							Child, objs[ GID_TOPIC       ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_TOPIC_HELP-1, TOPIC_SIZEOF ),
							Child, BalanceObject, End,
							Child, objs[ GID_MODET       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODET_GAD ),
							Child, objs[ GID_MODEN       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEN_GAD ),
							Child, objs[ GID_MODES       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODES_GAD ),
							Child, objs[ GID_MODEI       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEI_GAD ),
							Child, objs[ GID_MODEP       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEP_GAD ),
							Child, objs[ GID_MODEM       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEM_GAD ),
							Child, objs[ GID_MODEB       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEB_GAD ),
							Child, objs[ GID_MODEK       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEK_GAD ),
							Child, HGroup, MUIA_HorizWeight, 40,
								Child, objs[ GID_MODEKEYWORD ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_MODEKEYWORD_HELP-1, KEYWORDLIMIT_SIZEOF ),
								Child, objs[ GID_MODEL       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEL_GAD ),
								Child, objs[ GID_MODELIMIT   ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_MODELIMIT_HELP-1, USERLIMIT_SIZEOF ),
							End,
						End,
						Child, HGroup,
							Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATLOG ] = ChatLogObject, End, End,
							Child, VGroup, MUIA_HorizWeight, 20,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATUSERLIST      ] = ChatUserListObject, End, End,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATCHANNELLIST   ] = ChatChannelListObject, End, End,
							End,
						 End,
						Child, objs[ GID_CHATMESSAGE  ] = MessageInputObject,
													MUIA_Frame        , MUIV_Frame_String,
													MUIA_CycleChain   , 1,
													MUIA_ShortHelp    , LGS( MSG_MUICLASS_WINDOWCHAT_MESSAGE_HELP ),
													MUIA_String_MaxLen, MESSAGE_SIZEOF,
													End,
					 End,
		TAG_DONE ) ) ) {
		ULONG i;
		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );

		sprintf( &mccdata->mcc_WindowTitle[0], (const char *) LGS( MSG_MUICLASS_WINDOWCHAT_TITLE ), VERSION, REVISION );
		SetAttrs( obj, MUIA_Window_Title, mccdata->mcc_WindowTitle, TAG_DONE );

		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_Notify, MUIA_NList_Active       , MUIV_EveryTime, obj, 1, MM_WINDOWCHAT_CHANNELCHANGE );

		DoMethod( mccdata->mcc_ClassObjects[ GID_CLOSETAB        ], MUIM_Notify, MUIA_Pressed            , FALSE         , obj, 1, MM_WINDOWCHAT_CHANNELPART );

		DoMethod( obj                                             , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , MUIV_Notify_Application, 2, MM_APPLICATION_WINDOWCHATCLOSE, obj );

		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHATMESSAGE ],
					MA_MESSAGEINPUT_OBJECTCHATUSERLIST   , mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ],
					MA_MESSAGEINPUT_OBJECTCHATCHANNELLIST, mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ],
					MA_MESSAGEINPUT_OBJECTNETWORK        , mccdata->mcc_ClassObjects[ GID_NETWORK      ],
					MA_MESSAGEINPUT_OBJECTSETTINGS       , mccdata->mcc_ClassObjects[ WID_SETTINGS     ],
					TAG_DONE );

		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ],
					MA_CHATUSERLIST_OBJECTCHATCHANNELLIST, mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ],
					MA_CHATUSERLIST_OBJECTNETWORK        , mccdata->mcc_ClassObjects[ GID_NETWORK      ],
					MA_CHATUSERLIST_OBJECTSETTINGS       , mccdata->mcc_ClassObjects[ WID_SETTINGS     ],
					TAG_DONE );



		for( i = FIRSTMENU_ITEM ; i <= LASTMENU_ITEM ; i++ ) {
			DoMethod( mccdata->mcc_ClassObjects[ i ], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj, 2, MM_WINDOWCHAT_MENUSELECT, i );
		}

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static IPTR OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWCHAT ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static IPTR OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		IPTR tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_WINDOWCHAT_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (APTR) tidata;
				break;
			case MA_WINDOWCHAT_OBJECTNETWORK:
				mccdata->mcc_ClassObjects[ GID_NETWORK ] = (APTR) tidata;
				break;
			case MA_WINDOWCHAT_OBJECTQUIT:
				mccdata->mcc_ClassObjects[ WID_QUIT    ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_MenuSelect()
**
*/

/*************************************************************************/

static IPTR MM_MenuSelect( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_MENUSELECT *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
Object *tmpobj;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	switch( msg->MenuID ) {

/* project menu */
		case MID_ABOUT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWABOUT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_QUIT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWQUIT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
/* edit menu */

/* settings menu */
		case MID_SETTINGS:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;

/* window menu */
		case MID_SETTINGSMUI:
			DoMethod( _app( obj ), MUIM_Application_OpenConfigWindow, 0, NULL );
			break;
		case MID_IGNORELIST:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWIGNORELIST );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_URLGRABBER:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWURLGRABBER );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
	}
	return( 0 );
}
/* \\\ */
/* /// MM_VisualChange()
**
*/

/*************************************************************************/

static IPTR MM_VisualChange( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

/* first update colors of all classes involved */
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG         ], MM_CHATLOG_PENSUPDATE         );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST    ], MM_CHATUSERLIST_PENSUPDATE    );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MM_CHATCHANNELLIST_PENSUPDATE );
/* second update all elements effected by visual settings */
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG         ], MUIM_NList_Redraw, MUIV_NList_Redraw_All );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST    ], MUIM_NList_Redraw, MUIV_NList_Redraw_All );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_All );
	return( 0 );
}
/* \\\ */

/* /// MM_ChannelPart()
**
*/

/*************************************************************************/

static IPTR MM_ChannelPart( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s = NULL;
struct ChatChannelEntry *cce;
#define PART_BUFFER 0x100
char partbuffer[ PART_BUFFER ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	cce = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );
	if( cce ) {
		/* pointer magic */
		s = (APTR) ( ( (IPTR) List_GetListFromNode( cce->cce_Channel ) ) - (IPTR) offsetof( struct Server, s_ChannelList ) );
		strcpy( partbuffer, "/PART " );
		strcat( partbuffer, cce->cce_Channel->c_Name );

		DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_SERVERMESSAGESENDMSG, s, cce->cce_Channel, partbuffer );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelAdd()
**
*/

/*************************************************************************/

static IPTR MM_ChannelAdd( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELADD *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !LRC( OID_GUI_TABSERVER ) ) { /* only add server channel if user allows it */
		if( msg->Channel->c_Flags & CHANNELF_SERVER ) {
			return( 0 );
		}
	}

	/* only add, if not already in list */
	for( i = 0 ;  ; i++ ) {
		cce = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, i, &cce );
		if( cce ) {
			if( cce->cce_Channel == msg->Channel ) {
				return( 0 );
			}
		} else {
			break;
		}
	}

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_ADD, msg->Channel );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_InsertSingle, msg->Channel, MUIV_NList_Insert_Bottom );

	return( 0 );
}
/* \\\ */
/* /// MM_ChannelRemove()
**
*/

/*************************************************************************/

static IPTR MM_ChannelRemove( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELREMOVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_REMOVE, msg->Channel );

	for( i = 0 ;  ; i++ ) {
		cce = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, i, &cce );
		if( cce ) {
			if( cce->cce_Channel == msg->Channel ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Remove, i );
				if( i ) {
					i--; /* next time this pos is a differen list entry different */
				}
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelChange()
**
*/

/*************************************************************************/

static IPTR MM_ChannelChange( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELCHANGE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatLogEntry     *cle;
struct ChatChannelEntry *cce;
struct Channel          *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG      ], MUIM_NList_Clear );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Clear );

	cce = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );
	if( cce ) {
		if( ( c = cce->cce_Channel ) ) {
			struct Node *node;
			cce->cce_Pen = PEN_CHANNELLISTTEXT;
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active, cce );

			DoMethod( obj, MM_WINDOWCHAT_CHANNELCHANGETOPIC, c );

			for( cle = (APTR) c->c_ChatLogList.lh_Head ; cle->cle_Succ ; cle = cle->cle_Succ ) {
				if( !LRC( OID_GUI_SHOWJOINPART ) ) {
					if( ( cle->cle_Pen == PEN_LOGJOIN ) || ( cle->cle_Pen == PEN_LOGPART ) || ( cle->cle_Pen == PEN_LOGQUIT ) ) {
						continue; /* do not show */
					}
				}
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MUIM_NList_InsertSingleWrap, cle, MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT );
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MM_CHATLOG_SHOWLASTLINE, TRUE );

			for( node = (APTR) c->c_ChatNickList.lh_Head ; node->ln_Succ ; node = node->ln_Succ ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_InsertSingle, node, MUIV_NList_Insert_Bottom );
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Sort );

		}
	}
	SetAttrs( obj, MUIA_Window_ActiveObject, mccdata->mcc_ClassObjects[ GID_CHATMESSAGE  ], TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_ChannelChangeTopic()
**
*/

/*************************************************************************/

static IPTR MM_ChannelChangeTopic( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELCHANGETOPIC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;
struct Channel          *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	cce = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );
	if( cce ) {
		if( ( c = cce->cce_Channel ) ) {
			char *topic;
			if( ( topic = c->c_Topic ) ) {
				SetAttrs( mccdata->mcc_ClassObjects[ GID_TOPIC ], MUIA_NoNotify, TRUE, MUIA_String_Contents, topic, TAG_DONE );
			}
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ChannelIsVisible()
**
** This is just a gateway to keep code clean
*/

/*************************************************************************/

static IPTR MM_ChannelIsVisible( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELISVISIBLE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	return( DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MM_CHATCHANNELLIST_CHANNELISVISIBLE, msg->Channel ) );
}
/* \\\ */

/* /// MM_ChannelNickAdd()
**
*/

/*************************************************************************/

static IPTR MM_ChannelNickAdd( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELNICKADD *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannelEntry *cce;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	cce = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );
	if( cce ) {
		/* is this channel visible right now? */
		if( cce->cce_Channel == msg->Channel ) { /* yes, then update user list */
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_InsertSingle, msg->ChatNickEntry, MUIV_NList_Insert_Bottom );
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Sort );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelNickRemove()
**
*/

/*************************************************************************/

static IPTR MM_ChannelNickRemove( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELNICKREMOVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatUserEntry *cue;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ;  ; i++ ) {
		cue = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_GetEntry, i, &cue );
		if( cue ) {
			if( cue->cue_ChatNickEntry == msg->ChatNickEntry ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Remove, i );
				break;
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_MessageReceived()
**
*/

/*************************************************************************/

static IPTR MM_MessageReceived( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_MESSAGERECEIVED *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Channel          *c;
struct ChatChannelEntry *cce;
struct ChatLogEntry     *cle = msg->ChatLogEntry;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* pointer magic */

	c = (APTR) ( ( (IPTR) List_GetListFromNode( msg->ChatLogEntry ) ) - (IPTR) offsetof( struct Channel, c_ChatLogList ) );

/* is this channel visible right now? */

	cce = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cce );
	if( cce && cce->cce_Channel == c ) {
	/* yes, so add to log */
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MUIM_NList_InsertSingleWrap, cle, MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT );
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MM_CHATLOG_SHOWLASTLINE, FALSE );
		/* now update the chat channel list */
		cce->cce_Pen = PEN_CHANNELLISTTEXT;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active, cce );
	} else { /* it is not, so handle color */
		ULONG i;
		for( i = 0 ; ; i++ ) {
			cce = NULL;
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, i, &cce );
			if( cce ) {
				if( cce->cce_Channel == c ) { /* is this our channel to update */
					switch( cle->cle_Pen ) {
						case PEN_LOGHIGHLIGHT:
							cce->cce_Pen = PEN_CHANNELLISTHIGHLIGHT;
							break;
						default: /* no change */
						case PEN_LOGOWNTEXT:
							break;
						case PEN_LOGPRIVMSG:
						case PEN_LOGACTION:
						case PEN_LOGJOIN:
						case PEN_LOGPART:
						case PEN_LOGQUIT:
						case PEN_LOGKICK:
						case PEN_LOGNICKCHANGE:
							if( cce->cce_Pen < PEN_CHANNELLISTUSER ) {
								cce->cce_Pen = PEN_CHANNELLISTUSER;
							}
							break;
						case PEN_LOGMODE:
						case PEN_LOGCTCP:
						case PEN_LOGINFO:
						case PEN_LOGNOTICE:
						case PEN_LOGINVITE:
						case PEN_LOGTOPIC:
						case PEN_LOGWALLOPS:
							if( cce->cce_Pen < PEN_CHANNELLISTSERVER ) {
								cce->cce_Pen = PEN_CHANNELLISTSERVER;
							}
							break;
					}
					DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Redraw, i, cce );
					break;
				}
			} else {
				break;
			}
		}
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowChat_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowChat_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                 ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                         : return( OM_Dispose             ( cl, obj, (APTR) msg ) );

		case OM_GET                             : return( OM_Get                 ( cl, obj, (APTR) msg ) );
		case OM_SET                             : return( OM_Set                 ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_MENUSELECT           : return( MM_MenuSelect          ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_VISUALCHANGE         : return( MM_VisualChange        ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_MESSAGERECEIVED      : return( MM_MessageReceived     ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELPART          : return( MM_ChannelPart         ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELADD           : return( MM_ChannelAdd          ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELREMOVE        : return( MM_ChannelRemove       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELCHANGE        : return( MM_ChannelChange       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELCHANGETOPIC   : return( MM_ChannelChangeTopic  ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_CHANNELNICKADD       : return( MM_ChannelNickAdd      ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELNICKREMOVE    : return( MM_ChannelNickRemove   ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_CHANNELISVISIBLE     : return( MM_ChannelIsVisible    ( cl, obj, (APTR) msg ) );

    }

	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowChat_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowChat_InitClass( void )
{
	appclasses[ CLASSID_WINDOWCHAT ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowChat_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWCHAT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowChat_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowChat_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWCHAT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWCHAT ] );
		appclasses[ CLASSID_WINDOWCHAT ] = NULL;
    }
}
/* \\\ */




