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
#include <commctrl.h>
#include <stdlib.h>
#include "dialogs.h"
#include "ctl_groupbox.h"
#include "ctl_image.h"
#include "ctl_label.h"
#include "ctl_button.h"
#include "unicode.h"
#include "version.h"
#include "dlgabout.h"

static BOOL URL_Proc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Version_Proc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static void ExecDlgURL(HWND window, int item);

enum
    {
    ID_GRP_ABOUT=101,
    ID_GRP_ICON,
    ID_ICON,
    ID_ICON_SPACER,
    ID_GRP_VERSION,
    ID_LABEL_VERSION,
    ID_LABEL_LICENSE,
    ID_V_SPACER1,
    ID_V_SPACER2,
    ID_LABEL_COPYRIGHT,
    ID_LABEL_MAIL,
    ID_LABEL_URL,
    ID_GRP_CREDITS,
    ID_LABEL_CREDITS,
    ID_GRP_OK
    };

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxH, ID_GRP_ABOUT, NULL, 0, 0, NULL},
    {&CtlGroupH, ID_GRP_ICON, GRP_TITLE_FILL, 0, ID_GRP_ABOUT, NULL},
    {&CtlIcon, ID_ICON, L"IconApp", 0, ID_GRP_ICON, NULL},
    {&CtlGroupBoxSpacer, ID_ICON_SPACER, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlGroupV, ID_GRP_VERSION, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlLabel, ID_LABEL_VERSION, NULL, 0, ID_GRP_VERSION, Version_Proc},
    {&CtlLabel, ID_LABEL_LICENSE, L"FREEWARE", 0, ID_GRP_VERSION, NULL},
    {&CtlLabel, ID_V_SPACER1, L" ", 0, ID_GRP_VERSION, NULL},
    {&CtlLabel, ID_LABEL_COPYRIGHT, L"\xA9 2005,2006 Vadim Druzhin", 0, ID_GRP_VERSION, NULL},
    {&CtlLabel, ID_V_SPACER2, L" ", 0, ID_GRP_VERSION, NULL},
    {&CtlLabel, ID_LABEL_MAIL, L"E-Mail: cdslow@mail.ru", 0, ID_GRP_VERSION, NULL},
    {&CtlLabel, ID_LABEL_URL, L"http://cdslow.org.ru/ntpwedit/", 0, ID_GRP_VERSION, URL_Proc},
    {&CtlGroupBoxH, ID_GRP_CREDITS, NULL, 0, 0, NULL},
    {&CtlLabel, ID_LABEL_CREDITS, L""
        "\nIncluded parts of chntpw source and ntreg source (registry edit library)\n"
        "is Copyright (c) 1997-2004 Petter Nordahl-Hagen, pnordahl@eunet.no\n"
        "\nThis product includes software developed by the OpenSSL Project\n"
        "for use in the OpenSSL Toolkit. (http://www.openssl.org/)\n"
        "\nThis product includes cryptographic software written by\n"
        "Eric Young (eay@cryptsoft.com)\n",
        0, ID_GRP_CREDITS, NULL},
    {&CtlGroupBoxH, ID_GRP_OK, NULL, 0, 0, NULL},
    {&CtlDefButton, IDOK, L"OK", 0, ID_GRP_OK, NULL},
    };




static BOOL URL_Proc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_COMMAND==msg)
        {
        if(HIWORD(wParam)==STN_CLICKED)
            {
            ExecDlgURL(window, id);
            return TRUE;
            }
        }
    else if(WM_CTLCOLORSTATIC==msg)
        {
        SetTextColor((HDC)wParam, RGB(0, 0, 255));
        SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
        return (LRESULT)GetSysColorBrush(COLOR_3DFACE);
        }
    
    return FALSE;
    }

static BOOL Version_Proc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_INITDIALOG==msg)
        {
        SetDlgItemTextU(window, id, AppTitle);
        return TRUE;
        }
    
    return FALSE;
    }

void AboutDialog(HWND window)
    {
    DlgRunU(
        window,
        L"About NTPWEdit",
        0,
        WS_BORDER|WS_CAPTION|WS_SYSMENU,
        0,
        Items,
        sizeof(Items)/sizeof(*Items),
        NULL
        );
    }

static void ExecDlgURL(HWND window, int item)
    {
    char buf[256];
    
    GetDlgItemText(window, item, buf, sizeof(buf));
    ShellExecute(NULL, "open", buf, NULL, NULL, 0);
    }
