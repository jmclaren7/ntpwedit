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
#include "ctl_checkbox.h"

#define CHKBOX_EXTRA_H 2
#define CHKBOX_EXTRA_W 8
static void ChkBoxMinSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    HWND item=GetDlgItem(window, id);
    HDC dc;
    HFONT font;
    TEXTMETRIC metric;
    int dpi;

    GetDlgItemTextSize(window, id, sz);

    font=(HFONT)SendMessage(item, WM_GETFONT, 0, 0);
    dc=GetDC(item);
    SelectObject(dc, font);
    GetTextMetrics(dc, &metric);
    dpi=GetDeviceCaps(dc, LOGPIXELSX);
    if(dpi<96)
        dpi=96;
    ReleaseDC(item, dc);
    sz->cx+=CHKBOX_EXTRA_W+dpi/8+metric.tmMaxCharWidth/8;

    if(sz->cy<dpi/8+CHKBOX_EXTRA_H)
        sz->cy=dpi/8+CHKBOX_EXTRA_H;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

struct DLG_Control CtlCheckBox=
    {
    L"BUTTON",
    WS_TABSTOP|BS_AUTOCHECKBOX|BS_MULTILINE,
    0,
    TRUE,
    NULL,
    ChkBoxMinSize,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlRadioBox=
    {
    L"BUTTON",
    WS_TABSTOP|BS_AUTORADIOBUTTON|BS_MULTILINE,
    0,
    TRUE,
    NULL,
    ChkBoxMinSize,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlRadioBoxFirst=
    {
    L"BUTTON",
    WS_GROUP|WS_TABSTOP|BS_AUTORADIOBUTTON|BS_MULTILINE,
    0,
    TRUE,
    NULL,
    ChkBoxMinSize,
    DlgMoveItem,
    NULL
    };

