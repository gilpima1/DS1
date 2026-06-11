// You can edit anything you want in this file.
// However you need to implement all public StudentCourseManager function, as provided below as a template

#include "SegmentationResort26b1.h"


SegmentationResort::SegmentationResort() : currentMealIndex(0), firstRoom(nullptr), lastRoomCleaned(nullptr),
                                           lastRoomIsClean(false) {
}

SegmentationResort::~SegmentationResort()
{
    guests.clearAllData();
    rooms.clearAllData();
    tables.clearAllData();
}

StatusType SegmentationResort::checkIn(int geustId, int roomNum)
{
    if (geustId <= 0 || roomNum <= 0) {
        return StatusType::INVALID_INPUT;
    }

    bool guestAlreadyExist = guests.find(geustId) != nullptr;
    if (guestAlreadyExist) {
        return StatusType::FAILURE;
    }

    bool roomAlreadyExist = rooms.find(roomNum) != nullptr;
    if (roomAlreadyExist) {
        return StatusType::FAILURE;
    }

    //TODO: next and previous (for both room and guest) need to be set at the AVL tree level (where the parent node is known)
    Room* newRoom = nullptr;
    Guest* newGuest = nullptr;

    try {
        newRoom = new Room(roomNum);
    }catch (const std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    try {
        newGuest = new Guest(geustId, roomNum);
    }catch (const std::bad_alloc& e) {
        delete newRoom;
        return StatusType::ALLOCATION_ERROR;
    }

    //TODO: AVL insert doesnt return the new node, redundant find calls to check if exist etc.
    // maybe return the added node in the insert method and then we can just check if it is equal to null to check
    // if the insert failed.
    // also, need to update the room sorted list, cannot do that outside of the avl tree (knowing the parent node)
    try {
        rooms.insert(roomNum, newRoom);
    } catch (const std::bad_alloc& e) {
        delete newRoom;
        delete newGuest;
        return StatusType::ALLOCATION_ERROR;
    }

    try {
        guests.insert(geustId, newGuest);
    } catch (const std::bad_alloc& e) {
        rooms.remove(roomNum);
        delete newRoom;
        delete newGuest;
        return StatusType::ALLOCATION_ERROR;
    }

    if (firstRoom == nullptr) {
        firstRoom = newRoom;
        newRoom->next = nullptr;
        newRoom->previous = nullptr;
    } else if (roomNum < firstRoom->roomNumber) {
        newRoom->next = firstRoom;
        newRoom->previous = nullptr;
        firstRoom->previous = newRoom;
        firstRoom = newRoom;
    } else {
        auto predNode = rooms.findPredecessor(roomNum);
        Room* pred = predNode->data;
        newRoom->next = pred->next;
        newRoom->previous = pred;
        if (pred->next != nullptr) {
            pred->next->previous = newRoom;
        }
        pred->next = newRoom;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::checkOut(int geustId)
{
    if (geustId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    auto guestNode = guests.find(geustId);
    bool guestNotExist = guestNode == nullptr;
    if (guestNotExist) {
        return StatusType::FAILURE;
    }

    auto guest = guestNode->data;
    bool guestInDiningRoom = guest->activeTable != nullptr;
    if (guestInDiningRoom) {
        return StatusType::FAILURE;
    }

    //TODO: keep pointer at guest to room instead of just room num?
    auto room = rooms.find(guest->roomNum)->data;
    if (room == firstRoom) {
        firstRoom = room->next;
    }

    if (lastRoomCleaned == room) {
        lastRoomCleaned = room->previous;
    }

    if (room->next != nullptr && room->previous != nullptr) {
        room->next->previous = room->previous;
        room->previous->next = room->next;
    }else {
        if (room->next != nullptr) {
            room->next->previous = nullptr;
        }
        if (room->previous != nullptr) {
            room->previous->next = nullptr;
        }
    }
    rooms.remove(guest->roomNum);
    guests.remove(geustId);

    delete room;
    delete guest;

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::addTable(int tableId, int capacity)
{
    if (tableId <= 0 || capacity <= 0) {
        return StatusType::INVALID_INPUT;
    }

    bool tableAlreadyExist = tables.find(tableId) != nullptr;
    if (tableAlreadyExist) {
        return StatusType::FAILURE;
    }

    Table* newTable = nullptr;
    try {
        newTable = new Table(tableId, capacity);
        tables.insert(tableId, newTable);
    }catch (const std::bad_alloc& e) {
        delete newTable;
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::removeTable(int tableId)
{
    if (tableId <=0) {
        return StatusType::INVALID_INPUT;
    }

    auto tableNode = tables.find(tableId);
    bool tableNotExist = tableNode == nullptr;
    if (tableNotExist) {
        return StatusType::FAILURE;
    }

    auto table = tableNode->data;
    if (table->currentCount > 0) {
        return StatusType::FAILURE;
    }

    delete table;
    tables.remove(tableId);
    return StatusType::SUCCESS;
}

StatusType SegmentationResort::enterDiningRoom(int guestId, int tableId)
{
    if (guestId <= 0 || tableId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    auto tableNode = tables.find(tableId);
    bool tableNotExist = tableNode == nullptr;
    if (tableNotExist) {
        return StatusType::FAILURE;
    }

    auto table = tableNode->data;

    if (table->currentCount == table->capacity) {
        return StatusType::FAILURE;

    }

    auto guestNode = guests.find(guestId);
    bool guestNotExist = guestNode == nullptr;

    if (guestNotExist) {
        return StatusType::FAILURE;
    }

    // this includes the scenario that the guest is currently at the dining room
    if (guestNode->data->lastMeal == currentMealIndex) {
        return StatusType::FAILURE;
    }

    auto guest = guestNode->data;

    if (guest->activeTable != nullptr) {
        return StatusType::FAILURE;
    }

    try {
        table->Guests.insert(guest->GuestID, guest);
    } catch (const std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }

    guestNode->data->activeTable = table;
    table->currentCount++;
    guest->next = table->head;
    guest->previous = nullptr;
    if (table->head != nullptr) {

        table->head->previous = guest;

    }
    table->head = guest;
    guestNode->data->lastMeal = currentMealIndex;

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::leaveDiningRoom(int guestId, int tableId)
{
    if (guestId <= 0 || tableId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    auto tableNode = tables.find(tableId);
    bool tableNotExist = tableNode == nullptr;
    if (tableNotExist) {
        return StatusType::FAILURE;
    }

    auto table = tableNode->data;

    auto guestNode = table->Guests.find(guestId);
    bool guestNotExist = guestNode == nullptr;
    if (guestNotExist) {
        return StatusType::FAILURE;
    }

    auto guest = guestNode->data;

    guest->activeTable = nullptr;
    table->currentCount--;

    //maintain the guest linked list.
    if (guest->previous != nullptr) {
        guest->previous->next = guest->next;
    } else {
        table->head = guest->next;
    }

    if (guest->next != nullptr) {
        guest->next->previous = guest->previous;
    }
    guest->lastMeal = currentMealIndex;
    table->Guests.remove(guestId);

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::reheatFood()
{
    currentMealIndex++;
    return StatusType::SUCCESS;
}

StatusType SegmentationResort::joinTables(int tableId1, int tableId2)
{
    //check initial conditions
    if (tableId1 <= 0 || tableId2 <= 0 || tableId1 == tableId2) {
        return StatusType::INVALID_INPUT;
    }

    auto table1Node = tables.find(tableId1);
    auto table2Node = tables.find(tableId2);

    if (table1Node == nullptr || table2Node == nullptr) {
        return StatusType::FAILURE;
    }

    auto table1 = table1Node->data;
    auto table2 = table2Node->data;

    //update values
    table1->capacity += table2->capacity;

    //create ordered Guest array
    Guest** table1Array = nullptr;
    try {
        table1Array = new Guest*[table1->currentCount];
    } catch (const std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    table1->Guests.storeInOrder(table1Array);

    Guest** table2Array = nullptr;
    try {
        table2Array = new Guest*[table2->currentCount];
    } catch (const std::bad_alloc& e) {
        delete[] table1Array;
        return StatusType::ALLOCATION_ERROR;
    }
    table2->Guests.storeInOrder(table2Array);

    Guest** tableArray = nullptr;
    int totalCount = table1->currentCount + table2->currentCount;
    try {
        tableArray = new Guest*[totalCount];
    } catch (const std::bad_alloc& e) {
        delete[] table1Array;
        delete[] table2Array;
        return StatusType::ALLOCATION_ERROR;
    }

    int index1 = 0;
    int index2 = 0;

    for (int i=0; i<totalCount; i++) {
        if (index1 >= table1->currentCount) {
            tableArray[i] = table2Array[index2];
            index2++;
        } else if (index2 >= table2->currentCount) {
            tableArray[i] = table1Array[index1];
            index1++;
        } else if (table1Array[index1]->GuestID < table2Array[index2]->GuestID) {
            tableArray[i] = table1Array[index1];
            index1++;
        } else {
            tableArray[i] = table2Array[index2];
            index2++;
        }
    }

    int* keysArray = nullptr;
    try {
        keysArray = new int[totalCount];
    } catch (const std::bad_alloc& e) {
        delete[] table1Array;
        delete[] table2Array;
        delete[] tableArray;
        return StatusType::ALLOCATION_ERROR;
    }

    for (int i = 0; i < totalCount; i++) {
        keysArray[i] = tableArray[i]->GuestID;
        tableArray[i]->activeTable = table1;
    }

    if (totalCount > 0) {
        table1->Guests.buildFromArrays(keysArray, tableArray, totalCount);
    }

    if (table1->head == nullptr) {
        table1->head = table2->head;
    } else if (table2->head != nullptr) {
        Guest* guestPtr = table1->head;
        while (guestPtr->next != nullptr) {
            guestPtr = guestPtr->next;
        }
        guestPtr->next = table2->head;
        table2->head->previous = guestPtr;
    }

    table1->currentCount = totalCount;
    tables.remove(tableId2);

    delete table2;
    delete[] keysArray;
    delete[] table1Array;
    delete[] table2Array;
    delete[] tableArray;

    return StatusType::SUCCESS;
}

output_t<int> SegmentationResort::joinFriend(int guestId1, int guestId2)
{
    if (guestId1 <= 0 || guestId2 <= 0 || guestId1 == guestId2) {
        return StatusType::INVALID_INPUT;
    }

    auto guest1Node = guests.find(guestId1);
    auto guest2Node = guests.find(guestId2);

    if (guest1Node == nullptr || guest2Node == nullptr) {
        return StatusType::FAILURE;
    }

    auto guest1 = guest1Node->data;
    auto guest2 = guest2Node->data;

    if (guest2->activeTable == nullptr || guest2->activeTable->currentCount == guest2->activeTable->capacity) {
        return StatusType::FAILURE;
    }
    if (guest1->activeTable != nullptr || guest1->lastMeal == currentMealIndex) {
        return  StatusType::FAILURE;
    }

    auto table = guest2->activeTable;

    try {
        table->Guests.insert(guest1->GuestID, guest1);
    } catch (const std::bad_alloc& e) {
         return StatusType::ALLOCATION_ERROR;
    }

    guest1->activeTable = table;
    table->currentCount++;

    guest1->next = table->head;
    guest1->previous = nullptr;
    if (table->head != nullptr) {
        table->head->previous = guest1;
    }
    table->head = guest1;

    guest1->lastMeal = currentMealIndex;

    return table->tableID;
}

output_t<int> SegmentationResort::cleanNextRoom()
{
    if (firstRoom == nullptr) {
        return StatusType::FAILURE;
    }

    if (lastRoomCleaned != nullptr) {
        lastRoomCleaned = lastRoomCleaned->next;
        if (lastRoomCleaned == nullptr) {
            lastRoomCleaned = firstRoom;
        }
        return lastRoomCleaned->roomNumber;
    }

    lastRoomCleaned = firstRoom;
    return firstRoom->roomNumber;
}