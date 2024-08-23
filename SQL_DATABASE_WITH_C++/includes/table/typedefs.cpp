#include "../../includes/table/typedefs.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "../bplustree/multimap.h"
#include "../bplustree/map.h"

using namespace std;


TokenType::TokenType() 
{
    type = "undefined";
} // default ctor with default type

TokenType::TokenType(const string& str)
{
    type = str;
} // parameterized ctor

string TokenType::get_type() const 
{
    return type; // return the type
}



Token::Token() 
{
    _token = "undefined";
    type = TokenType("undefined");
} // default ctor with default type

Token::Token(const string& token_str, const TokenType& token_type) // first ctor for Token without setting precedence
{
    _token = token_str;
    type = token_type;
} // parameterized ctor

Token::Token(const string& token_str, const int p, const TokenType& token_type) // first ctor for Token without setting precedence
{
    _token = token_str;
    type = token_type;
    precedence = p;
} // parameterized ctor

string Token::get_token() const 
{
    return _token; // return the token
}

TokenType Token::get_type() const 
{
    return type; // return the type
}

vector<long> Token::eval(Token val, Token field, MMap<string, long> _indices, Map<string, long> data) // this is pretty much the same as select()
{
    string oper = this->get_token(); // obtain the actual string
    string value = val.get_token();
    vector<long> records;
    string field_key = field.get_token(); // Use the string key directly
    // Check if the field_key exists in _indices before proceeding
    if (oper == "=") 
    {
        // Check if the value exists in the MMap for the given field_key
        if (_indices.contains(value)) 
        {
            records = _indices.get(value);
        }
    } 
    else if (oper == "<=")
    { // if we are searching for less than or equal to
        for (auto it = _indices.begin(); it != _indices.end() && (*it).key <= value; ++it) // using the beginning and the condition that the key is less than or equal to value
        {// iterate through until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec);// push back into record
            }
        }
    } 
    else if (oper == ">=") 
    {// if we are searching for greater than or equal to
        for (auto it = _indices.lower_bound(value); it != _indices.end(); ++it) 
        { // iterate beginning at lower_bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == ">") 
    { // if we are searching for greater than
        for (auto it = _indices.upper_bound(value); it != _indices.end(); ++it) 
        { // iterate beginning at the upper bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == "<") 
    { // if we are searching for greater than
        for (auto it = _indices.begin(); it != _indices.end() && (*it).key < value; ++it)  // use the condition key < value
        { // iterate until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    }
    else  // we know that when we get here that the operator was neither =, <, >, <=, or >=
    {
        throw runtime_error("Invalid relational operand!");
    }

    return records;
}

int Token::get_precedence() // returns the precedence of the token
{
    return precedence;
}

 
TokenStr::TokenStr() : Token() {} // call base class default ctor

TokenStr::TokenStr(const string& str) : Token(str, TokenType("string")) {} // call base class parameterized ctor



Relational::Relational() : Token() {} // call base class default ctor

Relational::Relational(const string& str) : Token(str, TokenType("relational")) {} // call base class parameterized ctor

Relational::Relational(const string& str, const int& pre) : Token(str, pre, TokenType("relational")) {} // call second ctor with a precedence

vector<long> Relational::eval(Token val, Token field, MMap<string, long> _indices, Map<string, long> data) // only a relational token will evaluate anything
{
    string oper = this->get_token(); // obtain the actual string
    string value = val.get_token();
    vector<long> records;
    string field_key = field.get_token(); // Use the string key directly
    // Check if the field_key exists in _indices before proceeding
    if (oper == "=") 
    {
        // Check if the value exists in the MMap for the given field_key
        if (_indices.contains(value)) 
        {
            records = _indices.get(value);
        }
    } 
    else if (oper == "<=")
    { // if we are searching for less than or equal to
        for (auto it = _indices.begin(); it != _indices.end() && (*it).key <= value; ++it) // using the beginning and the condition that the key is less than or equal to value
        {// iterate through until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec);// push back into record
            }
        }
    } 
    else if (oper == ">=") 
    {// if we are searching for greater than or equal to
        for (auto it = _indices.lower_bound(value); it != _indices.end(); ++it) 
        { // iterate beginning at lower_bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == ">") 
    { // if we are searching for greater than
        for (auto it = _indices.upper_bound(value); it != _indices.end(); ++it) 
        { // iterate beginning at the upper bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == "<") 
    { // if we are searching for greater than
        for (auto it = _indices.begin(); it != _indices.end() && (*it).key < value; ++it)  // use the condition key < value
        { // iterate until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    }
    else  // we know that when we get here that the operator was neither =, <, >, <=, or >=
    {
        throw runtime_error("Invalid relational operand!");
    }

    return records;
}


Logical::Logical() : Token() {} // default ctor

Logical::Logical(const string& str) : Token(str, TokenType("logical")) {} // first ctor without precedence

Logical::Logical(const string& str, const int& pre) : Token(str, pre, TokenType("logical")) {} // second ctor including precednece


RParent::RParent() : Token() {} // default ctor

RParent::RParent(const string& str) : Token(str, TokenType("rparent")) {} // first ctor without precedence

RParent::RParent(const string& str, const int& pre) : Token(str, pre, TokenType("rparent")) {} // second ctor including precedence

LParent::LParent() : Token() {} // default ctor

LParent::LParent(const string& str) : Token(str, TokenType("lparent")) {} // first ctor without precedence

LParent::LParent(const string& str, const int& pre) : Token(str, pre, TokenType("lparent")) {} // second ctor including precedence