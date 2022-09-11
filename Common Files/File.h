#ifndef FileH
#define FileH

#include "General.h"
#include "String.h"
#include "WString.h"

#ifndef __BYTE
#define __BYTE __BYTE
static const Unsigned KILOBYTE = 1024;
static const Unsigned MEGABYTE = 1024 * 1024;
static const Unsigned GIGABYTE = 1024 * 1024 * 1024;
#endif

static char LineEndA[3] = "\r\n";

/* static is necessary when th header is included
    in more than one units */

inline Unsigned SetFilePointer(
    HANDLE          han,
    const Unsigned& position,
    DWORD           from_where = FILE_BEGIN
)
{
    LARGE_INTEGER large_integer;
    large_integer.QuadPart = position;
    large_integer.LowPart = SetFilePointer(han, large_integer.LowPart,
        &large_integer.HighPart, from_where);
    if (large_integer.LowPart == INVALID_SET_FILE_POINTER)
        print_error(L"Sorry\n SetFilePointer() failed");
    return (Unsigned)large_integer.QuadPart;
}

inline bool CreatePathAnsi(WString path)
{
    WString s;
    bool folder_did_not_exist = false;
    while (path.Size() > 0)
    {
        s = path.CutOff("/\\") + "\\";
        wchar_t* c = s.GetCStyle();

        bool did_succeed = (CreateDirectoryW(c, 0) != 0);

        if (did_succeed == true)          //if a new folder is created
            folder_did_not_exist = true;// it was not before
        SetCurrentDirectoryW(c);
        delWStr(c);
    }
    return folder_did_not_exist;
}

