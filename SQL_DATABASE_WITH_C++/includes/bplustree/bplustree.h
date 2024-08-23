#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iomanip>
#include <iostream>
#include "btree_array_funcs.h"

using namespace std;

template <class T>
class BPlusTree
{
public:
    class Iterator{
    public:
        friend class BPlusTree;
        Iterator(BPlusTree<T>* _it=NULL, int _key_ptr = 0):
                                                it(_it), key_ptr(_key_ptr){}

        T operator *() // dereferncing operator
        {
            if(it != nullptr && !it->empty())
            {
                return it->data[key_ptr]; // returns the item held at data[key_ptr]
            }
            return T();  // Return a default constructed T if iterator is null or empty
        }
        Iterator operator++(int un_used) // increment operator, don't really use this one
        {
            Iterator hold = ++(*this); // increment the pointer of this
            return hold;
        }
        Iterator operator++() // increment operator, this one is used the most
        {
            key_ptr++; // increment the key pointer count
            if(it != nullptr && key_ptr >= it->data_count) // check if there is anything left to iterate through
            {
                key_ptr = 0; // reset key_ptr
                it = it->next; // go to next
            }
            return *this;
        }
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs) // == operator checks all the data from both lhs and rhs
        {
            return (lhs.it == rhs.it && lhs.key_ptr == rhs.key_ptr);
        }
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs) // != operator just calls the == operator
        {
            return !(lhs == rhs);
        }
        void print_Iterator() // prints out the iterators
        {
            if(it) // makes sure this isnt null
            {
                cout<<"iterator: "; // print out the iterator using the print_array function
                print_array(it->data, it->data_count, key_ptr);
            }
            else
            {
                cout<<"iterator: NULL, key_ptr: "<<key_ptr<<endl;
            }
        }
        bool is_null(){return !it;} // determine if the iterator is null
        void info() // returns all the member variables regarding the iterator
        {
            cout<<endl<<"Iterator info:"<<endl;
            cout<<"key_ptr: "<<key_ptr<<endl;
            if(it != nullptr) {
                cout<<"it: "<<*it<<endl;
            } else {
                cout<<"it: NULL"<<endl;
            }
        }

    private:
        BPlusTree<T>* it;
        int key_ptr;
    };

    BPlusTree(bool dups = false)
    { // initializes all the data count, child count, next, and duplicates
        data_count = 0;
        child_count = 0;

        next = nullptr;
        dups_ok = dups;
    }
    BPlusTree(T *a, int size, bool dups = false)
    {   // this does the same thing but it is given an array to insert from so we do that process
        data_count = 0;
        child_count = 0;
        next = nullptr;
        for(int i = 0; i < size; i++) // iterate through the array and insert the elements into the BPlusTree
        {
            insert(a[i]);
        }
        dups_ok = dups;
    }

    //big three:
    BPlusTree(const BPlusTree<T>& other) // copy constructor
    {
        copy_tree(other); // call copy tree
    }
    ~BPlusTree() // deconstructor
    {
        if((data_count > 0 || child_count > 0) && data[0] == T()) // to take care of possible initalization issues
        {
            data_count = 0;
            child_count = 0;
        }
        clear_tree(); // call clear tree
    }
    BPlusTree<T>& operator =(const BPlusTree<T>& RHS) // assignment perator
    {
        if(this != &RHS) // self-assignment check
        {
            clear_tree(); // first clear the tree 
            copy_tree(RHS); // second copy everything from RHS
        }
        return *this;
    }

    void copy_tree(const BPlusTree<T>& other)  //copy other into this object
    {
        BPlusTree<T>* last_node = nullptr;
        if((data_count > 0 || child_count > 0) && data[0] == T()) // to take care of possible initalization issues
        {
            data_count = 0;
            child_count = 0;
        }
        copy_tree(other, last_node); // call overloaded copy_tree function
    }
    void copy_tree(const BPlusTree<T>& other, BPlusTree<T>*& last_node)
    {
        if(!other.empty())
        {
            // Copy data and counts from the other BTree
            data_count = other.data_count;
            child_count = other.child_count;
            next = other.next;
            // Copy data array
            for (int i = 0; i < data_count; ++i) 
            {
                data[i] = other.data[i];
            }

            // Copy child BTree objects recursively
            for (int i = 0; i < child_count; ++i) 
            {
                subset[i] = new BPlusTree<T>(); // Create new child BTree object
                subset[i]->copy_tree(*other.subset[i], last_node); // Recursively copy subtree
            }

            if(is_leaf()) // if we are at a leaf we reset the nexts
            {
                if(last_node == nullptr)
                {
                    last_node = this;
                }
                else 
                {
                    last_node->next = this;
                    last_node = this;
                }
            }
        }
    }

    void insert(const T& entry)   //insert entry into the tree
    {
        loose_insert(entry); // insert entry
        rid_extra(); // get rid of extra data
        if(data_count > MAXIMUM) // check if we need to grow_tree
        {
            grow_tree();
        }
        rid_extra(); // get rid of extra data
    }
    
    void rid_extra()
    {
        if(data_count == MAXIMUM) // make sure we set everything that is not holding data to empty
        {
            data[MAXIMUM] = T();
        }
        else 
        {
            for(int i = data_count; i < MAXIMUM; i++) // get rid of all the extra elements in the arrays
            {
                data[i] = T();
            }
        }
        for(int i = 0; i < child_count; i++)
        {
            subset[i]->rid_extra(); // iterate through the tree
        }
    }
    
    void remove(const T& entry)   //remove entry from the tree
    {
        loose_remove(entry); // call loose remove
        if(data_count < MINIMUM && child_count > 1) // check condition for shrinking the tree
        {
            shrink_tree();
        }
    }

    void clear_tree() 
    {
        if (this == nullptr) 
        {
            return;
        }

        // Recursively delete all nodes in the tree
        for (int i = 0; i < child_count; ++i) 
        {
            if (subset[i] != nullptr) 
            { 
                // Check if the child node is not nullptr
                subset[i]->clear_tree(); // Clear each child subtree
                delete subset[i]; // Delete the child node
                subset[i] = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }
        }

        // Reset data and counts
        data_count = 0;
        child_count = 0;
    }

    bool contains(const T& entry) const //true if entry can be found
    {
        if(data_count == 0) // if the tree is empty return false
        {
            return false;
        }
        int index = first_ge(data, data_count, entry); // get the index wher the entry should be
        if(data[index] == entry && is_leaf()) // if this is a leaf and the data[index] is equal to entry return true
        {
            return true;
        }
        else if(data[index] != entry && is_leaf()) // if this is a leaf and we don't find entry return false
        {
            return false;
        }
        else if(!is_leaf() && data[index] == entry) // if we find the breadcrumb go to the right
        {
            return subset[index + 1]->contains(entry);
        }
        else // else we continue down to the left
        {
            return subset[index]->contains(entry);
        }
    }

    T& get(const T& entry) //return a reference to entry
    {
        if(contains(entry)) // make sure that the entry is contained
        {
            Iterator found = find(entry); // use find() to find the iterator to entry
            return found.it->data[found.key_ptr]; // return the data from the iterator
        }
        throw runtime_error("Element not found");
    }
    const T& get(const T& entry)const   //return a reference to entry
    {
        if(data_count == 0) // if the tree is empty
        {
            throw runtime_error("Element not found"); // throw error mesage
        }
        int index = first_ge(data, data_count, entry); // get index where entry should be
        if(data[index] == entry && is_leaf()) // if this is a leaf and the data[index] is equal to entry return the value
        {
            return data[index];
        }
        else if(data[index] != entry && is_leaf()) // if this is a leaf and we don't find entry throw error message
        {
            throw runtime_error("Element not found");
        }
        else if(!is_leaf() && data[index] == entry) // if we find the breadcrumb go to the right
        {
            return subset[index + 1]->get(entry);
        }
        else // else we continue down the left
        {
            return subset[index]->get(entry);
        }
    }

    T& get_existing(const T& entry)   //return a reference to entry
    {
        return get(entry);
    }

    Iterator find(const T& key)  //return an iterator to this key.
    {
        if(data_count == 0) // if the tree is empty return an nullptr iterator
        {
            return Iterator(nullptr);
        }
        int index = first_ge(data, data_count, key); // get index where entry should be
        if(data[index] == key && is_leaf()) // if this is a leaf and the data[index] is equal to entry return the iterator to this entry
        {
            return Iterator(this, index);
        }
        else if(data[index] != key && is_leaf()) // if this is a leaf and we don't find entry still return iterator to this entry
        {
            return Iterator(this, index);
        }
        else if(!is_leaf() && data[index] == key) // if we find the breadcrumb go to the right
        {
            return subset[index + 1]->find(key);
        }
        else // else we continue down the left
        {
            return subset[index]->find(key);
        }
    }

    int size() const  //count the number of elements in the tree
    {
        if(empty()) // check if the tree is empty
        {
            return 0; // retur 0 sizee
        }
        int count = data_count; // count starts at the data count
        for(int i = 0; i < child_count; i++) // recursively go through and add all the data counts together
        {
            count += subset[i]->size();
        }
        return count;
    }

    bool empty() const  //true if the tree is empty
    {
        return (data_count == 0);
    }

    void print_tree(int level = 0, ostream &outs=cout) const
    {
        for(int i = data_count-1; i >= 0; i--) // iterate through the tree
        {
            if(child_count > 0) // if we are not at a leaf continue traversing the tree
            {
                subset[i+1]->print_tree(level+1);
            }
            outs<<setw(level*4)<<" "<<data[i]<<endl; // format the output
        }
        if(child_count > 0) // traverse down the right 
        {
            subset[0]->print_tree(level+1);
        }
    }

    friend ostream& operator<<(ostream& outs, const BPlusTree<T>& print_me)
    {
        print_me.print_tree(0, outs); // call the print_tree function
        return outs;
    }

    bool is_valid() // determine if the tree is valid
    {
        if(data_count < MINIMUM && !is_leaf()) // if the tree is empty it is not valid
        {
            return false;
        }
        for(int i = 0; i < data_count - 1; i++)
        {
            if(data[i] > data[i+1]) // if the data at i is greater than the data after i it is not valid
            {
                return false;
            }
        }
        if(!is_leaf())
        {
            for(int i = 0; i < data_count; i++)
            {   // these if conditions check to make sure that the data is all in ascending order
                if(subset[i]->data[subset[i]->data_count - 1] > data[i])
                {
                    return false;
                }
                if(subset[i + 1]->data[0] < data[i])
                {
                    return false;
                }
            }
            for(int i = 0; i < child_count; i++)
            {   // recursively traverse the tree
                if(!subset[i]->is_valid())
                {
                    return false;
                }
            }
        }   // if we make it here then the tree is valid
        return true;
    }

    void print_leaves() // print out the leaves
    {
        BPlusTree<T>* temp = this;
        while(temp->child_count > 0) // traverse through the tree
        {
            temp = temp->subset[0];
        }
        while(temp != nullptr) // print out all the information
        {
            for(int i = 0; i < temp->data_count; i++)
            {
                cout<<temp->data[i]<<" ";
            }
            cout<<" -> ";
            temp = temp->next;
        }
    }

    string in_order()
    {
        string result; // String to accumulate the result

        // Traverse each child recursively and concatenate the data
        if (!is_leaf()) 
        {
            // Traverse the left subtree
            result += subset[0]->in_order(); // Traverse left subtree
            for (int i = 0; i < data_count; i++) 
            {
                // Append current node's data in the specified format
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
                // Traverse the right subtree
                result += subset[i + 1]->in_order(); // Traverse right subtree
            }
        } 
        else 
        {
            // If it's a leaf node, just concatenate the data
            for (int i = 0; i < data_count; i++) 
            {
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
            }
        }

        return result; // Return the accumulated string
    }

    string pre_order()
    {
        string result; // String to accumulate the result

        // Traverse each child recursively and concatenate the data
        if (!is_leaf()) 
        {
            // Traverse the left subtree
            for (int i = 0; i < data_count; i++) 
            {
                // Append current node's data in the specified format
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
                result += subset[i]->pre_order(); // Traverse left subtree
                // Traverse the right subtree
            }
            result += subset[data_count]->pre_order(); // Traverse right subtree
        } 
        else 
        {
            // If it's a leaf node, just concatenate the data
            for (int i = 0; i < data_count; i++) 
            {
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
            }
        }

        return result; // Return the accumulated string
    }

    string post_order()
    {
        string result; // String to accumulate the result

        // Traverse each child recursively and concatenate the data
        if (!is_leaf()) 
        {
            result += subset[0]->post_order();
            // Traverse the left subtree
            for (int i = 0; i < data_count; i++) 
            {
                result += subset[i + 1]->post_order(); // Traverse right subtree
                // Append current node's data in the specified format
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
                // Traverse the right subtree
            }
        } 
        else 
        {
            // If it's a leaf node, just concatenate the data
            for (int i = 0; i < data_count; i++) 
            {
                result += to_string(data[i]) + "|"; // Convert current node's data to string and concatenate
            }
        }

        return result; // Return the accumulated string
    }

    Iterator begin() // returns an iterator that is at the beignning
    {
        BPlusTree<T>* temp = this;
        while(temp->child_count > 0) // traverses all the way down to the left
        {
            temp = temp->subset[0];
        }
        return Iterator(temp, 0);
    }
    Iterator end() // the end is always nullptr
    {
        return Iterator(nullptr); // End iterator points to null
    }

    Iterator lower_bound(const T& key) 
    {
        BPlusTree<T>* current = this;

        while (current != nullptr) 
        {
            int index = first_ge(current->data, current->data_count, key);
            
            // if key is found in a leaf node, return an iterator pointing to it.
            if (current->data[index] == key && current->is_leaf()) 
            {
                return Iterator(current, index);
            }

            // if we're at a leaf node and key is not found, return an iterator pointing to the next element or end.
            if (current->is_leaf()) 
            {
                if(key > current->data[index]) // if the key is greater than the data at index then push the iterator forward one time
                {
                    Iterator it(current, index);
                    return it++;
                }
                else // return the iterator at this index
                {
                    return Iterator(current, index);
                }
            }

            // Move to the next subset to continue searching
            current = current->subset[index];
        }
        // if we don't find where the key should belong return nullptr iterator
        return end();
    }

    Iterator upper_bound(const T& key) 
    {
        Iterator it = lower_bound(key);
        if(!contains(key)) // the lower bound will be the same as the upper bound if the key is not in the map
        {
            return it;
        }
        else 
        {
            return it++;  // Return end iterator if key is greater than or equal to all elements using lower bound
        }
    }
    // helper functions (initialization issues)
    void set_children(const int& num) // set the children count to num
    {
        child_count = num;
    }

    void set_data(const int& num) // set the data count to num
    {
        data_count = num;
    }

    int get_count() const
    {
        int count = data_count; // Start with the root node

        // Recursively count nodes in child subtrees
        for (int i = 0; i < child_count; ++i)
        {
            if (!is_leaf())
            {
                count += subset[i]->get_count(); // Recursively count nodes in child subtree
            }
        }

        return count;
    }
    // helper functions for sql
    int get_dc() // return the data count
    {
        return data_count;
    }

    int get_cc() // return the child count
    {
        return child_count;
    }

    T get_data_val(int index) // get the 
    {
        return data[index];
    }


