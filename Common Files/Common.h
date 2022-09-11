#ifndef CommonH
#define CommonH
#include "General.h"
#include "Window.h"
#include "Draw.h"
#include "EditBox.h"
#include "WString.h"
#include "WFile.h"
#include "Clipboard.h"
#include "Write.h"
#include "Button.h"
#include "Processes.h"
#include "StaticText.h"
#include "Page.h"
#include "ProgressReport.h"
#include "Scroll.h"

#include <filesystem>


extern Unsigned this_instance;

extern Window* pwin;
extern Devcon* pdevcon;
extern Color back_color;

extern Page* ppage;
extern EditBox* error_edit;

#define page (*ppage)

class Common
{
    static bool allow_winproc;
public:
    static Unsigned check_period1;
    static Unsigned check_period2;
    static Unsigned tell_page_num;
    static volatile bool paint_aternate;

    static void AlternatePaint()
    {
        if (paint_aternate == true)
            paint_aternate = false;
        else
            paint_aternate = true;
    }

    static LRESULT _stdcall WndProc
    (
        HWND hwnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam
    )
    {
        if (!allow_winproc)
            return DefWindowProc(hwnd, msg, wParam, lParam);

        switch (msg)
        {
        case WM_CREATE:
        {
            return 0;
        }

        case WM_WINDOWPOSCHANGING:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_WINDOWPOSCHANGED:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_MOVE:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_ACTIVATE:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_SIZE:
        {
            return 0;
        }

        case WM_SIZING:
        {
            return 0;
        }

        case WM_ERASEBKGND:
        {
            DefWindowProc(hwnd, msg, (Unsigned)pdevcon->v_hdc, lParam);
            page.Paint();
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            /*if(lParam != 0)
            {
                Picture* pp = (Picture*)lParam;
                pp -> Next();
                //return 0;
            } */

            page.Paint();
            pdevcon->UpdateHDC();
            EndPaint(hwnd, &ps);
            AlternatePaint(); // paint alternation to show painting used for other threads
            return 0;
        }

        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_CLOSE:
            {
                OnClose();
                pdevcon->UpdateHDC();
                break;
            }
            case SC_MINIMIZE:
                OnMinimize();
                pdevcon->UpdateHDC();
                break;

            case 61441:
            case 61442:
            case 61443:
            case 61444:
            case 61445:
            case 61446:
            case 61447:
            case 61448:
                break;

            case 61490: // doubleclick on titlebar
                OnDoubleClickTitlebar();
                pdevcon->UpdateHDC();
                break;

            case 61458: // click on titlebar
                OnClickTitlebar();
                pdevcon->UpdateHDC();
                break;

            case SC_MAXIMIZE:
                OnRestore();
                pdevcon->UpdateHDC();
                break;
                /*case SC_MAXIMIZE:
                case SC_RESTORE:
                    Paint();
                case SC_HOTKEY:
                    pwin -> Show();*/

                    /*case 274:
                         DefWindowProc(hwnd,msg,wParam,lParam);*/
            default:
                DefWindowProc(hwnd, msg, wParam, lParam);
            }
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            page.wmMousewheel(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            TrackLeave(hwnd);
            page.wmMousemove(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_MOUSELEAVE:
        {
            page.wmMouseleave(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_MOUSEHOVER:
        {

            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            page.wmLbuttondown(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_LBUTTONUP:
        {
            page.wmLbuttonup(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            page.wmRbuttondown(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_CHAR:
        {
            page.wmChar(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_SYSCHAR:
        {
            return 0;
        }

        case WM_TIMER:
        {
            return 0;
        }

        case WM_KEYDOWN:
        {
            page.wmKeydown(wParam, lParam);
            pdevcon->UpdateHDC();
            return 0;
        }

        case WM_SYSKEYDOWN:
        {
            if (wParam == VK_F4) // Alt + F4
            {
                page[tell_page_num].Activate();
                pdevcon->UpdateHDC();
            }
            return 0;
        }

        case WM_SYSKEYUP:
        {
            return 0;
        }

        case 10000: // tray icon
        {
            switch (lParam)
            {
            case WM_LBUTTONDOWN:
            {
                page[0].Activate();
                pdevcon->UpdateHDC();
                SwitchVis();
                break;
            }
            case WM_RBUTTONDOWN:
            {
                page[tell_page_num].Activate();
                pdevcon->UpdateHDC();
                SwitchVis();
                break;
            }
            case WM_MBUTTONDOWN:
            {
                SwitchVis();
                break;
            }
            }

            return 0;
        }

        case WM_NCPAINT:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_NCCALCSIZE:
        {
            DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;
        }

        case WM_CPPBUILDER_PICTURE:	// animating pics
        {
            Picture* pp = (Picture*)lParam;
            pp->Next();
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    static void CALLBACK TimerProc(HWND /*hwnd*/, Unsigned /*msg*/, Unsigned idEvent, Unsigned dwTime)
    {
        switch (idEvent)
        {
        case 1000:
        {
            page.wmTimer(idEvent, dwTime);
            pdevcon->UpdateHDC();
            if (check_period2 == 0)
                OnTimer2();
            break;
        }
        case 1001:
        {
            page.wmTimer2(idEvent, dwTime);
            OnTimer2();
            pdevcon->UpdateHDC();
        }
        }
    }

    static void SetPeriod1(Unsigned period)
    {
        KillTimer(pwin->hwnd, 1000);
        if (period != 0)
            SetTimer(pwin->hwnd, 1000, (UINT)period, (TIMERPROC)TimerProc);
        check_period1 = period;
    }

    static void SetPeriod2(Unsigned period)
    {
        KillTimer(pwin->hwnd, 1001);
        if (period != 0)
            SetTimer(pwin->hwnd, 1001, (UINT)period, (TIMERPROC)TimerProc);
        check_period2 = period;
        OnSetTimer2();
    }

    static void KillTimer1()
    {
        SetPeriod1(0);
    }

    static void KillTimer2()
    {
        SetPeriod2(0);
    }

    static void HideWin()
    {
        if (pwin->is_hidden == false)
        {
            SetPeriod1(check_period1 * 10);
            pwin->Hide();
            ppage->StopPaint();
            pdevcon->PreventUpdate();
        }
    }

    static void ShowWin()
    {
        if (pwin->is_hidden == true)
        {
            pdevcon->AllowUpdate();
            SetPeriod1(check_period1 / 10);
            pwin->Show();
            ppage->StartPaint();
        }
    }

    static void SwitchVis()
    {
        if (pwin->is_hidden)
            ShowWin();
        else
            HideWin();
    }

    static void TrackLeave(HWND hwnd)
    {
        TRACKMOUSEEVENT tme =
        {
            sizeof(TRACKMOUSEEVENT),
            TME_LEAVE,
            hwnd,
            0
        };
        TrackMouseEvent(&tme);
    }

    static void About(WString data, bool copy = true)
    {
        error_edit->Clear();
        error_edit->WriteString
        (
            WString(window_title) +
            WString
            (
                L"\n"
                L"Copyright © 2009-2019" L"\n\n"
                L"Our telegram channel:" L"\n"
                L"https://t.me/edukadoj" L"\n\n"
                L", \"About\" copies the address"
            )
        );

        if (copy)
        {
            Clipboard cb;
            cb.Copy(L"https://t.me/edukadoj");
        }
    }

    static void Exit(WString data);
    static void OnClose();
    static void OnRestore();
    static void OnClickTitlebar();
    static void OnDoubleClickTitlebar();
    static void OnMinimize();
    static void OnSetTimer2();
    static void OnTimer2();

    //nonstatic))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
    std::vector<Button*> buttons;
    std::vector<EditBox*> editboxes;
    std::vector<StaticText*> statictexts;
    std::vector<Picture*> pictures;
    std::vector<ProgressReport*> progresses;
    std::vector<HScroll*> hscrolls;
    std::vector<Context*> contexts;

    int width;
    int height;
    int nCmdShow;

    Button* newButton(RectF rectf, WString wstr)
    {
        Button* ret = new Button(pdevcon->v_hdc, rectf, (wchar_t*)WCS(wstr));
        buttons.push_back(ret);
        return ret;
    }

    EditBox* newEditBox(RectF rectf)
    {
        EditBox* ret = new EditBox(pdevcon->v_hdc, rectf);
        editboxes.push_back(ret);
        return ret;
    }

    StaticText* newStaticText(WString wstr, float x, float y)
    {
        StaticText* ret = new StaticText(pdevcon->v_hdc, (wchar_t*)WCS(wstr), x, y);
        statictexts.push_back(ret);
        return ret;
    }

    Picture* newPicture(float x, float y)
    {
        Picture* ret = new Picture(pdevcon->v_hdc, x, y);
        pictures.push_back(ret);
        return ret;
    }

    ProgressReport* newProgressReport(RectF rectf)
    {
        ProgressReport* ret = new ProgressReport(pdevcon->v_hdc, rectf);
        progresses.push_back(ret);
        return ret;
    }

    HScroll* newHScroll(RectF box, Unsigned all)
    {
        HScroll* ret = new HScroll(pdevcon->v_hdc, box, all);
        hscrolls.push_back(ret);
        return ret;
    }

    Context* newContext(RectF box)
    {
        Context* ret = new Context(pdevcon->v_hdc, box);
        contexts.push_back(ret);
        return ret;
    }

    void DeleteAll()
    {
        if (pwin)
            HideWin();
        ppage->Empty();
        if (pdevcon)
        {
            if (pdevcon->v_hdc)
                DeleteDC(pdevcon->v_hdc);
        }

        size_t size;

#define DEL(objects)\
        size = objects.size();\
        for(size_t i = 0; i < size; i++)\
            delete objects[i];\
        objects.clear();

        DEL(buttons);
        DEL(editboxes);
        DEL(statictexts);
        DEL(pictures);
        DEL(progresses);
        DEL(hscrolls);
        DEL(contexts);

        if (pdevcon)
        {
            delete pdevcon;
            pdevcon = 0;
        }
        if (pwin)
        {
            pwin->HideTrayIcon();
            delete pwin;
            pwin = 0;
        }
        allow_winproc = false;
    }

    virtual void Definitions() = 0;	// abdtract
    virtual void AtStart() = 0;	    // abstract

    void Startup()
    {
        CheckInstanceNumber();
        pwin = new Window(WndProc, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nCmdShow);
        HDC v_hdc = newHDC(pwin->hdc);
        pdevcon = new Devcon(pwin->hdc, v_hdc);
        ppage = new Page;
        Definitions(); // must be after the above

        ppage->Startup();

        About("", false);
        SetPeriod1(400);
        AtStart();

    RUN:
        allow_winproc = true;
        try
        {
            pwin->Run();
        }
        catch (int a)
        {
            if (a == 1)
                goto RUN;
        }

        DeleteAll();
    }

    int MainAction()
    {
        int restart = 0;

    START:
        try
        {
            restart++;
            Startup();
        }
        catch (WString message)
        {
            DeleteAll();
            MessageBoxW
            (
                0,
                WCS(message + WString("                                  ")),
                window_title,
                MB_OK
            );
            if (restart < 10)
                goto START;
            else
            {
                pwin->Hide();
                MessageBoxA
                (
                    0,
                    "An error for 10th time.\nThe propgram terminates now",
                    app_name,
                    MB_OK
                );
                return 1;
            }
        }
        catch (...)
        {
            wchar_t c[MAX_PATH + 1];
            SHGetSpecialFolderPathW(0, c, CSIDL_APPDATA, 1);
            WString S = WString(c) + WString(L"\\CppBuilder2006\\") + WString(app_name);
            std::filesystem::path p = (wchar_t*)WCS(S);
            std::filesystem::remove_all(p);

            MessageBoxA
            (
                0,
                "An unexpected error ocurred.\nThe propgram terminates now",
                app_name,
                MB_OK
            );
            return 1;
        }
        return 0;
    }

    Unsigned instance_number;

    void CheckInstanceNumber()
    {
        HINSTANCE hInstance = GetModuleHandle(0);
        char c[MAX_PATH];
        GetModuleFileNameA(hInstance, c, MAX_PATH);
        String name = c;
        name.Reverse();
        name = name.CutOff("\\/");
        name.Reverse();
        Processes pr;
        this_instance = pr.NumberOfProcsWithName(name);
        if (instance_number < this_instance)
        {
            MessageBoxA(0, "already running", "Error", MB_OK);
            Suicide();
        }
    }

    volatile bool new_threads;

    bool StartThread()// thread prereqisites
    {
        if (!new_threads)
        {
            auto size = editboxes.size();
            for (decltype(size) i = 0; i < size; i++)
                editboxes[i]->NoNewLayer();
            new_threads = true;
            return true;
        }
        return false;
    }

    void EndThread()
    {
        auto size = editboxes.size();
        for (decltype(size) i = 0; i < size; i++)
            editboxes[i]->AllowNewLayer();
        new_threads = false;
    }

    void WaitPaint() // used by threads
    {
        bool b = paint_aternate;
        SendMessage(pwin->hwnd, WM_PAINT, 0, 0);
        while (paint_aternate == b)
        {
            Sleep(20);
            SendMessage(pwin->hwnd, WM_PAINT, 0, 0);
        }
    }

    bool ClosingAllowed()
    {
        if (new_threads)
        {
            error_edit->Clear(false);
            error_edit->WriteString("Closing this window may damage""\n"
                "the files. Please try again later.""\n\n"
                "Press Exit button to exit\nby force.");
            page[tell_page_num].Activate();
            return false;
        }
        return true;
    }

public:
    Common(int width, int height, int nCmdShow)
        : width(width), height(height), nCmdShow(nCmdShow)
    {
        instance_number = 1;
        new_threads = false;
    }
};

#endif