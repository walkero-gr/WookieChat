/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingscolor.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <libraries/asl.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/asl.h>
#include <SDI_hook.h>
#include <string.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"
#include "version.h"

/*************************************************************************/

#define LINEBUFFER_SIZEOF 0x2000


/*
** gadgets used by this class
*/

enum
{
GID_CHANNELLISTBACKGROUND,
GID_CHANNELLISTTEXT,
GID_CHANNELLISTUSER,
GID_CHANNELLISTSERVER,
GID_CHANNELLISTHIGHLIGHT,
GID_USERLISTBACKGROUND,
GID_USERLISTTEXT,
GID_LOGLISTBACKGROUND,
GID_LOGPRIVMSG,
GID_LOGJOIN,
GID_LOGPART,
GID_LOGQUIT,
GID_LOGMODE,
GID_LOGCTCP,
GID_LOGACTION,
GID_LOGINFO,
GID_LOGOWNTEXT,
GID_LOGHIGHLIGHT,
GID_LOGNOTICE,
GID_LOGINVITE,
GID_LOGKICK,
GID_LOGNICKCHANGE,
GID_LOGTOPIC,
GID_LOGWALLOPS,
/* other objects */
GID_LAST,
/* these need no storage, so defined after GID_LAST */
GID_CMENU_IMPORTASTEXT,
GID_CMENU_EXPORTASTEXT,
GID_CMENU_RESETTODEFAULTS,
};

/*
** configitem structure
*/

struct ConfigItem {
	ULONG GadgetID;
	ULONG ObjectID;
	ULONG Attr;
	IPTR  Default;
};

