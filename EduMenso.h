#ifndef EduMensoH
#define EduMensoH
#include "Main.h"
#include "GetNthDeco.h"
#include "List.h"
#include "MainLists.h"
#include "Space.h"

extern void ReportError(char* c, Unsigned offset);

extern EditBox* math_edit;
extern EditBox* operators_edit;
extern EditBox* report_edit;
extern EditBox* things_edit;
extern EditBox* truths_edit;

class EduMenso
{
public:
    static WString NavigateFile(HWND hwnd)
    {
        SetCurrentDirectoryW(WCS(GetDesktop()));
        WString addr;
        OPENFILENAME ofn;
        wchar_t file_name[MAX_PATH] = L"";
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Text Files (*.text)\0*.text\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = file_name;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = L"text";
        if (GetOpenFileName(&ofn))
            addr = file_name;
        return addr;
    }

    static WString SaveFile(HWND hwnd)
    {
        SetCurrentDirectoryW(WCS(GetDesktop()));
        WString addr;
        OPENFILENAME ofn;
        wchar_t file_name[MAX_PATH] = L"";
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Text Files (*.text)\0*.text\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = file_name;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = L"text";
        if (GetSaveFileName(&ofn))
            addr = file_name;
        return addr;
    }

    static void PrintList(EditBox* peb, List& l)
    {
        for (Unsigned i = 0; i < l.Size(); i++)
        {
            String str = l[i];
            peb->cout((char*)CS(str)).cout("\n");
        }
    }

    static void ReportSuccess(const char* c, Unsigned offset)
    {
        report_edit->Clear();
        report_edit->WriteString(c);
        GetNthDeco gnd(math_edit, offset);
        if (gnd.y != MAXUnsigned)
        {
            math_edit->GotoXY(gnd.x, gnd.y);
            math_edit->Focus();
        }
        report_edit->HideBlinker();
    }

    static void MainFunction()
    {
        try
        {
            truths_edit->Clear(false);
            things_edit->Clear(false);
            operators_edit->Clear(false);
            report_edit->Clear(false);
            error_edit->Clear(false);
            error_edit->WriteString("Checking started.");
            pmain->WaitPaint();
            MainLists ML(GetAppDataPath() + L"Menso.txt");
            Space S(ML, 1);
            ReportSuccess("Success! No Errors", S.end);
            Sleep(400);
            PrintList(truths_edit, ML.truths);
            PrintList(things_edit, ML.things);
            PrintList(operators_edit, ML.operators);
            error_edit->WriteString("\n" "Checking ended.");
            pmain->WaitPaint();
        }
        catch (...)
        {
        }
    }

    static DWORD _stdcall ThreadFunc(void* /*lpParam*/)
    {
        if (!pmain->StartThread())
        {
            error_edit->WriteString("\n" "Please wait until checking ends.");
            page[pmain->tell_page_num].Activate();
            return 1;
        }
        MainFunction();
        pmain->EndThread();
        return 0;
    }

    void writef(char* c)
    {
        error_edit->cout(c).cout("\n");
        page[2].Activate();
    }

    //--------------------------------------------------

    static void CheckTheText(WString param)
    {
        math_edit->DelCppCommentSaveAs(GetAppDataPath() + L"Menso.txt");
        DWORD thread_id, thread_arg = 0;
        CreateThread
        (
            0,           // default security attributes
            0,           // use default stack size
            ThreadFunc,  // thread function
            &thread_arg, // argument to thread function
            0,           // use default creation flags
            &thread_id
        );
    }

    static void OpenButton(WString param)
    {
        HWND hwnd = ToType<HWND>(param);
        WString addr = NavigateFile(hwnd);
        if (addr != L"")
            math_edit->Open(addr);
    }

    static void SaveAsButton(WString param)
    {
        HWND hwnd = ToType<HWND>(param);
        WString addr = SaveFile(hwnd);
        if (addr != L"")
            math_edit->SaveAs(addr);
    }

    static void AfterEnter(WString data)
    {
        EditBox* p = ToType<EditBox*>(data);
        Unsigned cur = p->GetCurrentLineNumber();
        if (cur == 1)
            return;
        WString LINE = p->GetLine(cur - 1);
        Unsigned len = LINE.Size();
        Unsigned n = 0;
        wchar_t wch = 0;
        do
        {
            if (n >= len)
                break;
            wch = LINE[n];
            n++;
        } while (WString(L" 	").IsExactSuperstring(wch));
        if (n == 0)
            return;

        if (wch == L'{' && p->GetLine(cur + 1) == L"")
        {
            if (2 <= n)
            {
                p->WriteString(LINE[0][n - 2] + L"	");
                p->InsertLine(cur + 1, LINE[0][n - 2] + L"}");
            }
            else
            {
                p->WriteString(L"    ");
                p->InsertLine(cur + 1, L"}");
            }
        }
        else
        {
            if (2 <= n)
                p->WriteString(LINE[0][n - 2]);
        }
    }
};


#endif