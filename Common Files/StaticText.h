#ifndef StaticTextH
#define StaticTextH
#include "WString.h"
#include "Write.h"

class StaticText : public Write
{
public:
    WString text;
    float x;
    float y;

    void Hide(Color _color = back_color)
    {
        SetColor(_color);
        RectInterior(TextRect(text));
    }

    void Show()
    {
        Paint();
    }

    void Paint()
    {
        Hide();
        Gotoxy(x, y);
        SetColor(255, 0, 0, 0);
        WriteString(text);
    }


    bool inBox(PointF P)
    {
        return TextRect(text).Contains(P);
    }
    //=========================================================================
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

    void wmLbuttondown(WPARAM , LPARAM lParam)
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

    void wmLbuttonup(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmMousemove(WPARAM , LPARAM )
    {
        if (!is_focused)
            return;
    }

    void wmMouseleave(WPARAM , LPARAM )
    {
    }

public:
    StaticText(HDC v_hdc, const WString& text, float x, float y)
        : Write(v_hdc, x, y),
        text(text), x(x), y(y)
    {
    }
};


#endif
