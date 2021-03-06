/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_H
#define MUICLASS_H 1

/*************************************************************************/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <utility/utility.h>
#include <stdarg.h>

/*************************************************************************/

/*
** Class ID Enumeration
*/

enum
{
CLASSID_APPLICATION = 0,
CLASSID_ALIASLIST,
CLASSID_AUDIO,
CLASSID_BUTTONLIST,
CLASSID_CHANNELLIST,
CLASSID_CHATCHANNELLIST,
CLASSID_CHATLOG,
CLASSID_CHATUSERLIST,
CLASSID_EVENTLIST,
CLASSID_IGNORELIST,
CLASSID_MESSAGEINPUT,
CLASSID_NETWORK,
CLASSID_NICKLIST,
CLASSID_SETTINGSLIST,
CLASSID_SERVERLIST,
CLASSID_SETTINGSALIAS,
CLASSID_SETTINGSBUTTON,
CLASSID_SETTINGSCOLOR,
CLASSID_SETTINGSDCC,
CLASSID_SETTINGSEVENT,
CLASSID_SETTINGSGENERAL,
CLASSID_SETTINGSGUI,
CLASSID_SETTINGSLOG,
CLASSID_SETTINGSSERVER,
CLASSID_SETTINGSSOUND,
CLASSID_URLLIST,
CLASSID_WINDOWABOUT,
CLASSID_WINDOWCOLORSETTINGS,
CLASSID_WINDOWIGNORELIST,
CLASSID_WINDOWCHAT,
CLASSID_WINDOWQUIT,
CLASSID_WINDOWQUICKSETUP,
CLASSID_WINDOWSETTINGS,
CLASSID_WINDOWURLGRABBER,
CLASSID_LAST
};

extern struct MUI_CustomClass *appclasses[];
extern Object *application;

/*
** Prototypes
*/

ULONG MUIClass_Open ( void );
void  MUIClass_Close( void );

ULONG MUIPenSpecToRGB( Object *obj, struct MUI_PenSpec *penspec );

IPTR MUIGetVar( Object *obj, ULONG attr );
void  MUIInitStringArray( STRPTR array[], ULONG first, ULONG last );
char  MUIGetKeyLocale( ULONG text );
char  MUIGetKeyLocaleUpper( ULONG text );

TEXT  MUIGetUnderScore( ULONG text);
APTR  MUICreatePoppen( ULONG text, ULONG poptitle );
APTR  MUICreateButton( ULONG text );
APTR  MUICreateSmallButton( ULONG text );
APTR  MUICreateLabel( ULONG text );
APTR  MUICreateLabelLeft( ULONG text );
APTR  MUICreateCheckbox( ULONG text, ULONG defstate );
APTR  MUICreateCycle( ULONG text, APTR labels, ULONG first, ULONG last );
APTR  MUICreateSlider( ULONG text, ULONG def, ULONG min, ULONG max, ULONG format, ULONG fix );
APTR  MUICreateInteger( ULONG text, ULONG maxchars );
APTR  MUICreateString( ULONG text, ULONG maxchars );
APTR  MUICreateStringFixed( ULONG text, ULONG maxchars );
APTR  MUICreatePopASL ( ULONG text, ULONG maxchars, ULONG poptype, Object **strobj, struct TagItem *taglist );

struct MUI_PenSpec *MUIDataspaceImportPoppen( Object *poppen, Object *dataspace, ULONG objectid );
void                MUIDataspaceExportPoppen( Object *poppen, Object *dataspace, ULONG objectid );

/*************************************************************************/

/*
** mui list parsing
*/

#define FORCHILD(_o, _a) \
	{ \
		APTR child, _cstate = (APTR)((struct MinList *) MUIGetVar(_o, _a))->mlh_Head; \
		while ((child = NextObject(&_cstate)))

#define NEXTCHILD }

/*
** compatibly stuff
*/

#ifdef __AROS__
IPTR NewObjectAROS( struct IClass *classPtr, UBYTE *classID, Tag tag1, ... );
IPTR DoSuperNew(struct IClass *cl, Object *obj, Tag tag1, ...);
#define NEWOBJECT   (Object *)NewObjectAROS
#else
#define NEWOBJECT   NewObject
#endif


/*************************************************************************/

#endif /* MUICLASS_H */

