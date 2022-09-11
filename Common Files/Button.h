#ifndef ButtonH 
#define ButtonH

#include "General.h"
#include "Draw.h"
#include "WString.h"
#include "Devcon.h"
#include "Write.h"


class PrimaryButton : protected AdDraw
{
public:
    WString text; // text on button
    RectF box;
    Color interior;
    bool is_pressed;
    Write write;
    HDC hdc;
    bool stop_painting;

    WString button_action_param;
    ACTION action1; // by press
    ACTION action2; // by release

    virtual void DrawRelease()
    {
        SetColor(interior);
        RectInterior(RectF(box.X + 2, box.Y + 2, box.Width - 3, box.Height - 3));
        RoundLightRec(box);
        RoundLightRec(RectF(box.X + 1, box.Y + 1, box.Width - 2, box.Height - 2));
        float x = box.X + box.Width / 2 - write.StringWidth(text) / 2 - 2;
        float y = box.Y + box.Height / 2 - write.StringHeight(text) / 2 - 3;
        write.Gotoxy(x, y);
        write << text;
    }

    virtual void DrawPress()
    {
        SetColor(interior);
        RectInterior(RectF(box.X + 1, box.Y + 1, box.Width - 1, box.Height - 1));
        Color black(255, 0, 0, 0);
        RoundLightRec(box, 1, black, black);
        RoundLightRec(RectF(box.X + 1, box.Y + 1, box.Width - 2, box.Height - 2));
        RoundLightRec(RectF(box.X + 2, box.Y + 2, box.Width - 4, box.Height - 4));
        float x = box.X + box.Width / 2 - write.StringWidth(text) / 2 - 2;
        float y = box.Y + box.Height / 2 - write.StringHeight(text) / 2 - 3;
        x--;
        y++;
        write.Gotoxy(x, y);
        write << text;
    }

    void Paint()
    {
        if (stop_painting == true)
            return;
        if (is_pressed == true)
            DrawPress();
        else
            DrawRelease();
    }

    void Press(bool act = true)
    {
        if (is_pressed == false)
        {
            is_pressed = true;
            if (act)
                action1(button_action_param);
        }
    }

    void Release(bool act = true)
    {
        if (is_pressed == true)
        {
            is_pressed = false;
            if (act)
                action2(button_action_param);
        }
    }

    bool inBox(PointF P)
    {
        return box.Contains(P);
    }

    void SetButtonActionParam(WString data)
    {
        button_action_param = data;
    }

    void SetIntColor(Color _color)
    {
        interior = _color;
    }

    Color GetIntColor()
    {
        return interior;
    }

public:
    PrimaryButton(HDC hdc, RectF rf, WString text) :
        hdc(hdc), AdDraw(hdc), write(hdc), box(rf), text(text)
    {
        interior = Color(255, 100, 100, 220);
        is_pressed = false;
        write.SetColor(Color(255, 255, 255, 240));
        write.SetSize(15);
        write.LimitDrawing(box.X - 1, box.Y - 1, box.Width - 2, box.Height - 2);
        action1 = ZERO;
        action2 = ZERO;
    }
};

class OptionButton : public PrimaryButton
{
public:
    void Hide(Color _color = back_color)
    {
        stop_painting = true;
        SetColor(_color);
        RectInterior(RectF(box.X - 1, box.Y - 1, box.Width + 2, box.Height + 2));
    }

    void Show()
    {
        stop_painting = false;
        Paint();
    }

public:

    OptionButton(HDC& hdc, RectF rf, WString text)
        : PrimaryButton(hdc, rf, text)
    {
    }
};


class Button : public OptionButton
{
    bool change_color;
public:
    bool is_focused;

    void Focus()
    {
        is_focused = true;
    }

    void Unfocus()
    {
        if (this->is_pressed)
            Release(false);
        is_focused = false;
    }


    void On() // lamp on
    {
        if (!change_color)
        {
            Color _color = GetIntColor();
            _color = Color(_color.GetA(), _color.GetR() + 20, _color.GetG() + 20, _color.GetB() + 20);
            SetIntColor(_color);
            Paint();
            change_color = true;
        }
    }


    void Off() // lamp off
    {
        if (change_color)
        {
            Color _color = GetIntColor();
            _color = Color(_color.GetA(), _color.GetR() - 20, _color.GetG() - 20, _color.GetB() - 20);
            SetIntColor(_color);
            Paint();
            change_color = false;
        }
    }
public:
    void wmPaint(WPARAM , LPARAM )
    {

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

    void wmTimer2(WPARAM , Unsigned )
    {

    }

    void wmMousewheel(WPARAM , Unsigned )
    {
        if (!is_focused)
            return;
    }

    void wmLbuttondown(WPARAM , LPARAM lParam) // left click down
    {
        PointF P(LOWORD(lParam), HIWORD(lParam));
        if (inBox(P))
        {
            Press();
            Focus();
            Paint();
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

        if (this->is_pressed)
        {
            Release(false);
            Paint();
        }
    }

    void wmLbuttonup(WPARAM , LPARAM ) // left click up
    {
        if (!is_focused)
            return;
        Release();
        Paint();
    }

    void wmMousemove(WPARAM , LPARAM lParam)
    {
        PointF P(LOWORD(lParam), HIWORD(lParam));

        if (inBox(P))
            On();
        else
        {
            Unfocus();
            Off();
        }
    }

    void wmMouseleave(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
        if (is_pressed)
        {
            Release(false);
            Paint();
        }
        if (change_color == true)
        {
            Color _color = GetIntColor();
            _color = Color(_color.GetA(), _color.GetR() - 20, _color.GetG() - 20, _color.GetB() - 20);
            SetIntColor(_color);
            Paint();
            change_color = false;
        }
    }

public:
    Button(HDC hdc, RectF rf, WString text)
        : OptionButton(hdc, rf, text)
    {
        change_color = false;
        is_focused = true;
        //Paint();
    }
};

#endif
