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
#include <commctrl.h>
#include <stdlib.h>
#include "ntpw.h"
#include "log.h"
#include "unicode.h"
#include "dialogs.h"
#include "ctl_groupbox.h"
#include "ctl_edit.h"
#include "ctl_button.h"
#include "ctl_listview.h"
#include "ctl_image.h"
#include "ctl_label.h"
#include "message.h"
#include "dlgpass.h"
#include "dlgabout.h"
#include "version.h"

static BOOL Button_CANCEL(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Edit_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_OPEN(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_UNLOCK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_PASS(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_SAVE(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL ListProc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Button_ABOUT(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL OpenDialog(HWND window, char *namebuf, int bufsize);
static int ListUsers(HWND window, char *path);
static void ListInsert(HWND window, WORD id, int pos, int rid, WCHAR *name);
static void ListDeleteAll(HWND window, WORD id);
static void ListAutosizeColumn(HWND window, WORD id, int column);
static int ListGetPos(HWND window, WORD id);
static LPARAM ListGetParam(HWND window, WORD id, int pos);
static void EnableUserOptions(HWND window, int rid);
static void DisableUserOptions(HWND window);
static void CheckSave(HWND window);

enum
    {
    ID_GRP_ABOUT=101,
    ID_ICON_ABOUT,
    ID_SPACER_ABOUT1,
    ID_GRP_LABEL,
    ID_LABEL_ABOUT,
    ID_SPACER_ABOUT2,
    ID_BUTTON_ABOUT,
    ID_GRP_PATH,
    ID_EDIT_PATH,
    ID_SPACER_PATH,
    ID_BUTTON_PATH,
    ID_SPACER_OPEN,
    ID_BUTTON_OPEN,
    ID_GRP_LIST,
    ID_LIST_USERS,
    ID_SPACER_LIST,
    ID_GRP_ACTIONS,
    ID_BUTTON_UNLOCK,
    ID_SPACER_UNLOCK,
    ID_BUTTON_PASS,
    ID_GRP_BUTTON,
    ID_SPACER_OK,
    ID_BUTTON_SAVE
    };

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxH, ID_GRP_ABOUT, NULL, 0, 0, NULL},
    {&CtlIcon, ID_ICON_ABOUT, L"IconApp", 0, ID_GRP_ABOUT, NULL},
    {&CtlGroupBoxSpacer, ID_SPACER_ABOUT1, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlGroupH, ID_GRP_LABEL, GRP_TITLE_FILL_CX, 0, ID_GRP_ABOUT, NULL},
    {&CtlLabel, ID_LABEL_ABOUT, L"NTPWEdit \xA9 2005,2006 Vadim Druzhin", 0, ID_GRP_LABEL, NULL},
    {&CtlGroupBoxSpacer, ID_SPACER_ABOUT2, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlButton, ID_BUTTON_ABOUT, L"About", 0, ID_GRP_ABOUT, Button_ABOUT},
    {&CtlGroupBoxH, ID_GRP_PATH, L"Path to SAM file", 0, 0, NULL},
    {&CtlEdit, ID_EDIT_PATH, L"C:\\WINDOWS\\SYSTEM32\\CONFIG\\SAM", 0, ID_GRP_PATH, Edit_PATH},
    {&CtlGroupBoxSpacer, ID_SPACER_PATH, NULL, 0, ID_GRP_PATH, NULL},
    {&CtlSmallButton, ID_BUTTON_PATH, L"...", 0, ID_GRP_PATH, Button_PATH},
    {&CtlGroupBoxSpacer, ID_SPACER_OPEN, NULL, 0, ID_GRP_PATH, NULL},
    {&CtlButton, ID_BUTTON_OPEN, L"(Re)open", 0, ID_GRP_PATH, Button_OPEN},
    {&CtlGroupBoxV, ID_GRP_LIST, L"User list", 0, 0, NULL},
    {&CtlListView, ID_LIST_USERS, L"ID\tName", 0, ID_GRP_LIST, ListProc},
    {&CtlGroupBoxSpacer, ID_SPACER_LIST, NULL, 0, ID_GRP_LIST, NULL},
    {&CtlGroupH, ID_GRP_ACTIONS, NULL, 0, ID_GRP_LIST, NULL},
    {&CtlButton, ID_BUTTON_UNLOCK, L"Unlock", 0, ID_GRP_ACTIONS, Button_UNLOCK},
    {&CtlGroupBoxSpacer, ID_SPACER_UNLOCK, NULL, 0, ID_GRP_ACTIONS, NULL},
    {&CtlButton, ID_BUTTON_PASS, L"Change password", 0, ID_GRP_ACTIONS, Button_PASS},
    {&CtlGroupBoxH, ID_GRP_BUTTON, NULL, 0, 0, NULL},
    {&CtlButton, ID_BUTTON_SAVE, L"Save changes", 0, ID_GRP_BUTTON, Button_SAVE},
    {&CtlGroupBoxSpacer, ID_SPACER_OK, NULL, 0, ID_GRP_BUTTON, NULL},
    {&CtlDefButton, IDCANCEL, L"Exit", 0, ID_GRP_BUTTON, Button_CANCEL},
    };

static BOOL Button_CANCEL(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_INITDIALOG==msg)
        {
        SetFocus(GetDlgItem(window, ID_BUTTON_PATH));
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        if(IsWindowEnabled(GetDlgItem(window, ID_BUTTON_SAVE)))
            {
            if(IDOK==AppMessageBox(window,
                    L"Changes not saved.\n\nSave before exit?", MB_OKCANCEL)
                )
                Button_SAVE(window, ID_BUTTON_SAVE, msg, wParam, lParam);
            }
        EndDialog(window, 0);
        return TRUE;
        }

    return FALSE;
    }       

static BOOL Button_ABOUT(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_COMMAND==msg)
        {
        AboutDialog(window);
        return TRUE;
        }

    return FALSE;
    }       

static BOOL Button_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    char path[MAX_PATH];
    
    if(WM_COMMAND==msg)
        {
        GetDlgItemText(window, ID_EDIT_PATH, path, sizeof(path));
        if(OpenDialog(window, path, sizeof(path)))
            {
            SetDlgItemText(window, ID_EDIT_PATH, path);
            SendMessage(window, WM_COMMAND, ID_BUTTON_OPEN, 0);
            }
        return TRUE;
        }

    return FALSE;
    }       

