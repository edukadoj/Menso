// for an editbox

#ifndef EditBoxH
#define EditBoxH
#include <vector>
#include <map>

#include <math.h>
//using namespace std;

#include "WFile.h"
#include "File.h"
#include "Write.h"
#include "Clipboard.h"
#include "String.h"
#include "WString.h"
#include "WindowsPaths.h"

inline String WtoS(WString wstr)
{
    return (wchar_t*)(WCS)wstr;
}

inline WString StoW(String str)
{
    return (char*)(CS)str;
}

template<class T>
void swapvars(T& x, T& y)
{
    T temp = x;
    x = y;
    y = temp;
}

inline bool isShiftDown()
{
    short status = GetAsyncKeyState(VK_SHIFT);
    bool most_significant_bit = status >> (sizeof(short) - 1);
    return most_significant_bit;
}

inline bool isControlDown()
{
    short status = GetAsyncKeyState(VK_CONTROL);
    bool most_significant_bit = status >> (sizeof(short) - 1);
    return most_significant_bit;
}

inline bool isLClicked() // is left mouse button down
{
    int VK;
    if (GetSystemMetrics(SM_SWAPBUTTON))
        VK = VK_RBUTTON;
    else
        VK = VK_LBUTTON;
    short status = GetAsyncKeyState(VK);
    bool most_significant_bit = status >> (sizeof(short) - 1);
    return most_significant_bit;
}

class Recta : public AdDraw
{
    Color background_color;
public:
    float left;
    float right;
    float top;
    float bottom;

    //==============================================================================
    float Width()
    {
        return right - left;
    }
    //==============================================================================
    float Height()
    {
        return bottom - top;
    }
    //==============================================================================
    bool in(float x, float y)
    {
        return (left <= x) && (x <= right) && (top <= y) && (y <= bottom);
    }
    //==============================================================================
    bool inLeft(float x, float y = 0)
    {
        y;
        return x < left;
    }
    //==============================================================================
    bool inRight(float x, float y = 0)
    {
        y;
        return right < x;
    }
    //==============================================================================
    bool inTop(float x, float y)
    {
        x;
        return y < top;
    }
    //==============================================================================
    bool inBottom(float x, float y)
    {
        x;
        return bottom < y;
    }
    //==============================================================================
    void Move(float x, float y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }

    void SetBackground(Color _color)
    {
        background_color = _color;
    }

    Color GetBackground()
    {
        return background_color;
    }
    //==============================================================================
    void DrawIt(float corner_x, float corner_y)
    {
        SetColor(background_color);
        RectInterior(RectF(corner_x + 1, corner_y + 1, Width() - 1, Height() - 1));
        RoundLightRec(RectF(corner_x, corner_y, Width(), Height()));
    }

    Recta(HDC hdc, float left, float right, float top, float bottom) :
        AdDraw(hdc),
        left(left),
        right(right),
        top(top),
        bottom(bottom)
    {
        background_color = Color(255, 230, 230, 255);
    }
};

class LogicEditBox
{
public:
    static int all_editboxes;

    WPack text; // file

protected:
    Unsigned logic_x;
    Unsigned logic_y;
    bool only_one_line;

    //==============================================================================
    Unsigned GetLineNumber()
    {
        return (Unsigned)text.GetNumberOfWPacks() - 1;// - 1 because info pack not counted
    }
    //==============================================================================
    void SetLineNumber(Unsigned new_line_number)
    {
        text.SetNumberOfWPacks(new_line_number + 1);
    }
    //==============================================================================
    WString GetLine(Unsigned line)
    {
        if (line <= GetLineNumber())
            return text[line];
        return L"";
    }
    //==============================================================================
    void SetLine(Unsigned line, WString new_LINE)
    {
        if (GetLineNumber() < line)
            SetLineNumber(line);
        text[line] = new_LINE;
    }
    //==============================================================================
    Unsigned GetCharNumber(Unsigned line)
    {
        if (GetLineNumber() < line)
            return 0;
        WString LINE = text[line];
        return LINE.Size();
    }
    //==============================================================================
    void GoLeft()
    {
        if (0 < logic_x)
            logic_x--;
    }
    //==============================================================================
    void GoRight(int n = 1)
    {
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();
        if (logic_x + n <= len)
            logic_x += n;
    }
    //==============================================================================
    void GoUp()
    {
        if (1 < logic_y)
            logic_y--;
        if (GetCharNumber(logic_y) < logic_x)
            GoEnd();
    }
    //==============================================================================
    void GoDown()
    {
        Unsigned line = GetLineNumber();
        if (logic_y < line)
            logic_y++;
        if (GetCharNumber(logic_y) < logic_x)
            GoEnd();
    }
    //==============================================================================
    void GoHome()
    {
        logic_x = 0;
    }
    //==============================================================================
    void GoEnd()
    {
        logic_x = GetCharNumber(logic_y);
    }
    //==============================================================================
public:
    void PutChar(wchar_t wch)
    {
        PutString(wch);
    }
    //==============================================================================
    void PressEnter()
    {
        if (GetLineNumber() < logic_y)
            return;
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();

        WString new_LINE;

        if (logic_x < len)
        {
            new_LINE = LINE[logic_x][len - 1];
            if (0 < logic_x)
                LINE = LINE[0][logic_x - 1];
            else
                LINE = L"";
        }

        SetLine(logic_y, LINE);
        text[logic_y] += new_LINE; // insert line
        GoDown();
        GoHome();
    }
    //==============================================================================
    void BackSpace()
    {
        if (GetLineNumber() < logic_y)
            return;
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();
        if (logic_x <= len)
        {
            if (0 < logic_x)
            {
                WString part1, part2;
                if (0 < logic_x - 1)
                    part1 = LINE[0][logic_x - 2];
                if (logic_x < len)
                    part2 = LINE[logic_x][len - 1];
                SetLine(logic_y, part1 + part2);
                GoLeft();
            }
            else // if logic_x == 0
            {
                if (1 < logic_y) // not first line
                {
                    if (0 < len)
                    {
                        WString preLINE = GetLine(logic_y - 1);
                        SetLine(logic_y - 1, preLINE + LINE);
                        text[logic_y] = 0; // delete line
                        GoUp();
                        logic_x = preLINE.Size();
                    }
                    else
                    {
                        text[logic_y] = 0; // delete line
                        GoUp();
                        GoEnd();
                    }
                }
            }
        }
    }
    //==============================================================================
    void Delete()
    {
        Unsigned line_number = GetLineNumber();
        if (line_number < logic_y)
            return;
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();

        if (logic_x <= len)
        {
            if (logic_x < len)
            {
                WString part1, part2;

                if (0 < logic_x)
                    part1 = LINE[0][logic_x - 1];

                if (logic_x == len - 1)
                {
                    ;
                }
                else // if logic_x + 1 <= len - 1
                {
                    part2 = LINE[logic_x + 1][len - 1];
                }
                SetLine(logic_y, part1 + part2);
            }
            else // if logic_x == len
            {
                if (logic_y + 1 <= line_number)
                {
                    WString nextLINE = GetLine(logic_y + 1);
                    text[logic_y + 1] = 0; // delete next line
                    SetLine(logic_y, LINE + nextLINE);
                }
            }
        }
    }
    //==============================================================================

        //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    void DelFromTo(Unsigned line1, Unsigned x1, Unsigned line2, Unsigned x2)
    {
        struct  //function in function
        {
            void PartOfLine(WString& LINE, Unsigned x1, Unsigned x2)
            {
                Unsigned len = LINE.Size();
                if (len < x1)
                    x1 = len;
                if (len < x2)
                    x2 = len;
                if (x1 == x2)
                    return;
                if (x2 < x1)
                    swapvars(x1, x2);
                // now we have x1 < x2
                if (x1 == 0)
                {
                    if (x2 == len)
                        LINE = L"";
                    else if (x2 < len)
                        LINE = LINE[x2][len - 1];
                }
                else if (x1 > 0)
                {
                    if (x2 == len)
                        LINE = LINE[0][x1 - 1];
                    else if (x2 < len)
                        LINE = LINE[0][x1 - 1] + LINE[x2][len - 1];
                }
            }
        } Del;

        Unsigned line_number = GetLineNumber();
        if (line_number < line1)
            line1 = line_number;
        if (line_number < line2)
            line2 = line_number;

        if (line2 < line1)
        {
            swapvars(line1, line2);
            swapvars(x1, x2);
        }

        if (line1 == line2)
        {
            WString LINE = GetLine(line1);
            Del.PartOfLine(LINE, x1, x2);
            SetLine(line1, LINE);
            logic_y = line1;
            logic_x = min(x1, x2);
        }
        else if (line1 < line2)
        {
            for (Unsigned i = line1 + 1; i <= line2 - 1; line2--)
                text[i] = 0; // delete line
            WString LINE1 = GetLine(line1);
            WString LINE2 = GetLine(line2);
            Del.PartOfLine(LINE1, x1, MAXUnsigned);
            Del.PartOfLine(LINE2, 0, x2);
            WString LINE = LINE1 + LINE2;
            text[line2] = 0;
            SetLine(line1, LINE);
            logic_y = line1;
            logic_x = x1;
        }
    } // void DelFromTo(Unsigned line1, Unsigned x1, Unsigned line2, Unsigned x2)
    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


