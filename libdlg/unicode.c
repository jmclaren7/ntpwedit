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

static BOOL GetNonClientMetricsW(NONCLIENTMETRICSW *ncmw);
static BOOL GetNonClientMetricsA(NONCLIENTMETRICSW *ncmw);
static void LogfontAtoW(LOGFONTA *lfa, LOGFONTW *lfw);
static BOOL WINAPI InsertMenuItemWtoA(
    HMENU menu,
    UINT item,
    BOOL byposition,
    const MENUITEMINFOW *miiw
    );
static void DlgItemReplaceSelRcW(HWND dlg, int item, UINT rcid);
static void DlgItemReplaceSelRcA(HWND dlg, int item, UINT rcid);
static BOOL WINAPI SetDlgItemTextWtoA(HWND dlg, int item, WCHAR const *textw);
static UINT WINAPI GetDlgItemTextWtoA(
    HWND dlg, int item,
    WCHAR *textw, int max
    );
static BOOL WINAPI Shell_NotifyIconWtoA(DWORD msg, NOTIFYICONDATAW *nidw);
static BOOL GetVolumeLabelW(WCHAR *root, WCHAR *label, int size);
static BOOL GetVolumeLabelA(WCHAR *root, WCHAR *label, int size);

GetNonClientMetricsU_t *GetNonClientMetricsU=GetNonClientMetricsA;
DialogBoxIndirectParamU_t *DialogBoxIndirectParamU=DialogBoxIndirectParamA;
CreateDialogIndirectParamU_t *CreateDialogIndirectParamU=
    CreateDialogIndirectParamA;
InsertMenuItemU_t *InsertMenuItemU=
    InsertMenuItemWtoA;
DlgItemReplaceSelRcU_t *DlgItemReplaceSelRcU=
    DlgItemReplaceSelRcA;
SetDlgItemTextU_t *SetDlgItemTextU=SetDlgItemTextWtoA;
GetDlgItemTextU_t *GetDlgItemTextU=GetDlgItemTextWtoA;
GetWindowTextLengthU_t *GetWindowTextLengthU=GetWindowTextLengthA;
Shell_NotifyIconU_t *Shell_NotifyIconU=Shell_NotifyIconWtoA;
GetVolumeLabelU_t *GetVolumeLabelU=GetVolumeLabelA;

static BOOL OSRealUnicode=FALSE;

void UnicodeInit(void)
    {
    OSVERSIONINFO ovi;
    DWORD OSPlatformId;
    static HINSTANCE shell32=NULL;

    ovi.dwOSVersionInfoSize=sizeof(ovi);
    GetVersionEx(&ovi);
    OSPlatformId=ovi.dwPlatformId;
    OSRealUnicode=(OSPlatformId==VER_PLATFORM_WIN32_NT);
    if(OSRealUnicode)
        {
        GetNonClientMetricsU=GetNonClientMetricsW;
        DialogBoxIndirectParamU=DialogBoxIndirectParamW;
        CreateDialogIndirectParamU=CreateDialogIndirectParamW;
        InsertMenuItemU=InsertMenuItemW;
        DlgItemReplaceSelRcU=DlgItemReplaceSelRcW;
        SetDlgItemTextU=SetDlgItemTextW;
        GetDlgItemTextU=GetDlgItemTextW;
        GetWindowTextLengthU=GetWindowTextLengthW;
        GetVolumeLabelU=GetVolumeLabelW;
        if(NULL==shell32)
            shell32=LoadLibrary("shell32.dll");
        if(NULL!=shell32)
            Shell_NotifyIconU=(Shell_NotifyIconU_t *)GetProcAddress(
                shell32, "Shell_NotifyIconW");
        if(NULL==Shell_NotifyIconU)
            Shell_NotifyIconU=Shell_NotifyIconWtoA;
        }
    else
        {
        GetNonClientMetricsU=GetNonClientMetricsA;
        DialogBoxIndirectParamU=DialogBoxIndirectParamA;
        CreateDialogIndirectParamU=CreateDialogIndirectParamA;
        InsertMenuItemU=InsertMenuItemWtoA;
        DlgItemReplaceSelRcU=DlgItemReplaceSelRcA;
        SetDlgItemTextU=SetDlgItemTextWtoA;
        GetDlgItemTextU=GetDlgItemTextWtoA;
        GetWindowTextLengthU=GetWindowTextLengthA;
        Shell_NotifyIconU=Shell_NotifyIconWtoA;
        GetVolumeLabelU=GetVolumeLabelA;
        }
    }

BOOL IsRealUnicode(void)
    {
    return OSRealUnicode;
    }

char *TextBufWtoA(WCHAR const *textw)
    {
    char *texta;
    int len;

    len=WideCharToMultiByte(
        CP_ACP, WC_COMPOSITECHECK,
        textw, -1,
        NULL, 0,
        NULL, NULL
        );
    if(0==len)
        return NULL;

    texta=malloc(len);
    if(NULL==texta)
        return NULL;

    WideCharToMultiByte(
        CP_ACP, WC_COMPOSITECHECK,
        textw, -1,
        texta, len,
        NULL, NULL
        );

    return texta;
    }

