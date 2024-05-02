/* ===================================================================
 * Copyright (c) 2005-2012 Vadim Druzhin (cdslow@mail.ru).
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 * ===================================================================
 */

#define STRICT
#define _CRT_SECURE_NO_WARNINGS
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
    ID_SPACER1,
    ID_GRP_PROMPT,
    ID_LABEL_PROMPT
    };

static INT_PTR Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Edit1(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxV, ID_GRP_PROMPT, NULL, 0, 0, NULL},
    {&CtlLabel, ID_LABEL_PROMPT, L""
        L"Keep fields empty and press OK to reset password,\n"
        L"or enter new password into both fields to change it.",
        0, ID_GRP_PROMPT, NULL},
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

INT_PTR QueryPassword(HWND window, char *pass, int pass_size)
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

static INT_PTR Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    struct DLG_Data *data;
    struct Pass_Param *p;

    (void)id; /* Unused */
    (void)wParam; /* Unused */

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

        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
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

static INT_PTR Edit1(
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

        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
        p=data->param;
        if(!Get2Pass(window, &pass1, &pass2))
            return TRUE;
        EnableWindow(GetDlgItem(window, IDOK), 0==strcmp(pass1, pass2));
        free(pass2);
        free(pass1);
        }
    return FALSE;
    }