    WString CopyFromTo(Unsigned line1, Unsigned x1, Unsigned line2, Unsigned x2)
    {
        if (line1 == 0 || line2 == 0)
            return L"";

        Unsigned line_number = GetLineNumber();
        if (line_number < line1)
            line1 = line_number;
        if (line_number < line2)
            line2 = line_number;

        if (line2 < line1)
        {
            swapvars(line1, line2);
            swapvars(x1, x2);
        }
        if (line1 == line2)
            if (x2 < x1)
                swapvars(x1, x2);
        x2--;

        WString copywstr;
        if (line1 == line2)
        {
            WString LINE = GetLine(line1);
            Unsigned len = LINE.Size();
            if (len > 0)
                if (x2 >= len)
                {
                    if (x1 < len)
                        copywstr += LINE[x1][len - 1];
                }
                else
                {
                    if (x1 < len)
                        copywstr += LINE[x1][x2];
                }
        }
        else if (line1 < line2)
        {
            WString LINE = GetLine(line1);
            Unsigned len1 = LINE.Size();
            if (len1 > 0)
                if (x1 < len1)
                    copywstr += LINE[x1][len1 - 1];

            if (line1 + 1 <= line2)
            {
                copywstr += LineEndW;
                for (Unsigned i = line1 + 1; i < line2; i++)
                    copywstr += GetLine(i) + LineEndW;
            }

            LINE = GetLine(line2);
            len1 = LINE.Size();
            if (len1 > 0)
                if (x2 >= len1)
                    copywstr += LINE[0][len1 - 1];
                else
                    copywstr += LINE[0][x2];
        }
        return copywstr;
    }
    //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    void Paste(WString wstr)
    {
        Unsigned len = wstr.Size();

        Unsigned n = 0;
        if (!only_one_line)
            for (Unsigned i = 0; i < len; i++)
            {
                wchar_t wch = wstr[i];
                if (wch == 13)
                    if (i + 1 < len)
                    {
                        wch = wstr[i + 1];
                        if (wch == L'\n')
                        {
                            LogicEditBox::PutString(wstr[n][i - 1]);
                            LogicEditBox::PressEnter();
                            i++;
                            n = i + 1;
                            continue;
                        }
                    }
            }
        if (n < len)
            LogicEditBox::PutString(wstr[n][len - 1]);
    }

    //==============================================================================
    void PutString(WString wstr)
    {
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();
        if (logic_x < len)
            if (0 < logic_x)
                SetLine(logic_y, LINE[0][logic_x - 1] + wstr + LINE[logic_x][len - 1]);
            else
                SetLine(logic_y, wstr + LINE[logic_x][len - 1]);
        else if (logic_x == len)
            SetLine(logic_y, LINE + wstr);
        GoRight((int)wstr.Size());
    }
    //==============================================================================
    LogicEditBox() :
        text
        (
            GetAppDataPath() + WString("packtext") + all_editboxes
        )
    {
        logic_x = 0;
        logic_y = 1;
        all_editboxes++;
    }
    ~LogicEditBox()
    {
        all_editboxes--;
    }
};
//==============================================================================
//==============================================================================
//==============================================================================

class VisualEditBox : public LogicEditBox
{
protected:
    bool unisize;

public:
    HDC hdc;
    bool blinking;

    struct
    {
        bool is_caret_visible;
        float x;
        float y;
    } caret_status;

    float line_distance;
    float caret_x;
    float caret_y;

    float corner_x;
    float corner_y;
    Color text_color;

    __int8 is_refresh_necessary; // bool

    Recta box;

    Write write;

    std::vector<WString> box_tex;
    std::vector<float> start_deltas;

    //==============================================================================

    //==============================================================================
    Unsigned CaretHeight()
    {
        return (Unsigned)write.StringHeight(L"jWJNmy") + 2;
    }

    //==============================================================================
    void AdjustXYPosition(float& x, float& y, bool middle = true)
    {
        float line_height = (float)CaretHeight();
        x += CharWidth(L' ') / 2;
        if (middle)
            y += line_height * 3 / 4;
        else
            y += line_height / 4;
    }
    //==============================================================================
    void DrawCaret(float x, float y)
    {
        if (caret_status.is_caret_visible == false)
            caret_status.is_caret_visible = true;
        else
            caret_status.is_caret_visible = false;

        caret_status.x = x;
        caret_status.y = y;

        float line_height = (float)CaretHeight();

        float l = line_height / 2;
        //float d  = l / 5;

        AdjustXYPosition(x, y);

        box.LimitDrawing(corner_x + 1, corner_y + 1, box.Width(), box.Height() - 1);
        //box.NotLine(PointF(x - d,y - l - d),PointF(x,y - l));
        //box.NotLine(PointF(x + d,y - l - d),PointF(x,y - l));
        box.NotLine(PointF(x - 1, y - l), PointF(x, y + l));
        box.NotLine(PointF(x, y - l), PointF(x, y + l));
        box.NotLine(PointF(x + 1, y - l), PointF(x, y + l));
        //	box.NotLine(PointF(x,y + l),PointF(x - d,y + l + d));
        //	box.NotLine(PointF(x,y + l),PointF(x + d,y + l + d));
        box.RemoveDrawingLimits();
    }

    void HideCaret()
    {
        if (caret_status.is_caret_visible == true)
            DrawCaret(caret_status.x, caret_status.y);
    }

    void ShowCaret()
    {
        if (caret_status.is_caret_visible == false)
            DrawCaret(caret_status.x, caret_status.y);
    }

    void BlinkCaret()
    {
        //SwitchVisibility();
        DrawCaret(caret_status.x, caret_status.y);
    }

    void SetCaret(float x, float y, bool draw = true)
    {
        if (draw)
        {
            HideCaret();
            caret_status.x = x;
            caret_status.y = y;
            ShowCaret();
        }
        write.Gotoxy(x, y);
    }
    //==============================================================================
    void PrintAt(wchar_t wch, float x, float y)
    {
        if (unisize)
            x += CharWidth(wch) / 3;
        SetCaret(x, y, false);
        write << wch;
    }
    //==============================================================================
    float CharWidth(wchar_t wch)
    {
        if (!unisize)
        {
            float width = write.StringWidth(wch);
            if (width <= 1)
                return write.StringWidth(L'k');
            return width;
        }
        else
            return write.StringWidth(L'M');
    }
    //==============================================================================
    float StringWidth(WString wstr)
    {
        Unsigned len = wstr.Size();
        float sum = 0;
        for (Unsigned i = 0; i < len; i++)
            sum += CharWidth(wstr[i]);
        return sum;
    }
    //==============================================================================
    WString SpaceWString(Unsigned len)
    {
        wchar_t* wc = new wchar_t[len + 1];
        for (Unsigned i = 0; i < len; i++)
            wc[i] = ' '; // space;
        wc[len] = 0; // terminating null
        return wc;
    }
    //==============================================================================
    //==============================================================================

    virtual void Move(float delta_x, float delta_y)
    {
        box.Move(delta_x, delta_y);
    }
    //==============================================================================
    void AdjustBox()
    {
        WString LINE = GetLine(logic_y);

        Unsigned len = LINE.Size();
        if (len == 0)
            LINE = L"";
        else
            if (logic_x < len)
            {
                if (logic_x == 0)
                    LINE = L"";
                else
                    LINE = LINE[0][logic_x - 1];
            }

        float x = corner_x + StringWidth(LINE);

        Unsigned i = 0;
        float sum = 0;

        float upper_limit = box.top - corner_y;
        for (; sum < upper_limit; i++)
            sum += line_distance;

        float y = corner_y + line_distance * (logic_y - 1);// + upper_limit - 1;

        box.bottom -= line_distance;
        float thin_char_width = CharWidth(L'i');
        box.right -= thin_char_width;
        if (!box.in(x, y))
        {
            is_refresh_necessary = 1;
            float delta_x = 0, delta_y = 0;

            if (box.inLeft(x, y))
                delta_x = x - box.left;
            if (box.inRight(x, y))
                delta_x = x - box.right;
            if (box.inTop(x, y))
                delta_y = y - box.top;
            if (box.inBottom(x, y))
                delta_y = y - box.bottom;

            Move(delta_x, delta_y);
        }
        box.bottom += line_distance;
        box.right += thin_char_width;
    }
    //==============================================================================
    void UpdateCaret(bool draw = true)
    {
        WString LINE;

        Unsigned line_number = GetLineNumber();

        if (logic_y - 1 < line_number)
            LINE = GetLine(logic_y);
        else
            LINE = L"";
        Unsigned len = LINE.Size();

        if (logic_x < len)     // as a result: 0 < len
            if (logic_x == 0)
                LINE = L"";
            else
                LINE = LINE[0][logic_x - 1];

        Unsigned i = 0;
        float sum = 0;

        float upper_limit = box.top - corner_y;
        for (; sum < upper_limit; i++)
            sum += line_distance;

        float delta1 = sum - upper_limit;

        float x, y;
        x = 2 * corner_x + StringWidth(LINE) - box.left;
        y = 2 * corner_y + line_distance * (logic_y - 1) - box.top - delta1;
        if (draw && blinking)
            SetCaret(x, y);
        else
        {
            caret_status.x = x;
            caret_status.y = y;
        }
    }
    //==============================================================================
    virtual void PrintLine(Unsigned i)
    {
        float x = corner_x + start_deltas[i];
        WString LINE = box_tex[i];
        Unsigned len = LINE.Size();

        write.LimitDrawing(corner_x + 1, corner_y, box.Width(), box.Height());
        for (Unsigned j = 0; j < len; j++)
        {
            wchar_t wch = LINE[j];
            float dist = CharWidth(wch);
            x += dist;
            PrintAt(wch, x - dist, corner_y + i * line_distance);
        }
        write.RemoveDrawingLimits();
    }

    void Print()
    {
        if (is_refresh_necessary == -1)
        {
            box.DrawIt(corner_x, corner_y);
            is_refresh_necessary = 0;
            Unsigned lines = box_tex.size();
            for (Unsigned i = 0; i < lines; i++)
                PrintLine(i);
        }
    }


