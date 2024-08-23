#include "table.h"
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

using namespace std;

Table::Table()
{
    record_count = 0;
}

Table::Table(string fname, vectorstr fields)
{
    record_count = 0;
    tablename = fname;
    field_names = fields;
    // set up all file member variables
    string txtfile = fname + ".txt";
    ofstream textf(txtfile); // create and open textfile
    for (const auto& it : fields) { // store all the fields into the text file
        textf << it << endl;
    }
    textf.close(); // close text file
    // get file name and create the mmaps (_indices)
    filename = fname + ".bin";
    int index = 0;
    for (const auto& it : fields) {
        data.insert(it, index); // inserts the field into the map
        index++;
    }
    _indices.resize(index); // resize the _indices vector
    fstream file;
    open_fileW(file, filename.c_str()); // open the file for writing into
}   

Table::Table(const string& table) // table from pre-existing table
{
    tablename = table; // set all the name variables and get ready to read from the txt file and bin file
    filename = table + ".bin";
    string txtname = table + ".txt";
    ifstream txtf(txtname); // open txt file for reading
    int index = 0;
    data.clear(); // make sure that the vectors of map and mmap are all empty
    field_names.clear();
    string fds; // string to read the field of pre-existing field into this tables txt file
    while (getline(txtf, fds)) {
        data.insert(fds, index); // insert into map
        field_names.push_back(fds); // insert into mmap
        fds.clear(); // reset fds
        index++;
    }
    txtf.close(); // close txt file
    
    _indices.resize(index);
    // set the record count and get ready to read from bin into this bin
    record_count = 0;
    long temp_recno = 0;
    FileRecord r2; // record for reading
    fstream file;
    open_fileRW(file, filename.c_str());
    int i = 0;
    long bytes = r2.read(file, i);
    while (bytes > 0) 
    { 
        vector<string> filds = r2.get_fields(); // call helper function to get the fields from the record
        if(filds.size() == index) // ensure that we have not read any garbage from the binary file
        {
            for (int j = 0; j < filds.size(); j++) 
            { 
                _indices[j][filds[j]] += temp_recno; // insert all the values into the appropriate _indices map
            }
            // incrememnt all sizes, and index
            temp_recno++;
            record_count++;
            i++;
            bytes = r2.read(file, i);
        }
        else // there is garbage so we stop reading
        {
            bytes = 0;
        }
    }
    file.close(); // close file
}

Table& Table::operator=(const Table& RHS)
{
    if(tablename != RHS.tablename)
    {
        tablename = RHS.tablename;
        filename = tablename + ".bin";
        string txtname = tablename + ".txt";
        ifstream txtf(txtname); // open txt file for reading
        int index = 0;
        data.clear(); // make sure that the vectors of map and mmap are all empty
        field_names.clear();
        string fds; // string to read the field of pre-existing field into this tables txt file
        while (getline(txtf, fds)) {
            data.insert(fds, index); // insert into map
            field_names.push_back(fds); // insert into mmap
            fds.clear(); // reset fds
            index++;
        }
        txtf.close(); // close txt file
        
        _indices.resize(index);
        // set the record count and get ready to read from bin into this bin
        record_count = RHS.get_record_count();
        long temp_recno = 0;
        FileRecord r2; // record for reading
        fstream file;
        open_fileRW(file, filename.c_str());
        int i = 0;
        long bytes = r2.read(file, i);
        while (bytes > 0) 
        { 
            vector<string> filds = r2.get_fields(); // use helper function to get the record in vector form
            if(filds.size() == index) // make sure we don't read any garbage from the binary file
            {
                for (int j = 0; j < filds.size(); j++) // insert all the values into the appropriate maps
                { 
                    _indices[j][filds[j]] += temp_recno;
                }
                // incrememnt all sizes, and index
                temp_recno++;
                i++;
                bytes = r2.read(file, i);
            }
            else 
            {
                bytes = 0;
            }
        }
        file.close(); // close file
    }
    return *this;
}

void Table::insert_into(vectorstr row) 
{
    fstream file; // open file for writing
    open_fileRW(file, filename.c_str());
    FileRecord inserting(row); // create temp record var
    insert_record(inserting); // call the helper function to insert the record
}

void Table::insert_record(FileRecord rec) // for when we have a record already and want to insert that
{
    fstream file; // open file for writing
    open_fileRW(file, filename.c_str());
    long recno = rec.write(file); // skip step of making temp record, just read it into file
    record_count++; // increement the record_cout
    vector<string> fields = rec.get_fields(); // call helper function that gets all the fields as a vector
    for (int i = 0; i < _indices.size(); i++) 
    { // read the data into the mmap _indices
        if(_indices[i].get_key_count() == 0) // take care of potential inizialization issues by checking the key_count
        {
            _indices[i].set_children(0);  
            _indices[i].set_data(0); 
        }
        _indices[i][fields[i]] += recno; // add all the information into the respective _indices map
    }
}

