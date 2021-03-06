// Dolwin about dialog
#include "pch.h"

static bool opened = false;
static HWND dlgAbout;

// dialog procedure
static INT_PTR CALLBACK AboutProc(
    HWND    hwndDlg,    // handle to dialog box
    UINT    uMsg,       // message
    WPARAM  wParam,     // first message parameter
    LPARAM  lParam      // second message parameter
)
{
    UNREFERENCED_PARAMETER(lParam);

    TCHAR tmpbuf[256];    

    switch(uMsg)
    {
        // prepare swap dialog
        case WM_INITDIALOG:
        {
            dlgAbout = hwndDlg;
            ShowWindow(dlgAbout, SW_NORMAL);
            SendMessage(dlgAbout, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DOLWIN_ICON)));
            CenterChildWindow(GetParent(dlgAbout), dlgAbout);

            _stprintf_s(
                tmpbuf,
                _countof(tmpbuf) - 1,
                APPNAME _T(" - ") APPDESC _T("\n")
                _T("Copyright 2003,2004,2020, Dolwin team\n")
                _T("Build version %s (%s, %s) ")
#ifdef      _DEBUG
                _T("Debug ")
#else
                _T("Release ")
#endif

#if         _M_X64
                _T("x64")
#else
                _T("x86")
#endif

                ,
                APPVER,
                _T(__DATE__),
                _T(__TIME__)
            );
            SetDlgItemText(dlgAbout, IDC_ABOUT_RELEASE, tmpbuf);
            return TRUE;
        }

        // close button -> kill about
        case WM_CLOSE:
        {
            DestroyWindow(dlgAbout);
            dlgAbout = NULL;
            opened = false;
            break;
        }
        case WM_COMMAND:
        {
            if(wParam == IDCANCEL)
            {
                DestroyWindow(dlgAbout);
                dlgAbout = NULL;
                opened = false;
                return TRUE;
            }
            if(wParam == IDOK)
            {
                DestroyWindow(dlgAbout);
                dlgAbout = NULL;
                opened = false;
                return TRUE;
            }
            break;
        }
    }

    return FALSE;
}

// non-blocking call
void AboutDialog(HWND hwndParent)
{
    if(opened) return;

    // create modeless dialog
    CreateDialog(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_ABOUT),
        hwndParent,
        AboutProc
    );

    opened = true;
}
