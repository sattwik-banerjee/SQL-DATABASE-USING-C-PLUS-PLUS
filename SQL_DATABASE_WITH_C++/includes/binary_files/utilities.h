#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <exception>

using namespace std;


bool file_exists(const char filename[]);

void open_fileRW(fstream& f, const char filename[]);

void open_fileW(fstream& f, const char filename[]);

#endif