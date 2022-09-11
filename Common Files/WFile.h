#ifndef WFileH
#define WFileH

#include "General.h"
#include "WString.h"
#include "String.h"


#ifndef __BYTE
#define __BYTE __BYTE
static const Unsigned KILOBYTE = 1024;
static const Unsigned MEGABYTE = 1024 * 1024;
static const Unsigned GIGABYTE = 1024 * 1024 * 1024;
#endif

static wchar_t LineEndW[3] = L"\r\n";



/* static is necessary when th header is included
    in more than one units */

inline Unsigned SetWFilePointer(
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
        print_error(L"Sorry\n SetWFilePointer() failed");
    return large_integer.QuadPart;
}

inline bool CreatePath(WString path)
{
    WString s;
    bool folder_did_not_exist = false;
    while (path.Size() > 0)
    {
        s = path.CutOff("/\\") + "\\";
        wchar_t* c = s.GetCStyle();

        bool did_succeed = (CreateDirectory(c, 0) != 0);

        if (did_succeed == true)          //if a new folder is created
            folder_did_not_exist = true;// it was not before
        SetCurrentDirectory(c);
        delWStr(c);
    }
    return folder_did_not_exist;
}

inline HANDLE OpenWFile(WString& file_address, bool create_new_file_on_failure) // address may be changed for opening
{
    WString address = file_address;
    // suppose address == "c:/folder/ hello.txt"

    address.Reverse();//address == "txt.olleh/redlof/:c"
    WString filename = address.CutOff("/\\"); //filename == "txt.olleh"
    // address == "redlof/:c"

    address.Reverse(); // address == "c:/folder"
    filename.Reverse(); // filename == "hello.txt"

    CreatePath(address); // current directory is 'address'

    wchar_t* c = filename.GetCStyle();

    HANDLE han = CreateFile(
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
                print_error(L"OpenWFile()\nfile cannot be opened.");
                return 0;
            }
            file_address += WString(L"(next)");
            return OpenWFile(file_address, create_new_file_on_failure);
        }
        else
            return 0;
    }
    return han;
}

inline void WriteToWFile(HANDLE han, const WString& data, Unsigned position = MAXUnsigned)
{
    /*if (position < -1)
        print_error(L"Sorry.\nbad position parameter. Write()");*/
    if (position != MAXUnsigned)
        SetWFilePointer(han, position);

    wchar_t* c = data.GetCStyle();

    DWORD data_actually_wrote;
    const Unsigned& len = data.Size();

    bool did_succeed = (
        WriteFile
        (
            han,
            c,
            (DWORD)(len * sizeof(wchar_t)),
            &data_actually_wrote,
            0
        )

        != 0
        );
    delWStr(c);

    if (data_actually_wrote != len * sizeof(wchar_t) || did_succeed == false)
        print_error(L"Sorry.\nData was not wrote completely. Write()");
}


static bool reading_from_wfile_problem;

inline WString ReadFromWFile(
    HANDLE han,
    DWORD number_of_wchar_ts_to_read,
    Unsigned position = MAXUnsigned
)
{
    reading_from_wfile_problem = false;

    if (position != MAXUnsigned)
        SetWFilePointer(han, position);
    DWORD number_of_bytes_read;
    wchar_t* c = new wchar_t[number_of_wchar_ts_to_read]; // no null terminated

    DWORD number_of_bytes_to_read = number_of_wchar_ts_to_read * sizeof(wchar_t);

    bool did_succeed = (
        ReadFile
        (
            han,
            c,
            number_of_bytes_to_read,
            &number_of_bytes_read,
            0
        )
        != 0
        );

    if (did_succeed == false) // protecting code
        reading_from_wfile_problem = true;
    else if (number_of_bytes_to_read != number_of_bytes_read) // protecting code
        reading_from_wfile_problem = true;
    WString ret(c, number_of_bytes_read / sizeof(wchar_t));
    delete[] c;
    return ret;
}

inline Unsigned GetWFileSize(HANDLE han)
{
    LARGE_INTEGER large_integer;
    large_integer.LowPart = GetFileSize(han, (LPDWORD)& large_integer.HighPart);
    return (Unsigned)large_integer.QuadPart;
}

