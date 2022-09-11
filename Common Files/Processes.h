// Processes.h version 1.50

// this file is used to prevent multiple running processes of the app



#ifndef ProcessesH
#define ProcessesH


#include "General.h"
#include "String.h" 

#include <vector>
#include <map>



#include <psapi.h>
#pragma comment(lib,"psapi.lib")


inline bool Priv(bool enable_privilege = true)
{
    const char* privilege_name = "SeDebugPrivilege";
    HANDLE current_process = GetCurrentProcess();
    HANDLE process_token;
    if (!OpenProcessToken(current_process,
        TOKEN_ADJUST_PRIVILEGES,
        &process_token))
    {
        return false;
    }
    TOKEN_PRIVILEGES tp;
    LUID luid;
    if
        (
            !LookupPrivilegeValueA
            (
                0,            // lookup privilege on local system
                privilege_name,
                &luid
            )
            )
    {
        CloseHandle(process_token);
        return false;
    }
    tp.PrivilegeCount = 1;
    tp.Privileges->Luid = luid;
    if (enable_privilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;
    if
        (
            !AdjustTokenPrivileges
            (
                process_token,
                0,
                &tp,
                sizeof(TOKEN_PRIVILEGES),
                0,
                0
            )
            )
    {
        CloseHandle(process_token);
        return false;
    }
    CloseHandle(process_token);
    return true;
}

inline void Suicide()
{
    HANDLE current_process = GetCurrentProcess();
    DWORD exit_code;
    if (!GetExitCodeProcess(current_process, &exit_code))
        exit_code = 0;
    ExitProcess(exit_code);
}

class Processes
{
    Unsigned number_of_processes;
    std::vector<String> names;
    std::vector<DWORD> ids;

    void Add(DWORD process_id)
    {
        char name[MAX_PATH] = "";
        void* han = OpenProcess
        (
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE,
            process_id
        );
        if (han == 0)
        {
            names.push_back("Access Denied");
            return;
        }
        HINSTANCE hmod;
        DWORD number_of_bytes_filled;
        int res = EnumProcessModules
        (
            han,
            &hmod,
            sizeof hmod,
            &number_of_bytes_filled
        );
        if (res == 0)
        {
            names.push_back("Name was not retrieved");
            return;
        }
        GetModuleBaseNameA(han, hmod, name, sizeof name);
        for (Unsigned i = 0; name[i]; i++)
            name[i] = (char)tolower(name[i]);
        names.push_back(name);
        CloseHandle(han);
    }

    void Fill()
    {
        DWORD process_id_array[1024];
        DWORD number_of_bytes_filled;
        int success = EnumProcesses
        (
            process_id_array,
            sizeof process_id_array,
            &number_of_bytes_filled
        );
        if (success == 0)
            return;
        number_of_processes =
            number_of_bytes_filled / sizeof(DWORD);

        for (Unsigned i = 0; i < number_of_processes; i++)
        {
            Add(process_id_array[i]);
            ids.push_back(process_id_array[i]);
        }
    }

public:
    void ArrangeById()
    {
        typedef std::multimap<DWORD, String> MAP;
        typedef MAP::iterator ITERATOR;
        typedef std::pair<DWORD, String> PAIR;
        MAP m;
        ITERATOR it;
        for (Unsigned i = 0; i < number_of_processes; i++)
            m.insert(PAIR(ids[i], names[i]));
        names.clear();
        ids.clear();
        for (it = m.begin(); it != m.end(); it++)
        {
            ids.push_back(it->first);
            names.push_back(it->second);
        }
    }

    void ArrangeByName()
    {
        typedef std::multimap<String, DWORD> MAP;
        typedef MAP::iterator ITERATOR;
        typedef std::pair<String, DWORD> PAIR;
        MAP m;
        ITERATOR it;
        for (Unsigned i = 0; i < number_of_processes; i++)
            m.insert(PAIR(names[i], ids[i]));
        names.clear();
        ids.clear();
        for (it = m.begin(); it != m.end(); it++)
        {
            names.push_back(it->first);
            ids.push_back(it->second);
        }
    }

    void Update()
    {
        names.clear();
        ids.clear();
        Fill();
    }

    Unsigned GetNumber()
    {
        return number_of_processes;
    }

    void KillProcess(Unsigned i) // kill process by number
    {
        DWORD id = (DWORD)this[0][i];
        void* han = OpenProcess
        (
            PROCESS_ALL_ACCESS,
            FALSE,
            id
        );
        if (han == 0)
            return;
        DWORD d;
        if (GetExitCodeProcess(han, &d) == 0)
            d = 0;
        if (TerminateProcess(han, d) == 0)
        {
            //print_error("void operator -=(Unsigned i)");
            return;
        }
        CloseHandle(han);
        Update();
    }

    void KillProcess(String name) // kill process by name
    {
        for (Unsigned i = 0; i < number_of_processes; i++)
            if (this[0](i) == name)
                KillProcess(i);
    }

    Unsigned operator ~() // number_of_processes
    {
        return number_of_processes;
    }

    void operator +() // updates the names & ids of processes
    {
        Update();
    }

    void operator -() // arrange by name
    {
        ArrangeByName();
    }

    void operator !() // arrange by id
    {
        ArrangeById();
    }

    void operator -=(Unsigned i) // kill process by number
    {
        KillProcess(i);
    }

    void operator -=(String name) // kill process by name
    {
        KillProcess(name);
    }

    String operator()(Unsigned i) // get name by number
    {
        if (i < GetNumber())
            return names[i];
        else
            return "";
    }

    Unsigned operator[](Unsigned i) // get id by number
    {
        if (i < GetNumber())
            return ids[i];
        else
            return MAXUnsigned;
    }

    Unsigned NumberOfProcsWithName(String name)
    {
        name.ToLower();
        Unsigned ret = 0;
        Unsigned size = names.size();
        for (Unsigned i = 0; i < size; i++)
        {
            if (names[i] == name)
                ret++;
        }
        return ret;
    }

    Processes() // constructor creates the list of processes
    {

        Priv();
        Fill();
    }
};

#endif