static BOOL Edit_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    DWORD attr;
    char path[MAX_PATH];

    if(WM_INITDIALOG==msg || (WM_COMMAND==msg && EN_CHANGE==HIWORD(wParam)))
        {
        GetDlgItemText(window, ID_EDIT_PATH, path, sizeof(path));
        attr=GetFileAttributes(path);
        if(0xFFFFFFFF!=attr &&
            FILE_ATTRIBUTE_DIRECTORY!=(attr&FILE_ATTRIBUTE_DIRECTORY))
            EnableWindow(GetDlgItem(window, ID_BUTTON_OPEN), TRUE);
        else
            EnableWindow(GetDlgItem(window, ID_BUTTON_OPEN), FALSE);
        return TRUE;
        }

    return FALSE;
    }

static BOOL Button_OPEN(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    char path[MAX_PATH];

    if(WM_COMMAND==msg)
        {
        GetDlgItemText(window, ID_EDIT_PATH, path, sizeof(path));
        if(ListUsers(window, path)<0)
            AppMessageBox(window, L"Open failed!", MB_OK);
        else if(is_hives_ro())
            AppMessageBox(window,
                L"SAM file opened read-only,\n"
                "so changes cannot be saved!\n\n"
                "Modify file attributes/permissions\n"
                "to allow write access, and reopen it.", MB_OK);
        DisableUserOptions(window);
        CheckSave(window);
        return TRUE;
        }

    return FALSE;
    }

static BOOL Button_UNLOCK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        int pos;
        LPARAM param;

        pos=ListGetPos(window, ID_LIST_USERS);
        if(pos>=0)
            {
            param=ListGetParam(window, ID_LIST_USERS, pos);
            if(unlock_account(param))
                EnableWindow(GetDlgItem(window, id), FALSE);
            else
                AppMessageBox(window, L"Unlock failed!", MB_OK);
            CheckSave(window);
            }
        return TRUE;
        }

    return FALSE;
    }

static BOOL Button_PASS(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        int pos;
        LPARAM param;
        char pass[17];

        pos=ListGetPos(window, ID_LIST_USERS);
        if(pos>=0)
            {
            param=ListGetParam(window, ID_LIST_USERS, pos);
            if(QueryPassword(window, pass, sizeof(pass)))
                {
                if(!change_password(param, pass))
                    {
                    if(0==*pass)
                        AppMessageBox(window,
                            L"Password not changed!", MB_OK);
                    else
                        AppMessageBox(window,
                            L"Password not changed!\n"
                            "Try again using BLANK passsword.", MB_OK);
                    }
                CheckSave(window);
                }
            }
        return TRUE;
        }

    return FALSE;
    }

static BOOL Button_SAVE(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        if(!write_hives())
            AppMessageBox(window, L"Writing SAM failed!", MB_OK);
        CheckSave(window);
        }

    return FALSE;
    }

static BOOL ListProc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    if(WM_NOTIFY==msg)
        {
        NMHDR *nm=(NMHDR *)lParam;

        if(LVN_ITEMCHANGED==nm->code || NM_SETFOCUS==nm->code)
            {
            int pos;
            LPARAM param;

            pos=ListGetPos(window, id);
            if(pos>=0)
                {
                param=ListGetParam(window, id, pos);
                EnableUserOptions(window, param);
                }
            else
                DisableUserOptions(window);
            return TRUE;
            }
        }

    return FALSE;
    }

static int ListGetPos(HWND window, WORD id)
    {
    return SendDlgItemMessage(window, id, LVM_GETNEXTITEM,
        -1, LVNI_ALL|LVNI_SELECTED);
    }

static LPARAM ListGetParam(HWND window, WORD id, int pos)
    {
    LV_ITEM item;

    item.iItem=pos;
    item.mask=LVIF_PARAM;	
    item.iSubItem=0;
    SendDlgItemMessage(window, id, LVM_GETITEM, 0, (LPARAM)&item);
    return item.lParam;
    }

