
// device context provider. this file is use for interacting between virtual & real hdc

#ifndef DevconH
#define DevconH

#include "General.h"

#include "Draw.h"

extern Window* pwin;

class Devcon
{
private:

public:
    volatile HDC hdc;
    volatile HDC v_hdc;
    volatile bool is_update_hdc_possible;

    void AllowUpdate()
    {
        is_update_hdc_possible = true;
    }

    void PreventUpdate()
    {
        is_update_hdc_possible = false;
    }

    void UpdateHDC()
    {
        if (is_update_hdc_possible)
            CopyScreen(v_hdc, hdc);
    }

    Devcon(HDC hdc, HDC v_hdc) : hdc(hdc), v_hdc(v_hdc)
    {
        AllowUpdate();
    }
};


#endif
