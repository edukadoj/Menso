#ifndef ProgressReportH
#define ProgressReportH
#include "General.h"
#include "Draw.h"
#include "Write.h"
#include "Devcon.h"

//extern Window* pwin;
extern Devcon* pdevcon;

class ProgressReport : protected AdDraw
{
    RectF box;
    int percent;
    Write write;

public:
    bool is_shown;

    void Show()
    {
        pdevcon->PreventUpdate();
        RoundLightRec(box);

        float width = (box.Width - 6) * percent / 100;
        RectF int_box(box.X + 3, box.Y + 3, width, box.Height - 6);

        SetColor(255, 250, 250, 250);
        RectInterior(RectF(box.X + 1, box.Y + 1, box.Width - 2, box.Height - 2));
        for (float f = int_box.Y + 1; f <= int_box.GetBottom(); f += 3)
        {
            SetColor(255, 0, 250, 0);
            this->Line(PointF(int_box.X, f), PointF(int_box.GetRight(), f));
        }
        RoundLightRec(int_box, 1);

        WString text = WString(percent) + L"%";
        float x = box.X + box.Width / 2 - write.StringWidth(text) / 2 - 2;
        float y = box.Y + box.Height / 2 - write.StringHeight(text) / 2 - 3;
        write.Gotoxy(x, y);
        write.SetColor(255, 255, 0, 0);
        write << text;
        pdevcon->AllowUpdate();
        pdevcon->UpdateHDC();

        is_shown = true;
    }

    void Hide(Color _color = back_color)
    {
        SetColor(_color);
        RectInterior(RectF(box.X - 1, box.Y - 1, box.Width + 2, box.Height + 2));
        is_shown = false;
    }

    bool inBox(PointF P)
    {
        return box.Contains(P);
    }

    void Move(float dx, float dy)
    {
        Hide();
        box.X += dx;
        box.Y += dy;
        Show();
    }

    void SetPercent(int new_percent)
    {
        percent = new_percent;
        Paint();
    }

    void Paint()
    {
        if (is_shown)
            Show();
        else
            Hide();
    }

    //========================================================
public:
    bool is_focused;

    void Focus()
    {
        is_focused = true;
    }

    void Unfocus()
    {
        is_focused = false;
    }

public:
    void wmPaint(WPARAM , LPARAM )
    {
        Paint();
    }

    void wmKeydown(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmSyskeydown(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmChar(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmTimer(WPARAM , LPARAM )
    {
    }

    void wmTimer2(WPARAM , LPARAM )
    {
    }

    void wmMousewheel(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmLbuttondown(WPARAM , LPARAM lParam) // left click down
    {
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (inBox(P))
        {
            Focus();
            return;
        }
        else
            Unfocus();
    }

    void wmRbuttondown(WPARAM , LPARAM lParam)
    {
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (!inBox(P))
        {
            Unfocus();
            return;
        }
        else
            Focus();
    }

    void wmLbuttonup(WPARAM , LPARAM ) // left click up
    {
        if (!is_focused)
            return;
    }

    void wmMousemove(WPARAM , LPARAM lParam)
    {
        PointF P(LOWORD(lParam), HIWORD(lParam));
    }

    void wmMouseleave(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

public:
    ProgressReport(HDC hdc, RectF box) : AdDraw(hdc), box(box), write(hdc)
    {
        percent = 0;
    }

    ~ProgressReport()
    {

    }
};

#endif