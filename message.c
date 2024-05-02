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
#include <windows.h>
#include <stdarg.h>
#include "dialogs.h"
#include "ctl_groupbox.h"
#include "ctl_image.h"
#include "ctl_label.h"
#include "ctl_button.h"
#include "version.h"
#include "message.h"

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

static struct DLG_Item Items_Head[]=
    {
    {&CtlGroupBoxH, ID_GRP_MESSAGE, NULL, 0, 0, NULL},
    {&CtlIcon, ID_ICON, L"IconApp", 0, ID_GRP_MESSAGE, NULL},
    {&CtlGroupBoxSpacer, ID_ICON_SPACER, NULL, 0, ID_GRP_MESSAGE, NULL},
    };

static struct DLG_Item Item_Label=
    {&CtlLabel, ID_LABEL_MSG, NULL, 0, ID_GRP_MESSAGE, NULL};
    
static struct DLG_Item Items_Ok[]=
    {
    {&CtlGroupBoxH, ID_GRP_OK, NULL, 0, 0, NULL},
    {&CtlDefButton, IDOK, L"OK", 0, ID_GRP_OK, Button_OK},
    {&CtlGroupBoxSpacer, ID_BUTTON_SPACER, NULL, 0, ID_GRP_OK, NULL},
    {&CtlButton, IDCANCEL, L"Cancel", 0, ID_GRP_OK, Button_OK},
    };

static struct DLG_Item Items_Yes[]=
    {
    {&CtlGroupBoxH, ID_GRP_OK, NULL, 0, 0, NULL},
    {&CtlDefButton, IDYES, L"Yes", 0, ID_GRP_OK, Button_OK},
    {&CtlGroupBoxSpacer, ID_BUTTON_SPACER, NULL, 0, ID_GRP_OK, NULL},
    {&CtlButton, IDNO, L"No", 0, ID_GRP_OK, Button_OK},
    };

static struct DLG_Item *AssembleItems(int *count, ...)
    {
    va_list va;
    int item_count;
    struct DLG_Item *items;
    struct DLG_Item *assembled;
    int i;

    va_start(va, count);
    *count=0;
    for(;;)
        {
        item_count=va_arg(va, int);
        if(0==item_count)
            break;
        items=va_arg(va, struct DLG_Item *);
        if(NULL==items)
            break;
        *count+=item_count;
        }
    va_end(va);

    if(0==*count)
        return NULL;

    assembled=malloc(sizeof(*assembled) * *count);
    if(NULL==assembled)
        return NULL;

    va_start(va, count);
    for(i=0; i<*count; i+=item_count)
        {
        item_count=va_arg(va, int);
        items=va_arg(va, struct DLG_Item *);
        memcpy(assembled+i, items, sizeof(*items)*item_count);
        }
    va_end(va);

    return assembled;
    }

INT_PTR AppMessageBox(HWND window, WCHAR *msg, UINT flags)
    {
    struct DLG_Item label;
    struct DLG_Item *items;
    int count;
    INT_PTR ret;

    label=Item_Label;
    label.Title=msg;

    if(MB_YESNO==(flags&MB_TYPEMASK))
        items=AssembleItems(&count,
            sizeof(Items_Head)/sizeof(*Items_Head), Items_Head,
            1, &label,
            sizeof(Items_Yes)/sizeof(*Items_Yes), Items_Yes,
            0);
    else
        items=AssembleItems(&count,
            sizeof(Items_Head)/sizeof(*Items_Head), Items_Head,
            1, &label,
            sizeof(Items_Ok)/sizeof(*Items_Ok), Items_Ok,
            0);

    if(NULL==items)
        return IDCANCEL;

    if(MB_YESNO!=(flags&MB_TYPEMASK) && MB_OKCANCEL!=(flags&MB_TYPEMASK))
        count-=2;

    MessageBeep(flags&MB_TYPEMASK);
    ret=DlgRunU(
        window,
        AppTitle,
        0,
        WS_BORDER|WS_CAPTION|WS_SYSMENU,
        MB_TASKMODAL==(flags&MB_TASKMODAL) ? WS_EX_APPWINDOW : 0,
        items,
        count,
        NULL
        );

    free(items);

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

