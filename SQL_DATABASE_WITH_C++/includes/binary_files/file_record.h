#ifndef FILE_RECORD_H
#define FILE_RECORD_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <exception>
#include <iomanip>
#include <vector>
using namespace std;

//utility functions
// bool file_exists(const char filename[]);

// void open_fileRW(fstream& f, const char filename[]);
// void open_fileW(fstream& f, const char filename[]);
// void confuse_me();
// void simple_test();
//----------------------------------------------------------------------------
//                      C L A S S   F I L E R E C O R D
//----------------------------------------------------------------------------
static const int FIELD = 100; // remember to change when more field types are necessary

class FileRecord{
public:
    //when you construct a Record, it's either empty or it
    //  contains a word
    FileRecord();

    FileRecord(char str[]);
    // template <typename T>
    FileRecord(vector<string> s);

    vector<string> get_fields() const;

    long write(fstream &outs);

    long read(fstream &ins, long recno2);
    
    friend ostream& operator<<(ostream& outs, const FileRecord& r)
    {
        for(int i = 0; i < FIELD; i++)
        {
            int len = strlen(r._record[i]);
            if(strlen(r._record[i]) > 0)
            {
                outs<< setw(24) << r._record[i];
            }
        }
        return outs;
    }
    
    static const int MAX = 100;
    int recno;
    char _record[FIELD][MAX + 1];
};

#endif