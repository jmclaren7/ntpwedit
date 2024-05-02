PROJECT(dlglib C)

ADD_DEFINITIONS(-D_WIN32_WINNT=0x0500)

ADD_LIBRARY(dlglib
    ctl_button.c
    ctl_checkbox.c
    ctl_edit.c
    ctl_groupbox.c
    ctl_hotkey.c
    ctl_image.c
    ctl_label.c
    ctl_listview.c
    ctl_progress.c
    ctl_richview.c
    dialogs.c
    dlgpack.c
    unicode.c
    )