private:
    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;

    int data_count;
    T data[MAXIMUM + 1];
    int child_count;
    BPlusTree* subset[MAXIMUM + 2];
    BPlusTree* next;
    bool dups_ok; //true if duplicate keys may be inserted

    bool is_leaf() const // determine if is a leaf
    {
        return (child_count == 0);
    }

    //insert element functions
    void loose_insert(const T& entry)
    {
        int i = first_ge(data, data_count, entry); // find the index where entry should go
        bool found;
        found = (data[i] == entry); // set found to if data at i(index) is equal to entry
        if(found && is_leaf()) // we have a duplicate
        {
            data[i] = entry;
        }
        else if(found && !is_leaf()) // we have found a breadcrumb so we traverse down the right
        {
            subset[i+1]->loose_insert(entry); // recursively traverse the tree
            if(subset[i+1]->data_count > MAXIMUM)
            {
                fix_excess(i+1); // fix the excess
            }
        }
        else if(!found && is_leaf()) // new entry so we insert the item
        {
            insert_item(data, i, data_count, entry);
        }
        else if(!found && !is_leaf()) // else we continue traversing down the left
        {
            subset[i]->loose_insert(entry);
            if(subset[i]->data_count > MAXIMUM)
            {
                fix_excess(i); // fix the excess
            }
        }
    }

    void fix_excess(int i)
    {
        BPlusTree<T>* node = new BPlusTree<T>();
        // split the subset in excess
        split(subset[i]->data, subset[i]->data_count, node->data, node->data_count);
        // get the item that moves up
        T moving_up;
        detach_item(subset[i]->data, subset[i]->data_count, moving_up);
        // insert the item into data
        int cc = child_count;
        int index = first_ge(data, data_count, moving_up);
        insert_item(data, index, data_count, moving_up);
        child_count = cc;
        // shift the subsets
        for(int k = data_count; k > i + 1; --k)
        {
            subset[k] = subset[k - 1];
        }
        // create the new subtree and place the split array (node) into it
        subset[i + 1] = node;
        if(subset[i]->child_count > 2)
        {
            split(subset[i]->subset,subset[i]->child_count,node->subset,node->child_count);
        }
        //increase child_count
        child_count++;
        if(subset[i]->is_leaf())
        {
            subset[i + 1]->next = subset[i]->next;
            subset[i]->next = subset[i + 1];
        }
        if(subset[i + 1]->is_leaf())
        {
            T dub = data[i];
            insert_item(subset[i + 1]->data, 0, subset[i + 1]->data_count, dub);
        }
    }

    //remove element functions:
    void loose_remove(const T& entry)
    {
        int i = first_ge(data, data_count, entry); // gets the index where entry should be
        bool found = (i < data_count && data[i] == entry);

        if(found && is_leaf()) // if we have found the entry and it is a leaf we delete
        {
            T item;
            delete_item(data, i, data_count, item);
        }
        else if(found && !is_leaf()) // if we find the breadcrumb then we go to the right
        {
            subset[i+1]->loose_remove(entry);
            if(subset[i+1]->data_count < MINIMUM) // determine if we have a shortage
            {
                fix_shortage(i+1);
            }
        }
        else if(!found && is_leaf()) // if the entry is not present we return
        {
            return;
        }
        else if(!found && !is_leaf()) // if we haven't found it but we are not at the leaf we continue down the left
        {
            subset[i]->loose_remove(entry);
            if(subset[i]->data_count < MINIMUM)
            {
                fix_shortage(i);
            }
        }
    }

    void fix_shortage(int i)
    {   // determines which rotate/merge function to call depending on the shortage
        if(i > 0 && subset[i-1]->data_count > MINIMUM) // if the subset to the left is in shortage
        {
            rotate_right(i-1); // rotate right
        }
        else if(i < child_count-1 && subset[i+1]->data_count > MINIMUM) // if the subset to the right is in shortage
        {
            rotate_left(i); // rotate left
        }
        else if(i == 0) // else if we are at the leftmost subset we merge
        {
            merge_with_next_subset(i);
        }
        else // else we merge with the subset to the left
        {
            merge_with_next_subset(i-1);
        }
    }

    void rotate_left(int i)
    {   // this function rotates everything to the left
        T item;
        delete_item(subset[i+1]->data, 0, subset[i+1]->data_count, item);
        attach_item(subset[i]->data, subset[i]->data_count, data[i]);
        data[i] = item;
        if(subset[i+1]->child_count > 0)
        {
            BPlusTree<T>* temp;
            delete_item(subset[i+1]->subset, 0, subset[i+1]->child_count, temp);
            attach_item(subset[i]->subset, subset[i]->child_count, temp);
        }
    }

    void rotate_right(int i)
    {   // this function rotates everything to the right
        T item;
        detach_item(subset[i]->data, subset[i]->data_count, item);
        ordered_insert(subset[i+1]->data, subset[i+1]->data_count, data[i]);
        data[i] = item;
        if(subset[i]->child_count > 0)
        {
            BPlusTree<T>* temp;
            detach_item(subset[i]->subset, subset[i]->child_count, temp);
            ordered_insert(subset[i+1]->subset, subset[i+1]->child_count, temp);
        }
    }

    void merge_with_next_subset(int i)
    {   // this function merges two subsets together
        T item;
        delete_item(data, i, data_count, item);
        merge(subset[i]->data, subset[i]->data_count, subset[i+1]->data, subset[i+1]->data_count);
        merge(subset[i]->subset, subset[i]->child_count, subset[i+1]->subset, subset[i+1]->child_count);
        BPlusTree<T>* temp;
        delete_item(subset, i+1, child_count, temp);
        delete temp;
    }

    void grow_tree()
    {
        BPlusTree<T>* temp = new BPlusTree<T>; // create a temp btree for growing the tree
        copy_array(temp->data, data, temp->data_count, data_count); // copy all the information into temp
        copy_array(temp->subset, subset, temp->child_count, child_count); // copy all subsets into temp
        data_count = 0; // set data count to 0
        child_count = 1; // set child count to 1
        subset[0] = temp; // set the first child to temp
        // now call fix excess on the only child, grows the tree
        fix_excess(0);
    }

    void shrink_tree()
    {   // this function operates similar to grow tree but shrinks the tree instead
        BPlusTree<T>* temp = subset[0];
        copy_array(data, temp->data, data_count, temp->data_count);
        copy_array(subset, temp->subset, child_count, temp->child_count);
        temp->child_count = 0;
        delete temp;
    }
};

#endif // BPLUSTREE_H