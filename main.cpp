#include <iostream>
#include "AVL.h"
#include "data_types.h"

int main() {







    //TEST CODE//

    AVL<int, Guest*> guests;
    AVL<int, Room*> rooms;
    AVL<int, Table*> tables;

    guests.insert(1, new Guest(1, 11));
    guests.insert(2, new Guest(2, 12));
    guests.insert(3, new Guest(3, 13));

    Node<int, Guest*>* res = guests.find(2);
    if (res != nullptr) {
        std::cout << "guest found\n";
    }

    res = guests.find(4);
    if (res == nullptr) {
        std::cout << "guest does not exist\n";
    }

    guests.remove(2);
    res = guests.find(2);
    if (res == nullptr) {
        std::cout << "guest removed successfully\n";
    }

    rooms.insert(1, new Room(1));
    tables.insert(1, new Table(1, 10));

    if (rooms.find(1) != nullptr) {
        std::cout << "room found\n";
    }
    if (tables.find(1) != nullptr) {
        std::cout << "table found\n";
    }

    //TEST CODE//







    return 0;
}