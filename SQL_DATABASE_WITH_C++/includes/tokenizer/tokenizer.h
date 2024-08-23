#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <iostream>
#include <string>

using namespace std;

class Tokennize
{
public:
    Tokennize();
    Tokennize(string str, int type);
    friend ostream& operator <<(ostream& outs, const Tokennize& t)
    {
        outs << t._token;
        return outs;
    }
    int type() const;
    string type_string() const;
    string token_str() const;
private:
    string _token;
    int _type;
};

#endif