#pragma hdrstop
#include "Main.h"
#include "EduMenso.h"

extern Main* pmain = 0;

extern const char* app_name = "Edukadoj Menso Mind Logic Simulation";
extern const wchar_t* window_title = L"Edukadoj Menso Mind Logic Simulation 2.0 - alpha - Hossein Khass";
extern Color back_color(255, 255, 230, 200);



extern Page page;


void Common::Exit(WString data)
{
    if (pmain->ClosingAllowed())
        PostQuitMessage(0);
    else
        Suicide();
}

void Common::OnRestore()
{
}

void Common::OnClose()
{
    if (pmain->ClosingAllowed())
        DefWindowProc(pwin->hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
}

void Common::OnClickTitlebar()
{
    DefWindowProc(pwin->hwnd, WM_SYSCOMMAND, 61458, 0);
}

void Common::OnDoubleClickTitlebar()
{
    if (page.GetPage() != 0)
        page[0].Activate();
    else
        page[2].Activate();
}

void Common::OnMinimize()
{
    DefWindowProc(pwin->hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void Common::OnTimer2()
{
}
//)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

StaticText* st_math_text = 0;
StaticText* st_truths = 0;
StaticText* st_things = 0;
StaticText* st_operators = 0;
StaticText* st_report = 0;
EditBox* math_edit = 0;
EditBox* truths_edit = 0;
EditBox* things_edit = 0;
EditBox* operators_edit = 0;
EditBox* report_edit = 0;
Button* truths_but = 0;
Button* tell_but = 0;
Button* exit_but = 0;
Button* main_but = 0;
Button* things_but = 0;
Button* operators_but = 0;
Button* check_but = 0;
Button* open_but = 0;
Button* save_but = 0;
Button* about_but = 0;
Page subpage;


void Main::Definitions()
{
    EduMenso MT;

    int x = 0;
    int y = 40;

    main_but = newButton(RectF((REAL)20, (REAL)10, (REAL)80, (REAL)30), L"Main");
    main_but->SetIntColor(Color(150, 100, 100));
    tell_but = newButton(RectF((REAL)294, (REAL)10, (REAL)70, (REAL)30), L"Tell");
    tell_but->SetIntColor(Color(150, 100, 100));
    exit_but = newButton(RectF((REAL)(x + 140), (REAL)(y + 390), (REAL)100, (REAL)30), L"Exit");
    exit_but->SetIntColor(Color(190, 0, 0));
    exit_but->action2 = Exit;

    page << main_but << tell_but;

    {
        st_math_text = newStaticText("Menso Logic Code:", x + 20.f, y + 80.f);
        st_report = newStaticText("Report:", x + 20.f, y + 450.f);
    }

    {
        error_edit = newEditBox(RectF((REAL)(x + 26), (REAL)(y + 140), (REAL)330, (REAL)190));
        error_edit->NeverLetEdit();
        error_edit->NeverLetBlinking();
        error_edit->SetFont(L"verdana");
        error_edit->SetBackground(Color(0, 0, 0));
        error_edit->SetTextColor(Color(255, 255, 255));
        error_edit->SetSize(15);
        error_edit->AddRangeString(",");
    }

    {
        math_edit = newEditBox(RectF((REAL)(x + 20), (REAL)(y + 100), (REAL)400, (REAL)300));
        math_edit->SetBackground(Color(250, 240, 230));
        math_edit->AddDecosStringAndColor("new operator add all some replace with in if check pick set suppose select define noadds", Color(255, 0, 0, 255));
        math_edit->AddDecosStringAndColor("true false", Color(0, 150, 0));
        math_edit->AddDecosStringAndColor("object prop", Color(150, 0, 0));
        math_edit->AddDecosStringAndColor("+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"", Color(75, 92, 175));
        math_edit->AddRangeString("//");
        math_edit->AfterEnter = MT.AfterEnter;
    }


    {
        report_edit = newEditBox(RectF((REAL)(x + 20), (REAL)(y + 470), (REAL)700, (REAL)21));
        report_edit->NeverLetEdit();
        report_edit->NeverLetBlinking();
        report_edit->SetFont(L"verdana");
        report_edit->SetBackground(Color(60, 60, 60));
        report_edit->SetTextColor(Color(255, 255, 255));
        report_edit->SetSize(15);
    }


    check_but = newButton(RectF((REAL)(x + 20), (REAL)(y + 10), (REAL)160, (REAL)30), L"Check the code");
    check_but->SetButtonActionParam(ToData(pwin->hwnd));
    check_but->action2 = MT.CheckTheText;
    check_but->SetIntColor(Color(200, 0, 120));


    open_but = newButton(RectF((REAL)(x + 181), (REAL)(y + 10), (REAL)80, (REAL)30), L"Open");
    open_but->SetButtonActionParam(ToData(pwin->hwnd));
    open_but->action2 = MT.OpenButton;



    save_but = newButton(RectF((REAL)(x + 262), (REAL)(y + 10), (REAL)100, (REAL)30), L"Save As");
    save_but->SetButtonActionParam(ToData(pwin->hwnd));
    save_but->action2 = MT.SaveAsButton;


    about_but = newButton(RectF((REAL)(x + 140), (REAL)(y + 350), (REAL)100, (REAL)30), L"About");
    about_but->action2 = (ACTION)About;

    {
        page[0] << &subpage;

        int xx = x + 60;
        int yy = y;

        st_truths = newStaticText("Truths:", xx + 420.f, yy + 80.f);
        st_things = newStaticText("Things:", xx + 420.f, yy + 80.f);
        st_operators = newStaticText("Operators:", xx + 420.f, yy + 80.f);


        truths_but = newButton(RectF((REAL)(xx + 400), (REAL)(yy + 10), (REAL)80, (REAL)30), L"Truths");
        truths_but->SetIntColor(Color(255, 150, 100, 100));

        things_but = newButton(RectF((REAL)(xx + 481), (REAL)(yy + 10), (REAL)80, (REAL)30), L"Things");
        things_but->SetIntColor(Color(255, 150, 100, 100));

        operators_but = newButton(RectF((REAL)(xx + 562), (REAL)(yy + 10), (REAL)95, (REAL)30), L"Operators");
        operators_but->SetIntColor(Color(255, 150, 100, 100));

        subpage << truths_but << things_but << operators_but;

        {
            truths_edit = newEditBox(RectF((REAL)(xx + 420), (REAL)(yy + 100), (REAL)215, (REAL)300));
            truths_edit->SetBackground(Color(250, 240, 230));
            truths_edit->AddDecosStringAndColor("new operator add all some replace with in if check pick set suppose select define noadds", Color(255, 0, 0, 255));
            truths_edit->AddDecosStringAndColor("true false", Color(255, 0, 150, 0));
            truths_edit->AddDecosStringAndColor("object prop", Color(255, 150, 0, 0));
            truths_edit->AddDecosStringAndColor("+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"", Color(255, 75, 92, 175));
            truths_edit->NeverLetEdit();
        }

        {
            things_edit = newEditBox(RectF((REAL)(xx + 420), (REAL)(yy + 100), (REAL)215, (REAL)300));
            things_edit->SetBackground(Color(250, 240, 230));
            things_edit->AddDecosStringAndColor("new operator add all some replace with in if check pick set suppose select define noadds", Color(255, 0, 0, 255));
            things_edit->AddDecosStringAndColor("true false", Color(255, 0, 150, 0));
            things_edit->AddDecosStringAndColor("object prop", Color(255, 150, 0, 0));
            things_edit->AddDecosStringAndColor("+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"", Color(255, 75, 92, 175));
            things_edit->NeverLetEdit();
        }

        {
            operators_edit = newEditBox(RectF((REAL)(xx + 420), (REAL)(yy + 100), (REAL)215, (REAL)300));
            operators_edit->SetBackground(Color(250, 240, 230));
            operators_edit->AddDecosStringAndColor("new operator add all some replace with in if check pick set suppose select define noadds", Color(255, 0, 0, 255));
            operators_edit->AddDecosStringAndColor("true false", Color(255, 0, 150, 0));
            operators_edit->AddDecosStringAndColor("object prop", Color(255, 150, 0, 0));
            operators_edit->AddDecosStringAndColor("();,&|*!={}-+\\/~^%<>", Color(255, 75, 92, 175));
            operators_edit->NeverLetEdit();
        }

        subpage[0] << truths_edit << st_truths;
        subpage[1] << things_edit << st_things;
        subpage[2] << operators_edit << st_operators;
        subpage.Startup();
    }
    page[0] << math_edit << report_edit << check_but << open_but << save_but << st_math_text << st_report;
    page[1] << error_edit << about_but << exit_but;
}

int _stdcall WinMain(HINSTANCE, HINSTANCE, char*, int nCmdShow)
{
    delete new Main(765, 600, nCmdShow);
    return 0;
}