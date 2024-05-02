#ifndef __DIALOGS_H__
#define __DIALOGS_H__

struct DLG_Control
    {
    WCHAR const * const Class;
    DWORD const Style;
    DWORD const ExStyle;
    BOOL const Solid;
    void (*Init)(HWND window, int id);
    void (*Estimate)(HWND window, int id, SIZE *sz, SIZE *max);
    void (*Move)(HWND window, int id, int x, int y, int w, int h);
    void (*PreInit)(void);
    };

struct DLG_Item
    {
    struct DLG_Control *Control;
    WORD Id;
    WCHAR const *Title;
    int rcid;
    WORD GroupId;
    INT_PTR (*ItemProc)(
        HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
    };

struct DLG_ItemData
    {
    WORD Id;
    WORD GroupId;
    struct DLG_Control *Control;
    INT_PTR (*ItemProc)(
        HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
    int SizeKnown;
    SIZE ESZ;
    SIZE MSZ;
    };

struct DLG_Data
    {
    void *param;
    struct DLG_ItemData *Item;
    int count;
    POINT MinTrackSize;
    };

void *PackDialogU(
    DWORD style,
    DWORD exstyle,
    int x, int y,
    int cx, int cy,
    WCHAR const *titlestr,
    int titleid,
    LOGFONTW *fnt,
    int itemcount,
    struct DLG_Item *items
    );
size_t AdjustToDWORD(size_t n);
void DlgMoveItem(HWND window, int id, int x, int y, int cx, int cy);
INT_PTR DlgRunU(
    HWND window,
    WCHAR const *strtitle,
    int rctitle,
    DWORD style,
    DWORD exstyle,
    struct DLG_Item *items,
    int itemcount,
    void *param
    );
HWND DlgShowU(
    HWND window,
    WCHAR const *strtitle,
    int rctitle,
    DWORD style,
    DWORD exstyle,
    struct DLG_Item *items,
    int itemcount,
    void *param
    );
void GetWindowStrSize(HWND window, WCHAR *str, SIZE *sz);
void GetDlgItemTextSize(HWND window, int id, SIZE *sz);

extern struct DLG_Control CtlGroupV;
extern struct DLG_Control CtlGroupH;

#define GRP_AUTO    0
#define GRP_TITLE_AUTO    L"0"
#define GRP_FIXED   1
#define GRP_TITLE_FIXED   L"1"
#define GRP_MAX     2
#define GRP_TITLE_MAX     L"2"
#define GRP_MAX_CX  3
#define GRP_TITLE_MAX_CX  L"3"
#define GRP_MAX_CY  4
#define GRP_TITLE_MAX_CY  L"4"
#define GRP_FILL    5
#define GRP_TITLE_FILL    L"5"
#define GRP_FILL_CX 6
#define GRP_TITLE_FILL_CX L"6"
#define GRP_FILL_CY 7
#define GRP_TITLE_FILL_CY L"7"

#define ITEM_MSZ_FIXED 0
#define ITEM_MSZ_MAX   1
#define ITEM_MSZ_FILL  2

#endif /* __DIALOGS_H__ */
