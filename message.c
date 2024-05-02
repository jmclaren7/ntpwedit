/* ===================================================================
 * Copyright (c) 2005,2006 Vadim Druzhin (cdslow@mail.ru).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *    1. Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the following
 * disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 * 
 *    3. The name of the author may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ===================================================================
 */

#define STRICT
#include <windows.h>
#include "dialogs.h"
#include "ctl_groupbox.h"
#include "ctl_image.h"
#include "ctl_label.h"
#include "ctl_button.h"
#include "version.h"
#include "message.h"

static BOOL Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);

enum
    {
    ID_GRP_MESSAGE=101,
    ID_ICON,
    ID_ICON_SPACER,
    ID_LABEL_MSG,
    ID_GRP_OK,
    ID_BUTTON_SPACER
    };

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxH, ID_GRP_MESSAGE, NULL, 0, 0, NULL},
    {&CtlIcon, ID_ICON, L"IconApp", 0, ID_GRP_MESSAGE, NULL},
    {&CtlGroupBoxSpacer, ID_ICON_SPACER, NULL, 0, ID_GRP_MESSAGE, NULL},
    {&CtlLabel, ID_LABEL_MSG, NULL, 0, ID_GRP_MESSAGE, NULL},
    {&CtlGroupBoxH, ID_GRP_OK, NULL, 0, 0, NULL},
    {&CtlDefButton, IDOK, L"OK", 0, ID_GRP_OK, Button_OK},
    {&CtlGroupBoxSpacer, ID_BUTTON_SPACER, NULL, 0, ID_GRP_OK, NULL},
    {&CtlButton, IDCANCEL, L"Cancel", 0, ID_GRP_OK, Button_OK},
    };

int AppMessageBox(HWND window, WCHAR *msg, UINT flags)
    {
    struct DLG_Item *items;
    int i;
    int ret;

    items=malloc(sizeof(Items));
    if(NULL==items)
        return IDCANCEL;

    memcpy(items, Items, sizeof(Items));
    for(i=0; i<sizeof(Items)/sizeof(*Items); ++i)
        if(ID_LABEL_MSG==items[i].Id)
            {
            items[i].Title=msg;
            break;
            }

    if(MB_OKCANCEL==(flags&MB_TYPEMASK))
        i=sizeof(Items)/sizeof(*Items);
    else
        i=sizeof(Items)/sizeof(*Items)-2;

    MessageBeep(flags&MB_TYPEMASK);
    ret=DlgRunU(
        window,
        AppTitle,
        0,
        WS_BORDER|WS_CAPTION|WS_SYSMENU,
        MB_TASKMODAL==(flags&MB_TASKMODAL) ? WS_EX_APPWINDOW : 0,
        items,
        i,
        NULL
        );

    free(items);

    return ret;
    }

static BOOL Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_COMMAND==msg)
        {
        EndDialog(window, id);
        return TRUE;
        }
    return FALSE;
    }

