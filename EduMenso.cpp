// #pragma hdrstop
#include "EduMenso.h"
#include "Main.h"
#include "Space.h"

void ReportError(const char* c, Unsigned offset)
{
    Space::no_adds = false;
    report_edit->Clear();
    report_edit->cout(L"Error Report:").cout(L"\n").cout(c);
    truths_edit->cout(L"Error");
    things_edit->cout(L"Error");
    operators_edit->cout(L"Error");
    GetNthDeco gnd2(math_edit, offset);
    if (gnd2.y != MAXUnsigned)
    {
        if (offset > 0)
        {
            GetNthDeco gnd1(math_edit, offset - 1);
            if (gnd1.y != MAXUnsigned)
            {
                math_edit->CancelSelection();
                if (gnd1.y < gnd2.y)
                {
                    gnd1.y = gnd2.y;
                    gnd1.x = 0;
                }
                math_edit->Select(gnd1.x, gnd1.y, gnd2.x, gnd2.y);
            }
        }
        math_edit->GotoXY(gnd2.x, gnd2.y);
    }
    report_edit->HideBlinker();
    math_edit->Focus();
    throw 1;
}