#include "xor_list.h"
#include <cstddef>
#include <iostream>

void printList(const List<int>& list) {
    for (std::size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i] << ' ';
    }
    std::cout << '\n';
}

void printListReverse(const List<int>& list) {
    for (std::size_t i = list.size(); i > 0; --i) {
        std::cout << list[i-1] << ' ';
    }
    std::cout << '\n';
}

void printAtIndex(const List<int>& list) {
    std::size_t index;
    
    while (1) {
        try {
            std::cout << "Select an index\n> ";
            std::cin >> index;
            
            int data = list[index];
            std::cout << "The element is " << data << '\n';
            return;
        } catch (std::out_of_range const&) {
            std::cerr << "Index is out of range, please try again\n";
        }
    }
}

void addFront(List<int>& list) {
    int data;
    std::cout << "Provide the data you want to insert\n> ";
    std::cin >> data;
    list.push_front(data);
    std::cout << "Data is inserted successfully\n";
}

void addBack(List<int>& list) {
    int data;
    std::cout << "Provide the data you want to insert\n> ";
    std::cin >> data;
    list.push_back(data);
    std::cout << "Data is inserted successfully\n";
}

void insertData(List<int>& list) {
    std::size_t index;
    
    while (1) {
        std::cout << "Select an index\n> ";
        std::cin >> index;
        try {
            int data;
            std::cout << "Provide the data you want to insert\n> ";
            std::cin >> data;
            list.insert(data, index);
            std::cout << "Data is inserted successfully\n";
            return;
        } catch (std::out_of_range const&) {
            std::cerr << "Index is out of range, please try again\n";
        }
    }
}

void removeFront(List<int>& list) {
    try {
        list.pop_front();
        std::cout << "Front element removed successfully\n";
    } catch (std::runtime_error const& e) {
        std::cerr << e.what() << '\n';
    }
}

void removeBack(List<int>& list) {
    try {
        list.pop_back();
        std::cout << "Back element removed successfully\n";
    } catch (std::runtime_error const& e) {
        std::cerr << e.what() << '\n';
    }
}

void removeAtIndex(List<int>& list) {
    std::size_t index;
    
    while (1) {
        std::cout << "Select an index\n> ";
        std::cin >> index;
        try {
            list.erase(index);
            std::cout << "Element at index " << index << " removed successfully\n";
            return;
        } catch (std::out_of_range const&) {
            std::cerr << "Index is out of range, please try again\n";
        }
    }
}

std::size_t get_cmd() {
    
    std::size_t cmd;

    do {
        std::cout << "Welcome to some weird \"linked list\" demo, select one of the following commands:\n";
        std::cout << "\t1. print the current list\n";
        std::cout << "\t2. print the current list in reverse\n";
        std::cout << "\t3. print out a list element at a specific index\n";
        std::cout << "\t4. add a new element at the front of the list\n";
        std::cout << "\t5. add a new element at the back of the list\n";
        std::cout << "\t6. add a new element at a specific index\n";
        std::cout << "\t7. remove the element at the front\n";
        std::cout << "\t8. remove the element at the back\n";
        std::cout << "\t9. remove the element at a specific index\n";
        std::cout << "\t10. get the list size\n";
        std::cout << "\t11. see if the list is empty or not\n";
        std::cout << "\t12. exit program\n";
        std::cout << "> ";
    
        std::cin >> cmd; 

        if (cmd > 12) {
            std::cerr << "Invalid input; please try again\n";
        }

    } while (cmd > 12);

    return cmd; 
}

void runProgram() {
    List<int> list;

    while (1) {
        std::size_t cmd = get_cmd();
        if (cmd == 12) {
            break;
        }

        switch (cmd) {
            case 1:
                printList(list);
                break;
            case 2:
                printListReverse(list);
                break;
            case 3:
                printAtIndex(list);
                break;
            case 4:
                addFront(list);
                break;
            case 5:
                addBack(list);
                break;    
            case 6:
                insertData(list);
                break;
            case 7:
                removeFront(list);
                break;
            case 8:
                removeBack(list);
                break;
            case 9:
                removeAtIndex(list);
                break;
            case 10:
                std::cout << "Size of the list is " << list.size() << '\n';
                break;
            case 11:
                std::cout << "List is currently " << ((list.empty()) ? "empty\n" : "not empty\n");
                break;
            default:
                break;
        }         
    }

}

int main (int argc, char *argv[]) {

    runProgram(); 

    return 0;
}
