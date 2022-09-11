// for drawing with GDI & GDI+

#ifndef DrawH
#define DrawH
#include "General.h"
#include "Window.h"

//extern Window* pwin;

extern volatile Unsigned lay;

inline HDC NewLayer(HDC source_hdc)// error handling to be added
{
    HDC ret = CreateCompatibleDC(source_hdc);
    HBITMAP bitmap = CreateCompatibleBitmap
    (
        source_hdc,
        GetDeviceCaps(source_hdc, HORZRES),
        GetDeviceCaps(source_hdc, VERTRES)
    );
    lay++;
    HGDIOBJ so = SelectObject(ret, bitmap);
    if (!so || !bitmap || !ret)
        print_error("NewLayer() failed!");
    if (bitmap)
    {
        DeleteObject(bitmap);
        lay--;
    }
    return ret;
}

inline void CopyScreen(HDC from, HDC to)
{
    int width1 = GetDeviceCaps(from, HORZRES);
    int height1 = GetDeviceCaps(from, VERTRES);

    int width2 = GetDeviceCaps(to, HORZRES);
    int height2 = GetDeviceCaps(to, VERTRES);

    int width = width1 < width2 ? width1 : width2;
    int height = height1 < height2 ? height1 : height2;

    BitBlt
    (
        to,
        0, 0, width, height,
        from,
        0, 0,
        SRCCOPY
    );
}

inline HDC newHDC(HDC hdc)
{
    HDC ret = NewLayer(hdc);
    CopyScreen(hdc, ret);
    return ret;
}

class Draw
{
    Color color;
    HDC copy_hdc;
    HDC pre_hdc;

    float copy_width;
    float copy_height;

    HDC hdc;
public:
    Graphics* graphics;

