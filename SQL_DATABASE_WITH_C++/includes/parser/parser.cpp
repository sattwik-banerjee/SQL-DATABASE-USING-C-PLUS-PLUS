#include "../parser/parser.h"
#include "../table/table.h"
#include "../bplustree/multimap.h"
#include "../tokenizer/stokenize.h"
#include "../tokenizer/tokenizer.h"
#include "../tokenizer/constants.h"
#include "../table/actual_typedefs.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

Parser::Parser()
{
    flag = true; // set the flag to true (default)
}

Parser::Parser(char* s)
{
    strcpy(buffer_str, s);
    flag = true; // set flag to default value of true
    set_string(s); // set the string for input
}

void Parser::set_string(char* s)
{
    // make sure everything is empty for processing a new string command
    ptree.clear();
    keywords.clear();
    while(!tokens.empty())
    {
        tokens.pop();
    }
    int quote_count = 0; // temp local variable for the count of the quotes
    bool quote = false; // boolean for keeping track of quotes
    string fn; // temp string local for the case of a quote
    strcpy(buffer_str, s);
    make_table(_table); // initialize the table
    state.set_string(s); // set the string for input
    // table is made with constructor
    Tokennize t; // create temp token to store each token
    state >> t; // find each token
    while(state.more()) // continue iterating through the c_str
    {
        if(t.token_str() == "\"") // check to see if we find a "
        {
            quote_count++; // increment quote count
            quote = true; // set boolean to true since we have one quote
        }
        else if(quote && quote_count < 2) // if we have a " we want to make everything inside a single token
        {
            string str = t.token_str(); // get the string value of the token
            is_keyword(str); // check to see if the string is a keyword or not, if it is it will be converted to all lowercase
            fn += str; // no matter what we will add the string to the fn string
        }
        else 
        {
            string str = t.token_str(); // get the string value of the token
            is_keyword(str); // check to see if the string is a keyword or not, if it is it will be converted to all lowercase
            tokens.push(str); // push every individual token into the Queue of tokens
        }
        if(quote_count == 2) // only print the string inbetween the " " once we find the second quote
        {
            tokens.push(fn); // push all the strings that accumulated in the fn string
            quote = false; // make sure to reset all the quote identifiers
            quote_count = 0; // reset the quote_count
            fn = ""; // reset the fn string
        }
        state >> t; // continue reading
    }
    if(quote_count != 1) // mismatching quotes leads to an illegal SQL statement
    {
        build_keymap(); // build the map of keywords
        flag = get_parse_tree(); // build the parse tree
    }
}

bool Parser::is_keyword(string& str) // function to determine if a string is a keyword
{
    string res; // temp string that will be storing the lowercase version of str
    for(char c : str) // iterate through the characters in str
    {
        char tosr = (char) tolower(c); // make every character lowercase
        to_string(tosr); // make the character a string
        res += tosr; // add it to the temp string
    }
    if(res == "select" || res == "insert" || res == "make" || res == "create" || res == "delete" || res == "drop" || res == "from" || res == "*" || res == "where" || res == "into" || res == "values" || res == "table" || res == "fields")
    { // if the string is one of the keywords
        str = res; // set the string to the lowercase version (res)
        return true; // this means the string is also a keyword
    }
    return false; // if the string does not match any of the key words then we return false
}

string Parser::get_enum_key(const int& type) // this function converts int key to a string value to be stored in the ptree
{
    switch(type) 
        {
        case COMMAND:
            return "command";
        case FIELDS:
            return "fields";
        case FROM:
            return "FROM";
        case TABLE:
            return "table_name";
        case WHERE:
            return "where";
        case CONDITION:
            return "condition";
        case INTO:
            return "INTO";
        case VALUES:
            return "VALUES";
        case LIST:
            return "values";
        case TABLE_IT:
        case TABLE_NAME:
        case TABLE_D:
            return "table_name";
        case FIELDS_M:
            return "FIELDS";
        case LIST_M:
            return "cols";
        case LOGICAL:
            return "logical";
        case RELATIONAL:
            return "relational";
        default:
            return "";
    }
}

