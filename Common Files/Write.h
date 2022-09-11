#ifndef WriteH
#define WriteH

#include "General.h"

#include "Draw.h"
#include "WString.h"
#include "WFile.h"
#include <vector>

// this file is used for writing GDI+ text on the window

class Write : public Draw
{
protected:
    SolidBrush* pbrush;
    FontFamily* fontFamily;

public:
    Font* font;
    PointF position;
    WString font_name;
    float font_size;
    int font_style;// italic or bold or regular

    virtual void SetColor(Color _color)
    {
        Draw::SetColor(_color);
        pbrush->SetColor(_color);
    }

    void SetColor(Uchar transparency,
        Uchar red,
        Uchar green,
        Uchar blue)
    {
        Draw::SetColor(transparency, red, green, blue);
    }

    void Gotoxy(PointF p)
    {
        position = p;
    }

    void Gotoxy(float x, float y)
    {
        position = PointF(x, y);
    }

    virtual void WriteString(const WString& wstr)
    {
        graphics->DrawString(WCS(wstr), -1, font, position, pbrush);
        StringWidth(wstr);
        Gotoxy(position.X, position.Y);
    }

    float StringWidth(const WString& wstr)
    {
        RectF bound;
        graphics->MeasureString(WCS(wstr), -1, font, position, &bound);
        float X = bound.GetRight() - 8.5f * (font_size / 24);
        return X - position.X;
    }

    float StringHeight(const WString& wstr)
    {
        RectF bound;
        graphics->MeasureString(WCS(wstr), -1, font, position, &bound);
        float Y = bound.GetBottom() - 13 * (font_size / 24);
        return Y - position.Y;
    }

    RectF TextRect(const WString& wstr)
    {
        RectF bound;
        graphics->MeasureString(WCS(wstr), -1, font, position, &bound);
        return bound;
    }

    Write& operator << (const WString& rhs)
    {
        WriteString(rhs);
        return *this;
    }

    void SetFont(WString name)
    {
        delete font;
        delete fontFamily;
        font_name = name;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    void SetSize(float size)
    {
        delete font;
        delete fontFamily;
        font_size = size;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    void Italic()
    {
        delete font;
        delete fontFamily;
        font_style = FontStyleItalic;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    void Bold()
    {
        delete font;
        delete fontFamily;
        font_style = FontStyleBold;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    void BoldItalic()
    {
        delete font;
        delete fontFamily;
        font_style = FontStyleBoldItalic;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    void Regular()
    {
        delete font;
        delete fontFamily;
        font_style = FontStyleRegular;
        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
    }

    Write(HDC& hdc, float x = 0, float y = 0)
        : Draw(hdc),
        position(x, y)
    {
        pbrush = new SolidBrush(GetColor());
        font_name = L"Courier New";
        font_size = 13;
        font_style = FontStyleBold;

        fontFamily = new FontFamily(WCS(font_name));
        font = new Font(fontFamily, font_size, font_style, UnitPixel);
        graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
    }

    ~Write()
    {
        delete font;
        delete fontFamily;
        delete pbrush;
    }
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#endif
