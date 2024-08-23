#ifndef STOKENIZER_H
#define STOKENIZER_H

#include "../../includes/tokenizer/tokenizer.h"
#include "../../includes/tokenizer/constants.h"
#include <string>

class STokenizer
{
public:
    STokenizer();
    STokenizer(char str[]);
    bool done();            //true: there are no more tokens
    bool more();            //true: there are more tokens
    //

    //---------------
    //extract one token (very similar to the way cin >> works)
    friend STokenizer& operator >> (STokenizer& s, Tokennize& t)
    {
        int type = 0;
        string str = "";
        if(s.get_token(type, str))
        {
            string temp = str;
            t = Tokennize(temp, type);
        }
        return s;
    }

    //set a new string as the input string
    void set_string(char str[]);

    bool is_success(const int& num)
    {
        int success = _table[num][0];
        if(num != 0)
        {
            return true;
        }
        return false;
    }

private:
    //create table for all the tokens we will recognize
    //                      (e.g. doubles, words, etc.)
    void make_table(int _table[][MAX_COLUMNS]);

    //extract the longest string that match
    //     one of the acceptable token types
    bool get_token(int& start_state, string& token);
    //---------------------------------
    char _buffer[MAX_BUFFER];       //input string
    int _pos;                       //current position in the string
    static int _table[MAX_ROWS][MAX_COLUMNS];
};

#endif