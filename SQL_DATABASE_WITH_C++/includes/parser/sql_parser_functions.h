#ifndef SQL_PARSER_FUNCTIONS_H
#define SQL_PARSER_FUNCTIONS_H

#include "../table/table.h"
#include "../parser/parser.h"
#include <iostream>

using namespace std;

    // void sql()
    // {
    //     string outpt;
    //     cout << "Enter an sql statement" << endl; // get the SQL statement
    //     cin >> outpt;
    //     do 
    //     {
    //         char s[300];
    //         // create parser using c_string
    //         strcpy(s, str.c_str());
    //         Parser p(s);
    //         // get the ptree
    //         mmap_ss map;   
    //         map = p.parse_tree();
    //         if(map.empty())
    //         {
    //             cout << "Invalid SQL statement" << endl;
    //         }
    //         else 
    //         {
    //             map.print_lookup(); // print out the ptree
    //         }
    //         // now I have to turn the ptree into an insertion
    //         if(map["COMMAND"] == "select")
    //         {
    //             string table_name = map["TABLE"];
    //             Table t(table_name); // get table from pre-existing table
    //             cout << t.select(map["FIELDS"]) << endl;
    //         }
    //         else if(map["COMMAND"] == "create" || map["COMMAND"] == "make") // we are creating a new table
    //         {
    //             string table_name = map["TABLE_NAME"];
    //             Table t(table_name, map["FIELDS_M"]);
    //             cout << t << endl;
    //         }
    //         else if(map["COMMAND"] == "insert")
    //         {
    //             string table_name = map["TABLE_IT"];
    //             Table t(table_name);
    //             t.insert_into(map["LIST"]);
    //             cout << t << endl;
    //         }
    //         cout << "Enter an sql statement" << endl; // get the SQL statement
    //         cin >> outpt;
    //     }
    // }

#endif