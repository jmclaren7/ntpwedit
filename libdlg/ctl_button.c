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
#include "ctl_button.h"

#define BUTTON_EXTRA_LEFT   5
#define BUTTON_EXTRA_RIGHT  5
#define BUTTON_EXTRA_TOP    5
#define BUTTON_EXTRA_BOTTOM 5
/* 8: 79x13 (21x13) */
/* 24: 242x58 (62x39) */
static void Estimate(HWND window, int id, SIZE *sz, SIZE *max)
    {
    int w, h;

    GetDlgItemTextSize(window, id, sz);

    h=sz->cy*8/5;
    sz->cy+=BUTTON_EXTRA_TOP+BUTTON_EXTRA_BOTTOM;
    if(h>sz->cy) sz->cy=h;

    w=sz->cy*8/2;
    sz->cx+=BUTTON_EXTRA_LEFT+BUTTON_EXTRA_RIGHT;
    if(w>sz->cx) sz->cx=w;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

static void EstimateSmall(HWND window, int id, SIZE *sz, SIZE *max)
    {
    int w, h;

    GetDlgItemTextSize(window, id, sz);

    h=sz->cy*8/5;
    sz->cy+=BUTTON_EXTRA_TOP+BUTTON_EXTRA_BOTTOM;
    if(h>sz->cy) sz->cy=h;

    w=h;
    sz->cx+=BUTTON_EXTRA_LEFT+BUTTON_EXTRA_RIGHT;
    if(w>sz->cx) sz->cx=w;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

static void Init(HWND window, int id)
    {
    SetFocus(GetDlgItem(window, id));
    }

struct DLG_Control CtlButton=
    {
    L"BUTTON",
    WS_TABSTOP,
    0,
    TRUE,
    NULL,
    Estimate,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlDefButton=
    {
    L"BUTTON",
    WS_TABSTOP|BS_DEFPUSHBUTTON,
    0,
    TRUE,
    Init,
    Estimate,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlSmallButton=
    {
    L"BUTTON",
    WS_TABSTOP,
    0,
    TRUE,
    NULL,
    EstimateSmall,
    DlgMoveItem,
    NULL
    };