inline void SetWFileSize(HANDLE han, Unsigned end_of_file,
    DWORD from_where = FILE_BEGIN)
{
    SetWFilePointer(han, end_of_file, from_where);
    SetEndOfFile(han);
}

/*
template<typename type>
WString ToWData(type x);// converts x to WString

template<typename type>
type ToType(WString data);	*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class SimpleWFile
{
    friend struct SimpleWFileChar;

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
            print_error(L"Sorry\nSimpleWFile is already closed\nClose()");
        CloseHandle(han);
        is_open = false;
    }

    Unsigned GetPointerPosition() const
    {
        if (is_open == false)
            print_error(L"Sorry.GetPointerPosition()");
        return SetWFilePointer(han, 0, FILE_CURRENT) / sizeof(wchar_t);
    }

    Unsigned GetWFileLength() const
    {
        if (is_open == false)
            print_error(L"Sorry.GetWFileLength()");
        return GetWFileSize(han) / sizeof(wchar_t);
    }

    void GoToEndOfWFile()
    {
        Unsigned end_of_file = GetWFileLength();
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
            print_error(L"Sorry.Open(const WString& address)");
        han = OpenWFile(address, create_new_file_on_failure);
        is_open = true;
    }

    WString Read(DWORD number_of_chars_to_read)
    {
        if (is_open == false)
            print_error(L"Sorry.Read(unsigned number_of_bytes_to_read)");
        WString ret = ReadFromWFile(han, number_of_chars_to_read);
        if (reading_from_wfile_problem)
            this->DeleteAndThrow();
        return ret;
    }

    void SetWFileLength(const Unsigned& new_length)
    {
        if (is_open == false)
            print_error(L"Sorry.SetWFileLength(const ULONGLONG& new_length)");
        SetWFileSize(han, new_length * sizeof(wchar_t));
    }

    void SetPointerPosition(Unsigned new_position)
    {
        if (is_open == false)
            print_error(L"Sorry. SetPointerPosition");
        if (new_position > GetWFileLength()) // protecting code
            new_position = GetWFileLength();
        SetWFilePointer(han, new_position * sizeof(wchar_t));
    }

    void Write(const WString& data)
    {
        if (is_open == false)
            print_error(L"Sorry. Write");
        WriteToWFile(han, data);
    }

    void DeleteAndThrow()
    {
        CloseHandle(han);
        DeleteFile(WCS(address));
        throw WString("The file:\n") +
            WString(address.GetCStyle()) +
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

    WString MultiCutOff(const WString& ExSub);

    SimpleWFile& operator << (const WString& data)
    {
        Write(data);
        return *this;
    }

    SimpleWFileChar operator [] (const Unsigned& i);


    SimpleWFile(WString address, bool create_new_file_on_failure = true)
        : address(address), is_open(false), create_new_file_on_failure(create_new_file_on_failure)
    {
        Open();
    }

    SimpleWFile(const SimpleWFile&)
    {
        print_error(L"a SimpleWFile or a WPack cannot be copied");
    }

    ~SimpleWFile()
    {
        Close();
    }
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

struct SimpleWFileChar
{
public:

    SimpleWFile* This;

    Unsigned offset;

    operator wchar_t()
    {
        This->SetPointerPosition(offset);
        wchar_t ret_ch;
        DWORD number_of_bytes_read;
        bool did_succeed = (
            ReadFile
            (
                This->han,
                &ret_ch,
                sizeof(wchar_t),
                &number_of_bytes_read,
                0
            )
            != 0
            );

        if (did_succeed == false)
            print_error(L"Sorry,\nclass SimpleWFileChar failed.");
        return ret_ch;
    }

    SimpleWFileChar& operator = (const wchar_t& wch)
    {
        if (This->GetWFileLength() < offset)
            This->SetWFileLength(offset);
        This->SetPointerPosition(offset);
        DWORD data_actually_wrote;

        bool did_succeed = (
            WriteFile
            (
                This->han,
                &wch,
                sizeof(wchar_t),
                &data_actually_wrote,
                0
            )
            != 0
            );

        if (data_actually_wrote != sizeof(wchar_t) || did_succeed == false)
            print_error(L"Sorry.\nData was not wrote completely.SimpleWFileChar");
        return *this;
    }

    SimpleWFileChar& operator = (const WString& Str)
    {
        This->SetPointerPosition(offset);
        This->Write(Str);
        return *this;
    }

    WString operator [] (Unsigned i)
    {
        const Unsigned& length = This->GetWFileLength();
        if (length == 0)
            return "";
        if (i >= length)	// protecting code
            i = length - 1;

        This->SetPointerPosition(offset);
        if (offset <= i)
            return This->Read((DWORD)(i - offset + 1));
        return WString();
    }

    SimpleWFileChar(SimpleWFile* fileParam, const Unsigned& offsetParam)
    {
        This = fileParam;
        offset = offsetParam;
    }
};


inline SimpleWFileChar SimpleWFile::operator [] (const Unsigned& i)
{
    if (is_open == false)
        print_error(L"Sorry. operator [] (const ULONGLONG& i)");
    return SimpleWFileChar(this, i);
}

inline WString SimpleWFile::MultiCutOff(const WString& ExSub)
{
    Unsigned start = GetPointerPosition();
    Unsigned i = start;
    Unsigned j = 0;

    Unsigned pos1 = MAXUnsigned;

    SimpleWFile& file = *this;
    wchar_t* const& c2 = WCS(ExSub);

    Unsigned len1 = GetWFileLength();
    if (len1 == start)
        return L"";

    Unsigned len2 = ExSub.Size();

    bool is_consecutive = true;

    while (true)// to be breaked
    {
        if (len1 <= i)
            break;
        if (len2 <= j)
            break;

        if (c2[j] == (wchar_t)file[i]) // if c2[j] was found
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
            i++;    // search next block to find it
            is_consecutive = false;
            // not consecutive (if j==len2, this line is not reached)
        }
    }// while(1)

    if (is_consecutive == true)
        if (j == len2)   // if all were found
        {
            WString ret = file[start][pos1 - 1];
            SetPointerPosition(i);
            return ret;
        }
    // if not found
    return file[start][len1 - 1];
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef Unsigned Wuint;


// template<typename Wuint>
struct WPackItem;


// template<typename Wuint = unsigned long long>
class WPack : public SimpleWFile
{
    friend struct WPackItem/*<Wuint>*/;

    static auto const pack_unsigned_extreme_end = MAXUnsigned;