void Parser::build_keymap()
{
    bool del = false;
    Queue<string>::Iterator it; // iterate through the queue of string tokens
    for(it = tokens.begin(); it != tokens.end(); ++it)
    {   // determine if the string it is one of the following keywords, and if it is we add it to the keywords Map
        if(*it == "select" || *it == "insert" | *it == "make" || *it == "create" || *it == "delete" || *it == "drop")
        {
            if(*it == "delete" || *it == "drop")
            {
                del = true;
            }
            keywords[(*it)] = COMMAND;
        }
        else if(*it == "from")
        {
            keywords[(*it)] = FROM;
        }
        else if(*it == "*")
        {
            keywords[(*it)] = FIELDS;
        }
        else if(*it == "where")
        {
            keywords[(*it)] = WHERE;
        }
        else if(*it == "into")
        {
            keywords[(*it)] = INTO;
        }
        else if(*it == "values")
        {
            keywords[(*it)] = VALUES;
        }
        else if(*it == "table" && del)
        {
            keywords[(*it)] = DEL;
        }
        else if(*it == "table")
        {
            keywords[(*it)] = CREATE;
        }
        else if(*it == "fields")
        {
            keywords[(*it)] = FIELDS_M;
        }
    }
}

bool Parser::get_parse_tree()
{
    // set up ptree using the keywords map
    Queue<string>::Iterator it;
    flag = true; // default assignments
    bool was_word = false; // checker for the comma and fields/values exception
    bool from = false; // checker for if we recieved the keyword from (select)
    bool into = false; // checker for if we recieved the keyword into (insert)
    bool table = false; // checker for if we recieved the keyword table (make)
    bool fields = false; // checker for if we recieved the keyword fields (make)
    bool comma = false; // this will be used for determining if a comma is missing
    int state = -1;
    int new_state = -1;
    for (it = tokens.begin(); it != tokens.end(); it++)
    {
        if(keywords.contains(*it))
        {
            state = keywords.get((*it)); // obtain the previous state/col number
            if(state == COMMAND && ptree.get(get_enum_key(COMMAND)).size() == 1) // this means that we are recieving multiple commands
            {
                throw runtime_error("Can't evaluate more than 1 commands!");
            }
            if(*it == "from") // confirm if we recieved the keyword from (for select)
            {
                from = true;
                if(!ptree.contains(get_enum_key(FIELDS)) || ptree.get(get_enum_key(FIELDS)).empty()) // check to see if any fields were given in the SQL statement
                {
                    throw runtime_error("Expected fields!");
                }
            }
            if(*it == "into") // confirm if we recieved the keyword into (for insert)
            {
                into = true;
            }
            if(*it == "table") // confirm if we recieved the keyword table (for make)
            {
                table = true;
            }
            if(*it == "fields") // confirm if we recieved the keyword fields (for make)
            {
                fields = true;
            }
            if(*it == "*") // must consider "*" to be a word for the field exception checking
            {
                was_word = true;
            }
            if(*it != "fields" && *it != "table" && *it != "into" && *it != "values" && *it != "from") // filtering out the ptree
            {
                if(*it == "where")
                {
                    string y = "yes"; // want to mark where as yes
                    ptree[get_enum_key(state)] += y;
                }
                else 
                {
                    ptree[get_enum_key(state)] += (*it); // insert the key into the designated area with it's column
                }
            }
            new_state = -1;
        }
        else 
        {
            new_state = state + 1; // get the new state to add to (won't change until next keyword is reached)
            if(new_state == FROM) // for the case of * being the fields along with other fields, the new_state will be FROM so we need to decrement new_state
            {
                new_state--;
            }
            if(*it == "and" || *it == "or") // get the logical field
            {
                int temp_state = LOGICAL;
                string y = "yes"; // want to mark 'and', 'or' as yes
                vector<string> checker = ptree.get(get_enum_key(temp_state)); // make sure that we only insert one yes 
                if(checker.size() < 1)
                {
                    ptree[get_enum_key(temp_state)] += y;
                }
            }
            else if(*it == "<" || *it == "<=" || *it == "=" || *it == ">" || *it == ">=")
            {
                int temp_state = RELATIONAL;
                string y = "yes"; // want to mark <, >, <=, >=, = as yes
                vector<string> checker = ptree.get(get_enum_key(temp_state)); // make sure that we only insert one yes 
                if(checker.size() < 1)
                {
                    ptree[get_enum_key(temp_state)] += y;
                }
                
            }
            if(new_state == TABLE || new_state == TABLE_IT || new_state == TABLE_NAME) // special condition for the table
            {
                vector<string> checker = ptree.get(get_enum_key(new_state)); // make sure that we only insert one table name
                if(checker.size() < 1 && *it != "," && *it != " " && *it != "(" && *it != ")") // check the size of the vector
                {
                    ptree[get_enum_key(new_state)] += (*it); // insert the key into the designated area with it's column
                }
                else if(checker.size() == 1 && is_valid_token(*it)) // this means that one of the keywords are missin, either where, values, or fields
                {
                    string cmd = ptree.get(get_enum_key(COMMAND)).front();
                    if(cmd == "make")
                    {
                        throw runtime_error("Expected a fields");
                    }
                    else if(cmd == "select")
                    {
                        throw runtime_error("Expected a where");
                    }
                    else if(cmd == "insert")
                    {
                        throw runtime_error("Expected a values");
                    }
                }
            }
            else if(is_valid_token(*it)) // don't want to add ,'s and ' ''s to the ptree
            {
                if(new_state == ZERO) // this means that a command hasn't been inserted
                {
                    throw runtime_error("Expected a command!");
                }
                else 
                {
                    ptree[get_enum_key(new_state)] += (*it); // insert the key into the designated area with it's column
                }
                
            } 
            if(new_state == FIELDS || new_state == LIST || new_state == LIST_M) // this is for checking if a comma or field/value is missing in the SQL statement
            {
                if(is_valid_token(*it)) // if it is a valid token then it must be a word
                {
                    was_word = true;
                }
                else if(*it == "," && !was_word) // this checks the case where we have last, , first or vice versa for inserting
                {
                    if(new_state == FIELDS || new_state == LIST_M)
                    {
                        throw runtime_error("Expected a field");
                    }
                    else 
                    {
                        throw runtime_error("Expected a value");
                    }
                }
                else if(*it == " ") // this checks for last first and vice versa for inserting, also makes sure that 
                {
                    Queue<string>::Iterator temp = it;
                    temp++;
                    if(was_word && !temp.is_null() && is_valid_token(*temp) && *temp != "from")
                    {   // we had a word and made sure that no matter how many spaces between (age, last,   ) affects whether we need a comma or not
                        comma = true;
                    }
                    else if(!was_word && *temp == "from") // this is for the case of field_name, from
                    {
                        throw runtime_error("Expected a field");
                    }
                }
                else 
                {
                    was_word = false;
                }
            }
        }  
    }
    if(new_state == -1) // we ended on a keyword
    {
        flag = true; 
        if(fields && ptree.get(get_enum_key(LIST_M)).empty() && ptree.get(get_enum_key(COMMAND)).front() == "make")
        {   // when making a table the new_state could be -1 since almost all the first few words are keywords so we have to check
            throw runtime_error("Expected a list of fields!");
        }
    }
    else if(is_success(new_state) && !ptree[get_enum_key(new_state)].empty()) // this means that we got a valid SQL statement and that it actually has valid commands (not empty)
    {
        flag = false;
    }
    // seperate the if statements
    if(ptree.get(get_enum_key(FIELDS)).empty() && ptree.get(get_enum_key(COMMAND)).front() == "select") // this is for when we get just select, we throw this exception
    {
        throw runtime_error("Expected a list of fields!");
    }
    else if(!from && ptree.get(get_enum_key(COMMAND)).front() == "select") // this checks if we are missing the keyword from
    {
        throw runtime_error("Expected a from!");
    }
    else if(ptree.get(get_enum_key(FIELDS)).size() > 1 && ptree.get(get_enum_key(FIELDS)).front() == "*") // this is when the fields are like *, lname, fname, we want to catch that in this function
    {
        throw runtime_error("Fields can not contain \"*\" and other fields!");
    }
    else if(!into && ptree.get(get_enum_key(COMMAND)).front() == "insert") // this checks if we are missing the keyword into
    {
        throw runtime_error("Expected a into!");
    }
    else if(!table && ptree.get(get_enum_key(COMMAND)).front() == "make") // this checks that if we are doing a make table we recieve the table keyword
    {
        throw runtime_error("Expected a table!");
    }
    else if(!ptree.contains(get_enum_key(TABLE)) || ptree.get(get_enum_key(TABLE)).empty()) // this checks if we have recieved a table name, I can use any table enumerated key because they are all "table_name"
    {
        throw runtime_error("Expected a specific table name!");
    }
    else if(!fields && ptree.get(get_enum_key(COMMAND)).front() == "make") // this checks that if we are doing a make table we recieve the fields keyword
    {
        throw runtime_error("Expected a fields!");
    }
    else if(state == WHERE && ptree.get(get_enum_key(new_state)).empty()) // this means that the last state was where and that the conditions are empty
    {
        throw runtime_error("Expected a condition!");
    }
    else if(state == WHERE && ptree.get(get_enum_key(WHERE)).empty() && !ptree.get(get_enum_key(CONDITION)).empty()) // if we get a condition but no where we need to throw this exception
    {
        throw runtime_error("Expected a where!");
    }
    else if(state == VALUES && ptree.get(get_enum_key(LIST)).empty()) // when inserting into a table we need to check if there is a list of values to insert
    {
        throw runtime_error("Expected a list of values!");
    }
    else if(comma) // this means we are missing a comma, which I set to be the lowest priority of exceptions
    {
        throw runtime_error("Expected a comma!");
    }
    return flag; // return flag
}

