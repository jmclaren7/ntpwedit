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
#include <windows.h>
#include "dialogs.h"
#include "ctl_groupbox.h"

static void GetGroupTextSize(HWND window, int id, SIZE *sz);

#define GRPBOX_EXTRA_LEFT   9
#define GRPBOX_EXTRA_RIGHT  9
#define GRPBOX_EXTRA_BOTTOM 4
#define GRPBOX_EXTRA_TOP    4
static void GroupEstimateSize(
    HWND window, int id,
    SIZE *sz, SIZE *max,
    struct DLG_Control *base
    )
    {
    SIZE gtsz;

    base->Estimate(window, id, sz, max);
    GetGroupTextSize(window, id, &gtsz);
    if(sz->cx<gtsz.cx) sz->cx=gtsz.cx;
    sz->cx+=GRPBOX_EXTRA_LEFT+GRPBOX_EXTRA_RIGHT;
    sz->cy+=gtsz.cy+GRPBOX_EXTRA_BOTTOM;
    if(ITEM_MSZ_FIXED==max->cx)
        max->cx=ITEM_MSZ_FILL;
    if(ITEM_MSZ_FIXED==max->cy)
        max->cy=ITEM_MSZ_FILL;
    }

static void VGroupEstimateSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GroupEstimateSize(window, id, sz, max, &CtlGroupV);
    }

static void HGroupEstimateSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GroupEstimateSize(window, id, sz, max, &CtlGroupH);
    }

static void GroupMove(
    HWND window, int id,
    int x, int y,
    int w, int h,
    struct DLG_Control *base
    )
    {
    SIZE gtsz;

    GetGroupTextSize(window, id, &gtsz);
    DlgMoveItem(window, id, x, y, w, h);
    base->Move(window, id, x+GRPBOX_EXTRA_LEFT, y+gtsz.cy,
        w-GRPBOX_EXTRA_LEFT-GRPBOX_EXTRA_RIGHT, h-gtsz.cy-GRPBOX_EXTRA_BOTTOM);
    }

static void VGroupMove(HWND window, int id, int x, int y, int w, int h)
    {
    GroupMove(window, id, x, y, w, h, &CtlGroupV);
    }

static void HGroupMove(HWND window, int id, int x, int y, int w, int h)
    {
    GroupMove(window, id, x, y, w, h, &CtlGroupH);
    }

static void GetGroupTextSize(HWND window, int id, SIZE *sz)
    {
    HWND item=GetDlgItem(window, id);
    HFONT font;
    HDC dc;

    sz->cx=0;
    sz->cy=0;

    GetDlgItemTextSize(window, id, sz);
    if(sz->cy==0)
        {
        font=(HFONT)SendMessage(item, WM_GETFONT, 0, 0);
        dc=GetDC(item);
        SelectObject(dc, font);
        GetTextExtentPoint32W(dc, L" ", 1, sz);
        ReleaseDC(item, dc);
        sz->cy=sz->cy/2+GRPBOX_EXTRA_TOP;
        }
    }

static void SpacerEstimateSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    (void)window; /* Unused */
    (void)id;     /* Unused */
    sz->cy=sz->cx=(GRPBOX_EXTRA_LEFT+GRPBOX_EXTRA_RIGHT)/2;
    /*sz->cy=(GRPBOX_EXTRA_TOP+GRPBOX_EXTRA_BOTTOM)/2;*/
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

static void SpacerMove(HWND window, int id, int x, int y, int w, int h)
    {
    (void)window; /* Unused */
    (void)id;     /* Unused */
    (void)x;      /* Unused */
    (void)y;      /* Unused */
    (void)w;      /* Unused */
    (void)h;      /* Unused */
    }

struct DLG_Control CtlGroupBoxV=
    {
    L"BUTTON",
    BS_GROUPBOX,
    0,
    FALSE,
    NULL,
    VGroupEstimateSize,
    VGroupMove,
    NULL
    };

struct DLG_Control CtlGroupBoxH=
    {
    L"BUTTON",
    BS_GROUPBOX,
    0,
    FALSE,
    NULL,
    HGroupEstimateSize,
    HGroupMove,
    NULL
    };

struct DLG_Control CtlGroupBoxSpacer=
    {
    L"STATIC",
    SS_OWNERDRAW,
    0,
    FALSE,
    NULL,
    SpacerEstimateSize,
    SpacerMove,
    NULL
    };

