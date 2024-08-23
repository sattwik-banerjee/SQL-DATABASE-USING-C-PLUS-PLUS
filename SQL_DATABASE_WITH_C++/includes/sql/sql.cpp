#include "../sql/sql.h"
#include "../table/table.h"
#include "../parser/parser.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

SQL::SQL()
{
    txtfle = "tables.txt"; // create the table txt file
    make_table_tree(); // create / initalize the table map
}

Table SQL::command(const string& inp)
{
    try
    {
        char s[300]; // c_string that will hold the input string
        strcpy(s, inp.c_str()); // convert the inputted command into a c_string in order for the parser to break down the command
        Parser parser(s); // create a parser object that has successfully broken down the command if it is a valid SQL statement
        if(!parser.fail()) // if we get a valid sql statement we will perform the command
        {
            mmap_ss ptree;
            ptree = parser.parse_tree(); // get the ptree with the commands, table name, fields, values, etc.
            // determine the type of command being set
            vector<string> cmd = ptree.get("command");
            vector<string> tblnm = ptree.get("table_name"); // get the table name
            table_name = tblnm.front(); // with every command that is valid there will always be a table name
            if(cmd.size() == 1 && cmd.front() == "select") // we have the select command
            {
                if(tables.contains(table_name)) // makes sure that we are recieving a table name that refers to an existing table
                {
                    if(tables.get(table_name).rec_count() == 0) // this means that there is no records or items inserted in the table yet
                    {
                        throw runtime_error("Can not select from an empty table"); // so we throw a message saying that we can not select anything from an empty table
                    }
                    else 
                    {
                        vector<string> fds = ptree.get("fields"); // obtain the fields given from the SQL statement
                        vector<string> cond = ptree.get("condition"); // also get the conditions we will be checking
                        // check to make sure we got a complete condition by seeing if the rear of the vector is "and", "or", or if it is a relational operator (even if it is illegal just for the sense in checking)
                        if(!cond.empty()) // ensure that there is a condition
                        {   // this means that we have an incomplete condition and we need to throw an exception
                            string check_end = cond.back();
                            if(check_end == "and" || check_end == "or" || check_end == "<" || check_end == ">" || check_end == "<=" || check_end == ">=" || check_end == "=" || check_end == "!=")
                            {
                                throw runtime_error("Can not evaluate an incomplete expression!");
                            }
                            else if(cond.size() < 3) // this means that we did not recieve enough condition tokens
                            {
                                throw runtime_error("Can not evaluate an incomplete condition!");
                            }
                        }
                        vector<string> where = ptree.get("where"); // this vector is for detemining if we are missing a condition when we are expecting one 
                        Table selected; // create a table "selected" in order to store the table returned from the selection process
                        if(fds.size() == 1 && fds.front() == "*") // get all the columns, * means all
                        {
                            if(cond.empty() && where.empty()) // we want everything from the table since there are no conditions
                            {
                                selected = tables.get(table_name).select_all(); // we store the table returned from the select_all() function into sleected
                            }
                            else // we want all with conditions provided in the SQL statement
                            {
                                fds = tables.get(table_name).get_fields(); // get all the fields using a helper function from the table class
                                selected = tables.get(table_name).select(fds, cond); // call the select() function that takes in fields and conditions
                            }
                        }   // make sure that the fields do not have * because otherwise the program would enter the statement above
                        else if(vec_contains_all(fds))
                        {
                            throw runtime_error("Fields can not contain \"*\" and other fields!");
                        }
                        else if(cond.empty() && where.empty()) // we only want certain columns with out conditions
                        {
                            fds = verify_fields(fds); // verify all the fields provided and filter the fields to only contain valid fields
                            selected = tables.get(table_name).select_all(fds); // store the new table from an overloaded select_all() that takes in certain columns
                        }
                        else
                        {
                            fds = verify_fields(fds); // make sure that we only accept valid fields
                            selected = tables.get(table_name).select(fds, cond); // store the new table using the select() that takes in fields and a condition
                        }
                        recnos = tables.get(table_name).select_recnos(); // no matter which kind of select happens, the table object will store the records selected
                        // so we only need to call select_recnos() one time
                        return selected;
                    }
                }
                else // the user tries to select from a table that does not exist
                {
                    throw runtime_error("This table does not exist (has not been made yet)");
                }
            }
            else if(cmd.size() == 1 && (cmd.front() == "make" || cmd.front() == "create")) // we are making a new table
            {
                if(!tables.contains(table_name)) // check if the table exists, and if it doesn't create the table
                {
                    vector<string> fds = ptree.get("cols"); // get the columns to create the table with
                    Table t(tblnm.front(), fds); // create new table using tablename and fields
                    tables.insert(table_name, t);
                    // create the text file to store all the table names if it does not exist already
                    ofstream textf(txtfle, ios::app); // create and open textfile for appending
                    textf << table_name << endl; // write the tablename into the text file
                    textf.close(); // close text file
                    return t;
                }
                else // the table already exists so we return the existing table
                {
                    return tables.get(table_name);
                }
            }
            else if(cmd.size() == 1 && cmd.front() == "insert") // we are going to insert into the table
            {
                if(tables.contains(table_name)) // ensure that the table exists
                {
                    vector<string> vals = ptree.get("values"); // get the values we are inserting into the table from the SQL statement
                    if(vals.size() == tables.get(table_name).num_fields()) // check to see if the correct number of fields were given
                    {
                        tables.get(table_name).insert_into(vals); // insert into the table using the get function from the tables map
                        return tables.get(table_name);
                    }
                    else // the number of fields provided were not equal to the tables number of fields so we throw an error message
                    {
                        throw runtime_error("Inavlid amount of values for this table.");
                    }
                }
                else // the table does not exist yet so we throw an error message
                {
                    throw runtime_error("This table does not exist (has not been made yet)");
                }
            }
            else if(cmd.size() == 1 && cmd.front() == "delete" || cmd.front() == "drop") // we want to delete an entire table
            {
                if(tables.contains(table_name)) // make sure that the table is created/exists
                {
                    tables.get(table_name).clear_table(); // call the clear table function that clears the maps and all other member variables
                }
                else // else the table does not exist so we throw an error message that tells the user the table has not been made yet
                {
                    throw runtime_error("This table does not exist (has not been made yet)");
                }
            }
        }
        else // we did not recieve a valid SQL statement so we throw an error message displaying this
        {
            throw runtime_error("Invalid SQL statement.");
        }
    }
    catch(const exception& e)
    {
        cerr << "ERROR : " << e.what() << endl; // throw the error message whatever happened
    }
    Table none; // return an empty table
    recnos.clear(); // clear the records stored no matter what kind of exception
    return none;
}

