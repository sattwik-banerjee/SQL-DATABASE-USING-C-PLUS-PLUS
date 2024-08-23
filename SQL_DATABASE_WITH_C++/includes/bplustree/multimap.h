#ifndef MULTIMAP_H
#define MULTIMAP_H

#include "../bplustree/btree_array_funcs.h"
#include "../bplustree/bplustree.h"
#include "../tokenizer/constants.h"
#include <iostream>
#include <vector>

using namespace std;

template <typename K, typename V>



struct MPair{
    K key;
    vector<V> value_list;

    //--------------------------------------------------------------------------------
    /*      MPair CTORs:
     *  must have these CTORs:
     *  - default CTOR / CTOR with a key and no value: this will create an empty vector
     *  - CTOR with a key AND a value: pushes the value into the value_list
     *  _ CTOR with a key and a vector of values (to replace value_list)
     */
    //--------------------------------------------------------------------------------
    MPair(const K& k=K())
    {
        key = k;
    }
    MPair(const K& k, const V& v)
    {
        key = k;
        value_list.push_back(v);
    }
    MPair(const K& k, const vector<V>& vlist)
    {
        key = k;
        value_list = vlist;
    }
    //--------------------------------------------------------------------------------

    //You'll need to overlod << for your vector:
    friend ostream& operator <<(ostream& outs, const MPair<K, V>& print_me) // outputs all the data in the mmap
    {
        outs << print_me.key << ": [";
        for (const V& value : print_me.value_list) 
        {
            outs << value << " ";
        }
        outs << "]";
        return outs;
    }
    
    friend bool operator ==(const MPair<K, V>& lhs, const MPair<K, V>& rhs) // evaluates equality between lhs and rhs
    {
        return lhs.key == rhs.key;
    }
    friend bool operator < (const MPair<K, V>& lhs, const MPair<K, V>& rhs) // evaluates if lhs is less than rhs
    {
        return lhs.key < rhs.key;
    }
    friend bool operator <= (const MPair<K, V>& lhs, const MPair<K, V>& rhs) // evaluates if lhs is less than or equal to rhs
    {
        return lhs.key <= rhs.key;
    }
    friend bool operator > (const MPair<K, V>& lhs, const MPair<K, V>& rhs) // evaluates if lhs is greater than rhs
    {
        return lhs.key > rhs.key;
    }
    friend MPair<K, V> operator + (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return lhs;
    }
    friend bool operator !=(const MPair<K, V>& lhs, const MPair<K, V>& rhs) // evaluates if lhs and rhs are not equal
    {
        return !(lhs == rhs);
    }
};

template <typename K, typename V>
class MMap
{
public:
    class Iterator{
    public:
        friend class MMap;

        Iterator() : _it() {}

        Iterator(typename BPlusTree<MPair<K, V> >::Iterator _it) : _it(_it) {}

        // all the iterator functions call upon the B+Tree iterator functions
        Iterator operator ++(int unused) // this one is not so often used
        {
            Iterator temp(_it);
            _it++;
            return temp;
        }

        Iterator operator ++() // this operator ++ is used more often
        {
            ++_it;
            return *this;
        }

        MPair<K, V> operator *() // dereferencing operator
        {
            return *_it;
        }

        friend bool operator ==(const Iterator& lhs, const Iterator& rhs) // evaluates equality between lhs and rhs
        {
            return lhs._it == rhs._it;
        }

        friend bool operator !=(const Iterator& lhs, const Iterator& rhs) // evaluest non equality between lhs and rhs
        {
            return !(lhs == rhs);
        }

    private:
        typename BPlusTree<MPair<K, V> >::Iterator _it;
    };

