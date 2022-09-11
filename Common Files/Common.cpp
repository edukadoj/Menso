//#pragma hdrstop
#include "Common.h"

Unsigned this_instance = 0;

bool Common::allow_winproc = false;
Unsigned Common::check_period1 = 900;
Unsigned Common::check_period2 = 0;
Unsigned Common::tell_page_num = MAXUnsigned;
volatile bool Common::paint_aternate = false;
EditBox* error_edit = 0;

Window* pwin = 0;
Devcon* pdevcon = 0;

Page* ppage = 0;