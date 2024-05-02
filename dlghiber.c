/* ===================================================================
 * Copyright (c) 2016 Vadim Druzhin (cdslow@mail.ru).
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
#include <windows.h>
#include <stdarg.h>
#include "dialogs.h"
#include "ctl_groupbox.h"
#include "ctl_image.h"
#include "ctl_label.h"
#include "ctl_button.h"
#include "version.h"
#include "dlghiber.h"

static INT_PTR Button_OK(
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
    {&CtlLabel, ID_LABEL_MSG, L""
        "WARNING!\n\n"
        "Windows System on selected drive seems to be HIBERNATED.\n"
        "Modifying data on this drive could be dangerous.\n"
        "Windows System may not boot properly after modification,\n"
        "all changes may be lost or filesytem may become corrupted.\n\n"
        "Please EXIT program and properly REBOOT HOST WINDOWS SYSTEM.\n"
        "Then use 'Restart' to avoid hibernation\n"
        "(don't use 'Shutdown' or 'Power off' commands)."
        "", 0, ID_GRP_MESSAGE, NULL},
    {&CtlGroupBoxH, ID_GRP_OK, NULL, 0, 0, NULL},
    {&CtlDefButton, IDCANCEL, L"Exit", 0, ID_GRP_OK, Button_OK},
    {&CtlGroupBoxSpacer, ID_BUTTON_SPACER, NULL, 0, ID_GRP_OK, NULL},
    {&CtlButton, IDOK, L"Continue", 0, ID_GRP_OK, Button_OK},
    };

INT_PTR HiberWarning(HWND window)
    {
    INT_PTR ret;

    MessageBeep(MB_ICONWARNING);
    ret=DlgRunU(
        window,
        AppTitle,
        0,
        WS_BORDER|WS_CAPTION|WS_SYSMENU,
        WS_EX_APPWINDOW,
        Items,
        sizeof(Items)/sizeof(*Items),
        NULL
        );

    return ret;
    }

static INT_PTR Button_OK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_COMMAND==msg)
        {
        EndDialog(window, id);
        return TRUE;
        }
    return FALSE;
    }

