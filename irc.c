/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                            IRC Functions
   =================================================================== */

#include "includes.h"

#include "intern.h"

void SWhoIs(char *selnick)
{
    sprintf(sendstuff, "WHOIS %s\r\n", selnick);
    send_text(sendstuff);

}

void SQuery(char *selnick)
{
    if (selnick)
    {
        create_new_tab(selnick, 1, 2);
        DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

    }
}

void ChangeMyNick(char *mynewnick)
{

    sprintf(sendstuff, "NICK %s\r\n", mynewnick);
    //send_current(sendstuff);
    send_text(sendstuff);

}

void SJoin(char *selchannel)
{
    sprintf(sendstuff, "JOIN %s\r\n", selchannel);
    send_text(sendstuff);
}

void SMsg(char *selnick, char *message)
{

    /*#ifndef __AROS__
     struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)message },
     {CSA_SourceCodeset, (ULONG)charsets[local_charset]}, {CSA_DestCodeset, (ULONG)charsets[status_conductor->remote_charset]}, {TAG_DONE, 0} };
     charset_convert_1 = CodesetsConvertStrA(my_send_charset1_taglist);
     if(charset_convert_1)
     {
     sprintf(buffer3,"%s-> *%s* %s",timestamp,selnick, (char*)charset_convert_1);
     sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)selnick, (char*)charset_convert_1);
     CodesetsFreeA(charset_convert_1,NULL);
     }
     else
     {
     sprintf(buffer3,"%s-> *%s* %s",timestamp,selnick, message);
     sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)selnick, (char*)message);
     }
     #else
     sprintf(buffer3,"%s-> *%s* %s",timestamp,selnick, message);
     sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)selnick, (char*)message);
     #endif
     */

    //if the nick doesnt equal "." then lets copy the previous nick you message to a string for a shortcut!
    if (stricmp(selnick, "."))
        strcpy(status_conductor->last_msg_nick, selnick);

    sprintf(buffer3, "%s-> *%s* %s", timestamp, selnick, message);
    sprintf(string_to_send, "PRIVMSG %s :%s\r\n", (char*) selnick, (char*) message);

    send_text(string_to_send);
    add_text_to_conductor_list((char*) buffer3, 10, ACTIVITY);

}

void SNames(char *selchannel)
{
    sprintf(sendstuff, "names %s\r\n", selchannel);

    status_conductor->conductor = status_conductor->root;
    while (stricmp(status_conductor->conductor->name, selchannel))
    {
        status_conductor->conductor = status_conductor->conductor->next;
        if (!status_conductor->conductor)
            break;
    }

    if (status_conductor->conductor)
    {
        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);
        status_conductor->conductor->nicks = 0;

        sprintf(buffer3, "%s** %s %s", timestamp, GCS(catalog, 207, "Updating Names list for channel"),
                selchannel);
        add_text_to_conductor_list((char*) buffer3, 9, ACTIVITY);
    }

    send_text(sendstuff);

}

void SNotice(char *selnick, char *message)
{

    /*
     #ifndef __AROS__
     struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)message },
     {CSA_SourceCodeset, (ULONG)charsets[local_charset]}, {CSA_DestCodeset, (ULONG)charsets[status_conductor->remote_charset]}, {TAG_DONE, 0} };
     charset_convert_1 = CodesetsConvertStrA(my_send_charset1_taglist);
     if(charset_convert_1)
     {
     sprintf(buffer3,"%s-> -%s- %s",timestamp,selnick, (char*)charset_convert_1);
     sprintf(string_to_send,"NOTICE %s :%s\r\n",(char*)selnick, (char*)charset_convert_1);
     CodesetsFreeA(charset_convert_1,NULL);
     }
     else
     {
     sprintf(buffer3,"%s-> -%s- %s",timestamp,selnick, message);
     sprintf(string_to_send,"NOTICE %s :%s\r\n",(char*)selnick, (char*)message);
     }
     #else
     sprintf(buffer3,"%s-> -%s- %s",timestamp,selnick, message);
     sprintf(string_to_send,"NOTICE %s :%s\r\n",(char*)selnick, (char*)message);
     #endif

     send_text(string_to_send);
     add_text_to_conductor_list((char*)buffer3, 12, ACTIVITY);
     */

    if (stricmp(selnick, "."))
        strcpy(status_conductor->last_notice_nick, selnick);

    sprintf(buffer3, "%s-> -%s- %s", timestamp, selnick, message);
    sprintf(string_to_send, "NOTICE %s :%s\r\n", (char*) selnick, (char*) message);

    send_text(string_to_send);
    add_text_to_conductor_list((char*) buffer3, 12, ACTIVITY);

}

