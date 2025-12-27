#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

template <typename T>
class List {
public:
    List() : _front(nullptr), _back(nullptr), _size(0) {}

    // Prevent copying
    List(const List&) = delete;
    List& operator=(const List&) = delete;

    T& front() {
        if (!_front) {
            throw std::runtime_error("Accessing the front of an empty linked list");
        }
        return _front->element;
    }

    T& back() {
        if (!_back) {
            throw std::runtime_error("Accessing the back of an empty linked list");
        }
        return _back->element;
    }

    T& operator[](std::size_t index) {
        if (index >= _size) {
            throw std::out_of_range("Indexing " + std::to_string(index) + " with a list of size " + std::to_string(_size));
        }

        // iterating through the list; the magical part
        // always keep track of the previous node to get to the next node
        Node *curr = _front;
        uintptr_t prev = 0;
        for (std::size_t i = 0; i < index; ++i) {
            // move both pointers forward    
            // note that node->le_xor_thingy = next(node) ^ prev(node)
            // so next(node) = next(node) ^ prev(node) ^ prev(node) = node->le_xor_thingy ^ prev(node)  
            auto next = reinterpret_cast<Node*>(curr->le_xor_thingy ^ prev);
            prev = reinterpret_cast<Node*>(curr);
            curr = next;
        }

        return curr->element;
    }

    const T& operator[](std::size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Indexing " + std::to_string(index) + " with a list of size " + std::to_string(_size));
        }

        Node *curr = _front;
        uintptr_t prev = 0;
        for (std::size_t i = 0; i < index; ++i) {
            auto next = reinterpret_cast<Node*>(curr->le_xor_thingy ^ prev);
            prev = reinterpret_cast<uintptr_t>(curr);
            curr = next;
        }

