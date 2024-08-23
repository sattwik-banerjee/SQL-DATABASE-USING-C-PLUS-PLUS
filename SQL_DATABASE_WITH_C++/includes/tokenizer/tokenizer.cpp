#include <string>
#include <cstring>
#include "tokenizer.h"
using namespace std;

Tokennize::Tokennize()
{
    _token = "";
    _type = 0;
}
Tokennize::Tokennize(string str, int type)
{
    _token = str;
    _type = type;
}
int Tokennize::type() const
{
    return(_type);
}
string Tokennize::type_string() const
{
    if(_type == 1 || _type == 7)
    {
        return "NUMBER";
    }
    else if(_type == 2)
    {
        return "ALPHA";
    }
    else if(_type == 3)
    {
        return "SPACE";
    }
    else if(_type == 5 || _type == 6 || _type == 8)
    {
        return "PUNC";
    }
    else if(_type == 9)
    {
        return "OPERATOR";
    }
    return "UNKNOWN";
}
string Tokennize::token_str() const
{
    return (_token);
}