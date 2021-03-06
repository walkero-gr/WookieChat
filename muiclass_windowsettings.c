/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowsettings.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>
#include <string.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_settingslist.h"
#include "muiclass_settingsalias.h"
#include "muiclass_settingsbutton.h"
#include "muiclass_settingscolor.h"
#include "muiclass_settingsdcc.h"
#include "muiclass_settingsevent.h"
#include "muiclass_settingsgeneral.h"
#include "muiclass_settingsgui.h"
#include "muiclass_settingslog.h"
#include "muiclass_settingsserver.h"
#include "muiclass_settingssound.h"
#include "muiclass_windowsettings.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_PAGELIST = 0,
GID_PAGEGROUP,
GID_SERVER,
GID_GUI,
GID_LOG,
GID_DCC,
GID_EVENT,
GID_GENERAL,
GID_COLOR,
GID_SOUND,
GID_ALIAS,
GID_BUTTON,
GID_SAVE,
GID_USE,
GID_CANCEL,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	ULONG                  mcc_VisualChange;
};

/* /// GlobalReadConfig()
**
*/

/*************************************************************************/

IPTR GlobalReadConfig( ULONG objectid )
{
Object *settobj;
IPTR result = 0;

	if( ( settobj = (Object *) MUIGetVar( application, MA_APPLICATION_OBJECTWINDOWSETTINGS ) ) ) {
		result = DoMethod( settobj, MM_WINDOWSETTINGS_READCONFIG, objectid );
	}
	return( (IPTR) result );
}
/* \\\ */
/* /// GlobalReadConfigObj()
**
*/

/*************************************************************************/

IPTR GlobalReadConfigObj( Object *obj, ULONG objectid )
{
Object *settobj;
IPTR result = 0;

	if( ( settobj = (Object *) MUIGetVar( _app( obj ), MA_APPLICATION_OBJECTWINDOWSETTINGS ) ) ) {
		result = DoMethod( settobj, MM_WINDOWSETTINGS_READCONFIG, objectid );
	}
	return( (IPTR) result );
}
/* \\\ */

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWSETTINGS_TITLE ),
			MUIA_Window_ID               , MAKE_ID('S','E','T','T'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
				Child, HGroup, 
					Child, objs[ GID_PAGELIST ] = NListviewObject, MUIA_NListview_NList, SettingsListObject, End,
															MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Auto,
															MUIA_FixWidthTxt, "WWWWWWWWWWWWW",
														End,
							Child, objs[ GID_PAGEGROUP   ] = HGroup, MUIA_Group_PageMode, TRUE,
								Child, objs[ GID_SERVER  ] = SettingsServerObject, End,
								Child, objs[ GID_GENERAL ] = SettingsGeneralObject, End,
								Child, objs[ GID_COLOR   ] = SettingsColorObject, End,
								Child, objs[ GID_GUI     ] = SettingsGUIObject, End,
								Child, objs[ GID_LOG     ] = SettingsLogObject, End,
								Child, objs[ GID_DCC     ] = SettingsDCCObject, End,
								Child, objs[ GID_SOUND   ] = SettingsSoundObject, End,
								Child, objs[ GID_ALIAS   ] = SettingsAliasObject, End,
								Child, objs[ GID_BUTTON  ] = SettingsButtonObject, End,
								Child, objs[ GID_EVENT   ] = SettingsEventObject, End,
							End,
				End,
				Child, HGroup,
					Child, objs[ GID_SAVE   ] = MUICreateButton( MSG_MUICLASS_SETTINGS_SAVE_GAD ),
					Child, objs[ GID_USE    ] = MUICreateButton( MSG_MUICLASS_SETTINGS_USE_GAD ),
					Child, objs[ GID_CANCEL ] = MUICreateButton( MSG_MUICLASS_SETTINGS_CANCEL_GAD ),
				End,
			End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		for( i = 0 ; i < SETTINGS_LAST ; i++ ) {
			DoMethod( objs[ GID_PAGELIST ], MUIM_NList_InsertSingle, LGS( MSG_PG_SERVER + i ), MUIV_NList_Insert_Bottom );
		}

		/* create drawers */
		{ BPTR lock;
		if( ( lock = CreateDir( (_s_cs) DEFAULT_SETTINGSPATH ) ) ) {
			UnLock( lock );
		}
		if( ( lock = CreateDir( (_s_cs) DEFAULT_PRESETSPATH ) ) ) {
			UnLock( lock );
		}
		if( ( lock = CreateDir( (_s_cs) DEFAULT_PRESETSSOUNDSPATH ) ) ) {
			UnLock( lock );
		}
		if( ( lock = CreateDir( (_s_cs) DEFAULT_PRESETSCOLORSPATH ) ) ) {
			UnLock( lock );
		}}

		DoMethod( obj                                      , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , obj                    , 3, MUIM_Set, MUIA_Window_Open     , FALSE );
		DoMethod( obj                                      , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , obj                    , 2, MUIM_Application_Load, MUIV_Application_Load_ENV    );
		DoMethod( mccdata->mcc_ClassObjects[ GID_PAGELIST ], MUIM_Notify, MUIA_NList_Active       , MUIV_EveryTime, mccdata->mcc_ClassObjects[ GID_PAGEGROUP ], 3, MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue );

		DoMethod( mccdata->mcc_ClassObjects[ GID_SAVE     ], MUIM_Notify, MUIA_Pressed            , FALSE         , obj                    , 1, MM_WINDOWSETTINGS_SAVE   );
		DoMethod( mccdata->mcc_ClassObjects[ GID_USE      ], MUIM_Notify, MUIA_Pressed            , FALSE         , obj                    , 1, MM_WINDOWSETTINGS_USE    );
		DoMethod( mccdata->mcc_ClassObjects[ GID_CANCEL   ], MUIM_Notify, MUIA_Pressed            , FALSE         , obj                    , 1, MM_WINDOWSETTINGS_CANCEL );

		SetAttrs( objs[ GID_PAGELIST ], MUIA_NList_Active, SETTINGS_SERVER, TAG_DONE );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
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
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_WINDOWSETTINGS_VISUALCHANGE:
				mccdata->mcc_VisualChange = tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get( struct IClass *cl, Object *obj, struct opGet *msg )
{
struct mccdata *mccdata = INST_DATA(cl,obj);

	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWSETTINGS ; return( TRUE );
		case MA_WINDOWSETTINGS_VISUALCHANGE: *msg->opg_Storage = mccdata->mcc_VisualChange   ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static IPTR MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_WINDOWSETTINGS_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
IPTR result;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_SERVER ], MM_SETTINGSSERVER_READCONFIG, msg->ObjectID ) ) ) {
		if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_GUI ], MM_SETTINGSGUI_READCONFIG, msg->ObjectID ) ) ) {
			if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_GENERAL ], MM_SETTINGSGENERAL_READCONFIG, msg->ObjectID ) ) ) {
				if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_COLOR ], MM_SETTINGSCOLOR_READCONFIG, msg->ObjectID ) ) ) {
					if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_SOUND ], MM_SETTINGSSOUND_READCONFIG, msg->ObjectID ) ) ) {
						if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_DCC ], MM_SETTINGSDCC_READCONFIG, msg->ObjectID ) ) ) {
							if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_LOG ], MM_SETTINGSLOG_READCONFIG, msg->ObjectID ) ) ) {
								if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_BUTTON ], MM_SETTINGSBUTTON_READCONFIG, msg->ObjectID ) ) ) {
									if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_ALIAS ], MM_SETTINGSALIAS_READCONFIG, msg->ObjectID ) ) ) {
										if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_EVENT ], MM_SETTINGSEVENT_READCONFIG, msg->ObjectID ) ) ) {
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return( (IPTR) result );
}
/* \\\ */
/* /// MM_WriteConfig()
**
*/