private:
    WString file_info;

    void AddNewWPack(const WString& pack)
    {
        GoToEndOfWFile();
        GoBackward(sizeof(Wuint) / sizeof(wchar_t));              // go to start of last Wuint
        Wuint new_position = GetPointerPosition(); // before last Wuint
        Wuint pre_position = ReadInteger<Wuint>(); // get position of last pack
        SetPointerPosition(pre_position + sizeof(Wuint) / sizeof(wchar_t)); //
        WriteInteger<Wuint>(new_position);
        SetPointerPosition(new_position);
        AddWPack(pre_position, pack_unsigned_extreme_end, pack);
        WriteInteger<Wuint>(new_position); // the last Wuint points to new pack
        SetPointerPosition(new_position); // set pointer before new pack.
    }

    void AddWPack(
        const Wuint& pre_pack_position,
        const Wuint& next_pack_position,
        const WString& pack
    )
    {
        const unsigned& pack_len = (unsigned)(sizeof(wchar_t) * pack.Size());
        WriteInteger<Wuint>(pre_pack_position);
        WriteInteger<Wuint>(next_pack_position);
        WriteInteger<unsigned>(pack_len);
        Write(pack);
    }

    void DeleteWPack()
    {
        Wuint current_position = GetPointerPosition();
        Wuint pre_pack = ReadInteger<Wuint>();
        Wuint next_pack = ReadInteger<Wuint>();
        SetPointerPosition(current_position); // return to original place
        Wuint pre_position;
        if (pre_pack != pack_unsigned_extreme_end)  // if pre pack exists
        {
            GoToPreWPack();
            pre_position = GetPointerPosition();
            GoForward(sizeof(Wuint) / sizeof(wchar_t));
            WriteInteger<Wuint>(next_pack);
            SetPointerPosition(current_position); // return to original place
        }
        else
            print_error(L"DeleteWPack()\nthe first pack cannot be deleted");

        if (next_pack != pack_unsigned_extreme_end)
        {
            GoToNextWPack();
            WriteInteger<Wuint>(pre_pack);
            GoBackward(sizeof(Wuint) / sizeof(wchar_t)); // go to start of next pack
        }
        else
        {
            GoToEndOfWFile();
            GoBackward(sizeof(Wuint) / sizeof(wchar_t));
            WriteInteger<Wuint>(pre_position); // writing the last Wuint
            GoToLastWPack();
        }
    }

    Wuint GetWPackLength()
    {
        Wuint current_position = GetPointerPosition();
        GoForward(sizeof(Wuint) / sizeof(wchar_t) + sizeof(Wuint) / sizeof(wchar_t));
        unsigned pack_len = ReadInteger<unsigned>();
        SetPointerPosition(current_position);
        return pack_len;
    }

    WString GetWPack()
    {
        Wuint current_position = GetPointerPosition();
        GoForward(sizeof(Wuint) / sizeof(wchar_t) + sizeof(Wuint) / sizeof(wchar_t));
        WString data = Read(ReadInteger<unsigned>() / sizeof(wchar_t));
        SetPointerPosition(current_position);
        GoToNextWPack();
        return data;
    }

    void GoTo(const Wuint& n)
    {
        SetPointerPosition(0);
        for (Wuint i = 0; i < n; i++)
            if (GoToNextWPack() == false)
                print_error(L"GoTo()\nInvalid Parameter.");
    }

    void GoToLastWPack()
    {
        GoToEndOfWFile();
        GoBackward(sizeof(Wuint) / sizeof(wchar_t));
        Wuint last_pack = ReadInteger<Wuint>();
        SetPointerPosition(last_pack);
    }

    bool GoToNextWPack()
    {
        Wuint current_position = GetPointerPosition();
        GoForward(sizeof(Wuint) / sizeof(wchar_t));
        Wuint next_pack_position = ReadInteger<Wuint>();

        if (next_pack_position == pack_unsigned_extreme_end)
        {
            SetPointerPosition(current_position);// don't change position if false
            return false;                   // cannot go ahead
        }

        if (next_pack_position >= GetWFileLength()) // protecting code
            DeleteAndThrow();

        SetPointerPosition(next_pack_position);
        return true; // did successfully
    }

    bool GoToPreWPack()
    {
        Wuint current_position = GetPointerPosition();
        Wuint pre_pack_position = ReadInteger<Wuint>();
        if (pre_pack_position == pack_unsigned_extreme_end)
        {
            SetPointerPosition(current_position);
            return false; // cannot go back
        }

        if (pre_pack_position >= GetWFileLength()) // protecting code
            DeleteAndThrow();

        SetPointerPosition(pre_pack_position);
        return true; // did successfully
    }

    void Insert(const WString& new_pack)
    {
        Wuint current_position = GetPointerPosition();
        Wuint pre_position = ReadInteger<Wuint>();

        GoToEndOfWFile();
        GoBackward(sizeof(Wuint) / sizeof(wchar_t));// before the last Wuint
        Wuint last_uint = ReadInteger<Wuint>(); // read the last Wuint
        GoBackward(sizeof(Wuint) / sizeof(wchar_t));// before the last Wuint

        Wuint new_position = GetPointerPosition(); // place of new pack

        AddWPack(pre_position, current_position, new_pack); // adding new pack

        WriteInteger<Wuint>(last_uint);    // the last Wuint

        SetPointerPosition(pre_position + sizeof(Wuint) / sizeof(wchar_t));
        WriteInteger<Wuint>(new_position);
        SetPointerPosition(current_position);
        WriteInteger<Wuint>(new_position);
        SetPointerPosition(new_position);
    }

    template<typename type>
    type ReadInteger()
    {
#ifdef __BORLANDC__
#pragma option -w-ccc
#pragma option -w-rch
#endif
        if constexpr (sizeof(type) % sizeof(wchar_t) != 0)
            static_assert("bad type, ReadInteger 268");
            //print_error(L"bad type, ReadInteger 268");
        union
        {
            type ret;
            wchar_t a[sizeof(type) / sizeof(wchar_t)];
        };
        WString Str = Read(sizeof(type) / sizeof(wchar_t));
        //int size = Str.Size();
        for (Uchar i = 0; i < sizeof(type) / sizeof(wchar_t); i++)
            a[i] = Str[i];
        return ret;
    }


    void Swap(Wuint m, Wuint n)
    {
        Wuint current_position = GetPointerPosition();
        if (m == n)
            return;
        if (m > n) // m must be smaller
        {
            Wuint temp = m;
            m = n;
            n = temp;
        }

        if (m == 0)
            print_error(L"Swap()\nyou cannot change the first pack");

        SetPointerPosition(0);
        Wuint i;
        for (i = 0; i < m; i++)
            if (GoToNextWPack() == false)
                print_error(L"Swap()\nBad parameters");

        Wuint m_pos = GetPointerPosition();
        Wuint m_pre_pos = ReadInteger<Wuint>();
        Wuint m_next_pos = ReadInteger<Wuint>();
        SetPointerPosition(m_pos);
        for (i = m; i < n; i++)
            if (GoToNextWPack() == false)
                print_error(L"Swap()\nBad parameters");

        Wuint n_pos = GetPointerPosition();
        Wuint n_pre_pos = ReadInteger<Wuint>();
        Wuint n_next_pos = ReadInteger<Wuint>();

        if (m != n - 1)
        {
            SetPointerPosition(n_pos);
            WriteInteger<Wuint>(m_pre_pos);
            WriteInteger<Wuint>(m_next_pos);

            SetPointerPosition(m_pos);
            WriteInteger<Wuint>(n_pre_pos);
            WriteInteger<Wuint>(n_next_pos);

            if (n_pre_pos != pack_unsigned_extreme_end)
            {
                SetPointerPosition(n_pre_pos);
                GoForward(sizeof(Wuint) / sizeof(wchar_t));
                WriteInteger<Wuint>(m_pos);
            }

            if (m_next_pos != pack_unsigned_extreme_end)
            {
                SetPointerPosition(m_next_pos);
                WriteInteger<Wuint>(n_pos);
            }
        }
        else
        {
            SetPointerPosition(n_pos);
            WriteInteger<Wuint>(m_pre_pos);
            WriteInteger<Wuint>(m_pos);

            SetPointerPosition(m_pos);
            WriteInteger<Wuint>(n_pos);
            WriteInteger<Wuint>(n_next_pos);
        }

        if (n_next_pos != pack_unsigned_extreme_end)
        {
            SetPointerPosition(n_next_pos);
            WriteInteger<Wuint>(m_pos);
        }

        if (m_pre_pos != pack_unsigned_extreme_end)
        {
            SetPointerPosition(m_pre_pos);
            GoForward(sizeof(Wuint) / sizeof(wchar_t));
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
        if constexpr(sizeof(ll) % sizeof(wchar_t) != 0)
            static_assert(L"bad type WriteInteger 288");
        Write(WString((wchar_t*)& ll, sizeof(ll) / sizeof(wchar_t)));
    }

    //==========================================================================

public:
    void Empty();

    Wuint GetWFileLength()
    {
        return SimpleWFile::GetWFileLength();
    }

    Wuint GetNumberOfWPacks()
    {
        Wuint current_position = GetPointerPosition();
        Wuint ret = 1;
        SetPointerPosition(0);
        while (GoToNextWPack() == true)
        {
            ret++;
            if (ret > GetWFileLength()) // protecting code
                DeleteAndThrow();
        }
        SetPointerPosition(current_position);
        return ret;
    }

    void SetNumberOfWPacks(Wuint n);

    WPack& operator<< (const WString& new_pack)
    {
        AddNewWPack(new_pack);
        return *this;
    }

    WPackItem operator[](const Wuint& i); // [0] = info pack

    void Constructor()
    {
        if (GetWFileLength() == 0)
        {
            AddWPack(pack_unsigned_extreme_end, pack_unsigned_extreme_end, file_info);
            WriteInteger<Wuint>((Wuint)0); // the last Wuint
        }
        GetNumberOfWPacks(); // checks if the file is valid
    }

    WPack(WString address, WString file_info = L"no_file_info")
        : SimpleWFile(address), file_info(file_info)
    {
        Constructor();
    }
};



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


