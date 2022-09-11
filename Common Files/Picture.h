#ifndef PictureH
#define PictureH
#include <vector>

#include "General.h"
#include "..\\Common Files\\WString.h"
#include "..\\Common Files\\Draw.h"

extern Window* pwin;

class PrimaryPicture : public AdDraw
{
protected:
    std::vector<WString> addresses;
    std::vector<long long> resources;
    Unsigned current;
    RectF box;
    bool resource;
    volatile bool picture_animating;
    Unsigned ani_interval;

    void AdjustBox(RectF* prect = 0)
    {
        if (current != MAXUnsigned)
        {
            if (prect == 0)
                prect = &box;
            if (!resource)
            {
                Image image((wchar_t*)WCS(addresses[current]));
                //Unit unit;
                prect->Width = (REAL)image.GetWidth();
                prect->Height = (REAL)image.GetHeight();
            }
            else
            {
                HINSTANCE hInstance = GetModuleHandle(0);
                Bitmap image(hInstance, (WCHAR*)(long long)resources[current]);
                prect->Width = (REAL)image.GetWidth();
                prect->Height = (REAL)image.GetHeight();
            }
        }
    }

public:
    ACTION onClick;
    ACTION onBackspace;
    WString param;
    bool is_shown;

    void UseResource()
    {
        resource = true;
        if (current >= resources.size())
        {
            if (resources.empty())
                current = MAXUnsigned;
            else
                current = 0;
        }

    }

    void UseAddress()
    {
        resource = false;
        if (current >= addresses.size())
        {
            if (addresses.empty())
                current = MAXUnsigned;
            else
                current = 0;
        }
    }

    void Show()
    {
        if (current != MAXUnsigned)
        {
            if (!resource)
            {
                Image image((wchar_t*)WCS(addresses[current]));
                graphics->DrawImage(&image, box.X, box.Y, (REAL)image.GetWidth(), (REAL)image.GetHeight());
            }
            else
            {
                HINSTANCE hInstance = GetModuleHandle(0);
                Bitmap image(hInstance, (WCHAR*)(long long)resources[current]); // long long to remove a warning
                graphics->DrawImage(&image, box.X, box.Y, (REAL)image.GetWidth(), (REAL)image.GetHeight());
            }
        }
        is_shown = true;
    }

    void Hide(Color _color = back_color)
    {
        SetColor(_color);
        RectInterior(RectF(box.X, box.Y, box.Width, box.Height));
        is_shown = false;
    }

    bool inBox(PointF P)
    {
        return box.Contains(P);
    }

    void Move(float dx, float dy)
    {
        bool b = is_shown;
        Hide();
        is_shown = b;
        box.X += dx;
        box.Y += dy;
        Paint();
    }

    void AddAddress(WString new_address)
    {
        addresses.push_back(new_address);
        if (current == MAXUnsigned)
        {
            current = 0;
            AdjustBox();
            Paint();
        }
    }

    void AddResource(int id)
    {
        resources.push_back(id);
        if (current == MAXUnsigned)
        {
            UseResource();
            current = 0;
            AdjustBox();
            Paint();
        }
    }

    void Next()
    {
        if (current != MAXUnsigned)
        {
            if (!resource)
            {
                if (current < addresses.size() - 1)
                    current++;
                else
                    current = 0;
            }
            else
            {
                if (current < resources.size() - 1)
                    current++;
                else
                    current = 0;
            }
            RectF rect = box;
            AdjustBox(&rect);
            if (rect.Width <= box.Width || rect.Height <= box.Height)
                if (is_shown)
                {
                    bool b = is_shown;
                    Hide();
                    is_shown = b;
                }
            box = rect;
            Paint();
        }
    }

    void Activate(Unsigned i)
    {
        if (!resource)
        {
            if (addresses.size() <= i) // volatile protecting code
                i = addresses.size() - 1;
        }
        else
        {
            if (resources.size() <= i) // volatile protecting code
                i = resources.size() - 1;
        }
        current = i;
    }

    Unsigned GetCurrent()
    {
        return current;
    }

    void Paint()
    {
        if (current != MAXUnsigned)
        {
            RectF rect = box;
            AdjustBox(&rect);
            if (rect.Width <= box.Width || rect.Height < box.Height)
                if (is_shown)
                {
                    bool b = is_shown;
                    Hide();
                    is_shown = b;
                }
            box = rect;
            if (is_shown)
                Show();
        }
    }

    static DWORD WINAPI Animator(LPVOID lpParam)
    {
        PrimaryPicture* This = (PrimaryPicture*)lpParam;
        while (true)
        {
            if (This->picture_animating == false)
            {
                This->Activate(0);
                SendMessage(pwin->hwnd, WM_PAINT, 0, 0);
                return 0;
            }
            SendMessage(pwin->hwnd, WM_CPPBUILDER_PICTURE, 0, (LPARAM)This);
            Sleep((DWORD)This->ani_interval);
        }
    }

    void StartAnimating(Unsigned _ani_interval)
    {
        this->ani_interval = _ani_interval;
        if (picture_animating == true)
            return;
        picture_animating = true;
        DWORD thread_id;
        CreateThread
        (
            0,           // default security attributes
            0,           // use default stack size
            Animator,  // thread function
            this, // argument to thread function
            0,           // use default creation flags
            &thread_id
        );
    }

    void EndAnimating()
    {
        picture_animating = false;
    }

    void Animate()
    {

    }


public:
    PrimaryPicture(HDC& hdc, float x, float y)
        : AdDraw(hdc), box(0, 0, 0, 0)
    {
        onClick = ZERO;
        onBackspace = ZERO;
        is_shown = false;
        box.X = x;
        box.Y = y;
        current = MAXUnsigned;
        picture_animating = false;
    }
};

class Picture : public PrimaryPicture
{

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

    void wmChar(WPARAM wParam, LPARAM )
    {
        if (!is_focused)
            return;
        if (wParam == 8) // backspace
            onBackspace(param);
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
            onClick(param);
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
    Picture(HDC hdc, float x, float y)
        : PrimaryPicture(hdc, x, y)
    {
        param = ToData<Picture*>(this);
        resource = false;
    }
};


#endif
