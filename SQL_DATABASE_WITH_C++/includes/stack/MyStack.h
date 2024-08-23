#ifndef MTSTACK_H
#define MTSTACK_H

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../node/node.h"

template <typename ITEM_TYPE>
class Stack {
public:
    class Iterator {
    public:
        friend class Stack;
        Iterator() : _ptr(nullptr) {}
        Iterator(node<ITEM_TYPE>* p) : _ptr(p) {}

        ITEM_TYPE operator*() const {
            assert(_ptr != nullptr);
            return _ptr->_item;
        }

        bool is_null() const { return _ptr == nullptr; }

        friend bool operator!=(const Iterator& left, const Iterator& right) {
            return left._ptr != right._ptr;
        }

        friend bool operator==(const Iterator& left, const Iterator& right) {
            return left._ptr == right._ptr;
        }

        Iterator& operator++() {
            assert(_ptr != nullptr);
            _ptr = _ptr->_next;
            return *this;
        }

        Iterator operator++(int) {
            assert(_ptr != nullptr);
            Iterator temp = *this;
            _ptr = _ptr->_next;
            return temp;
        }

    private:
        node<ITEM_TYPE>* _ptr;
    };

    Stack() : _top(nullptr), _size(0) {}

    Stack(const Stack<ITEM_TYPE>& copyMe) : _top(nullptr), _size(0) {
        try {
            if (copyMe._top) {
                node<ITEM_TYPE>* source = copyMe._top;
                _top = new node<ITEM_TYPE>(source->_item);
                node<ITEM_TYPE>* dest = _top;
                source = source->_next;
                while (source) {
                    dest->_next = new node<ITEM_TYPE>(source->_item);
                    dest = dest->_next;
                    source = source->_next;
                }
                _size = copyMe._size;
            }
        } catch (std::exception& e) {
            std :: cerr << "Exception during copy constructor" << e.what() << endl;
            clear();
            throw; // Re-throw the exception after cleanup
        }
    }

    ~Stack() {
        try {
            while (_top) {
                node<ITEM_TYPE>* temp = _top;
                _top = _top->_next;
                delete temp;
            }
        } catch (std::exception& e) {
            std::cerr << "Exception during destruction: " << e.what() << std::endl;
        }
    }

    Stack<ITEM_TYPE>& operator=(const Stack<ITEM_TYPE>& RHS) {
        if (this != &RHS) {
            try {
                if (RHS._top) {
                    node<ITEM_TYPE>* source = RHS._top;
                    _top = new node<ITEM_TYPE>(source->_item);
                    node<ITEM_TYPE>* dest = _top;
                    source = source->_next;
                    while (source) {
                        dest->_next = new node<ITEM_TYPE>(source->_item);
                        dest = dest->_next;
                        source = source->_next;
                    }
                    _size = RHS._size;
                }
            } catch (std::exception& e) {
                std::cerr << "Exception during assignment: " << e.what() << std::endl;
            }
        }
        return *this;
    }

    ITEM_TYPE top() const {
        if (_top == nullptr) {
            throw std::out_of_range("Stack<ITEM_TYPE>::top(): stack is empty");
        }
        return _top->_item;
    }

    bool empty() const {
        return _top == nullptr;
    }

    void push(ITEM_TYPE item) {
        try {
            _top = new node<ITEM_TYPE>(item, _top);
            _size++;
        } catch (std::exception& e) {
            std::cerr << "Exception during push: " << e.what() << std::endl;
            throw; // Re-throw the exception
        }
    }

    ITEM_TYPE pop() {
        if (_top == nullptr) {
            throw std::out_of_range("Stack<ITEM_TYPE>::pop(): stack is empty");
        }
        ITEM_TYPE poppedItem;
        try {
            node<ITEM_TYPE>* temp = _top;
            poppedItem = _top->_item;
            _top = _top->_next;
            delete temp;
            _size--;
        } catch (std::exception& e) {
            std::cerr << "Exception during pop: " << e.what() << std::endl;
            throw; // Re-throw the exception
        }
        return poppedItem;
    }

    friend std::ostream& operator<<(std::ostream& outs, const Stack& printMe) {
        node<ITEM_TYPE>* current = printMe._top;
        outs << "Stack:Head -> ";
        while (current) {
            outs << current->_item << " ";
            current = current->_next;
        }
        outs << "|||";
        return outs;
    }

    Iterator begin() const {
        return Iterator(_top);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    int size() const { return _size; }

private:
    node<ITEM_TYPE>* _top;
    int _size;

    void clear() {
        while (_top) {
            node<ITEM_TYPE>* temp = _top;
            _top = _top->_next;
            delete temp;
        }
        _size = 0;
    }

};

#endif