/*************************************************************************/

static IPTR MM_WriteConfig( struct IClass *cl, Object *obj, struct MP_WINDOWSETTINGS_WRITECONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* right now only the wizard is allowed to change settings beside preferences */
	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVER ], MM_SETTINGSSERVER_WRITECONFIG, msg->ObjectID, msg->Data );

	return( 0 );
}
/* \\\ */

/* /// MM_Save()
**
*/

/*************************************************************************/

static IPTR MM_Save( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	SetAttrs( obj, MUIA_Window_Open, FALSE, TAG_DONE );
	DoMethod( _app(obj), MUIM_Application_Save, MUIV_Application_Save_ENVARC );
	DoMethod( _app(obj), MUIM_Application_Save, MUIV_Application_Save_ENV    );
	DoMethod( _app(obj), MM_APPLICATION_VISUALCHANGE );

	return( 0 );
}
/* \\\ */
/* /// MM_Use()
**
*/

/*************************************************************************/

static IPTR MM_Use( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	SetAttrs( obj, MUIA_Window_Open, FALSE, TAG_DONE );
	DoMethod( _app(obj), MUIM_Application_Save, MUIV_Application_Save_ENV    );
	DoMethod( _app(obj), MM_APPLICATION_VISUALCHANGE );

	return( 0 );
}
/* \\\ */
/* /// MM_Cancel()
**
*/

/*************************************************************************/

static IPTR MM_Cancel( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	SetAttrs( obj, MUIA_Window_Open, FALSE, TAG_DONE );
	DoMethod( _app(obj), MUIM_Application_Load, MUIV_Application_Save_ENV    );
	DoMethod( _app(obj), MM_APPLICATION_VISUALCHANGE );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowSettings_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowSettings_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New         ( cl, obj, (APTR) msg ) );
		case OM_SET                             : return( OM_Set         ( cl, obj, (APTR) msg ) );
		case OM_GET                             : return( OM_Get         ( cl, obj, (APTR) msg ) );

		case MM_WINDOWSETTINGS_SAVE             : return( MM_Save        ( cl, obj, (APTR) msg ) );
		case MM_WINDOWSETTINGS_USE              : return( MM_Use         ( cl, obj, (APTR) msg ) );
		case MM_WINDOWSETTINGS_CANCEL           : return( MM_Cancel      ( cl, obj, (APTR) msg ) );

		case MM_WINDOWSETTINGS_READCONFIG       : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );
		case MM_WINDOWSETTINGS_WRITECONFIG      : return( MM_WriteConfig ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowSettings_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowSettings_InitClass( void )
{
	appclasses[ CLASSID_WINDOWSETTINGS ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowSettings_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWSETTINGS ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowSettings_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowSettings_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWSETTINGS ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWSETTINGS ] );
		appclasses[ CLASSID_WINDOWSETTINGS ] = NULL;
    }
}
/* \\\ */



