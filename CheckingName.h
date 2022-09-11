// check if the name is legal

#ifndef CheckingNameH
#define CheckingNameH
#include "Common Files\\String.h"

class CheckingName
{
    String& name;
    String legal_chars;
public:
    bool is_legal;
private:
    void Check()
    {
        if (legal_chars.IsSuperset(name))
            is_legal = true;
        else
            is_legal = false;
    }
public:
    CheckingName(String& name)
        :name(name)
    {
        legal_chars = "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";
        Check();
    }
};

class CheckingOperator
{
    String& name;
    String legal_chars;
public:
    bool is_legal;
private:
    void Check()
    {
        if (legal_chars.IsSuperset(name))
            is_legal = true;
        else
            is_legal = false;
    }
public:
    CheckingOperator(String& name)
        :name(name)
    {
        legal_chars = "<>.~/%\\#@$+-^:'\"";//"+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"";
        Check();
    }
};

#endif