WCHAR *TextBufAtoW(char const *texta)
    {
    WCHAR *textw;
    int len;

    len=MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, texta, -1, NULL, 0);
    if(0==len)
        return NULL;

    textw=malloc(len*sizeof(*textw));
    if(NULL==textw)
        return NULL;

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, texta, -1, textw, len);

    return textw;
    }

int LoadStringU(HINSTANCE hinst, UINT id, WCHAR *buf, int buflen, int lng)
    {
    WCHAR *w;
    int len;

    w=MapStringU(hinst, id, lng, &len);
    if(w==NULL)
        return 0;

    if(buf!=NULL)
        {
        /* Adjust length */
        if(len>=buflen)
            len=buflen-1;

        /* Copy string into buffer */
        memcpy(buf, w, len*sizeof(*buf));
        buf[len]=0;
        }

    return len;
    }

WCHAR *MapStringU(HINSTANCE hinst, UINT id, int lng, int *len)
    {
    HRSRC rcr;
    HGLOBAL rcg;
    WCHAR *w;
    unsigned i;

    rcr=FindResourceEx(hinst, RT_STRING, MAKEINTRESOURCE((id>>4)+1), lng);
    if(rcr==NULL)
        return NULL;
    rcg=LoadResource(hinst, rcr);
    if(rcg==NULL)
        return NULL;
    w=LockResource(rcg);
    if(w==NULL)
        return NULL;

    /* Find string in table part */
    for(i=0; i<(id&0x0F); i++)
        w+=1+*w;

    /* Save string length (in characters) */
    *len=*w;

    return w+1;
    }

WCHAR *strcpyU(WCHAR *to, WCHAR const *from)
    {
    while((*to++=*from++)) {}
    return to-1;
    }

#if 0
int strlenU(WCHAR *str)
    {
    int len=0;

    while(*str++!=0) len++;

    return len;
    }
#endif

static BOOL GetNonClientMetricsW(NONCLIENTMETRICSW *ncmw)
    {
    ncmw->cbSize = sizeof(*ncmw);
    return SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(*ncmw), ncmw, 0);
    }

static BOOL GetNonClientMetricsA(NONCLIENTMETRICSW *ncmw)
    {
    NONCLIENTMETRICSA ncma;

    ncma.cbSize=sizeof(ncma);
    if(!SystemParametersInfoA(SPI_GETNONCLIENTMETRICS, sizeof(ncma), &ncma, 0))
        return FALSE;

    ncmw->cbSize=sizeof(*ncmw);
    ncmw->iBorderWidth=ncma.iBorderWidth;        
    ncmw->iScrollWidth=ncma.iScrollWidth;        
    ncmw->iScrollHeight=ncma.iScrollHeight;       
    ncmw->iCaptionWidth=ncma.iCaptionWidth;       
    ncmw->iCaptionHeight=ncma.iCaptionHeight;      
    ncmw->iSmCaptionWidth=ncma.iSmCaptionWidth;     
    ncmw->iSmCaptionHeight=ncma.iSmCaptionHeight;    
    ncmw->iMenuWidth=ncma.iMenuWidth;          
    ncmw->iMenuHeight=ncma.iMenuHeight;         
    LogfontAtoW(&ncma.lfCaptionFont, &ncmw->lfCaptionFont);
    LogfontAtoW(&ncma.lfSmCaptionFont, &ncmw->lfSmCaptionFont);
    LogfontAtoW(&ncma.lfMenuFont, &ncmw->lfMenuFont);     
    LogfontAtoW(&ncma.lfStatusFont, &ncmw->lfStatusFont);   
    LogfontAtoW(&ncma.lfMessageFont, &ncmw->lfMessageFont);  
    return TRUE;
    }

static void LogfontAtoW(LOGFONTA *lfa, LOGFONTW *lfw)
    {
    lfw->lfHeight=lfa->lfHeight;               
    lfw->lfWidth=lfa->lfWidth;                
    lfw->lfEscapement=lfa->lfEscapement;           
    lfw->lfOrientation=lfa->lfOrientation;          
    lfw->lfWeight=lfa->lfWeight;               
    lfw->lfItalic=lfa->lfItalic;               
    lfw->lfUnderline=lfa->lfUnderline;            
    lfw->lfStrikeOut=lfa->lfStrikeOut;            
    lfw->lfCharSet=lfa->lfCharSet;              
    lfw->lfOutPrecision=lfa->lfOutPrecision;         
    lfw->lfClipPrecision=lfa->lfClipPrecision;        
    lfw->lfQuality=lfa->lfQuality;              
    lfw->lfPitchAndFamily=lfa->lfPitchAndFamily;       
    MultiByteToWideChar(
        CP_ACP, MB_PRECOMPOSED,
        lfa->lfFaceName, -1,
        lfw->lfFaceName, LF_FACESIZE
        );
    }