struct WPackItem
{
    friend class WPack;

private:
    WPack* This;

    Wuint offset;

    WPackItem(WPack* packfile, const Wuint& offset)
    {
        this->This = packfile;
        this->offset = offset;
    }

    //==========================================================================

public:
    operator WString() const
    {
        Wuint current_position = This->GetPointerPosition();
        This->SetPointerPosition(0);
        for (Wuint i = 0; i < offset; i++)
            This->GoToNextWPack();
        This->GoForward(sizeof(Wuint) / sizeof(wchar_t) + sizeof(Wuint) / sizeof(wchar_t));
        WString ret = This->Read(This->ReadInteger<unsigned>() / sizeof(wchar_t));
        This->SetPointerPosition(current_position);
        return ret;
    }

    WPackItem& operator = (const WString& new_pack)
    {
        if (offset == 0)
            print_error(L"WPackItem::=\n"
                L"you cannot change the first pack.");

        This->SetPointerPosition(0);

        Wuint i;
        for (i = 0; i < offset; i++)
            if (This->GoToNextWPack() == false)
            {
                This->AddNewWPack(new_pack);
                return *this;
            }

        unsigned new_pack_len = (unsigned)(sizeof(wchar_t) * new_pack.Size());
        if (new_pack_len <= This->GetWPackLength())
        {
            This->GoForward(sizeof(Wuint) / sizeof(wchar_t) + sizeof(Wuint) / sizeof(wchar_t));
            This->WriteInteger<unsigned>(new_pack_len);
            This->Write(new_pack);
        }
        else
        {
            This->Insert(new_pack);
            This->GoToNextWPack();
            This->DeleteWPack();
        }
        return *this;
    }

