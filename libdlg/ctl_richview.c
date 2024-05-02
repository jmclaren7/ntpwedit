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
#include <richedit.h>
#include "unicode.h"
#include "dialogs.h"
#include "ctl_richview.h"

#define EXTRA_W 34
#define EXTRA_H 22
#define LIMIT_W 512
#define LIMIT_H (LIMIT_W/4*3)

static void Estimate(HWND window, int id, SIZE *sz, SIZE *max)
    {
    NONCLIENTMETRICSW metrics;

    GetNonClientMetricsU(&metrics);
    GetDlgItemTextSize(window, id, sz);
    sz->cx+=EXTRA_W;
    sz->cy+=EXTRA_H;
    if(sz->cx>LIMIT_W)
        {
        sz->cx=LIMIT_W;
        sz->cy+=metrics.iScrollHeight;
        }
    if(sz->cy>LIMIT_H)
        {
        sz->cy=LIMIT_H;
        sz->cx+=metrics.iScrollWidth;
        }
    max->cx=ITEM_MSZ_MAX;
    max->cy=ITEM_MSZ_MAX;
    }

static void PreInit(void)
    {
    static HINSTANCE dll=NULL;

    if(!dll)
        dll=LoadLibrary("RICHED32.DLL");
    }

static void Init(HWND window, int id)
    {
    HWND item;
    HFONT font;
    HDC dc;
    TEXTMETRIC tm;
    CHARFORMAT fmt;

    item=GetDlgItem(window, id);
    font=(HFONT)SendMessage(window, WM_GETFONT, 0, 0);
    dc=GetDC(item);
    SelectObject(dc, font);
    GetTextMetrics(dc, &tm);
    ReleaseDC(item, dc);

    fmt.cbSize=sizeof(fmt);
    SendMessage(item, EM_GETCHARFORMAT, 0, (LPARAM)&fmt);
    fmt.bCharSet=tm.tmCharSet;
    SendMessage(item, EM_SETCHARFORMAT, 0, (LPARAM)&fmt);
    }

struct DLG_Control CtlRichView=
    {
    L"RichEdit",
    WS_TABSTOP|WS_HSCROLL|WS_VSCROLL|
        ES_MULTILINE|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_READONLY|
        WS_BORDER,
    0,
    TRUE,
    Init,
    Estimate,
    DlgMoveItem,
    PreInit
    };

