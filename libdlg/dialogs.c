/* ===================================================================
 * Copyright (c) 2005-2014 Vadim Druzhin (cdslow@mail.ru).
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
#include <stdlib.h>
#include "unicode.h"
#include "dialogs.h"

static struct DLG_Data *SaveDialogData(
    struct DLG_Item *items,
    int itemcount,
    void *param
    );
static INT_PTR CALLBACK ModalProc(
    HWND window,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
    );
static void DlgGetBorderSize(HWND window, SIZE *sz);
static void DlgResizeWindow(HWND window, int cx, int cy);
static void DlgPlaceWindow(HWND window);
static void GetMonitorRect(POINT p, RECT *r);

INT_PTR DlgRunU(
    HWND window,
    WCHAR const *strtitle,
    int rctitle,
    DWORD style,
    DWORD exstyle,
    struct DLG_Item *items,
    int itemcount,
    void *param
    )
    {
    void *buf;
    struct DLG_Data *data;
    LOGFONTW *fnt;
    NONCLIENTMETRICSW ncmw;
    INT_PTR ret;

    if(GetNonClientMetricsU(&ncmw))
        fnt=&ncmw.lfMessageFont;
    else
        fnt=NULL;

    buf=PackDialogU(
        style|DS_CENTERMOUSE, exstyle,
        0, 0, 100, 100,
        strtitle, rctitle,
        fnt,
        itemcount, items
        );
    if(NULL==buf)
        return 0;
    data=SaveDialogData(items, itemcount, param);
    if(NULL==data)
        {
        free(buf);
        return 0;
        }

    ret=DialogBoxIndirectParamU(
        GetModuleHandle(NULL),
        (void *)AdjustToDWORD((size_t)buf),
        window,
        ModalProc,
        (LPARAM)data
        );

    free(buf);

    return ret;
    }

HWND DlgShowU(
    HWND window,
    WCHAR const *strtitle,
    int rctitle,
    DWORD style,
    DWORD exstyle,
    struct DLG_Item *items,
    int itemcount,
    void *param
    )
    {
    void *buf;
    struct DLG_Data *data;
    LOGFONTW *fnt;
    NONCLIENTMETRICSW ncmw;
    HWND dlg;

    if(GetNonClientMetricsU(&ncmw))
        fnt=&ncmw.lfMessageFont;
    else
        fnt=NULL;

    buf=PackDialogU(
        style|DS_CENTERMOUSE, exstyle,
        0, 0, 100, 100,
        strtitle, rctitle,
        fnt,
        itemcount, items
        );
    if(NULL==buf)
        return NULL;
    data=SaveDialogData(items, itemcount, param);
    if(NULL==data)
        {
        free(buf);
        return NULL;
        }

    dlg=CreateDialogIndirectParamU(
        GetModuleHandle(NULL),
        (void *)AdjustToDWORD((size_t)buf),
        window,
        ModalProc,
        (LPARAM)data
        );

    free(buf);

    return dlg;
    }

static struct DLG_Data *SaveDialogData(
    struct DLG_Item *items,
    int itemcount,
    void *param
    )
    {
    struct DLG_Data *data;
    int i;

    data=malloc(sizeof(*data)+sizeof(*data->Item)*itemcount);
    if(NULL==data)
        return NULL;

    data->param=param;
    data->Item=(struct DLG_ItemData *)(data+1);
    data->count=itemcount;
    data->MinTrackSize.x=0;
    data->MinTrackSize.y=0;
    for(i=0; i<itemcount; ++i)
        {
        data->Item[i].Id=items[i].Id;
        data->Item[i].GroupId=items[i].GroupId;
        data->Item[i].Control=items[i].Control;
        data->Item[i].ItemProc=items[i].ItemProc;
        data->Item[i].SizeKnown=0;
        data->Item[i].ESZ.cx=0;
        data->Item[i].ESZ.cy=0;
        data->Item[i].MSZ.cx=0;
        data->Item[i].MSZ.cy=0;
        }

    return data;
    }

static INT_PTR CALLBACK ModalProc(
    HWND window,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
    )
    {
    WORD id;
    struct DLG_Data *data;
    int i;

    if(WM_INITDIALOG==msg)
        {
        SIZE sz, max, bsz;

        SetWindowLongPtr(window, GWLP_USERDATA, lParam);
        data=(struct DLG_Data *)lParam;
        for(i=0; i<data->count; ++i)
            {
            if(NULL!=data->Item[i].Control->Init)
                data->Item[i].Control->Init(window, data->Item[i].Id);
            if(NULL!=data->Item[i].ItemProc)
                data->Item[i].ItemProc(window, data->Item[i].Id,
                    msg, wParam, lParam);
            }

        CtlGroupV.Estimate(window, 0, &sz, &max);
        DlgGetBorderSize(window, &bsz);
        sz.cx+=bsz.cx;
        sz.cy+=bsz.cy;
        data->MinTrackSize.x=sz.cx;
        data->MinTrackSize.y=sz.cy;
        DlgResizeWindow(window, sz.cx, sz.cy);
        DlgPlaceWindow(window);
        return FALSE;
        }
    else if(WM_SIZE==msg)
        {
        RECT r;
        HRGN rgndlg, rgn;

        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);

        GetClientRect(window, &r);
        CtlGroupV.Move(window, 0, 0, 0, r.right, r.bottom);

        rgndlg=CreateRectRgnIndirect(&r);
        for(i=0; i<data->count; ++i)
            {
            RedrawWindow(GetDlgItem(window, data->Item[i].Id), NULL, NULL,
                RDW_INVALIDATE|RDW_ERASE|RDW_FRAME);
            if(data->Item[i].Control->Solid)
                {
                GetWindowRect(GetDlgItem(window, data->Item[i].Id), &r);
                MapWindowPoints(NULL, window, (POINT *)(void *)&r, 2);
                rgn=CreateRectRgnIndirect(&r);
                CombineRgn(rgndlg, rgndlg, rgn, RGN_DIFF);
                DeleteObject(rgn);
                }
            }
        RedrawWindow(window, NULL, rgndlg, RDW_INVALIDATE|RDW_ERASE|RDW_NOCHILDREN);
        DeleteObject(rgndlg);
        return TRUE;
        }
    else if(WM_GETMINMAXINFO==msg)
        {
        MINMAXINFO *mmi=(LPMINMAXINFO)lParam;

        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
        if(data->MinTrackSize.x!=0 && data->MinTrackSize.y!=0)
            {
            mmi->ptMinTrackSize=data->MinTrackSize;
            if(mmi->ptMaxTrackSize.x<data->MinTrackSize.x)
                mmi->ptMaxTrackSize.x=data->MinTrackSize.x;
            if(mmi->ptMaxTrackSize.y<data->MinTrackSize.y)
                mmi->ptMaxTrackSize.y=data->MinTrackSize.y;
            return TRUE;
            }
        return FALSE;
        }
    else if(WM_DESTROY==msg)
        {
        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
        if(NULL!=data)
            {
            free(data);
            SetWindowLongPtr(window, GWLP_USERDATA, 0);
            }
        return TRUE;
        }
    else
        {
        id=LOWORD(wParam);
        if(WM_CTLCOLORSTATIC==msg)
            id=GetDlgCtrlID((HWND)lParam);
        data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
        if(NULL==data)
            return FALSE;
        for(i=0; i<data->count; ++i)
            {
            if(id==data->Item[i].Id && NULL!=data->Item[i].ItemProc)
                return data->Item[i].ItemProc(window, id, msg, wParam, lParam);
            }
        if(WM_COMMAND==msg && (IDOK==id || IDCANCEL==id))
            {
            EndDialog(window, 0);
            return TRUE;
            }
        }

    return FALSE;
    }

static void DlgGetBorderSize(HWND window, SIZE *sz)
    {
    RECT r;
    int clx, cly;

    GetClientRect(window, &r);
    clx=r.right;
    cly=r.bottom;
    GetWindowRect(window, &r);
    sz->cx=r.right-r.left-clx;
    sz->cy=r.bottom-r.top-cly;
    }

static void DlgResizeWindow(HWND window, int cx, int cy)
    {
    RECT r;
    int dx, dy;

    GetWindowRect(window, &r);
    dx=cx-r.right+r.left;
    dy=cy-r.bottom+r.top;
    MoveWindow(window, r.left-dx/2, r.top-dy/2, cx, cy, FALSE);
    }

static void DlgPlaceWindow(HWND window)
    {
    int move_x=0, move_y=0;
    HWND focus;
    RECT r;
    RECT s;
    POINT p;

    focus=GetFocus();
    if(NULL!=focus)
        {
        GetWindowRect(focus, &r);
        GetCursorPos(&p);
        move_x=p.x-(r.right+r.left)/2;
        move_y=p.y-(r.bottom+r.top)/2;
        }
    GetWindowRect(window, &r);
    GetMonitorRect(p, &s);
    if(r.right+move_x>s.right)
        move_x=s.right-r.right;
    if(r.bottom+move_y>s.bottom)
        move_y=s.bottom-r.bottom;
    if(r.left+move_x<s.left)
        move_x=s.left-r.left;
    if(r.top+move_y<s.top)
        move_y=s.top-r.top;
    MoveWindow(window, r.left+move_x, r.top+move_y,
        r.right-r.left, r.bottom-r.top, FALSE);
    }

static void GetMonitorRect(POINT p, RECT *r)
    {
    static HINSTANCE user32;
    typedef HMONITOR WINAPI MonitorFromPointType(POINT, DWORD);
    static MonitorFromPointType *DllMonitorFromPoint;
    typedef BOOL WINAPI GetMonitorInfoAType(HMONITOR, LPMONITORINFO);
    static GetMonitorInfoAType *DllGetMonitorInfoA;
    HMONITOR m;
    MONITORINFO mi;

    if(NULL==user32)
        {
        user32=LoadLibrary("user32.dll");
        if(NULL!=user32)
            {
            DllMonitorFromPoint=(MonitorFromPointType *)
                GetProcAddress(user32, "MonitorFromPoint");
            DllGetMonitorInfoA=(GetMonitorInfoAType *)
                GetProcAddress(user32, "GetMonitorInfoA");
            }
        }
    if(NULL!=DllMonitorFromPoint && NULL!=DllGetMonitorInfoA)
        {
        m=DllMonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
        if(NULL!=m)
            {
            mi.cbSize=sizeof(mi);
            if(DllGetMonitorInfoA(m, &mi))
                {
                *r=mi.rcWork;
                return;
                }
            }
        }
    SystemParametersInfo(SPI_GETWORKAREA, 0, r, FALSE);
    }

static void GroupEstimateSize(HWND window, int id, SIZE *sz, SIZE *max, BOOL V)
    {
    struct DLG_Data *data;
    int i;
    int grp_style=0;

    data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);

    sz->cx=0;
    sz->cy=0;
    max->cx=ITEM_MSZ_FIXED;
    max->cy=ITEM_MSZ_FIXED;
    for(i=0; i<data->count; ++i)
        {
        if(id==data->Item[i].Id)
            {
            if(data->Item[i].Control==&CtlGroupV ||
                data->Item[i].Control==&CtlGroupH)
                grp_style=GetDlgItemInt(window, id, NULL, FALSE);
            }
        else if(id==data->Item[i].GroupId)
            {
            data->Item[i].Control->Estimate(window, data->Item[i].Id,
                &data->Item[i].ESZ, &data->Item[i].MSZ);
            data->Item[i].SizeKnown=1;
            if(V)
                {
                if(sz->cx<data->Item[i].ESZ.cx) sz->cx=data->Item[i].ESZ.cx;
                sz->cy+=data->Item[i].ESZ.cy;
                }
            else
                {
                sz->cx+=data->Item[i].ESZ.cx;
                if(sz->cy<data->Item[i].ESZ.cy) sz->cy=data->Item[i].ESZ.cy;
                }
            if(ITEM_MSZ_MAX==data->Item[i].MSZ.cx)
                max->cx=ITEM_MSZ_MAX;
            else if(ITEM_MSZ_FILL==data->Item[i].MSZ.cx &&
                ITEM_MSZ_FIXED==max->cx)
                max->cx=ITEM_MSZ_FILL;
            if(ITEM_MSZ_MAX==data->Item[i].MSZ.cy)
                max->cy=ITEM_MSZ_MAX;
            else if(ITEM_MSZ_FILL==data->Item[i].MSZ.cy &&
                ITEM_MSZ_FIXED==max->cy)
                max->cy=ITEM_MSZ_FILL;
            }
        }
    switch(grp_style)
        {
    case GRP_AUTO: break;
    case GRP_FIXED:   max->cx=ITEM_MSZ_FIXED; max->cy=ITEM_MSZ_FIXED; break;
    case GRP_MAX:     max->cx=ITEM_MSZ_MAX; max->cy=ITEM_MSZ_MAX; break;
    case GRP_MAX_CX:  max->cx=ITEM_MSZ_MAX; break;
    case GRP_MAX_CY:  max->cy=ITEM_MSZ_MAX; break;
    case GRP_FILL:    max->cx=ITEM_MSZ_FILL; max->cy=ITEM_MSZ_FILL; break;
    case GRP_FILL_CX: max->cx=ITEM_MSZ_FILL; break;
    case GRP_FILL_CY: max->cy=ITEM_MSZ_FILL; break;
    default: break;
        }
    }

static void GroupEstimateSizeV(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GroupEstimateSize(window, id, sz, max, TRUE);
    }

static void GroupEstimateSizeH(HWND window, int id, SIZE *sz, SIZE *max)
    {
    GroupEstimateSize(window, id, sz, max, FALSE);
    }

struct GCI
    {
    int type;
    int tcount;
    int max;
    int sum;
    };

static void GroupCountItems(
    struct DLG_Data *data,
    int id,
    struct GCI *gci_cx,
    struct GCI *gci_cy
    )
    {
    int i;
    int cx_c_max=0, cx_c_fill=0, cx_c_fixed=0;
    int cy_c_max=0, cy_c_fill=0, cy_c_fixed=0;

    memset(gci_cx, 0, sizeof(*gci_cx));
    memset(gci_cy, 0, sizeof(*gci_cy));

    for(i=0; i<data->count; ++i)
        {
        if(id!=data->Item[i].Id && id==data->Item[i].GroupId)
            {
            gci_cx->sum+=data->Item[i].ESZ.cx;
            gci_cy->sum+=data->Item[i].ESZ.cy;
            if(data->Item[i].ESZ.cx>gci_cx->max)
                gci_cx->max=data->Item[i].ESZ.cx;
            if(data->Item[i].ESZ.cy>gci_cy->max)
                gci_cy->max=data->Item[i].ESZ.cy;
            if(ITEM_MSZ_MAX==data->Item[i].MSZ.cx)
                ++cx_c_max;
            else if(ITEM_MSZ_FILL==data->Item[i].MSZ.cx)
                ++cx_c_fill;
            else
                ++cx_c_fixed;
            if(ITEM_MSZ_MAX==data->Item[i].MSZ.cy)
                ++cy_c_max;
            else if(ITEM_MSZ_FILL==data->Item[i].MSZ.cy)
                ++cy_c_fill;
            else
                ++cy_c_fixed;
            }
        }

    if(cx_c_max!=0)
        {
        gci_cx->tcount=cx_c_max;
        gci_cx->type=ITEM_MSZ_MAX;
        }
    else if(cx_c_fill!=0)
        {
        gci_cx->tcount=cx_c_fill;
        gci_cx->type=ITEM_MSZ_FILL;
        }
    else
        {
        gci_cx->tcount=cx_c_fixed;
        gci_cx->type=ITEM_MSZ_FIXED;
        }

    if(cy_c_max!=0)
        {
        gci_cy->tcount=cy_c_max;
        gci_cy->type=ITEM_MSZ_MAX;
        }
    else if(cy_c_fill!=0)
        {
        gci_cy->tcount=cy_c_fill;
        gci_cy->type=ITEM_MSZ_FILL;
        }
    else
        {
        gci_cy->tcount=cy_c_fixed;
        gci_cy->type=ITEM_MSZ_FIXED;
        }

    }

static void GroupMove(HWND window, int id, int x, int y, int w, int h, BOOL V)
    {
    struct DLG_Data *data;
    struct GCI gci_cx, gci_cy;
    int i;
    int spacing_w, spacing_h, over_h;
    int item_w, item_h;
    int delta_x, delta_y;

    data=(struct DLG_Data *)GetWindowLongPtr(window, GWLP_USERDATA);
    GroupCountItems(data, id, &gci_cx, &gci_cy);

    if(V)
        {
        spacing_w=w-gci_cx.max;
        if(0==gci_cy.tcount)
            spacing_h=0;
        else
            spacing_h=(h-gci_cy.sum)/gci_cy.tcount;
        over_h=h-gci_cy.sum-spacing_h*gci_cy.tcount;
        }
    else
        {
        if(0==gci_cx.tcount)
            spacing_w=0;
        else
            spacing_w=(w-gci_cx.sum)/gci_cx.tcount;
        spacing_h=h-gci_cy.max;
        over_h=0;
        }

    if(!V && ITEM_MSZ_FIXED==gci_cx.type)
        x+=spacing_w/2;
    if(ITEM_MSZ_FIXED==gci_cy.type)
        y+=spacing_h/2;

    for(i=0; i<data->count; ++i)
        {
        if(id==data->Item[i].Id || id!=data->Item[i].GroupId)
            continue;
        item_w=data->Item[i].ESZ.cx;
        item_h=data->Item[i].ESZ.cy;
        delta_x=0;
        delta_y=0;
        if(V)
            {
            if(data->Item[i].MSZ.cx!=ITEM_MSZ_FIXED)
                item_w=w;
            delta_y=item_h;
            if(over_h>0)
                {
                ++delta_y;
                --over_h;
                }
            if(data->Item[i].MSZ.cy==gci_cy.type)
                delta_y+=spacing_h;
            if(data->Item[i].MSZ.cy!=ITEM_MSZ_FIXED)
                item_h=delta_y;
            }
        else /* H */
            {
            if(data->Item[i].MSZ.cy!=ITEM_MSZ_FIXED)
                item_h=h;
            delta_x=item_w;
            if(data->Item[i].MSZ.cx==gci_cx.type)
                delta_x+=spacing_w;
            if(data->Item[i].MSZ.cx!=ITEM_MSZ_FIXED)
                item_w=delta_x;
            }
        data->Item[i].Control->Move(window, data->Item[i].Id,
            x, y, item_w, item_h);
        if(NULL!=data->Item[i].ItemProc)
            {
            data->Item[i].ItemProc(window, data->Item[i].Id,
                WM_MOVE, 0, MAKELPARAM(x, y));
            data->Item[i].ItemProc(window, data->Item[i].Id,
                WM_SIZE, 0, MAKELPARAM(item_w, item_h));
            }
        x+=delta_x;
        y+=delta_y;
        }
    }