    Color GetPixel(float x, float y)
    {
        COLORREF rgb = ::GetPixel(hdc, (int)x, (int)y);
        Color ret = Color(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
        return ret;
    }

    void Line(PointF P1, PointF P2, float thick = 1)
    {
        Pen pen(color, thick);
        graphics->DrawLine(&pen, P1.X, P1.Y, P2.X, P2.Y);
    }

    void SetPixel(Color _color, float x, float y)
    {
        ::SetPixel(hdc, (int)x, (int)y, RGB(_color.GetR(), _color.GetG(), _color.GetB()));
        /*
        graphics -> DrawRectangle(&pen,RectF(x,y,0,0));
        Line(PointF(x,y),PointF(x+1,y));
        */
    }

    void SetPixel(float x, float y)
    {
        ::SetPixel(hdc, (int)x, (int)y, RGB(color.GetR(), color.GetG(), color.GetB()));
    }

    virtual void SetColor(Color _color)
    {
        this->color = _color;
    }

    void SetColor(Uchar transparency,
        Uchar red,
        Uchar green,
        Uchar blue)
    {
        SetColor(Color(transparency, red, green, blue));
    }

    Color GetColor()
    {
        return color;
    }

    void RectInterior(RectF rectf)
    {
        const Unsigned& n = 1;

        const float& small_width = rectf.Width / n;
        const float& small_height = rectf.Height / n;
        const float& left = rectf.GetLeft();
        const float& top = rectf.GetTop();


        SolidBrush brush(color);

        const Unsigned& step = 1;

        for (Unsigned x = 0; x < step; x++)
            for (Unsigned y = 0; y < step; y++)
                for (Unsigned i = x; i <= n; i += step)
                    for (Unsigned j = y; j <= n; j += step)
                        graphics->FillRectangle
                        (
                            &brush,
                            left + (i - 1) * small_width,
                            top + (j - 1) * small_height,
                            small_width,
                            small_height
                        );
    }

    void LimitDrawing(float left, float top, float width, float height)
    {
        graphics->SetClip(RectF(left, top, width, height));
    }

    void LimitDrawing(RectF rf)
    {
        graphics->SetClip(rf);
    }

    void RemoveDrawingLimits()
    {
        graphics->ResetClip();
    }

    //==============================================================================

    void Copy(float x, float y, float width, float height)
    {
        if (!copy_hdc)
            copy_hdc = NewLayer(hdc);

        copy_width = width;
        copy_height = height;

        BitBlt
        (
            copy_hdc,
            0, 0, (int)width, (int)height,
            hdc,
            (int)x, (int)y,
            SRCCOPY
        );
    }

    void Copy(RectF rect)
    {
        Copy(rect.X, rect.Y, rect.Width, rect.Height);
    }

    void Paste(float x, float y)
    {
        if (copy_hdc)
        {
            BitBlt
            (
                hdc,
                (int)x, (int)y, (int)copy_width, (int)copy_height,
                copy_hdc,
                0, 0,
                SRCCOPY
            );
            DeleteDC(copy_hdc);
            copy_hdc = 0;
        }
    }

    void NotPaste(float x, float y)
    {
        if (copy_hdc)
        {
            BitBlt
            (
                hdc,
                (int)x, (int)y, (int)copy_width, (int)copy_height,
                copy_hdc,
                0, 0,
                DSTINVERT
            );
            DeleteDC(copy_hdc);
            copy_hdc = 0;
        }
    }

    void SwitchOn() // do things on a virtual hdc to prevent flickering
    {
        if (!pre_hdc)
        {
            pre_hdc = hdc;
            SetHDC(NewLayer(hdc));
            CopyScreen(pre_hdc, hdc);
        }
    }

    void SwitchOff()
    {
        if (pre_hdc)
        {
            CopyScreen(hdc, pre_hdc);
            if (!DeleteDC(hdc))
                print_error(L"SwitchOff()");

            SetHDC(pre_hdc);
            pre_hdc = 0;
        }
    }

    void SwitchOff(RectF rectangle)
    {
        if (pre_hdc)
        {
            BitBlt
            (
                pre_hdc,
                (int)rectangle.GetLeft(), (int)rectangle.GetTop(),
                (int)rectangle.Width, (int)rectangle.Height,
                hdc,
                (int)rectangle.GetLeft(), (int)rectangle.GetTop(),
                SRCCOPY
            );
            DeleteDC(hdc);
            SetHDC(pre_hdc);
            pre_hdc = 0;
        }
    }

    void SetHDC(HDC new_hdc)
    {
        hdc = new_hdc;
        delete graphics;
        graphics = new Graphics(hdc);
    }
    //==============================================================================



    void NotLine(PointF P, PointF Q, int step = 1)
    {
        Region rgn;
        graphics->GetClip(&rgn);

        int i;
        int dx = (int)(Q.X - P.X);
        int dy = (int)(Q.Y - P.Y);
        int sx = 0 < dx ? step : -step;
        int sy = 0 < dy ? step : -step;
        int qx = 1, qy = 1;
        dx = abs(dx);
        dy = abs(dy);
        int dis = dx < dy ? dy : dx;

        for (i = 0; i <= dis; i += step)
        {
            if (!rgn.IsVisible(P, graphics))
                continue;
            Color _color = GetPixel(P.X, P.Y);
            _color = Color(255, _color.GetR(), 255 - _color.GetG(), 255 - _color.GetB());
            SetPixel(_color, P.X, P.Y);
            qx += dx;
            qy += dy;
            if (dis < qx)
            {
                qx -= dis;
                P.X += sx;
            }
            if (dis < qy)
            {
                qy -= dis;
                P.Y += sy;
            }
        }
    }

    void NotBox(RectF box)
    {

        for (float x = box.X; x < box.GetRight(); x++)
            for (float y = box.Y; y < box.GetBottom(); y++)
            {
                Color _color = GetPixel(x, y);
                _color = Color(255, 255 - _color.GetR(), 255 - _color.GetG(), 255 - _color.GetB());
                SetPixel(_color, x, y);
            }
    }


    //==============================================================================
    Draw(HDC& hdc) : hdc(hdc)
    {
        graphics = new Graphics(hdc);
        SetColor(255, 0, 0, 0);
        copy_hdc = 0;
        copy_width = 0;
        copy_height = 0;
        pre_hdc = 0;
    }

    ~Draw()
    {
        delete graphics;
    }
};


class AdDraw : public Draw
{
public:
    void LightRec(RectF rf, Color cl1 = Color(255, 210, 210, 210), Color cl2 = Color(255, 150, 150, 150))
    {
        float x = rf.X;
        float y = rf.Y;
        float width = rf.Width;
        float height = rf.Height;
        SetColor(cl1);
        Line(PointF(x, y), PointF(x + width, y));
        Line(PointF(x, y), PointF(x, y + height));
        SetColor(cl2);
        Line(PointF(x + width, y), PointF(x + width, y + height));
        Line(PointF(x, y + height), PointF(x + width, y + height));
    }