    //==============================================================================
    WString LineFromLenToLen(Unsigned line, float len1, float len2, float& start_delta)
    {
        if (len2 <= len1)
            return L"";
        WString LINE = GetLine(line);
        if (LINE == L"")
            return L"";
        Unsigned line_len = LINE.Size();

        Unsigned i = 0;
        float sum = 0;
        wchar_t wch;
        if (0 < len1)
        {
            for (; sum < len1; i++)
            {
                if (i < line_len)
                {
                    wch = LINE[i];
                    sum += CharWidth(wch);
                }
                else
                    return L"";
            }
        }
        start_delta = 0;
        if (sum > 0)
        {
            float delta = sum - len1;
            if (delta > 0)
            {
                start_delta = delta;
                //len2 -= delta;
            }
        }
        Unsigned j = i;
        for (; sum < len2; j++)
        {
            if (j < line_len)
            {
                wchar_t _wch = LINE[j];
                sum += CharWidth(_wch);
            }
            else
                break;
        }
        if (i < j)
        {
            if (i > 0)
            {
                start_delta -= CharWidth(LINE[i - 1]);
                return LINE[i - 1][j - 1];
            }
            else
                return LINE[i][j - 1];
        }
        else
            return L"";
    }
    //==============================================================================
    void LineMinMax(Unsigned& start, Unsigned& end)
    {
        float sum = 0;
        start = 1;

        float constant = box.top - corner_y;
        for (; sum < constant; start++)
            sum += line_distance;

        end = start;
        for (; sum <= box.bottom - corner_y; end++)
            sum += line_distance;
    }
    //==============================================================================
    void Refresh()
    {
        if (is_refresh_necessary == 1)
        {
            box_tex.clear();
            start_deltas.clear();

            Unsigned start;
            Unsigned end;
            LineMinMax(start, end);// chnages start & end

            float start_delta;
            for (Unsigned i = start; i < end; i++)
            {
                WString LINE = LineFromLenToLen
                (
                    i,
                    box.left - corner_x,
                    box.right - corner_x,
                    start_delta
                );
                box_tex.push_back(LINE);
                start_deltas.push_back(start_delta);
            }
            is_refresh_necessary = -1;
        }
    }
    //==============================================================================

    //==============================================================================

    virtual void InternalPaint()
    {
        HideCaret();
        AdjustBox();
        Refresh();
        Print();
        UpdateCaret();
    }
    //==============================================================================

    //==============================================================================
    virtual void PutChar(wchar_t wch)
    {
        is_refresh_necessary = 1;
        LogicEditBox::PutChar(wch);
        InternalPaint();
    }
    //==============================================================================
    virtual void GoLeft()
    {
        LogicEditBox::GoLeft();
        InternalPaint();
    }
    //==============================================================================
    virtual void GoRight()
    {

        LogicEditBox::GoRight();
        InternalPaint();
    }

    //==============================================================================
    virtual void GoUp(Unsigned n = 1)
    {
        for (Unsigned i = 0; i < n; i++)
            LogicEditBox::GoUp();
        InternalPaint();
    }
    //==============================================================================
    virtual void GoDown(Unsigned n = 1)
    {
        for (Unsigned i = 0; i < n; i++)
            LogicEditBox::GoDown();
        InternalPaint();
    }
    //==============================================================================
    virtual void GoHome()
    {
        LogicEditBox::GoHome();
        InternalPaint();
    }
    //==============================================================================
    virtual void GoEnd()
    {
        LogicEditBox::GoEnd();
        InternalPaint();
    }
    //==============================================================================
    virtual void BackSpace()
    {
        LogicEditBox::BackSpace();
        Paint();
    }
    //==============================================================================
    virtual void Delete()
    {
        LogicEditBox::Delete();
        Paint();
    }
    //==============================================================================
    virtual void PressEnter()
    {
        LogicEditBox::PressEnter();
        Paint();
    }
    //==============================================================================
    //==============================================================================
    //==============================================================================
public:
    virtual void SpecialInput(wchar_t wch, bool /*is_control_down*/, bool /*is_shift_down*/)
    {
        switch (wch)
        {
        case VK_LEFT:
            GoLeft();
            break;

        case VK_RIGHT:
            GoRight();
            break;

        case VK_UP:
            GoUp();
            break;

        case VK_DOWN:
            GoDown();
            break;

        case VK_END:
            GoEnd();
            break;

        case VK_HOME:
            GoHome();
            break;

        case VK_DELETE:
            Delete();
            break;

        case VK_PRIOR:
            GoUp((Unsigned)(box.Width() / line_distance));
            break;

        case VK_NEXT:
            GoDown((Unsigned)(box.Width() / line_distance));
            break;
        }
    }
    //==============================================================================
    virtual void Input(wchar_t wch)
    {
        switch (wch)
        {
        case 13:
            PressEnter();
            break;

        case 8: // backspace
            BackSpace();
            break;

        default:
            PutChar(wch);
            //wcout <<(int) wch;
        }
    }

    virtual void Paint()
    {
        is_refresh_necessary = 1;
        InternalPaint();
    }


    //==============================================================================
    //==============================================================================
    //==============================================================================
    VisualEditBox(HDC hdc, RectF rf) :
        LogicEditBox(),
        hdc(hdc),
        box(hdc, rf.X, rf.X + rf.Width, rf.Y, rf.Y + rf.Height),
        write(hdc)
    {
        unisize = false;

        caret_status.is_caret_visible = false;
        caret_status.x = corner_x;
        caret_status.y = corner_y;
        line_distance = (float)(Unsigned)write.StringHeight(L"jM!{}|گ") + 9;
        corner_x = box.left;
        corner_y = box.top;
        SetCaret(box.left, box.top, false); // after changing corners

        text_color = Color(255, 64, 12, 0);
        write.SetColor(text_color);
    }
}; // class VisualEditBox





class SelectEditBox : public VisualEditBox
{
protected:
    bool new_layer_allowed;
private:
    Unsigned LinesAboveBox()
    {
        Unsigned ret = 0;
        float sum = 0;
        float constant = box.top - corner_y;
        for (; sum < constant; ret++)
            sum += line_distance;
        return ret;
    }

    void ReRec(float x, float y, float width, float height)
    {
        if (width <= 0 || height <= 0)
            return;
        x++;
        width--;
        float left = corner_x + 1;
        float top = corner_y + 1;
        float right = corner_x + box.Width();
        float bottom = corner_y + box.Height();

        if (x < left)
        {
            float delta = left - x;
            x += delta;
            width -= delta;
        }
        if (y < top)
        {
            float delta = top - y;
            y += delta;
            height -= delta;
        }
        if (right < x + width)
        {
            width -= x + width - right - 1;
        }
        if (bottom <= y + height)
        {
            height -= y + height - bottom - 1;
        }
        if (x + width <= left || y + height <= top)
            return;
        if (right <= x || bottom <= y)
            return;
        if (new_layer_allowed)
        {
            box.Copy(x, y, width, height);
            box.NotPaste(x, y);
        }
        else
        {
            RectF draw_box(x, y, width, height);
            box.NotBox(draw_box);
        }
    }

    void SwitchSelectionDrawing()
    {
        if (select.is_selected == false)
            return;
        if (select.logic_start_y == select.logic_end_y)
        {
            if (select.logic_start_x == select.logic_end_x)
            {
                ; // do nothing
            }
            else if (select.logic_start_x < select.logic_end_x)
            {
                float x = select.start_x;
                float y = select.start_y;
                float width = select.end_x - x;
                float height = line_distance;
                ReRec(x, y, width, height);
            }
            else if (select.logic_start_x > select.logic_end_x)
            {
                float x = select.end_x;
                float y = select.end_y;
                float width = select.start_x - x;
                float height = line_distance;
                ReRec(x, y, width, height);
            }
        }
        else if (select.logic_start_y < select.logic_end_y)
        {
            float x = select.start_x;
            float y = select.start_y;
            float width = box.Width() - (x - corner_x) + 1;
            float height = line_distance;
            //		if(select.logic_start_x == 0)
            //			width--;
            ReRec(x, y, width, height);
            x = corner_x;
            width = box.Width();
            for (Unsigned i = select.logic_start_y + 1; i < select.logic_end_y; i++)
            {
                y += line_distance;
                ReRec(x, y, width, height);
            }
            y += line_distance;
            width = select.end_x - corner_x;
            ReRec(x, y, width, height);
        }
        else if (select.logic_start_y > select.logic_end_y)
        {
            float x = select.end_x;
            float y = select.end_y;
            float width = box.Width() - (x - corner_x) + 1;
            float height = line_distance;

            ReRec(x, y, width, height);

            x = corner_x;
            width = box.Width();
            for (Unsigned i = select.logic_end_y + 1; i < select.logic_start_y; i++)
            {
                y += line_distance;
                ReRec(x, y, width, height);
            }
            y += line_distance;
            width = select.start_x - corner_x;
            ReRec(x, y, width, height);
        }
    }

public:

    struct tagselect
    {
        bool is_selecting;
        float start_x;
        float start_y;
        Unsigned logic_start_x;
        Unsigned logic_start_y;

        bool is_selected;
        float end_x;
        float end_y;
        Unsigned logic_end_x;
        Unsigned logic_end_y;

        Unsigned x_drag_detect; // logical
        Unsigned y_drag_detect; // logical


        bool is_selection_drawn;

        void Move(float delta_x, float delta_y)
        {
            start_x -= delta_x;
            start_y -= delta_y;
            end_x -= delta_x;
            end_y -= delta_y;
        }