static void EnableUserOptions(HWND window, int rid)
    {
    if(1==is_account_locked(rid))
        EnableWindow(GetDlgItem(window, ID_BUTTON_UNLOCK), TRUE);
    EnableWindow(GetDlgItem(window, ID_BUTTON_PASS), TRUE);
    }

static void DisableUserOptions(HWND window)
    {
    EnableWindow(GetDlgItem(window, ID_BUTTON_UNLOCK), FALSE);
    EnableWindow(GetDlgItem(window, ID_BUTTON_PASS), FALSE);
    }

static void CheckSave(HWND window)
    {
    EnableWindow(GetDlgItem(window, ID_BUTTON_SAVE),
        is_hives_dirty() && !is_hives_ro());
    }

static int ListUsers(HWND window, char *path)
    {
    char *names[H_COUNT]={""};
    struct search_user su;
    struct user_info *ui;
    int count;

    ListDeleteAll(window, ID_LIST_USERS);

    names[H_SAM]=path;
    if(0==open_hives(names))
        return -1;

    ui=first_user(&su);
    count=0;
    while(ui)
        {
        if(500==ui->rid)
            ListInsert(window, ID_LIST_USERS,
                0, ui->rid, (WCHAR *)ui->unicode_name);
        else
            ListInsert(window, ID_LIST_USERS,
                count, ui->rid, (WCHAR *)ui->unicode_name);
        free(ui);
        ui=next_user(&su);
        ++count;
        }
    if(count>0)
        {
        ListAutosizeColumn(window, ID_LIST_USERS, 0);
        ListAutosizeColumn(window, ID_LIST_USERS, 1);
        RedrawWindow(window, NULL, NULL, RDW_INVALIDATE);	
        }

    return count;
    }

static void ListInsert(HWND window, WORD id, int pos, int rid, WCHAR *name)
    {
    LV_ITEMW litem;
    WCHAR buf[12];

    litem.mask=LVIF_TEXT|LVIF_PARAM;
    litem.iItem=pos;
    litem.iSubItem=0;
    litem.state=0;
    litem.stateMask=0;
    litem.pszText=buf;
    litem.cchTextMax=0;
    litem.iImage=0;    
    litem.lParam=rid;
    Num(buf, rid);
    SendDlgItemMessageW(window, id, LVM_INSERTITEMW, 0, (LPARAM)&litem);
    litem.mask=LVIF_TEXT;
    litem.iSubItem=1;
    litem.pszText=name;
    SendDlgItemMessageW(window, id, LVM_SETITEMW, 0, (LPARAM)&litem);
    }

static void ListDeleteAll(HWND window, WORD id)
    {
    SendDlgItemMessage(window, id, LVM_DELETEALLITEMS, 0, 0);
    }

static void ListAutosizeColumn(HWND window, WORD id, int column)
    {
    SendDlgItemMessage(window, id, LVM_SETCOLUMNWIDTH, column, LVSCW_AUTOSIZE);
    }

static BOOL OpenDialog(HWND window, char *namebuf, int bufsize)
    {
    static OPENFILENAME ofn;

    if(ofn.lStructSize==0)
        {
        ofn.lStructSize=sizeof(ofn);
        /*ofn.hwndOwner=window;*/
        ofn.hInstance=NULL;
        ofn.lpstrFilter="SAM file\0SAM\0All files\0*.*\0";
        ofn.lpstrCustomFilter=NULL;
        ofn.nMaxCustFilter=0;
        ofn.nFilterIndex=0;
        /*ofn.lpstrFile=namebuf;*/
        /*ofn.nMaxFile=bufsize;*/
        ofn.lpstrFileTitle=NULL;
        ofn.nMaxFileTitle=0;
        ofn.lpstrInitialDir=NULL;
        ofn.lpstrTitle=NULL;
        ofn.Flags=OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_EXPLORER|OFN_HIDEREADONLY;
        ofn.nFileOffset=0;
        ofn.nFileExtension=0;
        ofn.lpstrDefExt=NULL;
        ofn.lCustData=0;
        ofn.lpfnHook=NULL;
        ofn.lpTemplateName=NULL;
        }
    ofn.hwndOwner=window;
    ofn.lpstrFile=namebuf;
    ofn.nMaxFile=bufsize;

    return GetOpenFileName(&ofn);
    }

int main(void)
    {
    HWND window;

    UnicodeInit();

    /* Set icon for dialog windows */
    window=CreateWindow(WC_DIALOG, "", 0,
        0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if(window)
        {
        SetClassLong(window, GCL_HICON,
            (LONG)LoadIcon(GetModuleHandle(NULL), "IconApp"));
        DestroyWindow(window);
        }

    DlgRunU(
        NULL,
        AppTitle,
        0,
        /*WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_SIZEBOX|WS_MAXIMIZEBOX,*/
        WS_OVERLAPPEDWINDOW,
        0,
        Items,
        sizeof(Items)/sizeof(*Items),
        NULL
        );
    close_hives();

    return 0;
    }
