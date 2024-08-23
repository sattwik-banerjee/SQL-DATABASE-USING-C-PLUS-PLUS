#ifndef MAP_H
#define MAP_H

#include "btree_array_funcs.h"
#include <iostream>

using namespace std;

template <typename K, typename V>
struct Pair {
    K key;
    V value;

    Pair(const K& k = K(), const V& v = V()) : key(k), value(v) {}

    friend ostream& operator<<(ostream& outs, const Pair<K, V>& print_me) // outputs the data in the map
    {
        outs << "(" << print_me.key << ", " << print_me.value << ")";
        return outs;
    }

    friend bool operator==(const Pair<K, V>& lhs, const Pair<K, V>& rhs) // determines if the lhs and rhs are equal
    {
        return lhs.key == rhs.key;
    }

    friend bool operator<(const Pair<K, V>& lhs, const Pair<K, V>& rhs) // determine if the lhs is < than rhs
    {
        return lhs.key < rhs.key;
    }

    friend bool operator>(const Pair<K, V>& lhs, const Pair<K, V>& rhs) // determine if the lhs is > than the rhs
    {
        return lhs.key > rhs.key;
    }

    friend bool operator<=(const Pair<K, V>& lhs, const Pair<K, V>& rhs) // determine if the lhs is <= than the rhs
    {
        return lhs.key <= rhs.key;
    }

    friend Pair<K, V> operator+(const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs;
    }

    friend bool operator!=(const Pair<K, V>& lhs, const Pair<K, V>& rhs) // evaluates if lhs and rhs are not equal by using the == operator
    {
        return !(lhs == rhs);
    }
};

template <typename K, typename V>
class Map {
public:

    class Iterator {
    public:
        friend class Map;

        Iterator() : _it() {}
        Iterator(typename BPlusTree<Pair<K, V>>::Iterator it) : _it(it) {}

        Iterator operator++(int unused) // ++ operator, not so often used
        {   // all these iterators call upon the BPlusTree iterator functions
            Iterator it(_it);
            _it++;
            return it;
        }

        Iterator operator++() // ++ operator, this one is more used
        {
            _it++;
            return *this;
        }

        Pair<K, V> operator*() // dereferencing operator
        {
            return *_it;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs) // evaluates equality between lhs and rhs
        {
            return lhs._it == rhs._it;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) // evaluates non-equality between lhs and rhs
        {
            return !(lhs == rhs);
        }

    private:
        typename BPlusTree<Pair<K, V>>::Iterator _it;
    };

    Map()
    {
        key_count = 0;
    }

    // Iterators
    Iterator begin() // sets beginning using BPlusTree begin
    {
        return Iterator(map.begin());
    }

    Iterator end() // end is always nullptr but we can use the B+Tree end
    {
        return Iterator(map.end());
    }


    int size() const // call the B+Tree get_count() function to determine size 
    {
        return map.get_count();
    }

    bool empty() const // empty if key_count is 0
    {
        return key_count == 0;
    }

    // Element Access
    V& operator[](const K& key) // this is essentially how items are inserted into the map
    {
        Pair<K, V> temp(key);
        Pair<K, V> checker = Pair<K, V>();
        if((map.get_dc() > 0 || map.get_cc() > 0) && map.get_data_val(0) == checker) // check for possible initialization issues
        { // set the data and child count to 0
            map.set_data(0);
            map.set_children(0);
        }
        if (!contains(temp)) // determine if the key is already in the map
        {   // if not then we insert the key with an empty V value
            V value;
            insert(key, value);
            return map.get(temp).value;
        }   // else we just return the existing value
        return map.get(temp).value;
    }

    V& at(const K& key) // return the value at key
    {
        Pair<K, V> temp(key);
        const Pair<K, V>* found = map.get(temp);
        if (!found) // if key is not found we return nullptr bc there is nothing to do
        {
            throw std::out_of_range("Key not found in Map");
        }   // yay we found it so we return the value
        return found->value;
    }

    const V& at(const K& key) const // return the value at key
    {
        Pair<K, V> temp(key);
        const Pair<K, V>* found = map.get(temp);
        if (!found) // if key is not found we return nullptr bc there is nothing to do
        {
            throw std::out_of_range("Key not found in Map");
        }// yay we found it so we return the value
        return found->value;
    }

    // Modifiers
    void insert(const K& k, const V& v) 
    {
        Pair<K, V> temp(k, v);
        map.insert(temp); // call on B+Tree insert to insert the Pair
        key_count++; // increment the key_count
    }

    void erase(const K& key) // not written
    {
        return;
    }

    void clear() // calls B+Tree clear_tree to clear the map, reset key_count to 0
    {
        map.clear_tree();
        key_count = 0;
    }

    V& get(const K& key) // returns the value that key holds
    {
        Pair<K, V> temp(key);
        if (!map.contains(temp)) // if key is not in map throw error
        {
            throw std::out_of_range("Key not found in Map");
        }   //else return actual value
        return map.get(temp).value;
    }

    // Operations
    Iterator find(const K& key) 
    {
        Pair<K, V> temp(key); // call B+Tree find
        return Iterator(map.find(temp));
    }

    bool contains(const Pair<K, V>& target) const 
    {
        return map.contains(target); // calls B+Tree contains to determine if something is in the map
    }

    int count(const K& key) // returns the number of times key shows up, max 1 min 0
    {
        if(contains(key)) 
        {
            return 1;
        }
        return 0;
    }

    Iterator lower_bound(const K& key) 
    {
        return Iterator(map.lower_bound(Pair<K, V>(key))); // calls B+Tree lower_bound
    }

    Iterator upper_bound(const K& key) 
    {
        return Iterator(map.upper_bound(Pair<K, V>(key))); // calls B+Tree upper bound
    }

    bool is_valid() 
    {   // calls B+Tree valid function to check the map
        return map.is_valid();
    }

    friend ostream& operator<<(ostream& outs, const Map<K, V>& print_me) // output function
    {
        outs << print_me.map << endl;
        return outs;
    }

private:
    int key_count;
    BPlusTree<Pair<K, V>> map;
};

#endif