static BOOL WINAPI InsertMenuItemWtoA(
    HMENU menu,
    UINT item,
    BOOL byposition,
    const MENUITEMINFOW *miiw
    )
    {
    MENUITEMINFOA miia;
    char *buf=NULL;
    BOOL ret;

    miia.cbSize=sizeof(miia);          
    miia.fMask=miiw->fMask;           
    miia.fType=miiw->fType;           
    miia.fState=miiw->fState;          
    miia.wID=miiw->wID;             
    miia.hSubMenu=miiw->hSubMenu;       
    miia.hbmpChecked=miiw->hbmpChecked;  
    miia.hbmpUnchecked=miiw->hbmpUnchecked;
    miia.dwItemData=miiw->dwItemData;     
    miia.cch=miiw->cch;             
    if((miiw->fType&MFT_STRING)==MFT_STRING)
        {
        buf=TextBufWtoA(miiw->dwTypeData);
        miia.dwTypeData=buf;
        }
    else
        miia.dwTypeData=(char *)miiw->dwTypeData;    

    ret=InsertMenuItemA(menu, item, byposition, &miia);

    if(buf!=NULL)
        free(buf);
    
    return ret;
    }

static void DlgItemReplaceSelRcW(HWND dlg, int item, UINT rcid)
    {
    WCHAR buf[64];

    if(LoadStringU(NULL, rcid, buf, sizeof(buf)/sizeof(*buf), 0)>0)
        SendDlgItemMessageW(dlg, item, EM_REPLACESEL, FALSE, (LPARAM)buf);
    }

static void DlgItemReplaceSelRcA(HWND dlg, int item, UINT rcid)
    {
    char buf[64];

    if(LoadStringA(NULL, rcid, buf, sizeof(buf)/sizeof(*buf))>0)
        SendDlgItemMessageA(dlg, item, EM_REPLACESEL, FALSE, (LPARAM)buf);
    }

static BOOL WINAPI SetDlgItemTextWtoA(HWND dlg, int item, WCHAR const *textw)
    {
    char *texta;
    BOOL ret;

    texta=TextBufWtoA(textw);
    if(NULL==texta)
        return FALSE;

    ret=SetDlgItemTextA(dlg, item, texta);
    
    free(texta);

    return ret;
    }

static UINT WINAPI GetDlgItemTextWtoA(
    HWND dlg, int item,
    WCHAR *textw, int max
    )
    {
    char *texta;
    UINT ret;

    texta=malloc(max);
    if(texta==NULL)
        return FALSE;

    ret=GetDlgItemTextA(dlg, item, texta, max);
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, texta, -1, textw, max);
    
    free(texta);

    return ret;
    }

static BOOL WINAPI Shell_NotifyIconWtoA(DWORD msg, NOTIFYICONDATAW *nidw)
    {
    NOTIFYICONDATAA nida;

    nida.cbSize=sizeof(nida);
    nida.hWnd=nidw->hWnd;
    nida.uID=nidw->uID;
    nida.uFlags=nidw->uFlags;
    nida.uCallbackMessage=nidw->uCallbackMessage;
    nida.hIcon=nidw->hIcon;
    if(NIF_TIP==(nidw->uFlags&NIF_TIP))
        {
        WideCharToMultiByte(
            CP_ACP, WC_COMPOSITECHECK,
            nidw->szTip, -1,
            nida.szTip, sizeof(nida.szTip),
            NULL, NULL
            );
        }

    return Shell_NotifyIconA(msg, &nida);
    }

WCHAR *Num(WCHAR *to, int n)
    {
    int t;
    WCHAR *s;

    for(t=10; n/t!=0; t*=10)
        to++;
    s=to;
    *++to=0;
    do
        {
        *s--=L'0'+n%10;
        n/=10;
        }
    while(n!=0);

    return to;
    }

WCHAR *Rcs(WCHAR *to, int id)
    {
    WCHAR *w;
    int len=0;

    w=MapStringU(NULL, id, 0, &len);
    if(w!=NULL)
        memcpy(to, w, len*sizeof(*w));
    to[len]=0;

    return to+len;
    }

WCHAR *Crlf(WCHAR *to)
    {
    *to++=L'\r';
    *to++=L'\n';
    *to=0;

    return to;
    }
        
WCHAR *Stra(WCHAR *to, char *from)
    {
    int len;

    len=MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from, -1, NULL, 0);
    if(len>0)
        {
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from, -1, to, len);
        to+=len-1;
        }

    return to;
    }

static BOOL GetVolumeLabelW(WCHAR *root, WCHAR *label, int size)
    {
    return GetVolumeInformationW(root, label, size, NULL, NULL, NULL, NULL, 0);
    }

static BOOL GetVolumeLabelA(WCHAR *root, WCHAR *label, int size)
    {
    char *rootA;
    char *labelA;
    BOOL res;

    rootA=TextBufWtoA(root);
    if(NULL==rootA)
        return FALSE;

    labelA=malloc(size);
    if(NULL==labelA)
        {
        free(rootA);
        return FALSE;
        }

    res=GetVolumeInformationA(rootA, labelA, size, NULL, NULL, NULL, NULL, 0);
    if(res)
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, labelA, -1, label, size);

    free(labelA);
    free(rootA);

    return res;
    }
