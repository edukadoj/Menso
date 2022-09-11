#ifndef MainH
#define MainH

#include "Common Files\\General.h"
#include "Common Files\\Window.h"
#include "Common Files\\Draw.h"
#include "Common Files\\EditBox.h"
#include "Common Files\\WString.h"
#include "Common Files\\WFile.h"
#include "Common Files\\Clipboard.h"
#include "Common Files\\Write.h"
#include "Common Files\\Button.h"
#include "Common Files\\Processes.h"
#include "Common Files\\StaticText.h"
#include "Common Files\\Page.h"
#include "Common Files\\Picture.h"
#include "Common Files\\Common.h"


class Main : public Common
{
public:
    virtual void Definitions();

    void AtStart()
    {
    }

    //))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))


public:
    Main(int width, int height, int nCmdShow) : Common(width, height, nCmdShow)
    {
        tell_page_num = 1;
        instance_number = 10;
        extern Main* pmain;
        pmain = this;
        MainAction();
    }
};

extern Main* pmain;

#endif