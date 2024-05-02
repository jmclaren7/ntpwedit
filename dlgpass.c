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
#include <stdlib.h>
#include "dialogs.h"
#include "ctl_label.h"
#include "ctl_button.h"
#include "ctl_groupbox.h"
#include "ctl_edit.h"
#include "version.h"
#include "dlgpass.h"

struct Pass_Param
    {
    char *pass;
    int pass_size;
    };

enum
    {
    ID_GRP_PASSWORD=101,
    ID_GRP_LABELS,
    ID_LABEL1,
    ID_LABEL2,
    ID_SPACER,
    ID_GRP_EDIT,
    ID_EDIT1,
    ID_EDIT2,
    ID_GRP_BUTTON,
    ID_SPACER1
    };

static BOOL Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Edit1(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxH, ID_GRP_PASSWORD, NULL, 0, 0, NULL},
    {&CtlGroupV, ID_GRP_LABELS, GRP_TITLE_FILL_CY, 0, ID_GRP_PASSWORD, NULL},
    {&CtlLabel, ID_LABEL1, L"New password:", 0, ID_GRP_LABELS, NULL},
    {&CtlLabel, ID_LABEL2, L"Verify:", 0, ID_GRP_LABELS, NULL},
    {&CtlGroupBoxSpacer, ID_SPACER, NULL, 0, ID_GRP_PASSWORD, NULL},
    {&CtlGroupV, ID_GRP_EDIT, NULL, 0, ID_GRP_PASSWORD, NULL},
    {&CtlEdit, ID_EDIT1, NULL, 0, ID_GRP_EDIT, Edit1},
    {&CtlEdit, ID_EDIT2, NULL, 0, ID_GRP_EDIT, Edit1},
    {&CtlGroupBoxH, ID_GRP_BUTTON, NULL, 0, 0, NULL},
    {&CtlDefButton, IDOK, L"OK", 0, ID_GRP_BUTTON, Button_OK},
    {&CtlGroupBoxSpacer, ID_SPACER1, NULL, 0, ID_GRP_BUTTON, NULL},
    {&CtlButton, IDCANCEL, L"Cancel", 0, ID_GRP_BUTTON, NULL},
    };

BOOL QueryPassword(HWND window, char *pass, int pass_size)
    {
    struct Pass_Param p;

    p.pass=pass;
    p.pass_size=pass_size;
    return DlgRunU(
        window,
        AppTitle,
        0,
        WS_BORDER|WS_CAPTION,
        0,
        Items,
        sizeof(Items)/sizeof(*Items),
        &p
        );
    }

static char *GetPassword(HWND window, int id)
    {
    int sz;
    char *pass;

    sz=GetWindowTextLength(GetDlgItem(window, id))+1;
    pass=malloc(sz);
    if(NULL==pass)
        return NULL;
    GetDlgItemText(window, id, pass, sz);

    return pass;
    }

static BOOL Get2Pass(HWND window, char **pass1, char **pass2)
    {
    *pass1=GetPassword(window, ID_EDIT1);
    if(NULL==*pass1)
        return FALSE;
    *pass2=GetPassword(window, ID_EDIT2);
    if(NULL==*pass2)
        {
        free(*pass1);
        return FALSE;
        }
    return TRUE;
    }

static BOOL Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    struct DLG_Data *data;
    struct Pass_Param *p;

    if(WM_INITDIALOG==msg)
        {
        data=(struct DLG_Data *)lParam;
        p=data->param;
        SetFocus(GetDlgItem(window, ID_EDIT1));
        return TRUE;
        }
    if(WM_COMMAND==msg)
        {
        char *pass1;
        char *pass2;

        data=(struct DLG_Data *)GetWindowLong(window, GWL_USERDATA);
        p=data->param;
        if(!Get2Pass(window, &pass1, &pass2))
            return TRUE;
        if(0==strcmp(pass1, pass2))
            {
            strncpy(p->pass, pass1, p->pass_size);
            EndDialog(window, TRUE);
            }
        else
            MessageBeep(MB_OK);
        free(pass2);
        free(pass1);
        return TRUE;
        }
    return FALSE;
    }

static BOOL Edit1(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    struct DLG_Data *data;
    struct Pass_Param *p;

    if(WM_INITDIALOG==msg)
        {
        data=(struct DLG_Data *)lParam;
        p=data->param;
        SendDlgItemMessage(window, id, EM_LIMITTEXT, p->pass_size-1, 0);
        SendDlgItemMessage(window, id, EM_SETPASSWORDCHAR, '*', 0);
        return TRUE;
        }
    if(WM_COMMAND==msg && EN_CHANGE==HIWORD(wParam))
        {
        char *pass1;
        char *pass2;

        data=(struct DLG_Data *)GetWindowLong(window, GWL_USERDATA);
        p=data->param;
        if(!Get2Pass(window, &pass1, &pass2))
            return TRUE;
        EnableWindow(GetDlgItem(window, IDOK), 0==strcmp(pass1, pass2));
        free(pass2);
        free(pass1);
        }
    return FALSE;
    }

