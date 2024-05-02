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
#include "ctl_label.h"

static void MinSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GetDlgItemTextSize(window, id, sz);
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

static void MinSizeCentered(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GetDlgItemTextSize(window, id, sz);
    max->cx=ITEM_MSZ_FILL;
    max->cy=ITEM_MSZ_FIXED;
    }

struct DLG_Control CtlLabel=
    {
    L"STATIC",
    SS_LEFT|SS_NOTIFY,
    0,
    TRUE,
    NULL,
    MinSize,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlLabelCentered=
    {
    L"STATIC",
    SS_CENTER|SS_NOTIFY,
    0,
    TRUE,
    NULL,
    MinSizeCentered,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlLabelRight=
    {
    L"STATIC",
    SS_RIGHT|SS_NOTIFY,
    0,
    TRUE,
    NULL,
    MinSizeCentered,
    DlgMoveItem,
    NULL
    };