static struct ConfigItem TAB_CONFIGITEMS[] = {
	{ GID_CHANNELLISTBACKGROUND, OID_SETTINGSCOLOR + PEN_CHANNELLISTBACKGROUND, MUIA_Pendisplay_Spec, (IPTR) "r1D451D45,2B5D2B5D,8D798D79" },
	{ GID_CHANNELLISTTEXT      , OID_SETTINGSCOLOR + PEN_CHANNELLISTTEXT      , MUIA_Pendisplay_Spec, (IPTR) "r00000000,00000000,00000000" },
	{ GID_CHANNELLISTUSER      , OID_SETTINGSCOLOR + PEN_CHANNELLISTUSER      , MUIA_Pendisplay_Spec, (IPTR) "rFFFDFFFD,FFFCFFFC,FFFFFFFF" },
	{ GID_CHANNELLISTSERVER    , OID_SETTINGSCOLOR + PEN_CHANNELLISTSERVER    , MUIA_Pendisplay_Spec, (IPTR) "rDE50DE50,DE4ADE4A,DE47DE47" },
	{ GID_CHANNELLISTHIGHLIGHT , OID_SETTINGSCOLOR + PEN_CHANNELLISTHIGHLIGHT , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,FBCCFBCC,25A225A2" },
	{ GID_USERLISTBACKGROUND   , OID_SETTINGSCOLOR + PEN_USERLISTBACKGROUND   , MUIA_Pendisplay_Spec, (IPTR) "r1D451D45,2B5D2B5D,8D798D79" },
	{ GID_USERLISTTEXT         , OID_SETTINGSCOLOR + PEN_USERLISTTEXT         , MUIA_Pendisplay_Spec, (IPTR) "rDE50DE50,DE4ADE4A,DE47DE47" },
	{ GID_LOGLISTBACKGROUND    , OID_SETTINGSCOLOR + PEN_LOGLISTBACKGROUND    , MUIA_Pendisplay_Spec, (IPTR) "r00000000,00000000,00000000" },
	{ GID_LOGPRIVMSG           , OID_SETTINGSCOLOR + PEN_LOGPRIVMSG           , MUIA_Pendisplay_Spec, (IPTR) "rb7b7b7b7,b7b7b7b7,b7b7b7b7" },
	{ GID_LOGJOIN              , OID_SETTINGSCOLOR + PEN_LOGJOIN              , MUIA_Pendisplay_Spec, (IPTR) "r61E061E0,FFFFFFFF,8CDC8CDC" },
	{ GID_LOGPART              , OID_SETTINGSCOLOR + PEN_LOGPART              , MUIA_Pendisplay_Spec, (IPTR) "r209C209C,A1AFA1AF,1A271A27" },
	{ GID_LOGQUIT              , OID_SETTINGSCOLOR + PEN_LOGQUIT              , MUIA_Pendisplay_Spec, (IPTR) "r209C209C,A1AFA1AF,1A271A27" },
	{ GID_LOGMODE              , OID_SETTINGSCOLOR + PEN_LOGMODE              , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,AEACAEAC,D159D159" },
	{ GID_LOGCTCP              , OID_SETTINGSCOLOR + PEN_LOGCTCP              , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,07830783,0B570B57" },
	{ GID_LOGACTION            , OID_SETTINGSCOLOR + PEN_LOGACTION            , MUIA_Pendisplay_Spec, (IPTR) "r527F527F,FFFFFFFF,2D292D29" },
	{ GID_LOGINFO              , OID_SETTINGSCOLOR + PEN_LOGINFO              , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,1A591A59,25292529" },
	{ GID_LOGOWNTEXT           , OID_SETTINGSCOLOR + PEN_LOGOWNTEXT           , MUIA_Pendisplay_Spec, (IPTR) "r527F527F,FFFFFFFF,2D292D29" },
	{ GID_LOGHIGHLIGHT         , OID_SETTINGSCOLOR + PEN_LOGHIGHLIGHT         , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,FBCCFBCC,25A225A2" },
	{ GID_LOGNOTICE            , OID_SETTINGSCOLOR + PEN_LOGNOTICE            , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,07830783,0B570B57" },
	{ GID_LOGINVITE            , OID_SETTINGSCOLOR + PEN_LOGINVITE            , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,07830783,0B570B57" },
	{ GID_LOGKICK              , OID_SETTINGSCOLOR + PEN_LOGKICK              , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,07830783,0B570B57" },
	{ GID_LOGNICKCHANGE        , OID_SETTINGSCOLOR + PEN_LOGNICKCHANGE        , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,8FC08FC0,12D012D0" },
	{ GID_LOGTOPIC             , OID_SETTINGSCOLOR + PEN_LOGTOPIC             , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,8FC08FC0,12D012D0" },
	{ GID_LOGWALLOPS           , OID_SETTINGSCOLOR + PEN_LOGWALLOPS           , MUIA_Pendisplay_Spec, (IPTR) "rFFFFFFFF,07830783,0B570B57" },
	{ -1,0,0,0 },
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *) DoSuperNew( cl, obj,
				MUIA_Group_Horiz, TRUE,
				MUIA_ContextMenu,
						MenustripObject,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_SETTINGSCOLOR_COLORS_CMENU ),
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_SETTINGSCOLOR_IMPORT_CMENU ), MUIA_UserData, GID_CMENU_IMPORTASTEXT, End,
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_SETTINGSCOLOR_EXPORT_CMENU ), MUIA_UserData, GID_CMENU_EXPORTASTEXT, End,
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_SETTINGSCOLOR_RESETTODEFAULTS_CMENU ), MUIA_UserData, GID_CMENU_RESETTODEFAULTS, End,
							End,
						End,
				Child, HVSpace,
				Child, VGroup,
					Child, HVSpace,
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSCOLOR_LOGLIST_INFO ) ),
					Child, ColGroup(8),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGLISTBACKGROUND_GAD ),
						Child, objs[ GID_LOGLISTBACKGROUND     ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGLISTBACKGROUND_GAD      , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, HVSpace, Child, HVSpace, Child, HVSpace, Child, HVSpace, Child, HVSpace, Child, HVSpace,

						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGPRIVMSG_GAD ),
						Child, objs[ GID_LOGPRIVMSG            ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGPRIVMSG_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGOWNTEXT_GAD ),
						Child, objs[ GID_LOGOWNTEXT            ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGOWNTEXT_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGACTION_GAD ),
						Child, objs[ GID_LOGACTION             ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGACTION_GAD              , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGJOIN_GAD ),
						Child, objs[ GID_LOGJOIN               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGJOIN_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGPART_GAD ),
						Child, objs[ GID_LOGPART               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGPART_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGQUIT_GAD ),
						Child, objs[ GID_LOGQUIT               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGQUIT_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGMODE_GAD ),
						Child, objs[ GID_LOGMODE               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGMODE_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGCTCP_GAD ),
						Child, objs[ GID_LOGCTCP               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGCTCP_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGINFO_GAD ),
						Child, objs[ GID_LOGINFO               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGINFO_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGHIGHLIGHT_GAD ),
						Child, objs[ GID_LOGHIGHLIGHT          ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGHIGHLIGHT_GAD           , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGNOTICE_GAD ),
						Child, objs[ GID_LOGNOTICE             ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGNOTICE_GAD              , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGINVITE_GAD ),
						Child, objs[ GID_LOGINVITE             ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGINVITE_GAD              , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGKICK_GAD ),
						Child, objs[ GID_LOGKICK               ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGKICK_GAD                , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGNICKCHANGE_GAD ),
						Child, objs[ GID_LOGNICKCHANGE         ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGNICKCHANGE_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGTOPIC_GAD ),
						Child, objs[ GID_LOGTOPIC              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGTOPIC_GAD               , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_LOGWALLOPS_GAD ),
						Child, objs[ GID_LOGWALLOPS            ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_LOGWALLOPS_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					End,
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLIST_INFO ) ),
					Child, ColGroup(4),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTBACKGROUND_GAD ),
						Child, objs[ GID_CHANNELLISTBACKGROUND ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTBACKGROUND_GAD  , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, HVSpace, Child, HVSpace,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTTEXT_GAD ),
						Child, objs[ GID_CHANNELLISTTEXT       ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTTEXT_GAD        , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTUSER_GAD ),
						Child, objs[ GID_CHANNELLISTUSER       ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTUSER_GAD        , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTSERVER_GAD ),
						Child, objs[ GID_CHANNELLISTSERVER     ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTSERVER_GAD      ,  MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTHIGHLIGHT_GAD ),
						Child, objs[ GID_CHANNELLISTHIGHLIGHT  ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHANNELLISTHIGHLIGHT_GAD   , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					End,
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSCOLOR_USERLIST_INFO ) ),
					Child, HGroup,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_USERLISTBACKGROUND_GAD ),
						Child, objs[ GID_USERLISTBACKGROUND    ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_USERLISTBACKGROUND_GAD     , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_USERLISTTEXT_GAD ),
						Child, objs[ GID_USERLISTTEXT          ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_USERLISTTEXT_GAD           , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
						Child, HVSpace,
						Child, HVSpace,
					End,
					Child, HVSpace,
				End,
				Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( obj, MM_SETTINGSCOLOR_RESETTODEFAULTS );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// MM_ContextMenuSelect()
*/

/*************************************************************************/

static IPTR MM_ContextMenuSelect( struct IClass *cl, Object *obj, struct  MUIP_ContextMenuChoice *msg )
{
struct FileRequester *filerequester;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->item ) {
		switch( MUIGetVar( msg->item, MUIA_UserData ) ) {
			case GID_CMENU_IMPORTASTEXT:
				if( ( filerequester= (struct FileRequester*) AllocAslRequestTags( ASL_FileRequest,
						ASLFR_Window       , _window(obj),
						ASLFR_PopToFront   , TRUE,
						ASLFR_TitleText    , LGS( MSG_MUICLASS_SETTINGSCOLOR_IMPORT_ASLTITLE ),
						ASLFR_InitialDrawer, DEFAULT_PRESETSCOLORSPATH "/",
						TAG_DONE, NULL ) ) ) {

					SetAttrs( _app(obj), MUIA_Application_Sleep, TRUE, TAG_DONE );
					if( AslRequest( filerequester, NULL ) ) {
						ULONG length; STRPTR path;
						length = strlen( (char*) filerequester->fr_File ) + strlen( (char*) filerequester->fr_Drawer ) + 4;
						if( ( path = AllocVec( length, MEMF_ANY ) ) ) {
							strcpy( (char *) path, (char *) filerequester->fr_Drawer );
							AddPart( path, filerequester->fr_File, length );
							debug("ipath is '%s'\n", path);
							DoMethod( obj, MM_SETTINGSCOLOR_IMPORTASTEXT, path );

							FreeVec( path );
						}
					}
					FreeAslRequest(filerequester);
					SetAttrs( _app(obj), MUIA_Application_Sleep, FALSE, TAG_DONE );
				}
				break;
			case GID_CMENU_EXPORTASTEXT:
				if( ( filerequester= (struct FileRequester*) AllocAslRequestTags( ASL_FileRequest,
						ASLFR_Window       , _window(obj),
						ASLFR_PopToFront   , TRUE,
						ASLFR_TitleText    , LGS( MSG_MUICLASS_SETTINGSCOLOR_EXPORT_ASLTITLE ),
						ASLFR_InitialDrawer, DEFAULT_PRESETSCOLORSPATH "/",
						TAG_DONE, NULL ) ) ) {

					SetAttrs( _app(obj), MUIA_Application_Sleep, TRUE, TAG_DONE );
					if( AslRequest( filerequester, NULL ) ) {
						ULONG length; STRPTR path;
						length = strlen( (char*) filerequester->fr_File ) + strlen( (char*) filerequester->fr_Drawer ) + 4;
						if( ( path = AllocVec( length, MEMF_ANY ) ) ) {
							strcpy( (char *) path, (char *) filerequester->fr_Drawer );
							AddPart( path, filerequester->fr_File, length );
							debug("epath is '%s'\n", path);
							DoMethod( obj, MM_SETTINGSCOLOR_EXPORTASTEXT, path );
							FreeVec( path );
						}
					}
					FreeAslRequest(filerequester);
					SetAttrs( _app(obj), MUIA_Application_Sleep, FALSE, TAG_DONE );
				}
				break;
			case GID_CMENU_RESETTODEFAULTS:
				DoMethod( obj, MM_SETTINGSCOLOR_RESETTODEFAULTS );
				break;
		}
	}
	return( 0 );
}
/* \\\ */