void ChangeTopic(char *newtopic)
{
    if (newtopic)
        sprintf(sendstuff, "TOPIC %s :%s\r\n", status_conductor->current_query->name, newtopic);
    else
        sprintf(sendstuff, "TOPIC %s\r\n", status_conductor->current_query->name);

    send_text(sendstuff);

}

void SPart(char *message)
{

    if (message)
        sprintf(sendstuff, "PART %s :%s\r\n", status_current->current_query->name, message);
    else if (my_settings.part_msg[0] != '\0')
        sprintf(sendstuff, "PART %s :%s\r\n", status_current->current_query->name, my_settings.part_msg);
    else
        sprintf(sendstuff, "PART %s\r\n", status_current->current_query->name);

    send_text(sendstuff);
    close_tab();

}

void SQuit(char *message)
{
    if (!stricmp(message, ""))
        strcpy(message, my_settings.quit_msg);
    else
        strcpy(message, "Client exiting");
    sprintf(sendstuff, "QUIT :%s\r\n", message);

    status_conductor->quit_requested = TRUE;

    send_text(sendstuff);
}

void SAction(char *message)
{

    if (message)
    {
        /*
         #ifndef __AROS__
         struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)message },
         {CSA_SourceCodeset, (ULONG)charsets[local_charset]}, {CSA_DestCodeset, (ULONG)charsets[status_conductor->remote_charset]}, {TAG_DONE, 0} };
         charset_convert_1 = CodesetsConvertStrA(my_send_charset1_taglist);
         if(charset_convert_1)
         {
         sprintf(sendstuff,"PRIVMSG %s :\001ACTION %s\001\r\n",status_conductor->current_query->name, (char*)charset_convert_1);
         sprintf(buffer3,"%s* %s %s",timestamp, status_current->nick,(char*)charset_convert_1);

         CodesetsFreeA(charset_convert_1,NULL);
         }
         else
         {
         sprintf(sendstuff,"PRIVMSG %s :\001ACTION %s\001\r\n",status_conductor->current_query->name, message);
         sprintf(buffer3,"%s* %s %s",timestamp, status_current->nick,message);

         }
         #else
         sprintf(sendstuff,"PRIVMSG %s :\001ACTION %s\001\r\n",status_conductor->current_query->name, message);
         sprintf(buffer3,"%s* %s %s",timestamp, status_current->nick,message);
         #endif
         */

        sprintf(sendstuff, "PRIVMSG %s :\001ACTION %s\001\r\n", status_conductor->current_query->name, message);
        sprintf(buffer3, "%s* %s %s", timestamp, status_current->nick, message);

        send_text(sendstuff);
        add_text_to_conductor_list((char*) buffer3, 10, ACTIVITY);
    }
}



void SQuote(char *message)
{
    sprintf(sendstuff, "%s\r\n", message);

    send_text(sendstuff);

}

void SPing(char *string1)
{
    ping_time();

    sprintf(sendstuff, "PRIVMSG %s :\001PING %s\001\r\n", string1, pingtimestamp);
    send_text(sendstuff);

    timestamp_2_string();
    sprintf(buffer3, "%s%sCTCP%s %s PING %s %s", timestamp, GCS(catalog, 217, "["),
            GCS(catalog, 218, "]"), GCS(catalog, 213, "Sending CTCP"),
            GCS(catalog, 205, "to"), string1);

    add_text_to_current_list((char*) buffer3, 6, ACTIVITY);

}
