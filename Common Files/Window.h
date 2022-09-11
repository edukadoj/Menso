// this file is for creating & runng a window

#ifndef WindowH
#define WindowH

#pragma hdrstop

#include "General.h"
#include "WString.h"
#include <vector>


#ifdef __BORLANDC__
#pragma option -w-pch
#endif

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;


//#define LAYERED LAYERED


enum
{
    WM_CPPBUILDER_PICTURE = 100000
};

extern Color back_color;

class Window
{
public:
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    HWND hwnd;
    HDC          hdc;
    bool is_hidden;
    std::wstring class_name;

    volatile bool icon_is_animating;

private:
    WNDPROC WindowProcedure;
    int x;
    int y;
    int width;
    int height;
    int nCmdShow;

    void register_class()
    {
    START:
        HINSTANCE hInstance = GetModuleHandle(0);

        WNDCLASSEX wcx;
        wcx.cbSize = sizeof(WNDCLASSEX);
        wcx.style = CS_OWNDC | CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW/*| CS_DROPSHADOW*/;
        wcx.lpfnWndProc = WindowProcedure;
        wcx.cbClsExtra = 0;
        wcx.cbWndExtra = 0;
        wcx.hInstance = hInstance;
        wcx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(300));
        wcx.hCursor = LoadCursor(0, IDC_ARROW);
        wcx.hbrBackground = CreateSolidBrush(RGB(back_color.GetR(), back_color.GetG(), back_color.GetB()));
        wcx.lpszMenuName = 0;
        wcx.lpszClassName = class_name.c_str();
        wcx.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(300));;

        if (RegisterClassEx(&wcx) == 0)
        {
            if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
            {
                class_name[2]++;
                if (class_name[2] > 'z')
                    print_error("program failed to register window class");
                goto START;
            }
            print_error("program failed to register window class");
        }
    }

    void create_window(const int&)
    {
        hwnd = CreateWindowEx
        (
#ifdef LAYERED
            WS_EX_LAYERED,
#else
            0,
#endif
            class_name.c_str(),// lpClassName
            window_title,// lpWindowName
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS,// dwStyle
            x,// x
            y,// y
            width,// nWidth
            height,// nHeight
            0,// hWndParent
            0,// hMenu
            0,// hInstance
            0                    // lpParam
        );

#ifdef LAYERED
        SetLayeredWindowAttributes
        (
            hwnd,
            RGB(0, 0, 0),
            0,
            0
        );
#else
        AnimateWindow(hwnd, 200, AW_BLEND);
#endif

        Activate();
        UpdateWindow(hwnd);
        hdc = GetDC(hwnd);
        is_topmost = false;
    }

    void MessageLoop()
    {
        MSG Msg;
        while (GetMessage(&Msg, 0, 0, 0) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }

public:
    bool is_topmost;
    Unsigned last_ico_num;
    std::vector<Unsigned> icon_ids;

    void ShowTrayIcon(Unsigned ico_num = 300)
    {
        HINSTANCE hInstance = GetModuleHandle(0);

        WString starting_tip = WString(window_title) + L" is running.\nClick the icon to hide/show the window.";

        NOTIFYICONDATA nd;
        nd.cbSize = sizeof(NOTIFYICONDATA);
        nd.hWnd = hwnd;
        nd.uID = 1000;
        nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
        nd.uCallbackMessage = 10000;
        nd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ico_num));

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
        wcscpy_s(nd.szTip, WString(window_title).Size(), window_title);
#else
        wcscpy(nd.szTip, window_title);
#endif

        nd.dwState = NIS_SHAREDICON;
        nd.dwStateMask = NIS_SHAREDICON;

 
#if defined (_MSC_VER) && (_MSC_VER >= 1020)
        wcscpy_s(nd.szInfo, starting_tip.Size(), WCS(starting_tip));
#else
        wcscpy(nd.szInfo, WCS(starting_tip)); // balloon text
#endif

        nd.uTimeout = 300000;
        nd.uVersion = NOTIFYICON_VERSION;


#if defined (_MSC_VER) && (_MSC_VER >= 1020)
        wcscpy_s(nd.szInfoTitle, sizeof(L"Start")/sizeof(wchar_t) , L"Start");
#else
        wcscpy(nd.szInfoTitle, L"Start"); // balloon title
