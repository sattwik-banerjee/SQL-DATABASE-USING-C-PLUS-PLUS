#ifndef BTREE_ARRAY_FUNCS_H
#define BTREE_ARRAY_FUNCS_H

#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

template <class T>
T maximal(const T& a, const T& b) //return the larger of the two items
{
    if(a > b)
    {
        return a;
    }
    return b;
}

template <class T>
void swap(T& a, T& b)  //swap the two items
{
    T temp = a;
    a = b;
    b = temp;
}

template <class T>
int index_of_maximal(T data[ ], int n)                //return index of the largest item in data
{
    T max = data[0];
    T temp;
    int index = 0;
    for(int i = 1; i < n; i++)
    {
        temp = maximal(max, data[i]);
        if(temp != max)
        {
            index = i;
        }
    }
    return index;
}

template <class T>
void ordered_insert(T data[ ], int& n, T entry)        //insert entry into the sorted array
{
    return;
}                                                        //data with length n

template <class T>
int first_ge(const T data[ ], int n, const T& entry)   //return the first element in data that is
{
    int index = -1;
    int i = 0;
    bool ge = false;
    while(i < n && !ge)
    {
        if(entry <= data[i])
        {
            ge = true;
            index = i;
        }
        i++;
    }
    if(index == -1) // bigger than everything in the array
    {
        return n;
    }
    return index;
}                                                        //not less than entry

template <class T>
void attach_item(T data[ ], int& n, const T& entry)    //append entry to the right of data
{
    data[n] = entry;
    n++;
}
template <class T>
void insert_item(T data[ ], int i, int& n, T entry)    //insert entry at index i in data
{
    // Move elements to the right to make space for the new entry
    for (int k = n - 1; k >= i; k--) {
        data[k + 1] = data[k];
    }
    // Insert the new entry
    data[i] = entry;
    // Increment the count of items
    n++;
}

template <class T>
void detach_item(T data[ ], int& n, T& entry)          //remove the last element in data and place
{
    entry = data[n - 1];
    n--;
}                                                       //it in entry

template <class T>
void delete_item(T data[ ], int i, int& n, T& entry)   //delete item at index i and place it in entry
{
    entry = data[i];
    n--;
    for(int k = i; k < n; k++)
    {
        data[k] = data[k + 1];
    }
}

template <class T>
void merge(T data1[ ], int& n1, T data2[ ], int& n2)   //append data2 to the right of data1
{
    int new_size = n1 + n2;
    for(int i = 0; i < n2; i++)
    {
        data1[n1 + i] = data2[i];
    }
    n1 = new_size;
    n2 = 0;
}

template <class T>
void split(T data1[ ], int& n1, T data2[ ], int& n2)   //move n/2 elements from the right of data1
{
    int size2 = n1 / 2;
    int size1 = n1 - size2;
    for (int i = 0; i < size2; i++) 
    {
        data2[i] = data1[i + size1];
        data1[i + size1] = T();
    }
    n1 = size1;
    n2 = size2;
}                                                        //and move to data2

template <class T>
void copy_array(T dest[], const T src[], int& dest_size, int src_size)              //copy src[] into dest[]
{
    dest_size = src_size;
    for(int i = 0; i < dest_size; i++)
    {
        dest[i] = src[i];
    }
}
template <class T>
void print_array(const T data[], int n, int pos = -1)  //print array data
{
    for (int i = 0; i < n; i++)
    {
        cout << setw(4) << data[i];
    }
    cout<< "   [" << n << "]" << endl;
}

template <class T>
bool is_gt(const T data[], int n, const T& item)       //item > all data[i]
{
    int index = first_ge(data, n, item);
    if(index == n)
    {
        return true;
    }
    return false;
}

template <class T>
bool is_le(const T data[], int n, const T& item)       //item <= all data[i]
{
    int index = first_ge(data, n, item);
    if(index == 0)
    {
        return true;
    }
    return false;
}

//-------------- Vector Extra operators: ---------------------

template <typename T>
ostream& operator <<(ostream& outs, const vector<T>& list) //print vector list
{
    for (auto element : list) 
    {
        outs << element << " ";
    }
    return outs;
}

template <typename T>
vector<T>& operator +=(vector<T>& list, const T& addme) //list.push_back addme
{
    list.push_back(addme);
    return list;
}

#endif