/* ===================================================================
 * Copyright (c) 2005-2017 Vadim Druzhin (cdslow@mail.ru).
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
#include "dlghiber.h"
#include "version.h"

#define DEF_SAM_PATH L"C:\\WINDOWS\\SYSTEM32\\CONFIG\\SAM"
#define HIBR_TAG_LEN 4
#define HIBR_TAG "HIBR"

static INT_PTR Button_CANCEL(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Edit_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_OPEN(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_UNLOCK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_PASS(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_SAVE(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR ListProc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR Button_ABOUT(
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
static WCHAR *SearchSAM(void);
static BOOL CheckHiberfil(char const *path);
static HANDLE SuperOpen(char const *file_name);

enum
    {
    ID_GRP_ABOUT=101,
    ID_ICON_ABOUT,
    ID_SPACER_ABOUT1,
    ID_GRP_ABOUT_FILL,
    ID_SPACER_ABOUT2,
    ID_GRP_ABOUT_ICON,
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
    ID_BUTTON_SAVE,
    ID_GRP_MIDDLE
    };

static struct DLG_Item Items[]=
    {
    {&CtlGroupBoxH, ID_GRP_PATH, L"Path to SAM file", 0, 0, NULL},
    {&CtlEdit, ID_EDIT_PATH, NULL, 0, ID_GRP_PATH, Edit_PATH},
    {&CtlGroupBoxSpacer, ID_SPACER_PATH, NULL, 0, ID_GRP_PATH, NULL},
    {&CtlSmallButton, ID_BUTTON_PATH, L"...", 0, ID_GRP_PATH, Button_PATH},
    {&CtlGroupBoxSpacer, ID_SPACER_OPEN, NULL, 0, ID_GRP_PATH, NULL},
    {&CtlButton, ID_BUTTON_OPEN, L"Open", 0, ID_GRP_PATH, Button_OPEN},

    {&CtlGroupH, ID_GRP_MIDDLE, NULL, 0, 0, NULL},

    {&CtlGroupBoxV, ID_GRP_LIST, L"User list", 0, ID_GRP_MIDDLE, NULL},
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

    {&CtlGroupBoxV, ID_GRP_ABOUT, NULL, 0, ID_GRP_MIDDLE, NULL},
    {&CtlGroupBoxSpacer, ID_SPACER_ABOUT1, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlGroupH, ID_GRP_ABOUT_ICON, GRP_TITLE_FILL_CX, 0, ID_GRP_ABOUT, NULL},
    {&CtlIcon, ID_ICON_ABOUT, L"IconApp", 0, ID_GRP_ABOUT_ICON, NULL},
    {&CtlGroupBoxSpacer, ID_SPACER_ABOUT2, NULL, 0, ID_GRP_ABOUT, NULL},
    {&CtlButton, ID_BUTTON_ABOUT, L"About", 0, ID_GRP_ABOUT, Button_ABOUT},
    {&CtlGroupH, ID_GRP_ABOUT_FILL, GRP_TITLE_FILL, 0, ID_GRP_ABOUT, NULL},
    };

static INT_PTR Button_CANCEL(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)id; /* Unused */

    if(WM_INITDIALOG==msg)
        {
        SetFocus(GetDlgItem(window, ID_BUTTON_PATH));
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        if(IsWindowEnabled(GetDlgItem(window, ID_BUTTON_SAVE)))
            {
            if(IDYES==AppMessageBox(window,
                    L"Changes not saved.\n\nSave before exit?", MB_YESNO)
                )
                Button_SAVE(window, ID_BUTTON_SAVE, msg, wParam, lParam);
            }
        EndDialog(window, 0);
        return TRUE;
        }

    return FALSE;
    }       

static INT_PTR Button_ABOUT(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)id; /* Unused */
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_COMMAND==msg)
        {
        AboutDialog(window);
        return TRUE;
        }

    return FALSE;
    }       

static INT_PTR Button_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    char path[MAX_PATH];
    
    (void)id; /* Unused */
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

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

static INT_PTR Edit_PATH(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    DWORD attr;
    char path[MAX_PATH];

    (void)id; /* Unused */
    (void)lParam; /* Unused */

    /* Update initial SAM path in dialog */
    if(WM_INITDIALOG == msg)
        SetDlgItemTextW(window, ID_EDIT_PATH, SearchSAM());

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

static INT_PTR Button_OPEN(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    char path[MAX_PATH];

    (void)id; /* Unused */
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_COMMAND==msg)
        {
        GetDlgItemText(window, ID_EDIT_PATH, path, sizeof(path));

        if(CheckHiberfil(path))
            {
            if(HiberWarning(window) != IDOK)
                return Button_CANCEL(window, IDCANCEL, msg, wParam, lParam);
            }

        if(ListUsers(window, path)<0)
            AppMessageBox(window, L"Open failed!", MB_OK);
        else if(is_hives_ro())
            AppMessageBox(window,
                L"SAM file opened read-only,\n"
                L"so changes cannot be saved!\n\n"
                L"Modify file attributes/permissions\n"
                L"to allow write access, and reopen it.", MB_OK);
        DisableUserOptions(window);
        CheckSave(window);
        return TRUE;
        }

    return FALSE;
    }

