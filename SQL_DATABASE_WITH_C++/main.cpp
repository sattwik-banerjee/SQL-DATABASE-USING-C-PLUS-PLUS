#include <iostream>
#include <iomanip>

#include "includes/sql/sql.h"

using namespace std;

int main(int argv, char** argc) 
{
    cout << "\n" << endl; // formatting the output
    SQL sql;
    char selec;
    string inpt;
    // print out all the currently active tables
    sql.print_tablenames();
    // menu for the user to input commands and for the program to follow them
    sql.batch();
    do 
    {
        cout << "Enter an SQL command : " << endl;
        cout << "> ";
        getline(cin, inpt);
        // gets command ^
        cout << sql.command(inpt) << "\n" << endl;
        // if no errors are thrown and everything is valid ^ prints out the table
        cout << "Want to continue? Type Q or q if you want to quit : ";
        cin >> selec;
        // determines if the user is done with the program or if it would like to continue adding commands
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        // formatting the output \/
        cout << endl;
    } while(selec != 'Q' && selec != 'q');

    return 0;
}