    MMap()
    {
        key_count = 0;
    }

//  Iterators
    Iterator begin() // sets beginning using BPlusTree begin
    {
        return Iterator(mmap.begin());
    }
    Iterator end()// end is always nullptr but we can use the B+Tree end
    {
        return Iterator(nullptr);
    }
//  Capacity
    int size() const // call the B+Tree get_count() function to determine size 
    {
        return mmap.get_count();
    }
    bool empty() const // empty if key_count is 0 or we can use the empty function from the B+Tree
    {
        return mmap.empty();
    }

//  Element Access
    const vector<V>& operator[](const K& key) const // this is essentially how items are inserted into the mmap
    {
        MPair<K, V>* temp = new MPair<K, V>(key);
        if((mmap.data_count > 0 || mmap.child_count > 0) && mmap.data[0] == V()) // check for possible initialization issues
        {   // set the data and child count to 0
            mmap.data_count = 0;
            mmap.child_count = 0;
        }
        if (!contains(temp)) // if the map doesn't contain
        {
            V value;
            insert(key, value); // gets inserted using B+Tree insert
            return mmap.get(key).value;
        }   // else returns the vector list
        return mmap.get(*temp).value_list;
    }
    vector<V>& operator[](const K& key) // this is essentially how items are inserted into the mmap
    {
        MPair<K, V> temp = MPair<K, V>(key);
        MPair<K, V> checker = MPair<K, V>();
        if((mmap.get_dc() > 0 || mmap.get_cc() > 0) && mmap.get_data_val(0) == checker) // check for possible initialization issues
        { // set the data and child count to 0
            mmap.set_data(0);
            mmap.set_children(0);
        }
        if (!contains(key))  // if the map doesn't contain
        {
            V value;
            insert(key, value); // gets inserted using B+Tree insert
            return mmap.get(key).value_list;
        } // else returns the vector list
        return mmap.get(temp).value_list;
    }

//  Modifiers
    void insert(const K& k, const V& v)
    {   // calls upon the B+Tree insert
        MPair<K, V>* temp = new MPair<K, V>(k);
        mmap.insert(*temp);
        key_count++; 
    }
    void erase(const K& key)
    {   // not written
        return;
    }
    void clear()
    { // calls B+Tree clear_tree() to clear the mmap
        mmap.clear_tree();
        key_count = 0;
    }

//  Operations:
    bool contains(const K& key) const 
    {
        return mmap.contains(MPair<K, V>(key)); // calls B+Tree contains to determine if something is in the map
    }
    vector<V> &get(const K& key) // returns the value list of key
    {
        MPair<K, V> temp(key);
        if (!contains(key)) // if key is not in the mmap insert it int
        {
            MPair<K, V> new_pair(key);
            mmap.insert(new_pair);
            return mmap.get(key).value_list;
        }   // else return the value list anyways
        return mmap.get(temp).value_list;
    }

    Iterator find(const K& key)
    {
        return Iterator(mmap.find(MPair<K, V>(key))); // calls upon B+Tree find
    }
    int count(const K& key)
    {
        MPair<K, V> temp(key);
        return mmap.count(temp); // calls upon the B+Tree count to return the count of an item
    }

    Iterator lower_bound(const K& key)
    {
        return Iterator(mmap.lower_bound(MPair<K, V>(key))); // calls B+Tree lower_bound
    }
    Iterator upper_bound(const K& key)
    {
        return Iterator(mmap.upper_bound(MPair<K, V>(key))); // calls B+Tree upper bound
    }

    const V& at(const K& key) const // returns the value list at key
    {
        MPair<K, V> temp(key);
        if (!contains(key)) // if key is not in the map throw error
        {
            throw std::out_of_range("Key not found in Map");
        }   // else return the front of the value list
        return mmap.get(temp).value_list.front();
    }

    V& get_vlist(const K& key) // returns the value list at key
    {
        MPair<K, V> temp(key);
        if (!contains(key)) // if key is not in the map throw error
        {
            throw std::out_of_range("Key not found in Map");
        } // else return the front of the value list
        return mmap.get(temp).value_list;
    }

    bool is_valid()
    {
        return true;
    }

    int get_key_count() // helper function that returns the key count
    {
        return key_count;
    }

    int get_map_size() // helper function that returns the map size
    {
        return mmap.size();
    }

    int get_map_child() // helper function that returns the number of children
    {
        return mmap.get_children();
    }

    void set_children(const int& num) // helper function that can set the number of children
    {
        mmap.set_children(num);
    }

    void set_data(const int& num) // helper function that ca set the data count
    {
        mmap.set_data(num);
    }

    void print_lookup() // for the parser class prints out the mmap like a table
    {
        MMap<string, string>::Iterator it; // iterate through the ptree
        if(key_count != 0)
        {
            for(it = begin(); it != end(); ++it) // print ptree as a table
            {
                cout << setw(10) << (*it).key << " : " << (*it).value_list << endl;
            }
        }
    }

    friend ostream& operator<<(ostream& outs, const MMap<K, V>& print_me) // outputs the mmap
    {
        outs<<print_me.mmap<<endl;
        return outs;
    }

private:
    BPlusTree<MPair<K, V> > mmap;
    int key_count;
};

#endif