        tagselect()
        {
            is_selecting = false;
            start_x = 0;
            start_y = 0;
            logic_start_x = 0;
            logic_start_y = 0;

            is_selected = false;
            end_x = 0;
            end_y = 0;
            logic_end_x = 0;
            logic_end_y = 0;

            x_drag_detect = 0; // logical
            y_drag_detect = 0; // logical

            is_selection_drawn = false;
        }
    } select;


    virtual void MouseSetCaret(float x, float y)
    {
        Unsigned pre_lines = LinesAboveBox();
        Unsigned lines = pre_lines + box_tex.size();
        for (Unsigned i = pre_lines; i <= lines; i++)
        {
            float a = corner_y + (i - pre_lines - 1) * line_distance;
            float b = a + line_distance;
            if (a <= y && y < b)
            {
                float start;
                float xx = x + box.left - 2 * corner_x;
                WString LINE = LineFromLenToLen(i, 0, xx, start);
                Unsigned len = LINE.Size();
                if (1 <= len)
                {
                    logic_x = len - 1;
                    UpdateCaret();
                    float x1 = caret_status.x;
                    logic_x = len;
                    UpdateCaret();
                    float x2 = caret_status.x;
                    if (fabs(x - x1) <= fabs(x - x2))
                        logic_x = len - 1;
                    else
                        logic_x = len;
                }
                else
                    logic_x = 0;
                logic_y = min(i, GetLineNumber());
                if (logic_y == 0)
                    logic_y++;
                VisualEditBox::InternalPaint(); // VisualEditBox:: is necessary for avoiding infinite loop
                return;
            }
        }
        if (corner_y - line_distance < y && y < corner_y) // scroll up by mouse
        {
            if (0 < pre_lines)
                logic_y = pre_lines;
            else
                logic_y = 1;
            VisualEditBox::InternalPaint(); // VisualEditBox:: is necessary for avoiding infinite loop
        }
    }

    void Copy()
    {
        if (select.is_selected == false)
        {
            MessageBeep(UINT(-1));
            return;
        }
        WString copywstr = CopyFromTo
        (
            select.logic_start_y,
            select.logic_start_x,
            select.logic_end_y,
            select.logic_end_x
        );

        {
            Clipboard cb;
            cb.Copy(copywstr);
        }
    }

    void Paste()
    {
        Clipboard cb;
        WString copywstr = cb.Paste();
        LogicEditBox::Paste(copywstr);
        Paint();
    }

    void DeleteSelection()
    {
        DelFromTo
        (
            select.logic_start_y,
            select.logic_start_x,
            select.logic_end_y,
            select.logic_end_x
        );
        CancelSelection();
        Paint();
    }

    void SelectAll()
    {
        CancelSelection();
        logic_x = 0;
        logic_y = 1; // first line
        UpdateCaret(false);
        StartSelection(caret_x, caret_y);
        logic_y = GetLineNumber();
        logic_x = this->GetCharNumber(logic_y);
        UpdateCaret(false);
        EndSelection(caret_x, caret_y);

        Paint();
    }

    //------------------------------------------------------------------------------
    void CancelSelection()
    {
        select.is_selected = false;
    }


    void StartSelection(float x, float y)
    {
        MouseSetCaret(x, y);
        select.is_selecting = true;

        select.x_drag_detect = logic_x;
        select.y_drag_detect = logic_y;

        if (select.is_selected == false)
        {
            select.start_x = caret_status.x;
            select.start_y = caret_status.y;
            AdjustXYPosition(select.start_x, select.start_y, false);
            select.logic_start_x = logic_x;
            select.logic_start_y = logic_y;
        }
    }

    void EndSelection(float xx, float yy)
    {
        MouseSetCaret(xx, yy);
        if (logic_x == select.x_drag_detect && logic_y == select.y_drag_detect)
        {
            select.is_selecting = false;
            return;
        }
        if (select.is_selecting == false)
            return;
        select.is_selecting = false;
        select.is_selected = true;

        select.end_x = caret_status.x;
        select.end_y = caret_status.y;
        AdjustXYPosition(select.end_x, select.end_y, false);

        select.logic_end_x = logic_x;
        select.logic_end_y = logic_y;
    }

    void LogicSelect(Unsigned line1, Unsigned x1, Unsigned line2, Unsigned x2)
    {
        select.logic_start_x = x1;
        select.logic_start_y = line1;
        select.logic_end_x = x2;
        select.logic_end_y = line2;
        select.is_selecting = false;
        select.is_selected = true;
    }
    //------------------------------------------------------------------------------

    void ShowSelection()
    {
        if (select.is_selection_drawn == false)
        {
            SwitchSelectionDrawing();
            select.is_selection_drawn = true;
        }
    }

    void HideSelection()
    {
        if (select.is_selection_drawn == true)
        {
            SwitchSelectionDrawing();
            select.is_selection_drawn = false;
        }
    }

    virtual void InternalPaint()
    {
        HideSelection();
        VisualEditBox::InternalPaint();
        ShowSelection();
    }


    virtual void Move(float delta_x, float delta_y)
    {
        VisualEditBox::Move(delta_x, delta_y);
        select.Move(delta_x, delta_y);
    }

    virtual void GoLeft()
    {
        bool shift = isShiftDown();
        if (shift)
            StartSelection(caret_x, caret_y);

        else
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }

        VisualEditBox::GoLeft();
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            ShowSelection();
            Paint();
        }
    }
    //==============================================================================
    virtual void GoRight()
    {
        bool shift = isShiftDown();
        if (shift)
        {
            HideSelection();
            StartSelection(caret_x, caret_y);
        }
        else
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }
        VisualEditBox::GoRight();
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            ShowSelection();
            Paint();
        }
    }

    //==============================================================================
    virtual void GoUp(Unsigned n = 1)
    {
        bool shift = isShiftDown();
        if (shift)
            StartSelection(caret_x, caret_y);
        else if (n == 1)
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }
        VisualEditBox::GoUp(n);
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            Paint();
        }
    }
    //==============================================================================
    virtual void GoDown(Unsigned n = 1)
    {
        bool shift = isShiftDown();
        if (shift)
            StartSelection(caret_x, caret_y);
        else if (n == 1)
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }
        VisualEditBox::GoDown(n);
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            Paint();
        }
    }
    //==============================================================================
    virtual void GoHome()
    {
        bool shift = isShiftDown();
        if (shift)
            StartSelection(caret_x, caret_y);
        else
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }
        VisualEditBox::GoHome();
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            Paint();
        }
    }
    //==============================================================================
    virtual void GoEnd()
    {
        bool shift = isShiftDown();
        if (shift)
            StartSelection(caret_x, caret_y);
        else
        {
            CancelSelection();
            is_refresh_necessary = 1;
        }
        VisualEditBox::GoEnd();
        if (shift)
        {
            EndSelection(caret_x, caret_y);
            Paint();
        }
    }

    virtual void PutChar(wchar_t wch)
    {
        CancelSelection();
        VisualEditBox::PutChar(wch);
    }

    //==============================================================================

    //==============================================================================
    virtual void BackSpace()
    {
        CancelSelection();
        VisualEditBox::BackSpace();
    }
    //==============================================================================
    virtual void Delete()
    {
        CancelSelection();
        VisualEditBox::Delete();

    }
    //==============================================================================
    virtual void PressEnter()
    {
        CancelSelection();
        VisualEditBox::PressEnter();
    }

    virtual void Input(wchar_t wch)
    {
        switch (wch)
        {
        case '\x03': // ctrl + 'c'
            Copy();
            break;

        case '\x16': // ctrl + 'v'
        {
            if (select.is_selected == true)
                SpecialInput(VK_DELETE, 0, 0);
            Paste();
            break;
        }

        case '\x01': // ctrl + 'a'
            SelectAll();
            break;

        case '\x18': // ctrl + 'x'
            Input('\x03'); // copy
            SpecialInput(VK_DELETE, 0, 0);
            break;

        default:
        {
            if (select.is_selected == true)
                DeleteSelection();
            VisualEditBox::Input(wch);
        }
        }
    }

    virtual void SpecialInput(wchar_t wch, bool is_control_down, bool is_shift_down)
    {
        switch (wch)
        {
        case VK_INSERT: // ctrl + insert
        {
            if (is_control_down)
                Copy();
            else if (is_shift_down)
            {
                if (select.is_selected == true)
                    SpecialInput(VK_DELETE, 0, 0);
                Paste();
            }
            break;
        }

        case VK_DELETE:
        {
            if (select.is_selected == true)
                DeleteSelection();
            else
                VisualEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;
        }
        default:
            VisualEditBox::SpecialInput(wch, is_control_down, is_shift_down);
        }
    }


public:
    SelectEditBox(HDC hdc, RectF rf) :
        VisualEditBox(hdc, rf)
    {
        new_layer_allowed = true;
    }
};



class UndoEditBox : public SelectEditBox
{
protected:
    enum
    {
        LEFT_,
        RIGHT_,
        UP_,
        DOWN_,
        END_,
        HOME_,
        DELETE_NORMAL,
        DELETE_SELECTED,
        INSERT_,
        PRIOR_,
        NEXT_,
        CONTROLINSERT_,
        SHIFTINSERT,
        ENTER_,
        BACKSPACE_,
        WCHAR_,
        PASTE_,
        CLICK_,
        INSERTLINE_,
        SETLINE_
    };

    Unsigned CharAfter()
    {
        WString LINE = GetLine(logic_y);
        Unsigned len = LINE.Size();
        if (logic_x < len)
            return LINE[logic_x];
        return MAXUnsigned;
    }


    Unsigned CharBefore()
    {
        WString LINE = GetLine(logic_y);
        if (0 < logic_x)
            return LINE[logic_x - 1];
        return MAXUnsigned;
    }

