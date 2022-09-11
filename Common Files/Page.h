#ifndef PageH
#define PageH
#include "General.h"
#include "Button.h"
#include "EditBox.h"
#include "StaticText.h"
#include "Picture.h"
#include "ProgressReport.h"
#include "Scroll.h"
#include "File.h"
#include "Context.h"

//extern Devcon* pdevcon = 0;
//extern HDC v_hdc = 0;

class Page;

class PrimaryPage
{
    static int count;
protected:
    Unsigned current_page;

    bool stop_paint;

    std::vector<Button*> pagebuttons;

#define macTypeDef(name,class_name)                         \
    typedef std::multimap<Unsigned,class_name*> name##_MAP; \
    typedef name##_MAP::iterator name##_IT;                 \
    typedef std::pair<Unsigned,class_name*> name##_PAIR;

    macTypeDef(B, Button); // button
    macTypeDef(E, EditBox); // editbox
    macTypeDef(S, StaticText); // static text
    macTypeDef(PIC, Picture); // picture
    macTypeDef(PROG, ProgressReport); // progress report
    macTypeDef(HSC, HScroll);
    macTypeDef(CON, Context);
    macTypeDef(P, Page); // page

    B_MAP buttons;
    E_MAP editboxes;
    S_MAP statictexts;
    PIC_MAP pictures;
    PROG_MAP progresses;
    HSC_MAP hscrolls;
    CON_MAP contexts;
    P_MAP ppages;

    Pack status_file;


    void PageEditBoxActivate(Unsigned page_num)
    {
        bool found = false;
        for (E_IT it = editboxes.begin(); it != editboxes.end(); it++)
        {
            if (it->first != page_num)
                continue;
            EditBox* peb = it->second;
            if (peb->blinking_allowed == true)
                if (found == false)
                {
                    peb->Focus();
                    //peb -> Show();
                    found = true;
                }
                else
                {
                    peb->Unfocus();
                }
        }

        for (P_IT it = ppages.begin(); it != ppages.end(); it++)
        {
            if (it->first != page_num)
                continue;

            PrimaryPage* ppage = (PrimaryPage*)(it->second);
            if (found == false)
                ppage->PageEditBoxActivate(ppage->current_page);
            else
            {
                for (E_IT _it = (ppage->editboxes).begin(); _it != (ppage->editboxes).end(); _it++)
                    _it->second->Unfocus();
            }
        }
    }

public:
    void Empty()
    {
        pagebuttons.clear();
        buttons.clear();
        editboxes.clear();
        statictexts.clear();
        pictures.clear();
        progresses.clear();
        hscrolls.clear();
        contexts.clear();

        for (P_IT it = ppages.begin(); it != ppages.end(); it++)
            ((PrimaryPage*)(it->second))->Empty();
        ppages.clear();
    }

    void StopPaint()
    {
        stop_paint = true;
    }

    void StartPaint()
    {
        stop_paint = false;
    }


    PrimaryPage& operator << (Button* pbtn)
    {
        pagebuttons.push_back(pbtn);
        return *this;
    }

    Unsigned GetPage()
    {
        return current_page;
    }

    void NextPage()
    {
        Unsigned n = current_page + 1;
        Unsigned size = pagebuttons.size();
        if (size <= n)
            n = 0;
        (*this)[n].Activate();
    }

