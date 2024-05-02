/* ===================================================================
 * Copyright (c) 2005-2012 Vadim Druzhin (cdslow@mail.ru).
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
#ifndef __MINGW64__
#define _WIN32_IE 0x0300
#endif /* __MINGW64__ */
#include <windows.h>
#include <commctrl.h>
#include "unicode.h"
#include "dialogs.h"
#include "ctl_listview.h"

#define EXTRA_W 4
#define EXTRA_H 4
#define EXTRA_COLUMN 16/*12*/
static void InitColumns(HWND window, int id);
static void ListView_InsertColumnU(HWND item, int iCol, LV_COLUMNW *pcol);

static WCHAR *CopyDlgItemTextU(HWND window, int id)
    {
    int len;
    WCHAR *buf;

    len=GetWindowTextLengthU(GetDlgItem(window, id));
    if(0==len)
        return NULL;
    ++len;
    buf=malloc(len*sizeof(*buf));
    if(NULL==buf)
        return NULL;
    if(0==GetDlgItemTextU(window, id, buf, len))
        {
        free(buf);
        return NULL;
        }

    return buf;
    }

static void Estimate(HWND window, int id, SIZE *sz, SIZE *max)
    {
    DWORD vr;
    NONCLIENTMETRICSW metrics;
    WCHAR *buf;
    WCHAR *s;

    vr=ListView_ApproximateViewRect(GetDlgItem(window, id), -1, -1, 5);
    if(0!=vr)
        {
        sz->cx=LOWORD(vr);
        sz->cy=HIWORD(vr);
        }
    else
        {
        GetDlgItemTextSize(window, id, sz);
        sz->cx+=EXTRA_COLUMN;
        sz->cy*=8;
        buf=CopyDlgItemTextU(window, id);
        if(NULL!=buf)
            {
            for(s=buf; *s!=0; ++s)
                if(L'\t'==*s)
                    sz->cx+=EXTRA_COLUMN;
            free(buf);
            }
        }
    sz->cx+=EXTRA_W;
    sz->cy+=EXTRA_H;
    GetNonClientMetricsU(&metrics);
    sz->cx+=metrics.iScrollWidth;
    max->cx=ITEM_MSZ_MAX;
    max->cy=ITEM_MSZ_MAX;
    }

static void Init(HWND window, int id)
    {
    InitColumns(window, id);
    SendDlgItemMessage(window, id, LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    }

static void InitColumns(HWND window, int id)
    {
    HWND item=GetDlgItem(window, id);
    LV_COLUMNW col;
    WCHAR *buf;
    int i;
    WCHAR *s, *e;
    int end;
    HFONT font;
    HDC dc;
    SIZE sz;

    buf=CopyDlgItemTextU(window, id);
    if(NULL==buf)
        return;

    font=(HFONT)SendMessage(item, WM_GETFONT, 0, 0);
    dc=GetDC(item);
    SelectObject(dc, font);
    
    i=0;
    s=e=buf;
    end=0;
    do
        {
        while(*e!=0 && *e!=L'\t') {++e;}
        if(0==*e)
            end=1;
        else
            *e=0;
        GetTextExtentPoint32W(dc, s, (int)(e-s), &sz);
        col.mask=LVCF_TEXT|LVCF_WIDTH;
        col.pszText=s;
        col.cx=sz.cx+EXTRA_COLUMN;
        ListView_InsertColumnU(item, i, &col);
        ++e;
        s=e;
        ++i;
        }
    while(!end);

    ReleaseDC(item, dc);
    free(buf);
    }

static void ListView_InsertColumnU(HWND item, int iCol, LV_COLUMNW *pcol)
    {
    LV_COLUMNA col;

    if(IsRealUnicode())
        {
        SendMessageW(item, LVM_INSERTCOLUMNW, iCol, (LPARAM)pcol);
        return;
        }

    col.pszText=TextBufWtoA(pcol->pszText);
    if(NULL==col.pszText)
        return;
    col.mask=pcol->mask;
    col.fmt=pcol->fmt;
    col.cx=pcol->cx;
    col.cchTextMax=pcol->cchTextMax;
    col.iSubItem=pcol->iSubItem;
#if 0 /*(_WIN32_IE >= 0x0300)*/
	int iImage;
	int iOrder;
#endif

    SendMessageA(item, LVM_INSERTCOLUMNA, iCol, (LPARAM)&col);

    free(col.pszText);
    }

struct DLG_Control CtlListView=
    {
    WC_LISTVIEWW,
    WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|LVS_NOSORTHEADER|WS_BORDER,
    0,
    TRUE,
    Init,
    Estimate,
    DlgMoveItem,
    NULL
    };