/* Poppen are not supported by dataspace, so we need to adapt */

/* /// OM_Import()
**
*/

/*************************************************************************/

static IPTR OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].Attr == MUIA_Pendisplay_Spec ) {
			MUIDataspaceImportPoppen( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], msg->dataspace, TAB_CONFIGITEMS[ i ].ObjectID );
		}
	}
	return( DoSuperMethodA( cl, obj, (Msg)msg ) );
}
/* \\\ */
/* /// OM_Export()
**
*/

/*************************************************************************/

static IPTR OM_Export( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].Attr == MUIA_Pendisplay_Spec ) {
			MUIDataspaceExportPoppen( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], msg->dataspace, TAB_CONFIGITEMS[ i ].ObjectID );
		}
	}
	return( DoSuperMethodA( cl, obj, (Msg)msg ) );
}
/* \\\ */

/*
** custom methods
*/

/* /// MM_ResetToDefaults()
**
*/

/*************************************************************************/

static IPTR MM_ResetToDefaults( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ],
					TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default,
					MUIA_ObjectID            , TAB_CONFIGITEMS[ i ].ObjectID,
					TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static IPTR MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSCOLOR_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			return( (IPTR) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr ) );
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ExportAsText()
*/

/*************************************************************************/

static IPTR MM_ExportAsText( struct IClass *cl, Object *obj, struct MP_SETTINGSCOLOR_EXPORTASTEXT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
ULONG i;
char *str;

	debug( "%s (%ld) %s() - Class: 0x%08lxx Object: 0x%08lxx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( handle = Open( (_ub_cs) msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
			str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr );
			VFPrintf( handle, (_ub_cs) "%s\n", (RAWARG)&str );
		}
		Close( handle );
    }
	return( 0 );
}
/* \\\ */
/* /// MM_ImportAsText()
*/