    WPackItem& operator = (const int& x)
    {
        if (offset == 0)
            print_error(L"WPackItem::=\n"
                L"you cannot change the first pack.");
        if (x != 0)
            print_error(L"WPackItem::=\nbad use of operator =.");
        This->GoTo(offset);
        This->DeleteWPack();
        return *this;
    }

    WPackItem& operator = (const WPackItem& item)
    {
        *this = WString(item);
        return *this;
    }

    WPackItem& operator += (const WString& new_pack)
    {
        This->GoTo(offset);
        This->Insert(new_pack);
        This[0][offset] ^ This[0][offset + 1];
        return *this;
    }// insert

    bool operator == (const WPackItem& pack) const
    {
        return WString(*this) == WString(pack);
    }

    bool operator != (const WPackItem& pack) const
    {
        return WString(*this) != WString(pack);
    }

    bool operator < (const WPackItem& pack) const
    {
        return (WString)(*this) < (WString)(pack);
    }

    bool operator >  (const WPackItem& pack) const
    {
        return WString(*this) > WString(pack);
    }

    bool operator <= (const WPackItem& pack) const
    {
        return WString(*this) <= WString(pack);
    }

    bool operator >= (const WPackItem& pack) const
    {
        return WString(*this) >= WString(pack);
    }

