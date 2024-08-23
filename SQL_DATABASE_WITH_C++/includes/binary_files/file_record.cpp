#include "../binary_files/file_record.h"
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

//when you construct a Record, it's either empty or it
//  contains a word
FileRecord::FileRecord()
{
    for(int i = 0; i < FIELD; i++)
    {
        for(int j = 0; j < MAX + 1; j++)
        {
            _record[i][j] = '\0';
        }
    }
    recno = -1;
}

FileRecord::FileRecord(char str[])
{
    strncpy(_record[recno], str, MAX);
}
// template <typename T>
FileRecord::FileRecord(vector<string> s)
{
    int index = 0;
    string str;
    for (const auto& it : s) 
    {
        str += it;
        strncpy(_record[index], str.c_str(), MAX);
        _record[index][MAX] = '\0'; // Null-terminate the string  
        index++;
        str = "";  
    }
    for(int i = index; i < FIELD; i++)
    {
        _record[i][0] = '\0';
    }
}

vector<string> FileRecord::get_fields() const 
{
    vector<string> fields;
    for (int i = 0; i < FIELD; i++) {
        if (_record[i][0] != '\0') {
            fields.push_back(string(_record[i]));
        }
    }
    return fields;
}

long FileRecord::write(fstream &outs)
{
    //r.write(f); //take the Record r and write it into file f
    //  and return the record number of this Record

    //write to the end of the file.
    outs.seekg(0, outs.end);

    long pos = outs.tellp();    //retrieves the current position of the
    //                          //      file pointer
    for(int i = 0; i < FIELD; i++)
    {
        outs.write(_record[i], sizeof(_record[i]));
    }
    // outs.write(_record[recno], sizeof(_record[recno]));
    long sd = sizeof(_record);
    recno = pos / sizeof(_record);
    return recno;
}

long FileRecord::read(fstream &ins, long recno2)
{
    //returns the number of bytes read.
    recno = recno2;
    long pos = recno * sizeof(_record);
    ins.seekg(pos, ios_base::beg);
    //ins.read(&_record[0], sizeof(_record))
    for(int i = 0; i < FIELD; i++)
    {
        ins.read(_record[i], sizeof(_record[i]));
    }
    // ins.read(_record[recno], sizeof(_record[recno]));
    //don't you want to mark the end of  the cstring with null?
    //_record[] => [zero'\0'trash trash trash trash]
    //don't need the null character, but for those of us with OCD and PTSD:
    // _record[recno][MAX] = '\0';
    return ins.gcount();
}