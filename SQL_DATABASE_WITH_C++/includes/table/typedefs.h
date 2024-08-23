#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "../bplustree/multimap.h"
#include "../bplustree/map.h"

using namespace std;

class TokenType {
public: 
    TokenType();

    TokenType(const string& str);

    string get_type() const;

private:
    string type;
};

class Token {
public: 
    Token(); // default ctor with default type

    Token(const string& token_str, const TokenType& token_type); // first ctor for Token without setting precedence
     // parameterized ctor

    Token(const string& token_str, const int p, const TokenType& token_type); // first ctor for Token without setting precedence
    

    friend ostream& operator<< (ostream& outs, const Token& token)
    {
        outs << "Here is the token string: " << token._token << " and its type: " << token.type.get_type() << endl;
        return outs;
    }

    string get_token() const;

    TokenType get_type() const;
    vector<long> eval(Token val, Token field, MMap<string, long> _indices, Map<string, long> data); // this is pretty much the same as select()


    int get_precedence();

private : 
    string _token;
    TokenType type;
    int precedence;

};

class TokenStr : public Token {
public: 
    TokenStr(); // call base class default ctor

    TokenStr(const string& str); // call base class parameterized ctor

};

class Relational : public Token {
public: 
    Relational(); // call base class default ctor

    Relational(const string& str); // call base class parameterized ctor

    Relational(const string& str, const int& pre); // call second ctor with a precedence

    vector<long> eval(Token val, Token field, MMap<string, long> _indices, Map<string, long> data); // only a relational token will evaluate anything
};

class Logical : public Token {
public : 
    Logical(); // default ctor

    Logical(const string& str); // first ctor without precedence

    Logical(const string& str, const int& pre); // second ctor including precednece
};

class RParent : public Token { // this will always be the ')'
public : 
    RParent(); // default ctor

    RParent(const string& str); // first ctor without precedence

     RParent(const string& str, const int& pre); // second ctor including precedence
};

class LParent : public Token { // this will always be the '('
public : 
    LParent(); // default ctor

    LParent(const string& str); // first ctor without precedence

    LParent(const string& str, const int& pre); // second ctor including precedence
};

#endif