    void Undo()
    {
        do_pos -= 2;
        if (do_pos == MAXUnsigned || do_pos == (MAXUnsigned - 1))
        {
            do_pos = 0;
            return;
        }

        Unsigned sz = actions.size();
        if (sz == 0)
            return;

        int wch = actions[do_pos];

        switch (wch)
        {
        case ENTER_: // enter
            SelectEditBox::Input(8);//backspace
            break;

        case BACKSPACE_: // backspace
        {
            int _wch = actions[do_pos + 1];
            if (_wch == -1)
                SelectEditBox::Input(13);//enter
            else
                SelectEditBox::Input((wchar_t)_wch);
            break;
        }

        case LEFT_:
        {
            SelectEditBox::SpecialInput(VK_RIGHT, 0, 0);
            break;
        }

        case RIGHT_:
        {
            SelectEditBox::SpecialInput(VK_LEFT, 0, 0);
            break;
        }

        case UP_:
        {
            const int& x = actions[do_pos + 1];
            SelectEditBox::SpecialInput(VK_DOWN, 0, 0);
            LogicalSetCaret(x, logic_y);
            break;
        }

        case DOWN_:
        {
            const int& x = actions[do_pos + 1];
            SelectEditBox::SpecialInput(VK_UP, 0, 0);
            LogicalSetCaret(x, logic_y);
            break;
        }

        case END_:
        {
            const int& x = actions[do_pos + 1];
            LogicalSetCaret(x, logic_y);
            break;
        }

        case HOME_:
        {
            const int& x = actions[do_pos + 1];
            LogicalSetCaret(x, logic_y);
            break;
        }

        case DELETE_SELECTED:
        {
            const int& pos = actions[do_pos + 1];
            WString copywstr = data[pos];
            LogicEditBox::Paste(copywstr);

            WString data_string = data[pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos + 1]);

            select.logic_start_x = SD.x1;
            select.logic_start_y = SD.y1;
            select.logic_end_x = SD.x2;
            select.logic_end_y = SD.y2;
            select.is_selection_drawn = false;
            select.is_selecting = false;
            select.is_selected = true;
            Paint();
            break;
        }

        case DELETE_NORMAL:
        {
            const wchar_t& _wch = (wchar_t)actions[do_pos + 1];
            SelectEditBox::Input(_wch);
            LogicalSetCaret(logic_x - 1, logic_y);
            break;
        }

        case WCHAR_:
            SelectEditBox::Input(8);//backspace
            break;


        case PASTE_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            DelFromTo(SD.y1, SD.x1, SD.y2, SD.x2);
            Paint();
            break;
        }

        case CLICK_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            LogicalSetCaret(SD.x1, SD.y1);
            break;
        }


        case PRIOR_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            LogicalSetCaret(SD.x1, SD.y1);
            break;
        }

        case NEXT_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            LogicalSetCaret(SD.x1, SD.y1);
            break;
        }

        case INSERTLINE_:
        {
            const int& pos = actions[do_pos + 1];
            const Unsigned& line = ToType<Unsigned>(data[pos]);
            text[line] = 0;
            Paint();
            break;
        }

        case SETLINE_:
        {
            const int& pos = actions[do_pos + 1];
            const Unsigned& line = ToType<Unsigned>(data[pos]);
            const WString& LINE = data[pos + 1];
            LogicEditBox::SetLine(line, LINE);
            Paint();
        }
        }
    }

    void Redo()
    {
        const Unsigned& sz = actions.size();
        if (do_pos == sz || do_pos == sz - 1)
        {
            do_pos = (int)sz;
            return;
        }

        Unsigned wch = actions[do_pos];

        switch (wch)
        {
        case ENTER_: // enter
            SelectEditBox::Input(13);
            do_pos += 2;
            break;

        case BACKSPACE_: // backspace
            SelectEditBox::Input(8);//backspace
            do_pos += 2;
            break;

        case LEFT_:
            SelectEditBox::SpecialInput(VK_LEFT, 0, 0);
            do_pos += 2;
            break;

        case RIGHT_:
            SelectEditBox::SpecialInput(VK_RIGHT, 0, 0);
            do_pos += 2;
            break;

        case UP_:
            SelectEditBox::SpecialInput(VK_UP, 0, 0);
            do_pos += 2;
            break;

        case DOWN_:
            SelectEditBox::SpecialInput(VK_DOWN, 0, 0);
            do_pos += 2;
            break;

        case END_:
            SelectEditBox::SpecialInput(VK_END, 0, 0);
            do_pos += 2;
            break;

        case HOME_:
            SelectEditBox::SpecialInput(VK_HOME, 0, 0);
            do_pos += 2;
            break;

        case DELETE_SELECTED:
        {
            SelectEditBox::SpecialInput(VK_DELETE, 0, 0);
            do_pos += 2;
            break;
        }

        case DELETE_NORMAL:
        {
            SelectEditBox::SpecialInput(VK_DELETE, 0, 0);
            do_pos += 2;
            break;
        }

        case WCHAR_:
        {
            const wchar_t& _wch = (wchar_t)actions[do_pos + 1];
            SelectEditBox::Input(_wch);
            do_pos += 2;
            break;
        }

        case PASTE_:
        {
            const int& pos = actions[do_pos + 1];
            WString copywstr = data[pos + 1];
            LogicEditBox::Paste(copywstr);
            do_pos += 2;
            Paint();
            break;
        }

        case CLICK_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            do_pos += 2;
            LogicalSetCaret(SD.x2, SD.y2);
            break;
        }

        case PRIOR_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            LogicalSetCaret(SD.x2, SD.y2);
            do_pos += 2;
            break;
        }

        case NEXT_:
        {
            const int& pos = actions[do_pos + 1];
            struct Selection_Data SD = ToType<Selection_Data>(data[pos]);
            LogicalSetCaret(SD.x2, SD.y2);
            do_pos += 2;
            break;
        }

        case INSERTLINE_:
        {
            const int& pos = actions[do_pos + 1];
            Unsigned line = ToType<Unsigned>(data[pos]);
            WString LINE = data[pos + 1];
            if (line == 0)
                return;
            if (line >= GetLineNumber())
                SetLine(line, LINE);
            else
            {
                text[line] += LINE;
                text[line] ^ text[line + 1];
            }
            do_pos += 2;
            Paint();
            break;
        }

        case SETLINE_:
        {
            const int& pos = actions[do_pos + 1];
            const Unsigned& line = ToType<Unsigned>(data[pos]);
            text[line] = 0;
            do_pos += 2;
            break;
        }
        }
    }


