// a thing is something with formula name`kind

#ifndef ThingH
#define ThingH

//#include "Common Files\\General.h"
#include "String.h"
#include "List.h"
#include "Thing.h"
#include "MainLists.h"

class ThingOrOperator
{
    char attach_char;
public:
    String name;
    String kind;
    String thing; // = name + type

private:
    void Combine()
    {
        thing = name + attach_char + kind;
    }
    void Separate()
    {
        String crack = thing;
        name = crack.CutOff(attach_char);
        kind = crack;
    }

public:
    ThingOrOperator(String name, String kind)
        :name(name), kind(kind)
    {
        attach_char = '`';
        Combine();
    }
    ThingOrOperator(String thing) :thing(thing)
    {
        attach_char = '`';
        Separate();
    }
};

/*
    example:

    Thing k("hello`bye");
    cout<< k.name << endl;
    cout<< k.kind;
*/
//========================================================

class IsThing
{
public:
    bool is_thing;
    String name_and_type;
    String name;
    String kind;

private:
    const MainLists& ML;

    void Run(String to_be_thing_name)
    {
        Unsigned len = ML.things.Size();
        for (Unsigned i = 0; i < len; i++)
        {
            String thing = ML.things[i];
            ThingOrOperator T(thing);
            if (to_be_thing_name == T.name)
            {
                this->name_and_type = ML.things[i];
                is_thing = true;
                return;
            }
        }
        is_thing = false;
    }

public:
    IsThing(const MainLists& ML, const String& to_be_thing_name) : ML(ML)
    {
        Run(to_be_thing_name);
        ThingOrOperator T(name_and_type);
        name = T.name;
        kind = T.kind;
    }
};

//========================================================

class IsOperator
{
public:
    bool is_operator;
    String operator_thing;
    String name;
    String kind;
    Unsigned priority;

private:
    const MainLists& ML;

    void Run(String to_be_operator)
    {
        Unsigned len = ML.operators.Size();
        for (Unsigned i = 0; i < len; i++)
        {
            String _operator = ML.operators[i];
            ThingOrOperator T(_operator);
            if (to_be_operator == T.name)
            {
                this->operator_thing = ML.operators[i];
                is_operator = true;
                if (to_be_operator == "=")
                    priority = 0;
                else
                    priority = MAXUnsigned - 1 - i;
                return;
            }
        }
        is_operator = false;
    }

public:
    IsOperator(const MainLists& ML, const String& to_be_operator_name)
        : ML(ML), priority(MAXUnsigned)
    {
        Run(to_be_operator_name);
        ThingOrOperator T(operator_thing);
        name = T.name;
        kind = T.kind;
    }
};


#endif
