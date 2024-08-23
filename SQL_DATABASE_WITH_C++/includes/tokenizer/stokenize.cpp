#include "../../includes/tokenizer/stokenize.h"
#include "../../includes/tokenizer/constants.h"
#include "../../includes/tokenizer/tokenizer.h"
#include "../../includes/tokenizer/state_machine_functions.h"
#include <iostream>
#include <cstring>

using namespace std;

int STokenizer::_table[MAX_ROWS][MAX_COLUMNS];

STokenizer::STokenizer()
{
    _pos = 0;
    char str[] = "";
    strcpy(_buffer, str);
    make_table(_table);
}
STokenizer::STokenizer(char str[])
{
    _pos = 0;
    make_table(_table);
    strcpy(_buffer, str);
}
bool STokenizer::done()            //true: there are no more tokens
{
    if(_buffer[0] == '\0' && _pos == 0)
    {
        return (true);
    }
    return false;
}
bool STokenizer::more()           //true: there are more tokens
{
    return(!done());
}
//

//---------------
//extract one token (very similar to the way cin >> works)

//set a new string as the input string
void STokenizer::set_string(char str[])
{
    strcpy(_buffer, str);
}

//create table for all the tokens we will recognize
//                      (e.g. doubles, words, etc.)
void STokenizer::make_table(int _table[][MAX_COLUMNS])
{
    init_table(_table);
    //doubles:
    mark_fail(_table, 0);            //Mark states 0 and 7 as fail states
    mark_success(_table, 1);         //Mark states 1 - 6 as success states
    mark_success(_table, 2);
    mark_success(_table, 3);
    mark_success(_table, 4);
    mark_success(_table, 5);
    mark_fail(_table, 6);
    mark_success(_table, 7);
    mark_success(_table, 8);

    mark_cells(0, _table, DIGITS, 1);    //state [0] --- DIGITS ---> [1]
    mark_cells(0, _table, '.', '.', 8);  //state [0] --- '.' ------> [2] 
    mark_cells(1, _table, DIGITS, 1);    //state [1] --- DIGITS ---> [1]
    mark_cells(1, _table, '.', '.', 6);  //state [1] --- '.' ------> [2] 
    mark_cells(8, _table, DIGITS, 7);    //state [2] --- DIGITS ---> [3]
    mark_cells(7, _table, '.', '.', -1);
    mark_cells(7, _table, DIGITS, 7);    //state [1] --- DIGITS ---> [1]
    mark_cells(6, _table, DIGITS, 7);
    
    //words:
    mark_cells(0, _table, ALFA, 2);    //state [0] --- DIGITS ---> [1]
    mark_cells(2, _table, ALFA, 2);  //state [0] --- '.' ------> [2] 
    mark_cells(2, _table, DIGITS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(2, _table, SPACES, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(2, _table, OPERATORS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(2, _table, PUNC, -1);    //state [1] --- DIGITS ---> [1]

    //spaces:
    mark_cells(0, _table, SPACES, 3);    //state [0] --- DIGITS ---> [1]
    mark_cells(3, _table, SPACES, -1);  //state [0] --- '.' ------> [2] 
    mark_cells(3, _table, DIGITS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(3, _table, ALFA, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(3, _table, OPERATORS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(3, _table, PUNC, -1);    //state [1] --- DIGITS ---> [1]

    //operators:
    mark_cells(0, _table, OPERATORS, 4);    //state [0] --- DIGITS ---> [1]
    mark_cells(4, _table, OPERATORS, -1);  //state [0] --- '.' ------> [2] 
    mark_cells(4, _table, DIGITS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(4, _table, SPACES, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(4, _table, ALFA, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(4, _table, PUNC, -1);    //state [1] --- DIGITS ---> [1]

    //punctuation:
    mark_cells(0, _table, PUNC, 5);    //state [0] --- DIGITS ---> [1]
    mark_cells(5, _table, PUNC, -1);  //state [0] --- '.' ------> [2] 
    mark_cells(5, _table, DIGITS, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(5, _table, SPACES, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(5, _table, ALFA, -1);    //state [1] --- DIGITS ---> [1]
    mark_cells(5, _table, OPERATORS, -1);    //state [1] --- DIGITS ---> [1]

    mark_cells(0, _table, OPERATORS, 9);
    mark_cells(9, _table, OPERATORS, 9);

    mark_cell(2, _table, 95, 2);
    mark_cell(0, _table, 95, 2);
    
}

//extract the longest string that match
//     one of the acceptable token types
bool STokenizer::get_token(int& start_state, string& token)
{
    //initialize variables to process tokens from the _buffer
    int current_state = start_state;
    int previous_state = 0;
    int marker = 0;
    int success = 0;
    int size = 0;
    bool is_spanish = false;
    //turn cstr into str in order to take tokens from str
    std::string cstr(_buffer);
    token = cstr;
    //create temp
    string temp = token;
    //while loop to iterate through the string to find the tokens using the conditions below
    while(size < temp.size() && current_state != -1)
    {
        char sbtr = temp[size];
        if(sbtr < -1)
            is_spanish = true; //finds spanish characters
        current_state = _table[current_state][sbtr];
        success = _table[current_state][0];
        if(success != 0) //determines if we are in a success state
        {
            if(current_state != -1) //determines if the character is a fail state
            {
                previous_state = current_state;
                marker++;
            }
        }
        size++;
    }
    //determine if the _buffer is empty, therefore there are no more tokens
    if(cstr == "")
    {
        _pos = 0;
        return false;
    }
    if(current_state == -1) //not empty and reached a fail state
    {   
        if(marker != 1) //if there is only one character token, return the single token
            marker = size - 1;
        if(previous_state == 0) //this means that there were no tokens
        {
            if(size == 1) //checks to see if the character was a spanish chaarcter
            {
                //returns the character as an unknown token and updates the _buffer
                start_state = previous_state;
                string str = "";
                token = temp.substr(0, size);
                _pos = 0;
                //if the token is the last token in the cstr we tell the done function there is one
                //more token left to print then after that the process is done
                if(token == cstr)
                {
                    char str[] = "";
                    strcpy(_buffer, str);
                    _pos = 1;
                    return true;
                }
                else 
                {
                    //if spanish incrememnt the position by 2 and alter the _buffer
                    if(is_spanish)
                    {
                        marker += 2;
                    }
                    for(int i = size; i < temp.size(); i++)
                    {
                        str += temp[i];
                    }
                    const char* s = str.c_str();
                    strcpy(_buffer, s);
                    return true;
                }
            }
            return false;
        }
        else
        {   
            //copy the token and alter the _buffer to represent the rest of the cstr
            start_state = previous_state;
            string str = "";
            token = temp.substr(0, marker);
            _pos = 0;
            if(token == cstr)
            {
                char str[] = "";
                strcpy(_buffer, str);
                _pos = 1;
                return true;
            }
            else 
            {
                //once again check if the chaarcter is spanish character
                //and increment the position by 2
                if(is_spanish)
                {
                    marker += 2;
                }
                for(int i = marker; i < temp.size(); i++)
                {
                    str += temp[i];
                }
                const char* s = str.c_str();
                strcpy(_buffer, s);
                return true;
            }
        }
    }
    else // did not reach a fail state
    {
        //copy the string into token and alter the _buffer to represent the rest of the cstr
        if(marker != 1)
        {
            marker = size;
        }
        start_state = previous_state;
        string str = "";
        token = temp.substr(0, marker);
        _pos = 0;
        if(token == cstr)
        {
            char str[] = "";
            strcpy(_buffer, str);
            _pos = 1;
            return true;
        }
        else 
        {
            //determine if the character is a spanish character and increment the position by 2
            if(is_spanish)
                marker += 2;
            for(int i = marker; i < temp.size(); i++)
            {
                str += temp[i];
            }
            //copy the remainder of the cstr into the _buffer
            const char* s = str.c_str();
            strcpy(_buffer, s);
            return true;
        }
    }
    
}
//---------------------------------