        return curr->element;
    }

    std::size_t size() const noexcept {
        return _size;
    }    

    bool empty() const noexcept {
        return !(_front && _back);  
    }

    void push_back(const T& element) {
        // if the list is empty
        if (empty()) {
            _front = _back = new Node(element);
            ++_size;
            return;
        } 

        // list is not empty -> _back is not null
        auto newBack = new Node(element);
        
        // point new node's prev ptr to old end
        // note that newNode->le_xor_thingy = old end ^ next Node
        // notice that next node is null (0), and a^0 = a 
        // so this is just a simple assignment
        newBack->le_xor_thingy = reinterpret_cast<uintptr_t>(_back);

        // update old end's next node
        // note that since old end is pointing to null
        _back->le_xor_thingy ^= reinterpret_cast<uintptr_t>(newBack);

        // reassign
        _back = newBack;
    
        // update size
        ++_size;        
    }

    void push_back(T&& element) {
        // if the list is empty
        if (empty()) {
            _front = _back = new Node(std::move(element));
            ++_size;
            return;
        } 

        // list is not empty -> _back is not null
        auto newBack = new Node(std::move(element));
        
        newBack->le_xor_thingy = reinterpret_cast<uintptr_t>(_back);
        _back->le_xor_thingy ^= reinterpret_cast<uintptr_t>(newBack);
        _back = newBack;
        ++_size;        
    }

    void push_front(const T& element) {
        if (empty()) {
            _front = _back = new Node(element); 
            ++_size;
            return;
        }

        // list is not empty -> _front is not null 
        auto newFront = new Node(element);

        newFront->le_xor_thingy = reinterpret_cast<uintptr_t>(_front);

        _front->le_xor_thingy ^= reinterpret_cast<uintptr_t>(newFront);

        _front = newFront;
    
        ++_size;
    }

    void push_front(T&& element) {
        if (empty()) {
            _front = _back = new Node(std::move(element)); 
            ++_size;
            return;
        }

        auto newFront = new Node(std::move(element));
        newFront->le_xor_thingy = reinterpret_cast<uintptr_t>(_front);
        _front->le_xor_thingy ^= reinterpret_cast<uintptr_t>(newFront);
        _front = newFront;
        ++_size;
    }

    void erase(std::size_t index) {
        
        if (index == 0) {
            pop_front();
            return;
        } 
        else if (index == _size - 1) {
            pop_back();
            return;
        }

        auto [ prev, curr ] = iterate(index);         
        auto next = reinterpret_cast<Node*>(curr->le_xor_thingy ^ reinterpret_cast<uintptr_t>(prev));   
    
        uintptr_t cprev = reinterpret_cast<uintptr_t>(prev), 
                    ccurr = reinterpret_cast<uintptr_t>(curr), 
                    cnext = reinterpret_cast<uintptr_t>(next);

        prev->le_xor_thingy ^= ccurr ^ cnext;        
        next->le_xor_thingy ^= ccurr ^ cprev;

        delete curr;

        --_size;

        if (_size == 0) {
            _front = _back = nullptr;
        }
    }

    void insert(const T& data, std::size_t index) {
        
        if (index == 0) {
            push_front(data);
            return;
        }
        else if (index == _size) {
            push_back(data);
            return;
        }

        auto [ prev, curr ] = iterate(index);
           
        uintptr_t cprev = reinterpret_cast<uintptr_t>(prev), ccurr = reinterpret_cast<uintptr_t>(curr);

        auto nnode = new Node(data);
        nnode->le_xor_thingy = cprev ^ ccurr;

        uintptr_t cnew = reinterpret_cast<uintptr_t>(nnode);

        // insert a new node in between prev and curr
        prev->le_xor_thingy ^= ccurr ^ cnew;
        curr->le_xor_thingy ^= cprev ^ cnew;
    
        ++_size;
    }

    void insert(T&& data, std::size_t index) {
        
        if (index == 0) {
            push_front(std::move(data));
            return;
        }
        else if (index == _size) {
            push_back(std::move(data));
            return;
        }

        auto [ prev, curr ] = iterate(index);
           
        uintptr_t cprev = reinterpret_cast<uintptr_t>(prev), ccurr = reinterpret_cast<uintptr_t>(curr);

        auto nnode = new Node(std::move(data));
        nnode->le_xor_thingy = cprev ^ ccurr;

        uintptr_t cnew = reinterpret_cast<uintptr_t>(nnode);

        prev->le_xor_thingy ^= ccurr ^ cnew;
        curr->le_xor_thingy ^= cprev ^ cnew;
    
        ++_size;
    }

    void pop_front() {
        if (empty()) {
            throw std::runtime_error("Popping off of an empty linked list");
        }

        Node* newFront = reinterpret_cast<Node*>(_front->le_xor_thingy);
        if (newFront)
            newFront->le_xor_thingy ^= reinterpret_cast<uintptr_t>(_front);

        delete _front;

        _front = newFront;

        --_size;

        if (_size == 0) {
            _front = _back = nullptr;
        }
    }
    
    void pop_back() {
        if (empty()) {
            throw std::runtime_error("Popping off of an empty linked list");
        }
        
        Node* newBack = reinterpret_cast<Node*>(_back->le_xor_thingy);
        if (newBack)
            newBack->le_xor_thingy ^= reinterpret_cast<uintptr_t>(_back);

        delete _back;

        _back = newBack;

        --_size;

        if (_size == 0) {
            _front = _back = nullptr;
        }
    }

    void clear() {
        delete_everything();
        _front = _back = nullptr;
    }

    ~List() {
        delete_everything(); 
    }

private:
    struct Node {
        T element;
        uintptr_t le_xor_thingy; // I don't know how to name this 
        
        Node(const T& element) : element(element), le_xor_thingy(0) {}
    };

    void delete_everything() {
            
        while (!empty()) {
            pop_back();
        }

    }

    std::pair<Node*, Node*> iterate(std::size_t index) {
        if (index >= _size) {
            throw std::out_of_range("Indexing " + std::to_string(index) + " from a list of size " + std::to_string(_size));
        }

        uintptr_t prev = 0;
        Node* curr = _front;

        for (std::size_t i = 0; i < index; ++i) {
            auto next = reinterpret_cast<Node*>(curr->le_xor_thingy ^ prev);
            prev = reinterpret_cast<uintptr_t>(curr);
            curr = next;
        }

        return { reinterpret_cast<Node*>(prev), curr };
    }

    Node* _front, *_back;
    std::size_t _size;
};