    void Hide()
    {
        Unsigned b_size = pagebuttons.size();
        for (Unsigned i = 0; i < b_size; i++)
            pagebuttons[i]->Hide();

#define HIDE(name,objects)                                         \
        for(name##_IT it = objects.begin(); it != objects.end(); it++) \
            it -> second -> Hide()

        HIDE(B, buttons);
        HIDE(E, editboxes);
        HIDE(S, statictexts);
        HIDE(PIC, pictures);
        HIDE(PROG, progresses);
        HIDE(HSC, hscrolls);
        HIDE(CON, contexts);

        for (P_IT it = ppages.begin(); it != ppages.end(); it++)
            ((PrimaryPage*)(it->second))->Hide();
    }

    void Show()
    {
        Unsigned b_size = pagebuttons.size();
        for (Unsigned i = 0; i < b_size; i++)
            pagebuttons[i]->Show();

#define SHOW(name,objects)                                              \
        for(name##_IT it = objects.begin(); it != objects.end(); it++)  \
        {                                                               \
            if(it -> first == current_page)                             \
                it -> second -> Show();                                 \
        }

        SHOW(B, buttons);
        SHOW(E, editboxes);
        SHOW(S, statictexts);
        SHOW(PIC, pictures);
        SHOW(PROG, progresses);
        SHOW(HSC, hscrolls);
        SHOW(CON, contexts);

        for (P_IT it = ppages.begin(); it != ppages.end(); it++)
        {
            if (it->first == current_page)
                ((PrimaryPage*)(it->second))->Show();
        }
    }

    void Paint()
    {
        Hide();
        Show();
    }

    struct PageItem
    {
        PrimaryPage* This;
        Unsigned offset;

        PageItem(PrimaryPage* This, Unsigned offset)
            : This(This), offset(offset)
        {
        }

        void Activate(bool paint = true)
        {

            /*#define	UNFOCUS(name,objects)\
                for(name##_IT it = (This -> objects).begin(); it != (This -> objects).end(); it++)\
                    if(it -> first == This -> current_page)\
                        it -> second -> Unfocus();

                UNFOCUS(B,buttons);
                UNFOCUS(E,editboxes);
                UNFOCUS(S,statictexts);
                UNFOCUS(PIC,pictures);
                UNFOCUS(PROG,progresses);
                UNFOCUS(HSC,hscrolls); */

            for (HSC_IT it = (This->hscrolls).begin(); it != (This->hscrolls).end(); it++)
                if (it->first == This->current_page)
                    it->second->EndMoving(false);

            if (This->current_page != MAXUnsigned)
                (This->pagebuttons)[This->current_page]->Release();
            (This->pagebuttons)[offset]->Press();
            This->current_page = offset;
            (This->status_file)[1] = String((int)offset);
            This->PageEditBoxActivate(offset);
            if (paint)
                This->Paint();
        }

#define PAGEITEMDEF(name,class_name,objects)                    \
        PageItem& operator << (class_name* peb)                 \
        {                                                       \
            (This -> objects).insert(name##_PAIR(offset,peb));  \
            peb -> Hide();                                      \
            return *this;                                       \
        }

        PAGEITEMDEF(E, EditBox, editboxes)
            PAGEITEMDEF(B, Button, buttons)
            PAGEITEMDEF(S, StaticText, statictexts)
            PAGEITEMDEF(PROG, ProgressReport, progresses)
            PAGEITEMDEF(PIC, Picture, pictures)
            PAGEITEMDEF(HSC, HScroll, hscrolls)
            PAGEITEMDEF(CON, Context, contexts)

            PageItem& operator << (Page* ppage)
        {
            if (((PrimaryPage*)ppage)->pagebuttons.size() != 0)
                print_error("only empty subpages can be added. first add then fill the subpage");
            (This->ppages).insert(P_PAIR(offset, ppage));
            ((PrimaryPage*)ppage)->Hide();
            return *this;
        }
    };


    PageItem operator [](Unsigned i)
    {
        if (pagebuttons.size() <= i)
            i = pagebuttons.size() - 1;
        return PageItem(this, i);
    }

    bool isStatusFileCreated()
    {
        return (status_file.GetNumberOfPacks() > 1);
    }

    void Startup() // if file is created use this to restore from file
    {
        if (isStatusFileCreated())
        {
            String data = status_file[1];
            Unsigned a = 0;
            try
            {
                a = data.Int();//ToType<Unsigned>(data);
            }
            catch (...)
            {
                status_file.DeleteAndThrow();
            }
            (*this)[a].Activate(true);
        }
        else
            (*this)[0].Activate(true);

    }
    friend struct PageItem;

public:
    PrimaryPage() : status_file(
        GetAppDataPath() + WString("pagestatus") + WString(count)
    )
    {
        stop_paint = false;
        current_page = MAXUnsigned;
        count++;
    }

    ~PrimaryPage()
    {
        count--;
    }
};

class Page : public PrimaryPage
{
public:
#define macAccumFunc(func,wParam,lParam)                                  \
    {                                                                     \
        for(B_IT it = buttons.begin(); it != buttons.end(); it++)         \
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(E_IT it = editboxes.begin(); it != editboxes.end(); it++)     \
        {                                                                 \
            if(it -> first == current_page)                               \
                (it -> second) -> func(wParam,lParam);                    \
        }                                                                 \
        for(S_IT it = statictexts.begin(); it != statictexts.end(); it++) \
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(PIC_IT it = pictures.begin(); it != pictures.end(); it++)     \
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(PROG_IT it = progresses.begin(); it != progresses.end(); it++)\
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(HSC_IT it = hscrolls.begin(); it != hscrolls.end(); it++)     \
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(CON_IT it = contexts.begin(); it != contexts.end(); it++)     \
        {                                                                 \
            if(it -> first == current_page)                               \
                it -> second -> func(wParam,lParam);                      \
        }                                                                 \
        for(P_IT it = ppages.begin(); it != ppages.end(); it++)           \
        {                                                                 \
            if(it -> first == current_page)                               \
                ((Page*)(it -> second)) -> func(wParam,lParam);           \
        }                                                                 \
    }

    void wmPaint(WPARAM , Unsigned )
    {
        Paint();
    }

    void wmKeydown(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmKeydown, wParam, lParam);
    }

    void wmSyskeydown(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmSyskeydown, wParam, lParam);
    }

    void wmChar(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmChar, wParam, lParam);
    }

