#include "../../includes/tokenizer/state_machine_functions.h"
#include "../../includes/tokenizer/constants.h"
#include <iostream> 
#include <cstring>
using namespace std;

//Fill all cells of the array with -1
void init_table(int _table[][MAX_COLUMNS])
{
    for(int i = 0; i < MAX_ROWS; i++)
    {
        for(int j = 0; j < MAX_COLUMNS; j++)
        {
            _table[i][j] = -1;
        }
    }
}

//Mark this state (row) with a 1 (success)
void mark_success(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 1;
}

//Mark this state (row) with a 0 (fail)
void mark_fail(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 0;
}

//true if state is a success state
bool is_success(int _table[][MAX_COLUMNS], int state)
{
    return(_table[state][0] == 1);
}

//Mark a range of cells in the array. 
void mark_cells(int row, int _table[][MAX_COLUMNS], int from, int to, int state)
{   
    if(from == to)
    {
        _table[row][from] = state;
    }
    else
    {
        int j = from;
        while(j < to)
        {
            _table[row][j] = state;
            j++;
        }
    }
}

//Mark columns represented by the string columns[] for this row
void mark_cells(int row, int _table[][MAX_COLUMNS], const char columns[], int state)
{
    for(int i = 0; i < strlen(columns); i++)
    {
        char col = columns[i];
        _table[row][col] = state;
    }
}

//Mark this row and column
void mark_cell(int row, int table[][MAX_COLUMNS], int column, int state)
{
    table[row][column] = state;
}

//This can realistically be used on a small table
void print_table(int _table[][MAX_COLUMNS])
{
    for(int i = 0; i < MAX_ROWS; i++)
    {
        for(int j = 0; j < MAX_COLUMNS; j++)
        {
            cout << _table[i][j] << " ";
        }
        cout << endl;
    }
}

void print_segment(int _table[][MAX_COLUMNS], int row_from, int row_to, int col_from, int col_to)
{
    for(int i = row_from; i < row_to; i++)
    {
        for(int j = col_from; j < col_to; j++)
        {
            cout << _table[i][j] << " ";
        }
        cout << endl;
    }
}

//show string s and mark this position on the string:
//hello world   pos: 7
//       ^
void show_string(char s[], int _pos)
{
    cout << s;
    cout << endl;
    for(int i = 0; i < _pos; i++)
    {
        cout << " ";
    }
    cout << "^" << endl;
}

