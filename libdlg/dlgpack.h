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
    BOOL (*ItemProc)(
        HWND window, WORD id, UINT msg, WPARAM wParam, LPARAM lParam);
    };

struct DLG_ItemData
    {
    WORD Id;
    WORD GroupId;
    struct DLG_Control *Control;
    BOOL (*ItemProc)(
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