inline HANDLE OpenFile(WString& file_address, bool create_new_file_on_failure)
{
    WString address = file_address;

    // suppose address == "c:/folder/ hello.txt"
    address.Reverse();//address == "txt.olleh/redlof/:c"
    WString filename = address.CutOff(L"/\\"); //filename == "txt.olleh"
    // address == "redlof/:c"

    address.Reverse(); // address == "c:/folder"
    filename.Reverse(); // filename == "hello.txt"

    CreatePathAnsi(address); // current directory is 'address'

    wchar_t* c = filename.GetCStyle();

    HANDLE han = CreateFileW(
        c,
        FILE_ALL_ACCESS,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        0,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    delWStr(c);

    if (han == INVALID_HANDLE_VALUE)
    {
        if (create_new_file_on_failure)
        {
            if (address.Size() >= MAX_PATH)
            {
                print_error("OpenFile()\nfile cannot be opened.");
                return 0;
            }
            file_address += WString(L"(next)");
            return OpenFile(file_address, create_new_file_on_failure);
        }
        else
            return 0;
    }
    return han;
}

inline void WriteToFile(HANDLE han, const String& data, Unsigned position = MAXUnsigned)
{
    if (position < MAXUnsigned)
        print_error(L"Sorry.\nbad position parameter. Write()");
    if (position != MAXUnsigned)
        SetFilePointer(han, position);

    char* c = data.GetCStyle();

    DWORD data_actually_wrote;
    Unsigned len = data.Size();

    bool successful = (
        WriteFile
        (
            han,
            c,
            (DWORD)(len * sizeof(char)),
            &data_actually_wrote,
            0
        )

        != 0
        );
    delWStr(c);

    if (data_actually_wrote != len * sizeof(char) || successful == false)
        print_error(L"Sorry.\nData was not wrote completely. Write()");
}


static bool reading_from_file_problem;

inline String ReadFromFile(
    HANDLE han,
    DWORD number_of_chars_to_read,
    Unsigned position = MAXUnsigned
)
{
    reading_from_file_problem = false;
    if (position != MAXUnsigned)
        SetFilePointer(han, position);
    DWORD number_of_bytes_read;
    char* c = new char[number_of_chars_to_read]; // no null terminated
    bool did_succeed = (
        ReadFile
        (
            han,
            c,
            number_of_chars_to_read * (DWORD)sizeof(char),
            &number_of_bytes_read,
            0
        )
        != 0
        );

    if (did_succeed == false) // protecting code
        reading_from_file_problem = true;
    else if (number_of_chars_to_read != number_of_bytes_read) // protecting code
        reading_from_file_problem = true;

    String ret(c, number_of_bytes_read / sizeof(char));
    delete[] c;
    return ret;
}

inline Unsigned GetFileSize(HANDLE han)
{
    LARGE_INTEGER large_integer;
    large_integer.LowPart = GetFileSize(han, (LPDWORD)& large_integer.HighPart);
    return (Unsigned)large_integer.QuadPart;
}

inline void SetFileSize(HANDLE han, Unsigned end_of_file, DWORD from_where = FILE_BEGIN)
{
    SetFilePointer(han, end_of_file, from_where);
    SetEndOfFile(han);
}


template<typename type>
String ToData(type x);// converts x to String

template<typename type>
type ToType(String data);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class SimpleFile
{
    friend struct SimpleFileChar;
    Unsigned save_pointer_pos;

    bool create_new_file_on_failure;
public:
    HANDLE han;
    bool is_open;
    WString address;

    //--------------------------------------------------------------------------

    void Close()
    {
        if (is_open == false)
            return;
        CloseHandle(han);
        is_open = false;
    }

    Unsigned GetPointerPosition() const
    {
        if (is_open == false)
            print_error(L"Sorry.GetPointerPosition()");
        return SetFilePointer(han, 0, FILE_CURRENT) / sizeof(char);
    }

    Unsigned GetFileLength() const
    {
        if (is_open == false)
            print_error(L"Sorry.GetFileLength()");
        return GetFileSize(han) / sizeof(char);
    }

    void GoToEndOfFile()
    {
        Unsigned end_of_file = GetFileLength();
        SetPointerPosition(end_of_file);
    }

    void GoForward(const Unsigned& steps)
    {
        SetPointerPosition(GetPointerPosition() + steps);
    }

    void GoBackward(const Unsigned& steps)
    {
        SetPointerPosition(GetPointerPosition() - steps);
    }

    void Open()
    {
        if (is_open == true)
            print_error(L"Sorry.Open(const String& address)");
        han = OpenFile(address, create_new_file_on_failure);
        if (han == 0)
            throw 0;
        is_open = true;
    }

    String Read(DWORD number_of_chars_to_read)
    {
        if (is_open == false)
            print_error(L"Sorry.Read(unsigned number_of_bytes_to_read)");
        String ret = ReadFromFile(han, number_of_chars_to_read);
        if (reading_from_file_problem)
            DeleteAndThrow();
        return ret;
    }

    void SetFileLength(const Unsigned& new_length)
    {
        if (is_open == false)
            print_error(L"Sorry.SetFileLength(const ULONGLONG& new_length)");
        SetFileSize(han, new_length * sizeof(char));
    }

    void SetPointerPosition(Unsigned new_position)
    {
        if (is_open == false)
            print_error(L"Sorry. SetPointerPosition");
        if (new_position > GetFileLength()) // protecting code
            new_position = GetFileLength();
        SetFilePointer(han, new_position * sizeof(char));
    }

    void Write(const String& data)
    {
        if (is_open == false)
            print_error(L"Sorry. Write");
        WriteToFile(han, data);
    }

    void DeleteAndThrow() // deletes current file
    {
        CloseHandle(han);
        DeleteFileW(WCS(address));
        throw WString("the file:\n") +
            address +
            WString("\nis corrupted and will be deleted.\nThe program will restart.");
    }

    void Start() // saves where pointer is;
    {
        save_pointer_pos = GetPointerPosition();
    }

    void End()
    {
        if (save_pointer_pos != MAXUnsigned)
        {
            SetPointerPosition(save_pointer_pos);
            save_pointer_pos = MAXUnsigned;
        }
    }

    String MultiCutOff(const String& ExSub);

    SimpleFile& operator << (const String& data)
    {
        Write(data);
        return *this;
    }

    SimpleFileChar operator [] (const Unsigned& i);

    SimpleFile(WString address, bool create_new_file_on_failure = true)
        : address(address), is_open(false), create_new_file_on_failure(create_new_file_on_failure)
    {
        save_pointer_pos = MAXUnsigned;
        Open();
    }

    SimpleFile(const SimpleFile&)
    {
        print_error(L"a SimpleFile or a Pack cannot be copied");
    }

    ~SimpleFile()
    {
        Close();
    }
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

struct SimpleFileChar
{
public:
    SimpleFile* This;

    Unsigned offset;

    operator char()
    {
        This->SetPointerPosition(offset);
        char ret_ch;
        DWORD number_of_bytes_read;
        bool did_succeed = (
            ReadFile
            (
                This->han,
                &ret_ch,
                sizeof(char),
                &number_of_bytes_read,
                0
            )
            != 0
            );

        if (did_succeed == false)
            print_error(L"Sorry,\nclass SimpleFileChar failed.");
        return ret_ch;
    }

    SimpleFileChar& operator = (const char& wch)
    {
        if (This->GetFileLength() < offset)
            This->SetFileLength(offset);
        This->SetPointerPosition(offset);
        DWORD data_actually_wrote;

        bool did_succeed = (
            WriteFile
            (
                This->han,
                &wch,
                sizeof(char),
                &data_actually_wrote,
                0
            )
            != 0
            );

        if (data_actually_wrote != sizeof(char) || did_succeed == false)
            print_error(L"Sorry.\nData was not wrote completely.SimpleFileChar");
        return *this;
    }

    SimpleFileChar& operator = (const String& Str)
    {
        This->SetPointerPosition(offset);
        This->Write(Str);
        return *this;
    }

    String operator [] (Unsigned i)
    {
        Unsigned length = This->GetFileLength();
        if (length == 0)
            return "";
        if (i >= length) // protecting code
            i = length - 1;

        This->SetPointerPosition(offset);
        if (offset <= i)
            return This->Read((DWORD)(i - offset + 1));
        return String();
    }

    SimpleFileChar(SimpleFile* fileParam, const Unsigned& offsetParam)
    {
        This = fileParam;
        offset = offsetParam;
    }
};

inline SimpleFileChar SimpleFile::operator [] (const Unsigned& i)
{
    if (is_open == false)
        print_error(L"Sorry. operator [] (const ULONGLONG& i)");
    return SimpleFileChar(this, i);
}

inline String SimpleFile::MultiCutOff(const String& ExSub)
{
    Unsigned start = GetPointerPosition();
    Unsigned i = start;
    Unsigned j = 0;

    Unsigned pos1 = MAXUnsigned;

    SimpleFile& file = *this;

    Unsigned len1 = GetFileLength();
    if (len1 == start)
        return L"";

    Unsigned len2 = ExSub.Size();

    bool is_consecutive = true;

    char* c2 = ExSub.GetCStyle(); // delete c2
    while (true)// to be breaked
    {
        if (len1 <= i)
            break;
        if (len2 <= j)
            break;

        if (c2[j] == (char)file[i]) // if c2[j] was found
        {
            if (j == 0)
            {
                is_consecutive = true;
                pos1 = i;
            }
            j++;
            i++;
        }
        else // if c2[j] was not found
        {
            if (pos1 != MAXUnsigned)
            {
                i = pos1;
                j = 0;
                pos1 = MAXUnsigned;
            }
            i++; // search next block to find it
            is_consecutive = false;
            // not consecutive (if j==len2, this line is not reached)
        }
    } // while(true)
    delete[] c2;

    if (is_consecutive == true)
        if (j == len2)   // if all were found
        {
            String ret = file[start][pos1 - 1];
            SetPointerPosition(i);

            return ret;
        }
    // if not found
    return file[start][len1 - 1];
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


typedef Unsigned uint;

// template<typename uint>
struct PackItem;

// template<typename uint = unsigned long long>
class Pack : public SimpleFile
{
    friend struct PackItem/*<uint>*/;

    
    uint const pack_unsigned_extreme_end = MAXUnsigned;
private:
    String file_info;

    void AddNewPack(const String& pack)
    {
        GoToEndOfFile();
        GoBackward(sizeof(uint) / sizeof(char));              // go to start of last uint
        uint new_position = GetPointerPosition(); // before last uint
        uint pre_position = ReadInteger<uint>(); // get position of last pack
        SetPointerPosition(pre_position + sizeof(uint) / sizeof(char)); //
        WriteInteger<uint>(new_position);
        SetPointerPosition(new_position);
        AddPack(pre_position, pack_unsigned_extreme_end, pack);
        WriteInteger<uint>(new_position); // the last uint points to new pack
        SetPointerPosition(new_position); // set pointer before new pack.
    }

    void AddPack(
        const uint& pre_pack_position,
        const uint& next_pack_position,
        const String& pack
    )
    {
        unsigned pack_len = (unsigned)(sizeof(char) * pack.Size());
        WriteInteger<uint>(pre_pack_position);
        WriteInteger<uint>(next_pack_position);
        WriteInteger<unsigned>(pack_len);
        Write(pack);
    }

    void DeletePack()
    {
        uint current_position = GetPointerPosition();
        uint pre_pack = ReadInteger<uint>();
        uint next_pack = ReadInteger<uint>();
        SetPointerPosition(current_position); // return to original place
        uint pre_position;
        if (pre_pack != pack_unsigned_extreme_end) // if pre pack exists
        {
            GoToPrePack();
            pre_position = GetPointerPosition();
            GoForward(sizeof(uint) / sizeof(char));
            WriteInteger<uint>(next_pack);
            SetPointerPosition(current_position); // return to original place
        }
        else
            print_error(L"DeletePack()\nthe first pack cannot be deleted");

        if (next_pack != pack_unsigned_extreme_end)
        {
            GoToNextPack();
            WriteInteger<uint>(pre_pack);
            GoBackward(sizeof(uint) / sizeof(char)); // go to start of next pack
        }
        else
        {
            GoToEndOfFile();
            GoBackward(sizeof(uint) / sizeof(char));
            WriteInteger<uint>(pre_position); // writing the last uint
            GoToLastPack();
        }
    }

    uint GetPackLength()
    {
        uint current_position = GetPointerPosition();
        GoForward(sizeof(uint) / sizeof(char) + sizeof(uint) / sizeof(char));
        unsigned pack_len = ReadInteger<unsigned>();
        SetPointerPosition(current_position);
        return pack_len;
    }

    String GetPack()
    {
        uint current_position = GetPointerPosition();
        GoForward(sizeof(uint) / sizeof(char) + sizeof(uint) / sizeof(char));
        String data = Read(ReadInteger<unsigned>() / sizeof(char));
        SetPointerPosition(current_position);
        GoToNextPack();
        return data;
    }

    void GoTo(const uint& n)
    {
        SetPointerPosition(0);
        for (uint i = 0; i < n; i++)
            if (GoToNextPack() == false)
                print_error(L"GoTo()\nInvalid Parameter.");
    }

    void GoToLastPack()
    {
        GoToEndOfFile();
        GoBackward(sizeof(uint) / sizeof(char));
        uint last_pack = ReadInteger<uint>();
        SetPointerPosition(last_pack);
    }

    bool GoToNextPack()
    {
        uint current_position = GetPointerPosition();
        GoForward(sizeof(uint) / sizeof(char));
        uint next_pack_position = ReadInteger<uint>();
        if (next_pack_position == pack_unsigned_extreme_end)
        {
            SetPointerPosition(current_position);// don't change position if false
            return false;                   // cannot go ahead
        }

        if (next_pack_position >= GetFileLength()) // protecting code
            DeleteAndThrow();

        SetPointerPosition(next_pack_position);
        return true; // did successfully
    }

    bool GoToPrePack()
    {
        uint current_position = GetPointerPosition();
        uint pre_pack_position = ReadInteger<uint>();
        if (pre_pack_position == pack_unsigned_extreme_end)
        {
            SetPointerPosition(current_position);
            return false; // cannot go back
        }

        if (pre_pack_position >= GetFileLength()) // protecting code
            DeleteAndThrow();

        SetPointerPosition(pre_pack_position);
        return true; // did successfully
    }

    void Insert(const String& new_pack)
    {
        uint current_position = GetPointerPosition();
        uint pre_position = ReadInteger<uint>();

        GoToEndOfFile();
        GoBackward(sizeof(uint) / sizeof(char));// before the last uint
        uint last_uint = ReadInteger<uint>(); // read the last uint
        GoBackward(sizeof(uint) / sizeof(char));// before the last uint

        uint new_position = GetPointerPosition(); // place of new pack

        AddPack(pre_position, current_position, new_pack); // adding new pack

        WriteInteger<uint>(last_uint);    // the last uint

        SetPointerPosition(pre_position + sizeof(uint) / sizeof(char));
        WriteInteger<uint>(new_position);
        SetPointerPosition(current_position);
        WriteInteger<uint>(new_position);
        SetPointerPosition(new_position);
    }

    template<typename type>
    type ReadInteger()
    {
#ifdef __BORLANDC__
#pragma option -w-ccc
#pragma option -w-rch
#endif
        if constexpr(sizeof(type) % sizeof(char) != 0)
            print_error(L"bad type, ReadInteger 268");
        union
        {
            type ret;
            char a[sizeof(type) / sizeof(char)];
        };
        String Str = Read(sizeof(type) / sizeof(char));
        //int size = Str.Size();
        for (Uchar i = 0; i < sizeof(type) / sizeof(char); i++)
            a[i] = Str[i];
        return ret;
    }


    void Swap(uint m, uint n)
    {
        uint current_position = GetPointerPosition();
        if (m == n)
            return;
        if (m > n)    // m must be smaller
        {
            uint temp = m;
            m = n;
            n = temp;
        }

        if (m == 0)
            print_error(L"Swap()\nyou cannot change the first pack");

        SetPointerPosition(0);
        uint i;
        for (i = 0; i < m; i++)
            if (GoToNextPack() == false)
                print_error(L"Swap()\nBad parameters");

        uint m_pos = GetPointerPosition();
        uint m_pre_pos = ReadInteger<uint>();
        uint m_next_pos = ReadInteger<uint>();
        SetPointerPosition(m_pos);
        for (i = m; i < n; i++)
            if (GoToNextPack() == false)
                print_error(L"Swap()\nBad parameters");

        uint n_pos = GetPointerPosition();
        uint n_pre_pos = ReadInteger<uint>();
        uint n_next_pos = ReadInteger<uint>();

        if (m != n - 1)
        {
            SetPointerPosition(n_pos);
            WriteInteger<uint>(m_pre_pos);
            WriteInteger<uint>(m_next_pos);

            SetPointerPosition(m_pos);
            WriteInteger<uint>(n_pre_pos);
            WriteInteger<uint>(n_next_pos);

            if (n_pre_pos != pack_unsigned_extreme_end)
            {
                SetPointerPosition(n_pre_pos);
                GoForward(sizeof(uint) / sizeof(char));
                WriteInteger<uint>(m_pos);
            }

            if (m_next_pos != pack_unsigned_extreme_end)
            {
                SetPointerPosition(m_next_pos);
                WriteInteger<uint>(n_pos);
            }
        }
        else
        {
            SetPointerPosition(n_pos);
            WriteInteger<uint>(m_pre_pos);
            WriteInteger<uint>(m_pos);

            SetPointerPosition(m_pos);
            WriteInteger<uint>(n_pos);
            WriteInteger<uint>(n_next_pos);
        }

        if (n_next_pos != pack_unsigned_extreme_end)
        {
            SetPointerPosition(n_next_pos);
            WriteInteger<uint>(m_pos);
        }

        if (m_pre_pos != pack_unsigned_extreme_end)
        {
            SetPointerPosition(m_pre_pos);
            GoForward(sizeof(uint) / sizeof(char));
            WriteInteger(n_pos);
        }

        SetPointerPosition(current_position);
    }

    template<typename type>
    void WriteInteger(const type& ll)
    {
#ifdef __BORLANDC__
#pragma option -w-ccc
#pragma option -w-rch
#endif
        if constexpr(sizeof(ll) % sizeof(char) != 0)
            print_error(L"bad type WriteInteger 288");
        Write(String((char*)& ll, sizeof(ll) / sizeof(char)));
    }

    //==========================================================================

public:
    void Empty();

    uint GetFileLength()
    {
        return SimpleFile::GetFileLength();
    }

    uint GetNumberOfPacks()
    {
        uint current_position = GetPointerPosition();
        uint ret = 1;
        SetPointerPosition(0);
        while (GoToNextPack() == true)
        {
            ret++;
            if (ret > GetFileLength()) // protecting code
                DeleteAndThrow();
        }
        SetPointerPosition(current_position);
        return ret;
    }

    uint Size()
    {
        return GetNumberOfPacks();
    }

    void SetNumberOfPacks(uint n);

    Pack& operator<< (const String& new_pack)
    {
        AddNewPack(new_pack);
        return *this;
    }

    void operator = (Pack& p);

    PackItem operator[](const uint& i); // [0] = info pack

    void Constructor()
    {
        if (GetFileLength() == 0)
        {
            AddPack(pack_unsigned_extreme_end, pack_unsigned_extreme_end, file_info);
            WriteInteger<uint>((uint)0); // the last uint
        }
        GetNumberOfPacks(); // checks if the file is valid
    }

    Pack(WString address, String file_info = "no_file_info")
        : SimpleFile(address), file_info(file_info)
    {
        Constructor();
    }
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


struct PackItem
{
    friend class Pack;

private:

    Pack* This;

    uint offset;

    PackItem(Pack* packfile, const uint& offset)
    {
        this->This = packfile;
        if (This->GetNumberOfPacks() < offset)
            print_error(L"PackItem Pack::operator[]\n Bad Offset");
        this->offset = offset;
    }

    //==========================================================================

public:
    operator String() const
    {
        uint current_position = This->GetPointerPosition();
        This->SetPointerPosition(0);
        for (uint i = 0; i < offset; i++)
            This->GoToNextPack();
        This->GoForward(sizeof(uint) / sizeof(char) + sizeof(uint) / sizeof(char));
        String ret = This->Read(This->ReadInteger<unsigned>() / sizeof(char));
        This->SetPointerPosition(current_position);
        return ret;
    }

    PackItem& operator = (const String& new_pack)
    {
        if (offset == 0)
            print_error(L"PackItem::=\n"
                L"you cannot change the first pack.");

        This->SetPointerPosition(0);

        uint i;
        for (i = 0; i < offset; i++)
            if (This->GoToNextPack() == false)
            {
                This->AddNewPack(new_pack);
                return *this;
            }

        unsigned new_pack_len = (unsigned)(sizeof(char) * new_pack.Size());
        if (new_pack_len <= This->GetPackLength())
        {
            This->GoForward(sizeof(uint) / sizeof(char) + sizeof(uint) / sizeof(char));
            This->WriteInteger<unsigned>(new_pack_len);
            This->Write(new_pack);
        }
        else
        {
            This->Insert(new_pack);
            This->GoToNextPack();
            This->DeletePack();
        }
        return *this;
    }

    PackItem& operator = (const int& x)
    {
        if (offset == 0)
            print_error(L"PackItem::=\n"
                L"you cannot change the first pack.");
        if (x != 0)
            print_error(L"PackItem::=\nbad use of operator =.");
        This->GoTo(offset);
        This->DeletePack();
        return *this;
    }

    PackItem& operator = (const PackItem& item)
    {
        *this = String(item);
        return *this;
    }

    PackItem& operator += (const String& new_pack)
    {
        This->GoTo(offset);
        This->Insert(new_pack);
        This[0][offset] ^ This[0][offset + 1];
        return *this;
    }// insert

    bool operator == (const PackItem& pack) const
    {
        return String(*this) == String(pack);
    }

    bool operator != (const PackItem& pack) const
    {
        return String(*this) != String(pack);
    }

    bool operator < (const PackItem& pack) const
    {
        return (String)(*this) < (String)(pack);
    }

    bool operator >  (const PackItem& pack) const
    {
        return String(*this) > String(pack);
    }

    bool operator <= (const PackItem& pack) const
    {
        return String(*this) <= String(pack);
    }

    bool operator >= (const PackItem& pack) const
    {
        return String(*this) >= String(pack);
    }

    void operator ^ (const PackItem& pack)
    {
        if (This != pack.This)
            print_error(L"operator ^\n"
                L"both items must be in the same Pack");
        This->Swap(offset, pack.offset);
    }

    SimpleFileChar operator [](const Unsigned& i)
    {
        //eturn PackItem(this,i);
        This->GoTo(offset);

        if (This->GetPackLength() <= i)
            print_error(L"bad second offset");

        uint current_position = This->GetPointerPosition();
        SimpleFile* simple = This;
        return (*simple)[sizeof(uint) + sizeof(uint) +
            sizeof(unsigned) + current_position + i];
    }
};


inline void Pack::SetNumberOfPacks(uint n)
{
    while (GetNumberOfPacks() < n)
        this[0] << "";
    while (n < GetNumberOfPacks())
        (this[0])[GetNumberOfPacks() - 1] = 0;
}

inline void Pack::Empty()
{
    String _file_info = (*this)[0];
    SetFileLength(0);
    AddPack(pack_unsigned_extreme_end, pack_unsigned_extreme_end, _file_info);
    WriteInteger<uint>((uint)0); // the last uint
}

inline void Pack::operator = (Pack& p)
{
    Empty();
    uint size = p.Size();
    for (uint i = 1; i < size; i++)
        this[0][i] = String(p[i]);
}

inline PackItem Pack::operator[](const uint& i)
{
    return PackItem(this, i);
}

//--------------------------------------



#endif