#endif

        nd.dwInfoFlags = NIIF_WARNING;

        Shell_NotifyIcon(NIM_ADD, &nd);

        last_ico_num = ico_num;
    }

    void HideTrayIcon(Unsigned ico_num = 300)
    {
        HINSTANCE hInstance = GetModuleHandle(0);

        NOTIFYICONDATA nd;
        nd.cbSize = sizeof(NOTIFYICONDATA);
        nd.hWnd = hwnd;
        nd.uID = 1000;
        nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP /*| NIF_INFO*/;
        nd.uCallbackMessage = 10000;
        nd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ico_num));
        nd.dwState = NIS_SHAREDICON;
        nd.dwStateMask = NIS_SHAREDICON;
        nd.uTimeout = 300000;
        nd.uVersion = NOTIFYICON_VERSION;
        nd.dwInfoFlags = NIIF_WARNING;

        Shell_NotifyIcon(NIM_DELETE, &nd);

        last_ico_num = MAXUnsigned;
    }


    void ChangeTrayIcon(Unsigned ico_num)
    {
        HINSTANCE hInstance = GetModuleHandle(0);

        NOTIFYICONDATA nd;
        nd.cbSize = sizeof(NOTIFYICONDATA);
        nd.hWnd = hwnd;
        nd.uID = 1000;
        nd.uFlags = NIF_ICON;
        nd.uCallbackMessage = 10000;
        nd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ico_num));
        nd.dwState = NIS_SHAREDICON;
        nd.dwStateMask = NIS_SHAREDICON;
        nd.uTimeout = 300000;
        nd.uVersion = NOTIFYICON_VERSION;
        nd.dwInfoFlags = NIIF_WARNING;

        Shell_NotifyIcon(NIM_MODIFY, &nd);

        last_ico_num = ico_num;
    }

    static DWORD _stdcall IconAnimator(void* lpParam)
    {
        Window* pwin = (Window*)lpParam;
        while (true)
        {
            Unsigned size = pwin->icon_ids.size();
            if (size == 0)
                return 1;
            for (Unsigned i = 0; i < size; i++)
            {
                if (pwin->icon_is_animating == false)
                {
                    pwin->ChangeTrayIcon(300);
                    return 0;
                }
                Sleep(200);
                pwin->ChangeTrayIcon(pwin->icon_ids[i]);
            }
        }
    }

    void StartAnimatingIcons()
    {
        if (icon_is_animating == true)
            return;
        icon_is_animating = true;
        DWORD thread_id;

        CreateThread
        (
            0,           // default security attributes
            0,           // use default stack size
            IconAnimator,  // thread function
            this, // argument to thread function
            0,           // use default creation flags
            &thread_id
        );
    }

    void EndAnimatingIcons()
    {
        icon_is_animating = false;
    }

    void AddIcon(Unsigned id)
    {
        icon_ids.push_back(id);
    }

    void Activate()
    {
        SetForegroundWindow(hwnd);
        SetActiveWindow(hwnd);
    }

    void Show()
    {
        ShowWindow(hwnd, SW_SHOW);
        SetActiveWindow(hwnd);
        ShowWindow(hwnd, SW_RESTORE);
        SwitchToThisWindow(hwnd, TRUE);
        is_hidden = false;
    }

    void Hide()
    {
        ShowWindow(hwnd, SW_HIDE);
        is_hidden = true;
    }

    void TopMost()
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        is_topmost = true;
    }

    void NotTopMost()
    {
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        is_topmost = false;
    }

    void Run()
    {
        MessageLoop();
    }

public:

    Window(WNDPROC WindowProcedure,
        int x,
        int y,
        int width,
        int height,
        int nCmdShow)
        : hwnd(0), WindowProcedure(WindowProcedure),
        x(x), y(y), width(width), height(height), nCmdShow(nCmdShow), class_name(L"classname")
    {
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, 0);
        register_class();
        create_window(nCmdShow);
        is_hidden = false;
        //ShowTrayIcon();
        last_ico_num = MAXUnsigned;
        icon_is_animating = false;
    }

    ~Window()
    {
        ReleaseDC(hwnd, hdc);
        GdiplusShutdown(gdiplusToken);
        HideTrayIcon();
        UnregisterClass(class_name.c_str(), GetModuleHandle(0));
    }
};

#endif