static void GroupMoveV(HWND window, int id, int x, int y, int w, int h)
    {
    GroupMove(window, id, x, y, w, h, TRUE);
    }

static void GroupMoveH(HWND window, int id, int x, int y, int w, int h)
    {
    GroupMove(window, id, x, y, w, h, FALSE);
    }

void GetWindowStrSize(HWND window, WCHAR *str, SIZE *sz)
    {
    HFONT font;
    HDC dc;
    int len;
    WCHAR *str_end;
    SIZE lsz;

    sz->cx=0;
    sz->cy=0;
    if(0==*str)
        return;

    font=(HFONT)SendMessage(window, WM_GETFONT, 0, 0);
    if(NULL==font)
        font=(HFONT)SendMessage(GetParent(window), WM_GETFONT, 0, 0);
    dc=GetDC(window);
    SelectObject(dc, font);

    do
        {
        str_end=str;
        while(*str_end!=0 && *str_end!=L'\n')
            ++str_end;
        len=(int)(str_end-str);
        if(0==len)
            len=1;
        lsz.cx=0;
        lsz.cy=0;
        GetTextExtentPoint32W(dc, str, len, &lsz);
        if(lsz.cx>sz->cx)
            sz->cx=lsz.cx;
        sz->cy+=lsz.cy;
        if(0==*str_end)
            break;
        str=str_end+1;
        }
    while(*str_end!=0);
    
    ReleaseDC(window, dc);
    }

void GetDlgItemTextSize(HWND window, int id, SIZE *sz)
    {
    HWND item=GetDlgItem(window, id);
    WCHAR *title;
    int len;

    sz->cx=0;
    sz->cy=0;

    len=GetWindowTextLengthU(item)+1;
    if(len<=1) return;

    title=malloc(sizeof(*title)*len);
    if(NULL==title) return;

    *title=0;
    GetDlgItemTextU(window, id, title, len);
    GetWindowStrSize(item, title, sz);

    free(title);
    }

void DlgMoveItem(HWND window, int id, int x, int y, int cx, int cy)
    {
    MoveWindow(GetDlgItem(window, id), x, y, cx, cy, FALSE);
    }

struct DLG_Control CtlGroupV=
    {
    L"STATIC",
    SS_OWNERDRAW,
    0,
    FALSE,
    NULL,
    GroupEstimateSizeV,
    GroupMoveV,
    NULL
    };

struct DLG_Control CtlGroupH=
    {
    L"STATIC",
    SS_OWNERDRAW,
    0,
    FALSE,
    NULL,
    GroupEstimateSizeH,
    GroupMoveH,
    NULL
    };