vector<long> SQL::select_recnos() // function that returns the selected_records from a select command
{
    return(recnos);
}

void SQL::make_table_tree()
{
    tables.clear(); // ensure that the tables is clear
    ifstream txtf(txtfle); // open up the tables text file to read in all the table names we have made
    string inpt; // to store the table names we read
    while(getline(txtf, inpt))
    {
        Table rd(inpt); // create a table object using the from pre-existing table ctor
        tables[inpt] = rd; // input all data into the tables multimap
    }
}

void SQL::print_tablenames() // this function displays already created and saved tables to the user
{
    cout << "SQL: Currently tracking these tables:" << endl;
    cout << "-------------------------------------" << endl;
    Map<string, Table>::Iterator it; // iterate through the map of table names and objects
    for(it = tables.begin(); it != tables.end(); ++it) // print ptree as a table
    {
        cout << (*it).key << endl; // output the key alone, meaning only the table name stored
    }
    cout << "-------------------------------------\n" << endl;
}

Table SQL::get_table() // helper function to print the current table
{
    return tables.get(table_name); // gets the current table
}

vector<string> SQL::verify_fields(vector<string> fds) // this function will return only the valid fields
{
    Map<string, long> real_fields = tables.get(table_name).get_data(); // get all the actual fields
    vector<string> result;
    for(const auto& it : fds) // iterate through the inputted fields
    {
        if(vec_has(it, real_fields)) // if the inputted field is amongst the actual fields of the table
        {
            result.push_back(it); // push the inputted field into the result vector
        }
    }
    return result;
}

bool SQL::vec_has(const string& item, const Map<string, long>& vec) // helper function for determining if a field is valid
{
    if (vec.contains(item)) // determine if this record field(item) is currently in the map of fields(vec)
    {
        return true; // return true if field(item) is in the map of fields(vec)
    }   
    return false; // the field(item) is not present, so return false;
}

bool SQL::vec_contains_all(const vector<string>& vec) // helper function to determine if the fields vector contains the '*' character
{   // the only way to perform this is by using a linear search unfortunately
    for(const auto& it : vec)
    {
        if(it == "*") // the * character is present so we return true
        {
            return true;
        }
    }   // we did not find the * character so we return false
    return false;
}

void SQL::batch()
{
    vector<string> commands = 
    {
        // 1. valid command:
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 2. Expected comma:
        "select lname fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 3. Expected: field name
        "select lname, , major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 4. Expected from:
        "select lname, fname, major  student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 5. Expected table name:
        "select lname, fname, major from  where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 6. Expected condition:
        "select lname, fname, major from student where",
        // 7. Missing left paren:
        "select lname, fname, major from student where (lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 8. Missing right paren:
        "select lname, fname, major from student where ((lname=Yang or major=CS and age<23 )or lname=Jackson",
        // 9. :
        "select lname, fname, major from student where ((lname= or major=CS) and age<23 )or lname=Jackson",
        // 10. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
        // 11. :
        "select lname, fname, major from student where ((lname=Yang  major=CS) and age<23 )or lname=Jackson",
        // 12. :
        "select lname, fname, major from student where ((lname=Yang or ) and age<23 )or lname=Jackson",
        // 13. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or",
        // 14. :
        "select lname, fname, major from student where ((lname=Yang or major=CS) and age<23 )or lname=Jackson",
    };
    for(int i = 0; i < commands.size(); i++) // iterate through all the commands
    {   // print out the numbe of the command and the command as well
        cout << (i + 1) << ") " << commands[i] << endl; 
        cout << command(commands[i]) << endl; // print the output from the command
    }
}