    void RoundLightRec(RectF rf, float thick = 1, Color cl1 = Color(210, 210, 210), Color cl2 = Color(150, 150, 150))
    {
        float x = rf.X;
        float y = rf.Y;
        float width = rf.Width;
        float height = rf.Height;
        SetColor(cl1);
        Line(PointF(x + 3, y), PointF(x + width - 3, y), thick);
        Line(PointF(x + 3, y), PointF(x + 2, y + 1 / 4), thick);
        Line(PointF(x + 2, y + 1 / 4), PointF(x + 1, y + 1), thick);
        Line(PointF(x + 1, y + 1), PointF(x + 1 / 4, y + 2), thick);
        Line(PointF(x, y + 3), PointF(x, y + height - 3), thick);
        Line(PointF(x + width - 3, y), PointF(x + width - 2, y + 1 / 4), thick);
        Line(PointF(x + width - 2, y + 1 / 4), PointF(x + width - 1, y + 1), thick);
        Line(PointF(x, y + height - 3), PointF(x + 1 / 4, y + height - 2), thick);
        Line(PointF(x + 1 / 4, y + height - 2), PointF(x + 1, y + height - 1), thick);
        SetColor(cl2);
        Line(PointF(x + width - 3, y + height), PointF(x + 3, y + height), thick);
        Line(PointF(x + width - 3, y + height), PointF(x + width - 2, y + height - 1 / 4), thick);
        Line(PointF(x + width - 2, y + height - 1 / 4), PointF(x + width - 1, y + height - 1), thick);
        Line(PointF(x + width - 1, y + height - 1), PointF(x + width - 1 / 4, y + height - 2), thick);
        Line(PointF(x + width, y + height - 3), PointF(x + width, y + 3), thick);
        Line(PointF(x + width - 1, y + 1), PointF(x + width + 1 / 4, y + 2), thick);
        Line(PointF(x + width + 1 / 4, y + 2), PointF(x + width, y + 3), thick);
        Line(PointF(x + 1, y + height - 1), PointF(x + 2, y + height - 1 / 4), thick);
        Line(PointF(x + 2, y + height - 1 / 4), PointF(x + 3, y + height), thick);
    }

public:
    AdDraw(HDC hdc) : Draw(hdc)
    {
    }
};

inline RectF Shrink(const RectF& box, float step = 1)
{
    return RectF(box.X + step, box.Y + step, box.Width - 2 * step, box.Height - 2 * step);
}




#endif

/*

 an important example:

 VOID Example_GetPixel(HDC hdc)

{

   Graphics graphics(hdc);

   // Create a Bitmap object from a JPEG file.
   Bitmap myBitmap(L"Climber.jpg");

   // Get the value of a pixel from myBitmap.
   Color pixelColor;
   myBitmap.GetPixel(25, 25, &pixelColor);

   // Fill a rectangle with the pixel color.
   SolidBrush brush(pixelColor);
   graphics.FillRectangle(&brush, Rect(0, 0, 100, 100));
}
*/