static INT_PTR Button_UNLOCK(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        int pos;
        int param;

        pos=ListGetPos(window, ID_LIST_USERS);
        if(pos>=0)
            {
            param=(int)ListGetParam(window, ID_LIST_USERS, pos);
            if(unlock_account(param))
                {
                EnableWindow(GetDlgItem(window, id), FALSE);
                SendMessage(window, WM_NEXTDLGCTL,
                    (WPARAM)GetDlgItem(window, ID_LIST_USERS), TRUE);
                }
            else
                AppMessageBox(window, L"Unlock failed!", MB_OK);
            CheckSave(window);
            }
        return TRUE;
        }

    return FALSE;
    }

static INT_PTR Button_PASS(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        int pos;
        int param;
        char pass[17];

        pos=ListGetPos(window, ID_LIST_USERS);
        if(pos>=0)
            {
            param=(int)ListGetParam(window, ID_LIST_USERS, pos);
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
                            L"Try again using BLANK passsword.", MB_OK);
                    }
                CheckSave(window);
                }
            }
        return TRUE;
        }

    return FALSE;
    }

static INT_PTR Button_SAVE(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)wParam; /* Unused */
    (void)lParam; /* Unused */

    if(WM_INITDIALOG==msg)
        {
        EnableWindow(GetDlgItem(window, id), FALSE);
        return TRUE;
        }
    else if(WM_COMMAND==msg)
        {
        if(!write_hives())
            AppMessageBox(window, L"Writing SAM failed!", MB_OK);
        SendMessage(window, WM_NEXTDLGCTL,
            (WPARAM)GetDlgItem(window, IDCANCEL), TRUE);
        CheckSave(window);
        }

    return FALSE;
    }

static INT_PTR ListProc(
    HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    (void)wParam; /* Unused */

    if(WM_NOTIFY==msg)
        {
        NMHDR *nm=(NMHDR *)lParam;

        if((UINT)LVN_ITEMCHANGED==nm->code || (UINT)NM_SETFOCUS==nm->code)
            {
            int pos;
            int param;

            pos=ListGetPos(window, id);
            if(pos>=0)
                {
                param=(int)ListGetParam(window, id, pos);
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
    return (int)SendDlgItemMessage(window, id, LVM_GETNEXTITEM,
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
    if(0!=is_account_locked(rid))
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
        SetClassLongPtr(window, GCLP_HICON,
            (LONG_PTR)LoadIcon(GetModuleHandle(NULL), "IconApp"));
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

#ifdef _MSC_VER
int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
    )
    {
    (void)hInstance; /* Unused */
    (void)hPrevInstance; /* Unused */
    (void)lpCmdLine; /* Unused */
    (void)nCmdShow; /* Unused */

    return main();
    }
#endif /* _MSC_VER */

static WCHAR *SearchSAM(void)
    {
    static WCHAR sam[] = DEF_SAM_PATH;
    WCHAR root[] = L"C:\\";

    while(root[0] <= L'Z')
        {
        UINT dt = GetDriveTypeW(root);

        if(dt == DRIVE_FIXED)
            {
            DWORD attr;

            sam[0] = root[0];
            attr = GetFileAttributesW(sam);

            if(attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY))
                break;

            sam[0] = L'C';
            }

        ++root[0];
        }

    return sam;
    }

static BOOL CheckHiberfil(char const *path)
    {
    char hiberfil[] = "C:\\HIBERFIL.SYS";
    DWORD attr;
    HANDLE in;
    char buf[HIBR_TAG_LEN];
    DWORD count;
    BOOL res = TRUE;

    hiberfil[0] = path[0];
    attr = GetFileAttributes(hiberfil);

    if(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
        return FALSE;

    in = CreateFile(hiberfil, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if(in == INVALID_HANDLE_VALUE)
        in = SuperOpen(hiberfil);

    if(in == INVALID_HANDLE_VALUE)
        return TRUE;

    if(ReadFile(in, buf, HIBR_TAG_LEN, &count, NULL))
        {
        if(count == HIBR_TAG_LEN && memcmp(buf, HIBR_TAG, HIBR_TAG_LEN) != 0)
            res = FALSE;
        }

    CloseHandle(in);

    return res;
    }

static HANDLE SuperOpen(char const *file_name)
    {
    HANDLE ret = INVALID_HANDLE_VALUE;
    HANDLE token = INVALID_HANDLE_VALUE;
    size_t const priv_size = sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES);
    TOKEN_PRIVILEGES *priv = NULL;
    TOKEN_PRIVILEGES *old_priv = NULL;
    DWORD old_priv_size = 0;

    do
        {
        if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
            break;

        priv = malloc(priv_size);
        if(!priv)
            break;

        old_priv = malloc(priv_size);
        if(!old_priv)
            break;

        priv->PrivilegeCount = 2;

        if(!LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &(priv->Privileges[0].Luid)))
            break;

        priv->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if(!LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &(priv->Privileges[1].Luid)))
            break;

        priv->Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;

        if(!AdjustTokenPrivileges(token, FALSE, priv, priv_size, old_priv, &old_priv_size))
            break;

        ret = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        }
    while(0);

    if(old_priv_size != 0)
        AdjustTokenPrivileges(token, FALSE, old_priv, 0, NULL, NULL);

    free(old_priv);
    free(priv);
    CloseHandle(token);

    return ret;
    }