protected:
    int do_pos;
    std::vector<int> actions;

    int data_pos;
    std::vector<WString> data;

    struct Selection_Data
    {
        Unsigned x1;
        Unsigned y1;
        Unsigned x2;
        Unsigned y2;
    };

    virtual void MouseSetCaret(float x, float y)
    {
        actions.erase(actions.begin() + do_pos, actions.end());
        actions.push_back(CLICK_);
        Selection_Data SD;
        SD.x1 = logic_x;
        SD.y1 = logic_y;
        SelectEditBox::MouseSetCaret(x, y);
        SD.x2 = logic_x;
        SD.y2 = logic_y;
        data.push_back(ToData(SD));
        actions.push_back(data_pos);
        data_pos++;
        do_pos += 2;
    }

    virtual void Input(wchar_t wch)
    {
        switch (wch)
        {
        case '\x03': // ctrl + 'c'
            SelectEditBox::Input(wch);
            break;

        case '\x16': // ctrl + 'v'
        {
            struct Selection_Data SD;
            SD.x1 = logic_x;
            SD.y1 = logic_y;
            SelectEditBox::Input(wch);
            SD.x2 = logic_x;
            SD.y2 = logic_y;
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(PASTE_);
            data.push_back(ToData(SD));
            actions.push_back(data_pos);
            data_pos++;
            {
                Clipboard cb;
                WString copywstr = cb.Paste();
                data.push_back(copywstr);
                data_pos++;
            }
            do_pos += 2;
            break;
        }

        case '\x01': // ctrl + 'a'
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::Input(wch);
            break;

        case '\x1A': // ctrl + 'z'
            Undo();
            break;

        case '\x19': // ctrl + 'y'
            Redo();
            break;

        case 13:  // enter
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(ENTER_);
            actions.push_back(-1);
            SelectEditBox::Input(wch);
            do_pos += 2;
            break;

        case 8:   // backspace
        {
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(BACKSPACE_);
            Unsigned wch_bef = CharBefore();
            actions.push_back((int)wch_bef);
            SelectEditBox::Input(wch);
            do_pos += 2;
            break;
        }

        default:
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(WCHAR_);
            actions.push_back(wch);
            SelectEditBox::Input(wch);
            do_pos += 2;
        }
    }

    virtual void SpecialInput(wchar_t wch, bool is_control_down, bool is_shift_down)
    {
        Unsigned pre_x = logic_x;
        Unsigned pre_y = logic_y;

        Unsigned wch_af = CharAfter();

        switch (wch)
        {
        case VK_LEFT:
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (pre_x != logic_x)
            {
                actions.push_back(LEFT_);
                actions.push_back(-1);
                do_pos += 2;
            }
            break;

        case VK_RIGHT:
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (pre_x != logic_x)
            {
                actions.push_back(RIGHT_);
                actions.push_back(-1);
                do_pos += 2;
            }
            break;

        case VK_UP:
            try
            {
                actions.erase(actions.begin() + do_pos, actions.end());
            }
            catch (...)
            {
                print_error("unhandled exception");
            }
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (pre_y != logic_y)
            {
                actions.push_back(UP_);
                actions.push_back((int)pre_x);
                do_pos += 2;
            }
            break;

        case VK_DOWN:
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (pre_y != logic_y)
            {
                actions.push_back(DOWN_);
                actions.push_back((int)pre_x);
                do_pos += 2;
            }
            break;

        case VK_END:
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            actions.push_back(END_);
            actions.push_back((int)pre_x);
            do_pos += 2;
            break;

        case VK_HOME:
            actions.erase(actions.begin() + do_pos, actions.end());
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            actions.push_back(HOME_);
            actions.push_back((int)pre_x);
            do_pos += 2;
            break;

        case VK_INSERT:
        {
            if (is_control_down)
            {
                actions.erase(actions.begin() + do_pos, actions.end());
                SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            else if (is_shift_down)
            {
                struct Selection_Data SD;
                SD.x1 = logic_x;
                SD.y1 = logic_y;
                SelectEditBox::Input(wch);
                SD.x2 = logic_x;
                SD.y2 = logic_y;
                actions.erase(actions.begin() + do_pos, actions.end());
                actions.push_back(PASTE_);
                data.push_back(ToData(SD));
                actions.push_back(data_pos);
                data_pos++;

                {
                    Clipboard cb;
                    WString copywstr = cb.Paste();
                    data.push_back(copywstr);
                    data_pos++;
                }

                do_pos += 2;
            }
            break;
        }

        case VK_DELETE:
            if (select.is_selected == true)
            {
                actions.erase(actions.begin() + do_pos, actions.end());
                actions.push_back(DELETE_SELECTED);
                WString copywstr = CopyFromTo
                (
                    select.logic_start_y,
                    select.logic_start_x,
                    select.logic_end_y,
                    select.logic_end_x
                );
                data.push_back(copywstr);
                data_pos++;
                struct Selection_Data SD =
                {
                    select.logic_start_x,
                    select.logic_start_y,
                    select.logic_end_x,
                    select.logic_end_y
                };
                data.push_back(ToData<Selection_Data>(SD));
                data_pos++;

                actions.push_back(data_pos - 2);
                do_pos += 2;
                SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            else
            {
                actions.erase(actions.begin() + do_pos, actions.end());
                SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                actions.push_back(DELETE_NORMAL);
                actions.push_back((int)wch_af);
                do_pos += 2;
            }

            break;

        case VK_PRIOR:
        {
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(PRIOR_);
            Selection_Data SD;
            SD.x1 = logic_x;
            SD.y1 = logic_y;
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            SD.x2 = logic_x;
            SD.y2 = logic_y;
            data.push_back(ToData(SD));
            actions.push_back(data_pos);
            data_pos++;
            do_pos += 2;
            break;
        }

        case VK_NEXT:
        {
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(NEXT_);
            Selection_Data SD;
            SD.x1 = logic_x;
            SD.y1 = logic_y;
            SelectEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            SD.x2 = logic_x;
            SD.y2 = logic_y;
            data.push_back(ToData(SD));
            actions.push_back(data_pos);
            data_pos++;
            do_pos += 2;
            break;
        }

        case VK_BACK: // backspace
        {
            if (is_shift_down == false)
                Undo();
            else
                Redo();
            break;
        }

        case VK_F1:
        {
            Undo();
            break;
        }


        case 16:
        case VK_F2:
            Redo();
            break;
        }
    }
public:
    void LogicalSetCaret(Unsigned x, Unsigned y)
    {
        Unsigned line_number = this->GetLineNumber();
        if (line_number < y)
            y = line_number;
        if (y == 0)
            y = 1;
        Unsigned len = GetCharNumber(y);
        if (len < x)
            x = len;
        logic_x = x;
        logic_y = y;
        is_refresh_necessary = true;
        InternalPaint();
    }

public:
    UndoEditBox(HDC hdc, RectF rf) :
        SelectEditBox(hdc, rf)
    {
        do_pos = 0;
        data_pos = 0;
    }
};

class ColorEditBox : public UndoEditBox
{
private:
    std::vector<WString> all_decos;
    std::vector<WString> range_decos;

    WString LINE0;
    Color current_text_color;
    Unsigned line;
    float xxxx;

    bool range;
    Color range_color;

    void Decompose(std::vector<WString>& decos, WString LINE)
    {
        decos.clear();
        WString CharSet = L" 	;:{}()8&!@#%~/\\|,.^*-[]'\"+=<>?«»`";
        while (true)
        {
            Unsigned first_part_len = LINE.nthOccurrence(CharSet);
            if (first_part_len == MAXUnsigned)
            {
                if (LINE != "")
                    decos.push_back(LINE);
                return;
            }
            wchar_t wch = LINE[first_part_len];

            WString deco1 = LINE.CutOff((Unsigned)first_part_len);
            WString deco2 = wch;
            if (deco1 != "")
                decos.push_back(deco1);
            decos.push_back(deco2);
        }
    }

    void PrintWord(WString word)
    {
        bool b = false;
        if (range)
        {
            write.SetColor(range_color);
        }
        else
            if (namecolor.Contains(word))
            {
                write.SetColor(namecolor.last_color);
                b = true;
            }

        Unsigned len = word.Size();
        for (Unsigned j = 0; j < len; j++)
        {
            wchar_t wch = word[j];
            float dist = CharWidth(wch);
            xxxx += dist;
            PrintAt(wch, xxxx - dist, corner_y + line * line_distance);
        }
        if (b)
            write.SetColor(current_text_color);
    }

protected:
    struct tagNameColor
    {
        typedef std::map<WString, Color>           MAP;
        typedef MAP::iterator                ITERATOR;
        typedef std::pair<WString, Color>          PAIR;
        MAP m;

        Color last_color; // color found by Contains() method

        void Add(WString wstr, Color cl)
        {
            m.insert(PAIR(wstr, cl));
        }

        void Del(WString wstr)
        {
            for (ITERATOR it = m.begin(); it != m.end(); it++)
                if (it->first == wstr)
                {
                    m.erase(it);
                    break;
                }
        }

        bool Contains(WString wstr)
        {
            for (ITERATOR it = m.begin(); it != m.end(); it++)
                if (it->first == wstr)
                {
                    last_color = it->second;
                    return true;
                }
            return false;
        }


    } namecolor;

    virtual void PrintLine(Unsigned line_number)
    {
        line = line_number;
        xxxx = corner_x + start_deltas[line];
        WString LINE = box_tex[line];
        current_text_color = write.GetColor();
        Decompose(all_decos, LINE);

        Unsigned all_size = all_decos.size();
        Unsigned range_size = range_decos.size();

        range = false;
        write.LimitDrawing(corner_x + 1, corner_y, box.Width() - 1, box.Height());

        std::vector<WString> maybe_decos;
        for (Unsigned j = 0; j < all_size; j++)
        {
            bool b = (range_size <= all_size + 1) && (j < all_size - range_size + 1) && (0 < range_size);
            if (b)
                if (all_decos[j] == range_decos[0]) // optimize for speed
                {
                    maybe_decos.clear();
                    for (Unsigned r = 0; r < range_size; r++)
                        maybe_decos.push_back(all_decos[j + r]);
                    if (maybe_decos == range_decos)
                        range = true;
                    maybe_decos.clear();
                }
            PrintWord(all_decos[j]);
        }
        write.SetColor(current_text_color);
        write.RemoveDrawingLimits();
    }

    virtual void AddDecosStringAndColor(const WString& str, Color color)
    {
        std::vector<WString> v;
        Decompose(v, str);
        for (Unsigned i = 0; i < v.size(); i++)
            namecolor.Add(v[i], color);
    }

    virtual void AddRangeString(const WString& rangestr, Color color = Color(255, 130, 130, 130)) // eg: rangestr may be L"//" in a C++ text
    {
        Decompose(range_decos, rangestr);
        range_color = color;
    }

public:
    ColorEditBox(HDC hdc, RectF rf) :
        UndoEditBox(hdc, rf)
    {
        range = false;
    }
};



class EditOptions : public ColorEditBox
{
protected:
    bool stop_painting;
    bool stop_edit;

    bool use_allowed_chars;
    WString ALLOWED_CHARS;

    bool use_allowed_start_chars;
    WString START_CHARS; // charcter only allowsed at start

    void InternalPaint()
    {
        if (!stop_painting)
            ColorEditBox::InternalPaint();
    }

    void EquippedInput(wchar_t wch)
    {
        if (stop_edit == true)
        {
            switch (wch)
            {
            case '\x03':
                break;

            default:
                return;
            }
        }

        if (use_allowed_chars == true)
        {
            if (ALLOWED_CHARS.IsExactSuperstring(wch))
            {
                if (use_allowed_start_chars == true)
                {
                    if (START_CHARS.IsExactSuperstring(wch))
                    {
                        if (logic_x == 0)
                        {
                            WString LINE = GetLine(logic_y);
                            if (LINE == "")
                                ColorEditBox::Input(wch);
                            else
                            {
                                wchar_t wch0 = LINE[0];
                                if (!START_CHARS.IsExactSuperstring(wch0))
                                    ColorEditBox::Input(wch);
                                else
                                    MessageBeep(UINT(-1));
                            }
                        }
                        else
                            MessageBeep(UINT(-1));
                    }
                    else
                    {
                        if (logic_x == 0)
                        {
                            WString LINE = GetLine(logic_y);
                            if (LINE == "")
                                ColorEditBox::Input(wch);
                            else
                            {
                                wchar_t wch0 = LINE[0];
                                if (!START_CHARS.IsExactSuperstring(wch0))
                                    ColorEditBox::Input(wch);
                                else
                                    MessageBeep(UINT(-1));
                            }
                        }
                        else
                            ColorEditBox::Input(wch);
                    }
                }
                else
                    ColorEditBox::Input(wch);
            }
            else
                MessageBeep(UINT(-1));
        }
        else
            ColorEditBox::Input(wch);
    }


public:
    ACTION AfterEnter;
    ACTION AfterMoveRight;
    ACTION AfterMoveLeft;

    WString action_param;

    virtual void Input(wchar_t wch, bool after = true)
    {
        switch (wch)
        {
        case '\x03': // ctrl + 'c'
            EquippedInput(wch);
            break;

        case '\x16': // ctrl + 'v'
            if (use_allowed_chars)
            {
                Clipboard cb;
                WString copystr = cb.Paste();
                if (ALLOWED_CHARS.IsSuperset(copystr))
                {
                    if (use_allowed_start_chars)
                    {
                        if (copystr.IsSuperset(START_CHARS))
                        {
                            Unsigned size = copystr.Size();
                            if (size > 0)
                            {
                                if (!copystr[1][size - 1].IsSuperset(START_CHARS))
                                    EquippedInput(wch);
                                else
                                    MessageBeep(UINT(-1));
                            }
                            else
                                EquippedInput(wch);
                        }
                        else
                            EquippedInput(wch);
                    }
                    else
                        EquippedInput(wch);
                }
                else
                    MessageBeep(UINT(-1));
            }
            else
                EquippedInput(wch);
            break;

        case '\x01': // ctrl + 'a'
            EquippedInput(wch);
            break;

        case '\x1A': // ctrl + 'z'
            EquippedInput(wch);
            break;

        case '\x19': // ctrl + 'y'
            EquippedInput(wch);
            break;

        case 13:  // enter
            if (!only_one_line)
            {
                EquippedInput(wch);
                if (after)
                    AfterEnter(action_param);
            }
            break;

        case 8:   // backspace
            EquippedInput(wch);
            break;

        default:
            EquippedInput(wch);
        }
    }

    virtual void SpecialInput(wchar_t wch, bool is_control_down, bool is_shift_down, bool after = true)
    {
        switch (wch)
        {
        case VK_LEFT:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (after)
                AfterMoveLeft(action_param);
            break;

        case VK_RIGHT:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            if (after)
                AfterMoveRight(action_param);
            break;

        case VK_UP:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_DOWN:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_END:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_HOME:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_INSERT:
            if (is_control_down)
            {
                ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            else if (is_shift_down)
            {
                if (stop_edit == false)
                {
                    if (use_allowed_chars)
                    {
                        Clipboard cb;
                        WString copystr = cb.Paste();
                        if (ALLOWED_CHARS.IsSuperset(copystr))
                        {
                            if (use_allowed_start_chars)
                            {
                                if (copystr.IsSuperset(START_CHARS))
                                {
                                    Unsigned size = copystr.Size();
                                    if (size > 0)
                                    {
                                        if (!copystr[1][size - 1].IsSuperset(START_CHARS))
                                            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                                        else
                                            MessageBeep(UINT(-1));
                                    }
                                    else
                                        ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                                }
                                else
                                    ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                            }
                            else
                                ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                        }
                    }
                    else
                        ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
                }
                else
                    MessageBeep(UINT(-1));
            }
            break;

        case VK_DELETE:
            if (select.is_selected == true)
            {
                if (stop_edit == false)
                    ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            else
            {
                if (stop_edit == false)
                    ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            break;

        case VK_PRIOR:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_NEXT:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case VK_BACK: // backspace
            if (is_shift_down == false)
            {
                if (stop_edit == false)
                    ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            else
            {
                if (stop_edit == false)
                    ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            }
            break;

        case VK_F1:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        case 16:
        case VK_F2:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
            break;

        default:
            ColorEditBox::SpecialInput(wch, is_control_down, is_shift_down);
        }
    }

    void SetActionParam(WString _data)
    {
        action_param = _data;
    }
    //-----------------------------------------------
    void SetFont(WString font_name)
    {
        write.SetFont(font_name);
    }

    void Italic()
    {
        write.Italic();
    }

    void Bold()
    {
        write.Bold();
    }

    void BoldItalic()
    {
        write.BoldItalic();
    }

    void SetSize(float size)
    {
        write.SetSize(size);
    }

    void SetBackground(Color color)
    {
        box.SetBackground(color);
    }

    Unsigned GetCurrentLineNumber()
    {
        return logic_y;
    }

    void SetTextColor(Color color)
    {
        write.SetColor(color);
    }

    void SetLineDistance(float distance)
    {
        line_distance = distance;
    }

    void newColoredWord(WString word, Color color)
    {
        namecolor.Add(word, color);
    }

    void deleteColoredWord(WString word)
    {
        namecolor.Del(word);
    }
    //------------------------------------------------------------------------------
    Unsigned GetNumberOfLines()
    {
        return GetLineNumber();
    }
    //------------------------------------------------------------------------------
    void WriteString(WString wstr, bool with_undo = false)
    {
        bool sel = select.is_selected;
        if (sel == true)
            CancelSelection();
        if (wstr == L"")
            return;
        struct Selection_Data SD;
        if (with_undo)
        {
            SD.x1 = logic_x;
            SD.y1 = logic_y;
        }
        WString copywstr = wstr;

        bool new_line_at_end = false;
        wstr.Reverse();
        if (wstr[0] == L'\n')
            new_line_at_end = true;
        wstr.Reverse();

        std::vector<WString> v;
        while (wstr != L"")
            v.push_back(wstr.CutOff(L"\n"));
        Unsigned sz = v.size();
        bool temp = stop_painting;
        stop_painting = true;
        for (Unsigned i = 0; i < sz; i++)
        {
            LogicEditBox::Paste(v[i]);
            if (i < sz - 1 || new_line_at_end)
                ColorEditBox::Input(L'\r');
        }
        stop_painting = temp;

        if (with_undo)
        {
            SD.x2 = logic_x;
            SD.y2 = logic_y;
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(PASTE_);
            data.push_back(ToData(SD));
            actions.push_back(data_pos);
            data_pos++;
            data.push_back(copywstr);
            data_pos++;
            do_pos += 2;
        }
        if (sel == true)
            LogicSelect
            (
                select.logic_start_y,
                select.logic_start_x,
                select.logic_end_y,
                select.logic_end_x
            );
        if (!stop_painting)
            Paint();
    }



    /*void WriteString(String str)
    {
        WriteString(WString(CS(str)));
    } */

    EditOptions& cout(WString wstr)
    {
        WriteString(wstr, false);
        return *this;
    }


    void WriteData(WString wstr, bool with_undo = false)
    {
        bool sel = select.is_selected;
        if (sel == true)
            CancelSelection();
        if (wstr == L"")
            return;
        struct Selection_Data SD;
        if (with_undo)
        {
            SD.x1 = logic_x;
            SD.y1 = logic_y;
        }
        WString copywstr = wstr;

        //bool new_line_at_end = false;

        bool temp = stop_painting;
        stop_painting = true;
        LogicEditBox::PutString(wstr);
        stop_painting = temp;

        if (with_undo)
        {
            SD.x2 = logic_x;
            SD.y2 = logic_y;
            actions.erase(actions.begin() + do_pos, actions.end());
            actions.push_back(PASTE_);
            data.push_back(ToData(SD));
            actions.push_back(data_pos);
            data_pos++;
            data.push_back(copywstr);
            data_pos++;
            do_pos += 2;
        }
        if (sel == true)
            LogicSelect
            (
                select.logic_start_y,
                select.logic_start_x,
                select.logic_end_y,
                select.logic_end_x
            );
        if (!stop_painting)
            Paint();
    }

    void SetLine(Unsigned i, WString LINE)
    {
        LogicEditBox::SetLine(i, LINE);
        actions.push_back(SETLINE_);
        actions.push_back(data_pos);
        data.push_back(ToData(i));
        data.push_back(LINE);
        do_pos += 2;
        data_pos += 2;
        Paint();
    }

    WString GetLine(Unsigned i)
    {
        return LogicEditBox::GetLine(i);
    }

    void InsertLine(Unsigned i, WString LINE)
    {
        if (i == 0)
            return;
        if (i >= GetNumberOfLines())
            SetLine(i, LINE);
        else
        {
            text[i] += LINE;
            text[i] ^ text[i + 1];
        }

        actions.push_back(INSERTLINE_);
        actions.push_back(data_pos);
        data.push_back(ToData(i));
        data.push_back(LINE);
        data_pos += 2;
        do_pos += 2;
        Paint();
    }


    void GotoXY(Unsigned x, Unsigned y)// not undo
    {
        logic_x = x;
        logic_y = y;
        Paint();
    }

    void HideBlinker()
    {
        HideCaret();
        blinking = false;
    }

    void ShowBlinker()
    {
        if (!stop_painting)
            ShowCaret();
        blinking = true;
    }

    bool isBlinkingAllowed()
    {
        return blinking;
    }

    void Hide(Color color = back_color)
    {
        stop_painting = true;
        HideCaret();
        RectF rf(corner_x, corner_y, box.Width() + 1, box.Height() + 1);
        box.SetColor(color);
        box.RectInterior(rf);
    }

    void Show()
    {
        stop_painting = false;
        ShowCaret();
        Paint();
    }


    void StopEdit()
    {
        stop_edit = true;
    }

    void AllowEdit()
    {
        stop_edit = false;
    }

    bool isEditAllowed()  // allow edit?
    {
        return !stop_edit;
    }

    //==============================================================================
    void Open(WString addr)
    {
        WString stdaddr = (wchar_t*)(WCS(addr));
        SimpleFile file(stdaddr);
        text.Empty();
        Unsigned n = 0;
        Unsigned file_size = file.GetFileLength();

        while (true)
        {
            if (file.GetPointerPosition() == file_size)
                break;
            n++;
            String cut = file.MultiCutOff(LineEndA);
            Unsigned len = cut.Size();
            wchar_t* c = new wchar_t[len];
            WString wstr(c, len);
            delete[] c;
            for (Unsigned i = 0; i < len; i++)
                wstr[i] = cut[i];
            SetLine(n, wstr);
        }
        Paint();
    }

    void SaveAs(WString addr)
    {
        SimpleFile file(addr);
        file.SetFileLength(0);
        Unsigned lines = GetNumberOfLines();
        for (Unsigned i = 1; i <= lines; i++)
        {
            String LINE = GetLine(i);
            file << LINE << LineEndA;
        }
    }

    void DelCppCommentSaveAs(WString addr)
    {
        SimpleFile file(addr);
        file.SetFileLength(0);
        Unsigned lines = GetNumberOfLines();
        for (Unsigned i = 1; i <= lines; i++)
        {
            String LINE = (wchar_t*)(WCS)GetLine(i);
            LINE = LINE.CutOff("//");
            file << LINE << LineEndA;
        }
    }
    //=============================================================================
    void Clear(bool paint = true)
    {
        text.Empty();
        //SetLine(1,L"");
        logic_x = 0;
        logic_y = 1;
        if (paint)
            Paint();
    }

    void UseAllowedChars(WString CharSet)
    {
        ALLOWED_CHARS += CharSet;
        use_allowed_chars = true;
    }

    void NotAllowedChars()
    {
        ALLOWED_CHARS = L"\x03\x16\x01\x1A\x19\x08";
        use_allowed_chars = false;
    }

    void UseAllowedStartChars(WString CharSet) // chars that can be used at start of line
    {
        START_CHARS += CharSet;
        use_allowed_start_chars = true;
    }

    void NotAllowedStartChars()
    {
        START_CHARS = L"";
        use_allowed_start_chars = false;
    }

    WString GetAllText()
    {
        Unsigned end_y = GetNumberOfLines();
        Unsigned end_x = GetCharNumber(end_y);
        return CopyFromTo(1, 0, end_y, end_x);
    }

    bool inBox(PointF P)
    {
        if (P.X < corner_x)
            return false;
        if (corner_x + box.Width() < P.X)
            return false;
        if (P.Y < corner_y)
            return false;
        if (corner_y + box.Height() < P.Y)
            return false;
        return true;
    }

    bool inExBox(PointF P) // in edit box when it is extended
    {
        if (P.X < corner_x - 10)
            return false;
        if (corner_x + box.Width() + 10 < P.X)
            return false;
        if (P.Y < corner_y - 10)
            return false;
        if (corner_y + box.Height() + 10 < P.Y)
            return false;
        return true;
    }

    void UniSize()
    {
        unisize = true;
    }

    void MultiSize()
    {
        unisize = false;
    }

    virtual void AddDecosStringAndColor(WString str, Color color)
    {
        ColorEditBox::AddDecosStringAndColor(str, color);
    }

    virtual void AddRangeString(WString rangestr) // eg: rangestr may be L"//" in a C++ text
    {
        ColorEditBox::AddRangeString(rangestr);
    }

    void OnlyOneLine()
    {
        only_one_line = true;
    }

    void MultiLine()
    {
        only_one_line = false;
    }

    void NoNewLayer()
    {
        new_layer_allowed = false;
    }

    void AllowNewLayer()
    {
        new_layer_allowed = true;
    }

    void Select(Unsigned x1, Unsigned y1, Unsigned x2, Unsigned y2)
    {
        GotoXY(x1, y1);
        StartSelection(caret_x, caret_y);
        GotoXY(x2, y2);
        EndSelection(caret_x, caret_y);
    }

public:
    EditOptions(HDC hdc, RectF rf) :
        ColorEditBox(hdc, rf)
    {
        blinking = true;
        stop_painting = false;
        stop_edit = false;
        only_one_line = false;

        NotAllowedChars();
        NotAllowedStartChars();

        AfterEnter = ZERO;
        AfterMoveLeft = ZERO;
        AfterMoveRight = ZERO;
        action_param = ToData<void*>(this);
    }
};

class EditBox : public EditOptions
{
private:

    bool conditions()
    {
        return (stop_painting == false);
    }

    bool keyconditions()
    {
        return true;
    }

public:
    bool is_focused;
    bool editing_allowed;
    bool blinking_allowed;

    void NeverLetEdit()
    {
        StopEdit();
        editing_allowed = false;
    }

    void NeverLetBlinking()
    {
        HideBlinker();
        blinking_allowed = false;
    }

    void Focus()
    {
        is_focused = true;
        if (editing_allowed)
            AllowEdit();
        if (blinking_allowed)
            ShowBlinker();
    }

    void Unfocus()
    {
        is_focused = false;
        if (editing_allowed)
            StopEdit();
        if (blinking_allowed)
            HideBlinker();
        if (select.is_selected == true)
        {
            CancelSelection();
            Paint();
        }
    }


public:
    void wmPaint(WPARAM , LPARAM )
    {
        if (!conditions())
            return;
        Paint();
    }

    void wmKeydown(WPARAM wParam, LPARAM )
    {
        if (!is_focused)
            return;

        if (!conditions())
            return;
        bool is_control_down = isControlDown();
        bool is_shift_down = isShiftDown();

        if (!keyconditions())
        {
            if (is_control_down)
            {
                if
                    (
                        VK_LEFT == wParam ||
                        VK_UP == wParam ||
                        VK_RIGHT == wParam ||
                        VK_DOWN == wParam ||
                        VK_INSERT == wParam ||
                        VK_HOME == wParam ||
                        VK_END == wParam ||
                        VK_PRIOR == wParam || // page up
                        VK_NEXT == wParam
                        )
                    this->SpecialInput((wchar_t)wParam, is_control_down, is_shift_down);
            }
            else
            {
                if
                    (
                        VK_LEFT == wParam ||
                        VK_UP == wParam ||
                        VK_RIGHT == wParam ||
                        VK_DOWN == wParam ||
                        VK_HOME == wParam ||
                        VK_END == wParam ||
                        VK_PRIOR == wParam || // page up
                        VK_NEXT == wParam  // page down
                        )
                    this->SpecialInput((wchar_t)wParam, is_control_down, is_shift_down);
            }
            return;
        }

        if
            (
                VK_LEFT == wParam ||
                VK_UP == wParam ||
                VK_RIGHT == wParam ||
                VK_DOWN == wParam ||
                VK_INSERT == wParam ||
                VK_DELETE == wParam ||
                VK_HOME == wParam ||
                VK_END == wParam ||
                VK_PRIOR == wParam || // page up
                VK_NEXT == wParam || // page down
                VK_F1 == wParam ||
                VK_F2 == wParam
                )
            this->SpecialInput((wchar_t)wParam, is_control_down, is_shift_down);
    }

    void wmSyskeydown(WPARAM wParam, LPARAM )
    {
        if (!is_focused)
            return;
        if (!keyconditions())
            return;
        if (!conditions())
            return;
        if (wParam == VK_BACK)
            this->SpecialInput((wchar_t)wParam, isControlDown(), isShiftDown());
    }

    void wmChar(WPARAM wParam, LPARAM )
    {
        if (!is_focused)
            return;
        if (!keyconditions())
        {
            if (wParam == '\x03')      //copy
                this->Input((wchar_t)wParam);
            return;
        }
        if (!conditions())
            return;
        this->Input((wchar_t)wParam);
    }

    void wmTimer(WPARAM , LPARAM )
    {
        if (!conditions())
            return;
        if (blinking == true)
            this->BlinkCaret();
    }

    void wmTimer2(WPARAM , LPARAM )
    {
    }

    void wmMousewheel(WPARAM wParam, LPARAM lParam)
    {
        if (!is_focused)
            return;
        if (!conditions())
            return;
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inExBox(P))
            return;

        if (HIWORD(wParam) <= WHEEL_DELTA)
        {
            for (Unsigned i = 0; i < 3; i++)
                this->SpecialInput(VK_UP, 0, 0);
        }
        else
        {
            for (Unsigned i = 0; i < 3; i++)
                this->SpecialInput(VK_DOWN, 0, 0);
        }
    }

    void wmLbuttondown(WPARAM wParam, LPARAM lParam) // left click down
    {
        if (!conditions())
            return;
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inExBox(P))
        {
            Unfocus();
            return;
        }
        else
            Focus();

        if (select.is_selected == true)
        {
            if (isShiftDown() == false)
            {
                CancelSelection();
                Paint();
            }
            else
            {
                select.is_selecting = true;
                wmMousemove(wParam, lParam);
                return;
            }
        }
        StartSelection(LOWORD(lParam), HIWORD(lParam));
        ShowSelection();
        //Paint();
    }

    void wmRbuttondown(WPARAM , LPARAM lParam)
    {
        if (!conditions())
            return;
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inExBox(P))
        {
            Unfocus();
            return;
        }
        else
            Focus();
    }

    void wmLbuttonup(WPARAM , LPARAM lParam)  // left click up
    {
        if (!is_focused)
            return;
        if (!conditions())
            return;
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inExBox(P))
            return;
        EndSelection(P.X, HIWORD(lParam));
        ShowSelection();
        //Paint();
    }

    void wmMousemove(WPARAM , LPARAM lParam)
    {
        if (!conditions())
            return;
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inExBox(P))
            return;

        if (isLClicked())
        {
            if (select.is_selecting)
            {
                HideSelection();
                EndSelection(LOWORD(lParam), HIWORD(lParam));
                StartSelection(LOWORD(lParam), HIWORD(lParam));
                ShowSelection();
            }
        }
        //Paint();
    }

    void wmMouseleave(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

public:
    EditBox(HDC hdc, RectF rf) :
        EditOptions(hdc, rf)
    {
        is_focused = true;
        this->editing_allowed = true;
        this->blinking_allowed = true;
        //Paint();
    }
};


#endif
