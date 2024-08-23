#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../node/node.h"

template <typename T>
class Queue {
public:
    class Iterator {
    public:
        friend class Queue;
        Iterator() : _ptr(nullptr) {}
        Iterator(node<T>* p) : _ptr(p) {}

        T operator*() const {
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
        node<T>* _ptr;
    };

    Queue() : _front(nullptr), _rear(nullptr), _size(0) {}

    Queue(const Queue<T>& copyMe) : _front(nullptr), _rear(nullptr), _size(0) {
        try {
            node<T>* temp = copyMe._front;
            while (temp) {
                push(temp->_item);
                temp = temp->_next;
            }
        } catch (std::exception& e) {
            cerr << "Exception during copy constructor" << endl;
            throw; // Re-throw the exception after cleanup
        }
    }

    ~Queue() {
        try {
            while (_front) {
                node<T>* temp = _front;
                _front = _front->_next;
                delete temp;
            }
            _rear = nullptr;
            _size = 0;
        } catch (std::exception& e) {
            std::cerr << "Exception during destruction: " << e.what() << std::endl;
        }
    }

    Queue& operator=(const Queue<T>& RHS) {
        if (this != &RHS) {
            try {
                while (_front) {
                    node<T>* temp = _front;
                    _front = _front->_next;
                    delete temp;
                }
                _rear = nullptr;
                _size = 0;
                
                node<T>* temp = RHS._front;
                while (temp) {
                    push(temp->_item);
                    temp = temp->_next;
                }
            } catch (std::exception& e) {
                std::cerr << "Exception during assignment: " << e.what() << std::endl;
            }
        }
        return *this;
    }

    bool empty() const {
        return _size == 0;
    }

    T front() const {
        if (_front == nullptr) {
            throw std::out_of_range("Queue<T>::front(): queue is empty");
        }
        return _front->_item;
    }

    T back() const {
        if (_rear == nullptr) {
            throw std::out_of_range("Queue<T>::back(): queue is empty");
        }
        return _rear->_item;
    }

    void push(T item) {
        try {
            node<T>* newNode = new node<T>(item);
            if (_rear == nullptr) {
                _front = _rear = newNode;
            } else {
                _rear->_next = newNode;
                _rear = newNode;
            }
            _size++;
        } catch (std::exception& e) {
            std::cerr << "Exception during push: " << e.what() << std::endl;
            throw; // Re-throw the exception
        }
    }

    T pop() {
        if (_front == nullptr) {
            throw std::out_of_range("Queue<T>::pop(): queue is empty");
        }
        T poppedItem;
        try {
            node<T>* temp = _front;
            poppedItem = _front->_item;
            _front = _front->_next;
            if (_front == nullptr) {
                _rear = nullptr;
            }
            delete temp;
            _size--;
        } catch (std::exception& e) {
            std::cerr << "Exception during pop: " << e.what() << std::endl;
            throw; // Re-throw the exception
        }
        return poppedItem;
    }

    Iterator begin() const {
        return Iterator(_front);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    int size() const { return _size; }

    friend std::ostream& operator<<(std::ostream& outs, const Queue& printMe) {
        node<T>* current = printMe._front;
        outs << "Queue:Head -> ";
        while (current) {
            outs << current->_item << " ";
            current = current->_next;
        }
        outs << "|||";
        return outs;
    }

private:
    node<T>* _front;
    node<T>* _rear;
    int _size;

    void swap(Queue<T>& other) noexcept {
        using std::swap;
        swap(_front, other._front);
        swap(_rear, other._rear);
        swap(_size, other._size);
    }
};

#endif
