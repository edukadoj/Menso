#ifndef ContextH
#define ContextH

#include "General.h"
#include "Draw.h"
#include "Write.h"

class Context : public AdDraw
{
public:
    RectF box;
    bool is_shown;

    ACTION onshow;
    WString param;

    void Show()
    {
        LimitDrawing(box);
        onshow(param);
        is_shown = true;
    }

    void Hide(Color _color = back_color)
    {
        if (is_shown)
        {
            SetColor(_color);
            RectInterior(RectF(box.X - 1, box.Y - 1, box.Width + 2, box.Height + 2));
            is_shown = false;
        }
    }

    bool inBox(PointF P)
    {
        return box.Contains(P);
    }

    void Move(float dx, float dy)
    {
        bool status = false;
        if (is_shown)
        {
            status = true;
            Hide();
        }
        box.X += dx;
        box.Y += dy;
        if (status)
            Show();
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

    void wmMousemove(WPARAM , LPARAM )
    {

    }

    void wmMouseleave(WPARAM , LPARAM )
    {

        if (!is_focused)
            return;
    }

public:
    Context(HDC hdc, RectF box) : AdDraw(hdc), box(box)
    {
        is_shown = false;
    }

    ~Context()
    {
    }
};

#endif	  