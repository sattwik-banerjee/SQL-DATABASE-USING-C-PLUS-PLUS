#ifndef SQL_H
#define SQL_H

#include "../table/table.h"
#include "../parser/parser.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

class SQL
{
    public : 

        SQL();

        Table command(const string& inp);

        vector<long> select_recnos();

        void make_table_tree();

        void print_tablenames();

        Table get_table();

        vector<string> verify_fields(vector<string> fds);

        bool vec_has(const string& item, const Map<string, long>& vec);

        bool vec_contains_all(const vector<string>& vec);

        void batch();

    private : 
        string table_name; // to keep track of the current table name
        vector<long> recnos; // to store all the record numbes
        Parser p; // parser object for parsing
        Map<string, Table> tables; // map of all created tbales
        string txtfle; // the name of the text file where we write into all the table names for exising tables
};

#endif