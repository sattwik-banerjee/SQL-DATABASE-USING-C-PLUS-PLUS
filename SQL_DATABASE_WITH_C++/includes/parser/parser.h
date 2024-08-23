#ifndef PARSER_H
#define PARSER_H

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

class Parser 
{
public : 
    Parser();

    Parser(char* s);

    void set_string(char* s);

    bool is_keyword(string& str);

    string get_enum_key(const int& type);

    void build_keymap();

    bool get_parse_tree();

    bool fail();

    bool is_valid_token(const string& token);

    MMap<string, string> parse_tree();

    void print_lookup();

    bool is_success(const int& num);

    void make_table(int _table[][MAX_COLUMNS]);

    //Fill all cells of the array with -1
    void init_table(int _table[][MAX_COLUMNS]);

    //Mark this state (row) with a 1 (success)
    void mark_success(int _table[][MAX_COLUMNS], int state);

    //Mark this state (row) with a 0 (fail)
    void mark_fail(int _table[][MAX_COLUMNS], int state);

private : 
    char buffer_str[300];
    MMap<string, string> ptree; // multi map that is the parser itself
    Map<string, int> keywords; // stores keywords
    STokenizer state; //
    bool flag;
    Queue<string> tokens; // holds all the tokens
    enum keys{ZERO, COMMAND, FIELDS, FROM, TABLE, WHERE, CONDITION, INTO, TABLE_IT, VALUES, LIST, CREATE, TABLE_NAME, FIELDS_M, LIST_M, DEL, TABLE_D, LOGICAL, RELATIONAL}; // enumerators
    // enum inserting{ZERO, COMMAND, INTO, TABLE_IT, VALUES, LIST};
    int _table[MAX_ROWS][MAX_COLUMNS];
};

#endif