#define LINEBUFFER_SIZEOF 0x2000

/*************************************************************************/

static IPTR MM_ImportAsText( struct IClass *cl, Object *obj, struct MP_SETTINGSCOLOR_IMPORTASTEXT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
ULONG i, length;
char *linebuffer;

	debug( "%s (%ld) %s() - Class: 0x%08lxx Object: 0x%08lxx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		if( ( handle = Open( (_ub_cs) msg->Name, MODE_OLDFILE ) ) ) {
			for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
				if( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
					/* strip potential 0x0d and 0x0a from line end */
					while( ( length = strlen( linebuffer ) ) ) {
						debug("length is %ld\n", length );
						if( ( linebuffer[ length - 1 ] == '\n' ) || ( linebuffer[ length - 1 ] == '\r' ) ) {
							linebuffer[ length - 1 ] = '\0';
						} else {
							break;
						}
					}
					debug("line is '%s'\n", linebuffer );
					SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr, linebuffer, TAG_DONE );
				}
			}
			Close( handle );
        }
		FreeVec( linebuffer );
    }
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsColor_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsColor_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                   ( cl, obj, (APTR) msg ) );
		case MUIM_Import                        : return( OM_Import                ( cl, obj, (APTR) msg ) );
		case MUIM_Export                        : return( OM_Export                ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuChoice             : return( MM_ContextMenuSelect     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSCOLOR_RESETTODEFAULTS   : return( MM_ResetToDefaults       ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSCOLOR_READCONFIG        : return( MM_ReadConfig            ( cl, obj, (APTR) msg ) );

		case MM_SETTINGSCOLOR_EXPORTASTEXT      : return( MM_ExportAsText          ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSCOLOR_IMPORTASTEXT      : return( MM_ImportAsText          ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsColor_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsColor_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSCOLOR ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsColor_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSCOLOR ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsColor_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsColor_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSCOLOR ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSCOLOR ] );
		appclasses[ CLASSID_SETTINGSCOLOR ] = NULL;
    }
}
/* \\\ */



