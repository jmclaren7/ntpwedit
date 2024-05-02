#ifndef __UNICODE_H__
#define __UNICODE_H__

void UnicodeInit(void);
BOOL IsRealUnicode(void);
char *TextBufWtoA(WCHAR const *textw);
WCHAR *TextBufAtoW(char const *texta);
WCHAR *MapStringU(HINSTANCE hinst, UINT id, int lng, int *len);
int LoadStringU(HINSTANCE hinst, UINT id, WCHAR *buf, int buflen, int lng);
WCHAR *strcpyU(WCHAR *to, WCHAR const *from);
extern BOOL (*GetNonClientMetricsU)(NONCLIENTMETRICSW *ncmw);
extern int WINAPI (*DialogBoxIndirectParamU)
    (HINSTANCE,LPCDLGTEMPLATE,HWND,DLGPROC,LPARAM);
extern HWND WINAPI (*CreateDialogIndirectParamU)
    (HINSTANCE,LPCDLGTEMPLATE,HWND,DLGPROC,LPARAM);
extern BOOL WINAPI (*InsertMenuItemU)(HMENU,UINT,BOOL,LPCMENUITEMINFOW);
extern void (*DlgItemReplaceSelRcU)(HWND dlg, int item, UINT rcid);
extern BOOL WINAPI (*SetDlgItemTextU)(HWND,int,LPCWSTR);
extern UINT WINAPI (*GetDlgItemTextU)(HWND,int,LPWSTR,int);
extern int WINAPI (*GetWindowTextLengthU)(HWND);
extern int WINAPI (*GetClassNameU)(HWND,LPWSTR,int);
extern BOOL WINAPI (*Shell_NotifyIconU)(DWORD,PNOTIFYICONDATAW);
WCHAR *Num(WCHAR *to, int n);
WCHAR *Rcs(WCHAR *to, int id);
WCHAR *Crlf(WCHAR *to);
WCHAR *Stra(WCHAR *to, char *from);

#endif /* __UNICODE_H__ */
