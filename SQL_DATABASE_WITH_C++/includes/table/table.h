#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <initializer_list>
#include "../bplustree/multimap.h"
#include "../binary_files/utilities.h"
#include "../binary_files/file_record.h"
#include "../bplustree/map.h"
#include "typedefs.h"
#include "../queue/MyQueue.h"
#include <time.h>
#include "resultset.h"
#include "../stack/MyStack.h"
#include "actual_typedefs.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

static int serial = -1;
static string fn;

class Table {
public:

    Table();

    Table(string fname, vectorstr fields); 

    Table(const string& table);

    Table& operator=(const Table& RHS);

    void insert_into(vectorstr row);

    void insert_record(FileRecord rec);

    Table select_all();

    Table select_all(vector<string> fields);

    Table select(const vector<string>& field, const string& field_type, const string& oper, const string& value);

    Table select(const vector<string>& fields, const Queue<Token*>& prefix);

    bool vec_has(const long& item, const vector<long>& vec);

    vector<long> put_recs_in_order(const vector<long>& recs);

    Table select(const vector<string>& fields, const vector<string>& infix);

    Table sy(const vector<string>& fields, const vector<Token*>& tks);

    Table table_from_vector(const vector<string>& fields, const vector<long>& records);

    vector<long> select_recnos();

    void close_file() const;

    int get_record_count() const;

    string get_tablename() const;

    friend ostream& operator<<(ostream& outs, const Table& t)
    {
        t.close_file(); // ensure that the file is closed
        int record = t.get_record_count();
        if(t.get_tablename() != "" && record > 0) // make sure we have a valid table and that the table is not empty
        {
            FileRecord r2;
            fstream f; // local file
            cout << "Table name : " << t.tablename << ", records : " << record << "\n" << endl; // begin printing out all the table information
            cout << setw(26) << "record"; // formatting
            for (const auto& it : t.field_names) { // iterate through the data (field_names)
                cout << setw(24) << it; // output all the field names
            }
            cout << "\n" << endl; // formatting
            open_fileRW(f, t.filename.c_str()); // open bin file for reading
            int i = 0;
            long bytes = r2.read(f, i);
            while (bytes > 0) { // print out each record 
                cout << setw(26) << i << setw(26) << r2 << endl; // output record number and record itself
                i++; // get next record
                bytes = r2.read(f, i);
            }
        }
        return outs;
    }

    void print_mmap();

    Map<string, long> get_data();

    vector<string> get_fields();

    void clear_table();

    int num_fields();

    int rec_count();

private: 
    string tablename; // to store table name
    vector<MMap<string, long>> _indices; // mmap for searching 
    Map<string, long> data; // map to hold the field
    vector<string> field_names; // vector to hold the field_names
    static const int MAX = 100;
    string filename; // filename to make opening easier
    int record_count; // the record count
    vector<long> selected_records; // the records selected when doing a select operation
};

#endif