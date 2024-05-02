#ifndef __UNICODE_H__
#define __UNICODE_H__

void UnicodeInit(void);
BOOL IsRealUnicode(void);
char *TextBufWtoA(WCHAR const *textw);
WCHAR *TextBufAtoW(char const *texta);
WCHAR *MapStringU(HINSTANCE hinst, UINT id, int lng, int *len);
int LoadStringU(HINSTANCE hinst, UINT id, WCHAR *buf, int buflen, int lng);
WCHAR *strcpyU(WCHAR *to, WCHAR const *from);

typedef BOOL GetNonClientMetricsU_t(NONCLIENTMETRICSW *ncmw);
extern GetNonClientMetricsU_t *GetNonClientMetricsU;

typedef INT_PTR WINAPI DialogBoxIndirectParamU_t
    (HINSTANCE,LPCDLGTEMPLATE,HWND,DLGPROC,LPARAM);
extern DialogBoxIndirectParamU_t *DialogBoxIndirectParamU;

typedef HWND WINAPI CreateDialogIndirectParamU_t
    (HINSTANCE,LPCDLGTEMPLATE,HWND,DLGPROC,LPARAM);
extern CreateDialogIndirectParamU_t *CreateDialogIndirectParamU;

typedef BOOL WINAPI InsertMenuItemU_t(HMENU,UINT,BOOL,LPCMENUITEMINFOW);
extern InsertMenuItemU_t *InsertMenuItemU;

typedef void DlgItemReplaceSelRcU_t(HWND dlg, int item, UINT rcid);
extern DlgItemReplaceSelRcU_t *DlgItemReplaceSelRcU;

typedef BOOL WINAPI SetDlgItemTextU_t(HWND,int,LPCWSTR);
extern SetDlgItemTextU_t *SetDlgItemTextU;

typedef UINT WINAPI GetDlgItemTextU_t(HWND,int,LPWSTR,int);
extern GetDlgItemTextU_t *GetDlgItemTextU;

typedef int WINAPI GetWindowTextLengthU_t(HWND);
extern GetWindowTextLengthU_t *GetWindowTextLengthU;

typedef BOOL WINAPI Shell_NotifyIconU_t(DWORD,PNOTIFYICONDATAW);
extern Shell_NotifyIconU_t *Shell_NotifyIconU;

WCHAR *Num(WCHAR *to, int n);
WCHAR *Rcs(WCHAR *to, int id);
WCHAR *Crlf(WCHAR *to);
WCHAR *Stra(WCHAR *to, char *from);

typedef BOOL GetVolumeLabelU_t(WCHAR *root, WCHAR *label, int size);
extern GetVolumeLabelU_t *GetVolumeLabelU;

#endif /* __UNICODE_H__ */