    void operator ^ (const WPackItem& pack)
    {
        if (This != pack.This)
            print_error(L"operator ^\n"
                L"both items must be in the same WPack");
        This->Swap(offset, pack.offset);
    }

    SimpleWFileChar operator [](const Unsigned& i)
    {
        //eturn WPackItem(this,i);
        This->GoTo(offset);

        if (This->GetWPackLength() <= i)
            print_error(L"bad second offset");

        Wuint current_position = This->GetPointerPosition();
        SimpleWFile* simple = This;
        return (*simple)[sizeof(Wuint) + sizeof(Wuint) +
            sizeof(unsigned) + current_position + i];
    }
};

inline void WPack::SetNumberOfWPacks(Wuint n)
{
    while (GetNumberOfWPacks() < n)
        this[0] << "";
    while (n < GetNumberOfWPacks())
        (this[0])[GetNumberOfWPacks() - 1] = 0;
}

inline void WPack::Empty()
{
    WString _file_info = (*this)[0];
    SetWFileLength(0);
    AddWPack(pack_unsigned_extreme_end, pack_unsigned_extreme_end, _file_info);
    WriteInteger<Wuint>((Wuint)0); // the last Wuint
}

inline WPackItem WPack::operator[](const Wuint& i)
{
    return WPackItem(this, i);
}

#endif