bool Parser::fail() // determine if the user inputted a failing statement
{
    if(flag) // if flag is true that means we got an invalid SQL statement
    {
        return true;
    }
    return false; // this means that the flag was false, meaning valid SQL statement
}

bool Parser::is_valid_token(const string& token) // this is a helper function that makes sure we only accept valid tokens
{
    if(token != "," && token != " " && token != "\t" && token != "\n" && token != "\0" && token != "?" && token != "." && token != ":" && token != ";" && token != "'" && token != "`" && token != "~" && token != "!" && token != "!" && token != "+" && token != "-" && token != "%" && token != "&" && token != "|")
    {
        return true; // the string token was not equal to any of those above
    }
    return false; // the string was not a valid token
}

MMap<string, string> Parser::parse_tree() // return the mmap ptree
{
    if(!flag) // means we have a valid statement
    {
        return ptree;
    }
    MMap<string, string> notvalid; // if we don't get a valid token we return an empty map
    return(notvalid);
}

void Parser::print_lookup()
{
    MMap<string, string>::Iterator it; // iterate through the ptree
    for(it = ptree.begin(); it != ptree.end(); ++it) // print ptree as a table
    {
        cout << (*it).key << " :" << setw(10) << (*it).value_list << endl; // output both the keyword and the values 
    }
}