Table Table::select_all() 
{
    vector<long> recs; // this will hold all the records from this table which is 0 -> record_count
    for(long i = 0; i < record_count; i++)
    {
        recs.push_back(i); // push every reccord into recs
    }
    return(table_from_vector(field_names, recs)); // create table from this vector
}

Table Table::select_all(vector<string> fields) // helper function for sql, takes a fields argument
{
    vector<long> recs; // this will hold all the records from this table which is 0 -> record_count
    for(long i = 0; i < record_count; i++)
    {
        recs.push_back(i); // push every reccord into recs
    }
    return(table_from_vector(fields, recs)); // create table from this vector
}

Table Table::select(const vector<string>& field, const string& field_type, const string& oper, const string& value) 
{
    vector<long> records; // record vectors
    close_file(); // ensure the file is closed
    long index; // index to store the index of the map we wil be searching
    if(data.contains(field_type)) // make sure 
    {
        index = data.get(field_type); // index of the mmap to search through
    }
    else // we have a columne that doesn't exist, so we throw an error message
    {
        throw runtime_error("Invalid field!");
    }
    if (oper == "=") 
    { // if we are searching for equals
        if (_indices[index].contains(value)) // use contains to search for the value
        {
            records = _indices[index].get(value); // set records equal to the value list gotten from the .get(value)
        }
    } 
    else if (oper == "<=") 
    { // if we are searching for less than or equal to
        for (auto it = _indices[index].begin(); it != _indices[index].end() && (*it).key <= value; ++it) // using the beginning and the condition that the key is less than or equal to value
        { // iterate through until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == ">=") 
    { // if we are searching for greater than or equal to
        for (auto it = _indices[index].lower_bound(value); it != _indices[index].end(); ++it) 
        { // iterate beginning at lower_bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == ">") 
    { // if we are searching for greater than
        for (auto it = _indices[index].upper_bound(value); it != _indices[index].end(); ++it) 
        { // iterate beginning at the upper bound
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    } 
    else if (oper == "<") 
    { // if we are searching for lt
        for (auto it = _indices[index].begin(); it != _indices[index].end() && (*it).key < value; ++it) // use the condition key < value
        { // iterate until for condition is met
            for (const auto& rec : (*it).value_list) 
            {
                records.push_back(rec); // push back into record
            }
        }
    }
    else // we know that when we get here that the operator was neither =, <, >, <=, or >=
    {
        throw runtime_error("Invalid relational operand!");
    }
    // create a new table and store all the records into this new table
    return(table_from_vector(field, records));   
}

Table Table::select(const vector<string>& fields, const Queue<Token*>& prefix) // this is pretty much the rpn() function since it performs rpn
{
    close_file(); // ensure file has been closed
    // Iterate through Queue to see if we push onto stack or do rpn
    Stack<Token*> strings;
    Queue<Token*>::Iterator it;
    ResultSet res;
    if(prefix.empty()) // don't do anything if the queue is empty
    {
        vector<long> emptyqueue;
        return(table_from_vector(fields, emptyqueue));
    }
    for (it = prefix.begin(); it != nullptr; ++it) // using iterator either push onto stack of call rpn
    {
        Token* token = *it;
        // Check the type of the token
        if (token->get_type().get_type() == "relational") 
        { // get the two values for the operation
            if(strings.size() > 2) // not enough strings to perform the evaluation
            {
                throw runtime_error("Expected another logical operator, \"and\" or \"or\"");
            }
            else if(strings.size() <= 1) // not enough strings to perform the evaluation
            {
                throw runtime_error("Expected a complete conditional statement that looks like this : (x = y)!");
            }
            Token* value = strings.pop(); // get the value
            Token* field = strings.pop(); // get the field
            if(data.contains(field->get_token())) // ensure the field actually exists
            {
                long index = data.get(field->get_token()); // find the mmap to use
                vector<long> temp; // temp vector of longs to store the record numbers that satisfy the condition
                temp = token->eval(*value, *field, _indices[index], data); // add the resulting vector of records into result set
                res.add(temp); // add vector of records onto result set
            }
            else // we recieved an invalid field type
            {
                throw runtime_error("The field name given is not in this table!");
            }
        } 
        else if (token->get_type().get_type() == "string") 
        {
            strings.push(token); // push all string tokens onto the string stack
        } 
        else // must be a logical
        {
            if(res.size() < 2) // if the result set has less than 2 vectors on it by the time it gets to the logical token, that means that a condition (x = y) is missing
            {
                throw runtime_error("One more conditional statement (x = y) was expected");
            }
            vector<long> logres; 
            vector<long> finres;
            // since it's logical must determine if it's and or or
            if(token->get_token() == "and")
            {
                int count = 0;
                // must empty out the result set and then return only the record number in that are duplicates vectors (for logical there will be > 2 vectors)
                while(!res.empty() && count < 2) // only want to pop res twice because each logical can only evaluate two sets
                {
                    vector<long> hold = res.pop();
                    if(!logres.empty()) // if it's not empty then we have already popped once so we can check for identical values
                    {
                        for(const auto& j : hold) // bot vectors may not be the same size, must check if each value is contained within the other
                        {
                            for(const auto& i : logres) // pretty much a double for loop
                            {
                                if(j == i)
                                {
                                    finres.push_back(j); // found a match so put that into the final result vect
                                }
                            }
                        }
                    }
                    else 
                    {
                        logres = hold; // it is empty so nothing has been popped yet
                    }
                    count++;
                } // after we have finished searching the result set we push finres back into the vector
                res.add(finres);
            }
            else if(token->get_token() == "or")
            {
                int count = 0;
                // must empty the result set and get rid of all duplicates in the vectors and return that
                while(!res.empty() && count < 2) // only want to pop res twice because each logical can only evaluate two sets
                {
                    vector<long> hold = res.pop();
                    if(!logres.empty()) // evaluating logical condition
                    { // need to combine the vectors in the result set and make sure that there are no duplicates in the final vector pushed into the set
                        for(const auto& k : hold) // add the elements that are non duplicates to the finres vector
                        {
                            if(!vec_has(k, logres)) // if it is not a duplicate
                            {
                                logres.push_back(k);
                            }
                        }
                    }
                    else 
                    {
                        logres = hold; // only one item has been popped
                    }
                    count++;
                }
                res.add(logres); // we have now found the records that satisfy the logical statement, push that into result set
            }
        }
    }
    if(res.size() > 1) // if the result set has more than 1 vector in it by the time it gets to the logical token, that means that a logical token is missing
    {
        throw runtime_error("Expected another logical operator, \"and\" or \"or\"");
    }
    return(table_from_vector(fields, res.pop())); // res should end up with a size of one and we will create a table from that vector
}

bool Table::vec_has(const long& item, const vector<long>& vec) // helper function for the logical token (the or token)
{
    for(const auto& it : vec) // iterate through vector
    {
        if (item == it) // determine if this record value is already in the record
        {
            return true; // return true if vector contains this value item
        }   
    }
    return false; // this means the vector does not contain the item
}

vector<long> Table::put_recs_in_order(const vector<long>& recs) // helper function for outputting and inserting records
{
    //Use the sort function to sort the vector in ascending order
    vector<long> records = recs;
    sort(records.begin(), records.end()); // use the already existing sort to sort the vector
    return records;
}

Table Table::select(const vector<string>& fields, const vector<string>& infix) // the set up to use shunting yard then rpn
{
    close_file(); // ensure that the file is closed
    // must make every string in infix be a token
    vector<Token*> tks; // to store the newly tokenized infix
    // after performing the tokenization check to make sure that there are no mismatching parenthesis
    for(const auto& it : infix) // iterate through the infix vector
    {
        if(it == "and") // if it is an and operator
        {
            Logical* i = new Logical(it, 16); // make new logical type with 14 precedence
            tks.push_back(i); 
            i = nullptr;
        }
        else if(it == "or") // if it is an or operator
        {
            Logical* i = new Logical(it, 15);  // make new logical with 15 precedence
            tks.push_back(i);
            i = nullptr;
        }
        else if (it == "<=" || it == ">=" || it == "<" || it == ">" || it == "=") // if it is >, <, <=, >= or =
        {
            Relational* i = new Relational(it, 18);  // make new relational with 9 precedence
            tks.push_back(i);
            i = nullptr;
        }
        else if(it == "(") // if it is a (
        {
            LParent* i = new LParent(it, 100); // make new left parent type
            tks.push_back(i);
            i = nullptr;
        }
        else if(it == ")") // if it is a )
        {
            RParent* i = new RParent(it, 100); // make a new right parent type
            tks.push_back(i);
            i = nullptr;
        }
        else if(it == "!=")
        {
            throw runtime_error("\"!=\" Is a invalid relational operand!");
        }
        else // it is a token str
        {
            TokenStr* i = new TokenStr(it); // both ( and ) and strings have no precedence
            tks.push_back(i);
            i = nullptr;
        }
    }
    // after having successfully tokenized everything, call shunting yard
    return(sy(fields, tks));
}

Table Table::sy(const vector<string>& fields, const vector<Token*>& tks) // this function will execute the shunting yard algorithm
{   // will create a postfix Queue of Token* and then call the select that takes a Queue
    Queue<Token*> outpt; // store the strings into here
    Stack<Token*> ops; // put all operators(logical, relational, parent) into here

    for(const auto& it : tks) // iterate through the infix tokens
    {
        if(it->get_type().get_type() == "string") // pushed string token onto the output queue
        {
            outpt.push(it); // goes to output queue
        }
        else // any other type will be pushed onto the ops stack
        {
            // must make sure that if Queue is not empty, the operator at the top has a lower precedence than the one we're about th push
            if(ops.empty())
            {
                ops.push(it);
            }
            else if(it->get_type().get_type() == "rparent") // we are at a ) so we pop everything until we find the lparent
            {
                while(!ops.empty() && ops.top()->get_type().get_type() != "lparent") // while condition set also make sure case of mismathing parenthesis
                {
                    outpt.push(ops.pop()); // push everything from stack to queue
                }
                if(!ops.empty()) // if there is an lparent
                {
                    ops.pop(); // pop the lparent too
                }
                else // there is a missing lparenthesis
                {
                    throw runtime_error("Missing left parenthesis");
                }
            }   
            else // we must check to see if the top has a greater precedence than the one we're about to put onto the stack
            {
                while(!ops.empty() && ops.top()->get_type().get_type() != "lparent" && ops.top()->get_precedence() >= it->get_precedence())
                {
                    outpt.push(ops.pop()); // push the operator with the smaller precedence into the output queue
                }
                ops.push(it); // push it onto the stack anyways
            }
        }
    } // after having done this process we should end up with a postfix Queue that we will then call select and have a table :)
    // after doing the shunting yard must check to see if the operation stack is empty, if not empty it into the output Queue
    while(!ops.empty())
    {
        if(ops.top()->get_type().get_type() == "lparent") // in the case we have mismtching parenthesis, this means there is a missing right parenthesis
        {
            throw runtime_error("Missing right parenthesis");
        }
        else // ensure no loose parenthesis are being put into queue
        {
            outpt.push(ops.pop()); // push remaining operators into output queue   
        }
    }
    return(select(fields, outpt)); // this will perform rpn on the outpt queue and return a table created with the vector of records
}

Table Table::table_from_vector(const vector<string>& fields, const vector<long>& records)
{
    fstream file;
    vector<long> recs;
    // put records in order
    // initialize random seed
    serial++; // increment the serial number
    if(records.empty()) // make sure that there are actually some records to create a table with
    {
        Table none(tablename + to_string(serial), fields);
        selected_records = recs;
        return none;
    }
    else 
    {
        recs = put_recs_in_order(records); // make sure that the records are in order before reading them into the file
        // increment serial
        selected_records = recs; // set all member variables to the arguments
        field_names = fields;
        Table tempor(tablename + to_string(serial), fields); // create new temp table
        FileRecord r2;
        open_fileRW(file, filename.c_str()); // open file for reading
        while (!recs.empty()) { // the golden while loop once again
            long i = recs.front();
            recs.erase(recs.begin());
            r2.read(file, i);
            FileRecord temp;
            for (int k = 0; k < field_names.size(); k++) { // restructuring the data held in the records
                long old_index = data.get(field_names[k]); // get old index (aka old value)
                long new_index = tempor.data.get(field_names[k]); // get the new index (where to store)
                strcpy(temp._record[new_index], r2._record[old_index]); // copy old index into new index
                temp.recno = r2.recno; // copy the record number 
            }
            tempor.insert_record(temp); // insert newly structured record into the new table tempor
        }

        file.close(); // close file
        return tempor;
    }
}

vector<long> Table::select_recnos() // function that returns the selected records
{
    return selected_records;
}

void Table::close_file() const // function that ensures that the file is closed
{
    fstream file(filename);
    if (file.is_open()) // check if the file is open
    {
        file.close(); // close it
    }
}

int Table::get_record_count() const // helper function for printing out a table
{
    return record_count;
}

string Table::get_tablename() const // helper function for the sql class
{
    return tablename;
}

void Table::print_mmap() // for debugging
{
    for(const auto& it : _indices) // print out the maps within _indices
    {
        cout << it << endl;
    }
}

Map<string, long> Table::get_data() // helper function for sql, for veryfying the fields
{
    return data;
}

vector<string> Table::get_fields() // helper function that returns the fields
{
    return field_names;
}

void Table::clear_table()
{   // clear the entire table and all it's member variables
    tablename = "";
    _indices.clear();
    data.clear();
    field_names.clear();
    record_count = 0;
    selected_records.clear();
    filename = "";
}

int Table::num_fields() // helper for the sql class
{
    return(field_names.size());
}

int Table::rec_count() // returns the record number, not for the output function
{
    return record_count;
}