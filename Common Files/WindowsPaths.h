#ifndef WindowsPathsH
#define WindowsPathsH
#include "String.h"
#include "WString.h"

#include <shlobj.h>

extern Unsigned this_instance;

/*
inline String GetAppDataPathA()
{
    char c[MAX_PATH + 1];
    SHGetSpecialFolderPathA(0,c,CSIDL_APPDATA,1);
    return String(c);
}	  */

inline WString GetAppDataPath()
{
    wchar_t c[MAX_PATH + 1];
    SHGetSpecialFolderPathW(0, c, CSIDL_APPDATA, 1);
    return WString(c) + WString(L"\\CppBuilder2006\\") + WString(app_name) + WString("\\instance") + WString(int(this_instance)) + WString("\\");
}

inline WString GetDesktop()
{
    wchar_t c[MAX_PATH + 1];
    SHGetSpecialFolderPathW(0, c, CSIDL_COMMON_DESKTOPDIRECTORY, 1);
    return WString(c);
}


#endif
