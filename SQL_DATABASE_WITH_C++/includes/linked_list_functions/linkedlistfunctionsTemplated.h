#ifndef LINKEDLISTFUNCTIONESTEMPLATE_H
#define LINKEDLISTFUNCTIONESTEMPLATE_H
#include "../node/node.h"

using namespace std;

//Linked List General Functions:
template <typename T>
void _print_list(node<T>* head)
{   
    cout << "Head -> ";
    node<T>* temp = head;
    while(temp != NULL)
    {
        cout << temp->_item << "-> ";
        temp = temp->_next;
    }
    cout << endl;
}

//recursive fun! :)
template <typename T>
void _print_list_backwards(node<T> *head)
{
    cout << "" << endl;
}

//return ptr to key or NULL
template <typename T>
node<T>* _search_list(node<T>* head, T key)
{
    while(head != NULL)
    {
        if(head->_item == key)
        {
            return head;
        } 
        head = head->_next;
    }
    return NULL;
}


template <typename T>
node<T>* _insert_head(node<T> *&head, T insert_this)
{
    node<T>* hd = new node<T>(insert_this, head);
    head = hd;
    return head;
}

//insert after ptr: insert head if marker null
template <typename T>
node<T>* _insert_after(node<T>*& head, node<T> *after_this, T insert_this)
{ 
    if(head->_next == NULL)
    {
        head = new node<T>(insert_this);
    }
    else if(after_this == head)
    {
        _insert_head(head, insert_this);
    }
    else
    {
        node<T>* it = new node<T>(insert_this);
        if(after_this->_next->_next != NULL)
        {
            node<T>* temp = after_this->_next->_next;
            after_this->_next = it;
            it->_next = temp;
        }
        else
        {
            after_this->_next = it;
            it->_next = NULL;
        }
    }
    return after_this->_next;
}

//insert before ptr: insert head if marker null
template <typename T>
node<T>* _insert_before(node<T>*& head,node<T>* before_this,T insert_this)
{
    node<T>* inst = new node<T>(insert_this);
    node<T>* temp = head;
    if(head->_next == NULL)
    {
        head = new node<T>(insert_this);
    }
    else if(before_this == head)
    {
        _insert_head(head, insert_this);
    }
    while(temp->_next != before_this)
    {
        temp = temp->_next;
    }
    temp->_next= inst;
    inst->_next = before_this;
    return inst;
}
//ptr to previous node
template <typename T>
node<T>* _previous_node(node<T>* head, node<T>* prev_to_this)
{
    node<T>* temp = head;
    while(temp->_next != prev_to_this)
    {
        temp = temp->_next;
    }
    return temp;
}

//delete, return item
template <typename T>
T _delete_node(node<T>*& head, node<T>* delete_this)
{
    node<T>* temp = head;
    if(delete_this->_next == NULL)
    {
        while(temp->_next != delete_this)
        {
            temp = temp->_next;
        }
        temp->_next = NULL;
    }
    else
    {
        while(temp->_next != delete_this)
        {
            temp = temp->_next;
        }
        temp->_next = delete_this->_next;
    }
    return delete_this->_item;
}

//duplicate the list...
template <typename T>
node<T>* _copy_list(node<T>* head)
{
    node<T>* copy = _copy_list(head, nullptr);
}

//duplicate the list, return pointer to last node in dest... 
//     use this function in your queue big three 
template <typename T>
node<T> *_copy_list(node<T> *&dest, node<T> *src)
{
    node<T>* copy = new node<T>;
    if(dest == NULL)
    {
        return copy;
    }
    else
    {
        while(dest != src)
        {
            copy->_item = dest->_item;
            copy->_next = dest->_next;

            dest = dest->_next;
        }
    }
    if(src == NULL)
    {
        copy->_next = src;
    }
    else
    {
        copy->_item = src->_item;
        copy->_next = src->_next;
    }

    return dest;
}


//delete all the nodes
template <typename T>
void _clear_list(node<T>*& head)
{
    node<T> temp = head->_next;
    while(temp != NULL)
    {
        delete head;
        head = temp;
        temp = temp->_next;
    }
    delete head;
}

//_item at this position
template <typename T>
T& _at(node<T>* head, int pos)
{
    node<T>* temp = head;
    int count = 0;
    while(count != pos)
    {
        temp = temp->_next;
        count++;
    }
    return temp->_item;
}

#endif