    void wmTimer(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmTimer, wParam, lParam);
    }

    void wmTimer2(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmTimer2, wParam, lParam);
    }

    void wmMousewheel(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmMousewheel, wParam, lParam);
    }

    void wmLbuttondown(WPARAM wParam, LPARAM lParam) // left click down
    {
        macAccumFunc(wmLbuttondown, wParam, lParam);

        if (current_page != MAXUnsigned)
            pagebuttons[current_page]->Release(false);

        Unsigned b_size = pagebuttons.size();
        for (Unsigned i = 0; i < b_size; i++)
        {
            pagebuttons[i]->wmLbuttondown(wParam, lParam);
            if (pagebuttons[i]->is_pressed)
            {
                (*this)[i].Activate(false);
                PageEditBoxActivate(i);
                Paint();
            }
        }

        if (current_page != MAXUnsigned)
            pagebuttons[current_page]->Press(false);
    }

    void wmRbuttondown(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmRbuttondown, wParam, lParam);
    }

    void wmLbuttonup(WPARAM wParam, LPARAM lParam) // left click up
    {
        macAccumFunc(wmLbuttonup, wParam, lParam);
    }

    void wmMousemove(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmMousemove, wParam, lParam);

        Unsigned b_size = pagebuttons.size();
        for (Unsigned i = 0; i < b_size; i++)
        {
            PointF P(LOWORD(lParam), HIWORD(lParam));
            if (pagebuttons[i]->inBox(P))
                pagebuttons[i]->On();
            else
                pagebuttons[i]->Off();
        }
    }

    void wmMouseleave(WPARAM wParam, LPARAM lParam)
    {
        macAccumFunc(wmMouseleave, wParam, lParam);
    }

    void DeleteAll()
    {
        if (pdevcon->v_hdc)
            DeleteDC(pdevcon->v_hdc);

#define DELETEALL(name,objects)                                         \
        for(name##_IT it = objects.begin(); it != objects.end(); it++)  \
        {                                                               \
            if(it -> first == current_page)                             \
                delete it -> second;                                    \
        }

        DELETEALL(B, buttons);
        DELETEALL(E, editboxes);
        DELETEALL(S, statictexts);
        DELETEALL(PIC, pictures);
        DELETEALL(PROG, progresses);
        DELETEALL(HSC, hscrolls);
        DELETEALL(CON, contexts);

        Unsigned b_size = pagebuttons.size();
        for (Unsigned i = 0; i < b_size; i++)
            delete pagebuttons[i];
    }

public:
    Page()
    {
        current_page = MAXUnsigned;
    }
    ~Page()
    {
    }
};

#endif
