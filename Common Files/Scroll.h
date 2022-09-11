#ifndef ScrollH
#define ScrollH

#include "General.h"
#include "Draw.h"
#include "Write.h"

class HScroll : protected AdDraw
{
    RectF box;
    Unsigned slider;
    Unsigned all;
    Write write;

    bool is_moving;

public:
    ACTION aftermoving;
    ACTION whilemoving;
    WString param;


    bool is_shown;

    void Show()
    {
        RoundLightRec(box);

        float x = box.X + 4.5f;
        if (all != 0)
            x += (box.Width - 9) * slider / all;
        RectF int_box(x - 3.3f, box.Y + 2, 6.2f, box.Height - 4);


        SetColor(255, 230, 230, 230);
        RectInterior(Shrink(box));

        RoundLightRec(int_box, 1, Color(255, 0, 0, 0), Color(255, 0, 0, 0));
        for (float f = 1; f < int_box.Width / 2 && f < int_box.Height / 2; f += 1)
            RoundLightRec(Shrink(int_box, f), 1);

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
        //RectF rect(box.X,box.Y - 10,box.Width,box.Height + 20);
        return box.Contains(P);
    }

    Unsigned GetSlider()
    {
        return slider;
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

    void StartMoving()
    {
        is_moving = true;
    }

    void EndMoving(bool do_action)
    {
        if (is_moving == true)
        {
            if (do_action)
                aftermoving(param);
            is_moving = false;
        }
    }

    void Set(Unsigned new_slider, Unsigned new_all)
    {
        slider = new_slider;
        all = new_all;
        if (slider > all)
            slider = all;
        if (is_shown)
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
            StartMoving();
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
        if (is_moving)
            EndMoving(true);
        if (!is_focused)
            return;
    }

    void wmMousemove(WPARAM , LPARAM lParam)
    {
        if (is_moving)
        {
            PointF P(LOWORD(lParam), HIWORD(lParam));
            float x = P.X - box.X + box.Width / (2 * all);
            if (x >= 0)
            {
                Unsigned size = (Unsigned)((x * all) / box.Width);
                Set(size, all);
            }
            else
                Set(0, all);
            whilemoving(param);
        }
    }

    void wmMouseleave(WPARAM , LPARAM )
    {
        if (is_moving)
            EndMoving(false);
        if (!is_focused)
            return;
    }

public:
    HScroll(HDC hdc, RectF box, Unsigned all) : AdDraw(hdc), box(box), write(hdc), all(all)
    {
        slider = 0;
        is_moving = false;
        is_shown = false;
    }

    ~HScroll()
    {

    }
};

#endif