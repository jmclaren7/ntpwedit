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
#include "ctl_image.h"

static void IconMinSize(HWND window, int id, SIZE *sz, SIZE *max)
    {
    RECT r;

    GetWindowRect(GetDlgItem(window, id), &r);
    sz->cx=r.right-r.left;
    sz->cy=r.bottom-r.top;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

static void IconSize16(HWND window, int id, SIZE *sz, SIZE *max)
    {
    (void)window; /* Unused */
    (void)id;     /* Unused */
    sz->cx=16;
    sz->cy=16;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    }

struct DLG_Control CtlIcon=
    {
    L"STATIC",
    SS_ICON,
    0,
    TRUE,
    NULL,
    IconMinSize,
    DlgMoveItem,
    NULL
    };

struct DLG_Control CtlIcon16=
    {
    L"STATIC",
    SS_ICON,
    0,
    TRUE,
    NULL,
    IconSize16,
    DlgMoveItem,
    NULL
    };