bool Parser::is_success(const int& num) // helper function that determines if the row we are in a sucess state
{
    int success = _table[num][0]; // check the 0th column
    if(success != 0) // it is a success if we do not have a 0
    {
        return true;
    }
    return false; // that means we were in a fail state
}
// need to make our own state machine table so that when we get tokens, we can correctly parse them
void Parser::make_table(int _table[][MAX_COLUMNS])
{
    init_table(_table); // initialize the table

    // set up the success states
    mark_fail(_table, 0); // zero
    mark_fail(_table, 1); // command
    mark_fail(_table, 2); // fields
    mark_fail(_table, 3); // from 
    mark_success(_table, 4); // table
    mark_fail(_table, 5); // where
    mark_success(_table, 6); // condition
    mark_fail(_table, 7); // into
    mark_fail(_table, 8); // table for inserting into
    mark_fail(_table, 9); // values
    mark_success(_table, 10); // list of values to insert
    mark_fail(_table, 11); // create
    mark_fail(_table, 12); // table to create
    mark_fail(_table, 13); // fields for the table we create
    mark_success(_table, 14); // list of the fields
    mark_fail(_table, 15); // delete
    mark_success(_table, 16); // table name to delete
}

//Fill all cells of the array with -1
void Parser::init_table(int _table[][MAX_COLUMNS])
{
    for(int i = 0; i < MAX_ROWS; i++)
    {
        for(int j = 0; j < MAX_COLUMNS; j++)
        {
            _table[i][j] = -1;
        }
    }
}

//Mark this state (row) with a 1 (success)
void Parser::mark_success(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 1;
}

//Mark this state (row) with a 0 (fail)
void Parser::mark_fail(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 0;
}
