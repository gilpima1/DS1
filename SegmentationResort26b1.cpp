// You can edit anything you want in this file.
// However you need to implement all public StudentCourseManager function, as provided below as a template

#include "SegmentationResort26b1.h"


SegmentationResort::SegmentationResort() : currentMealIndex(0), firstRoom(nullptr), lastRoomCleaned(nullptr),
                                           lastRoomIsClean(false) {
}

SegmentationResort::~SegmentationResort()
{

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
    Room* newRoom;
    Guest* newGuest;
    try {
        newRoom = new Room(roomNum);
    }catch (std::bad_alloc e) {
        return StatusType::ALLOCATION_ERROR;
    }
    try {
        newGuest = new Guest(geustId, roomNum);
    }catch (std::bad_alloc e) {
        delete newRoom;
        return StatusType::ALLOCATION_ERROR;
    }

    //TODO: AVL insert doesnt return the new node, redundant find calls to check if exist etc.
    // maybe return the added node in the insert method and then we can just check if it is equal to null to check
    // if the insert failed.
    // also, need to update the room sorted list, cannot do that outside of the avl tree (knowing the parent node)
    rooms.insert(roomNum, newRoom);
    guests.insert(geustId, newGuest);
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
        lastRoomCleaned = room->next;
        lastRoomIsClean = false;
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

    Table* newTable;
    try {
        newTable = new Table(tableId, capacity);
    }catch (std::bad_alloc e) {
        return StatusType::ALLOCATION_ERROR;
    }

    tables.insert(tableId, newTable);
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

    guestNode->data->activeTable = table;
    table->currentCount++;
    auto guest = guestNode->data;
    table->Guests.insert(guest->GuestID, guest);
    if (table->head == nullptr) {
        table->head = guest;
    }else {
        auto ptr = table->head;
        while (ptr->next != nullptr) {
            if (ptr->next == nullptr) {
                ptr->next = guest;
                guest->previous = ptr;
                break;
            }
            ptr = ptr->next;
        }
    }

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
    auto guestNode = guests.find(guestId);
    bool guestNotExist = guestNode == nullptr;
    if (guestNotExist) {
        return StatusType::FAILURE;
    }

    auto table = tableNode->data;

    bool guestNotAtTable = table->Guests.find(guestId) == nullptr;
    if (guestNotAtTable) {
        return StatusType::FAILURE;
    }

    guestNode->data->activeTable = nullptr;
    table->currentCount--;
    table->Guests.remove(guestId);

    //maintain the guest linked list.
    auto ptr = table->head;
    while (ptr != nullptr) {
        if (ptr == guestNode->data) {
            if (guestNode->data->next != nullptr) {
                guestNode->data->next->previous = guestNode->data->previous;
            }
            if (guestNode->data->previous != nullptr) {
                guestNode->data->previous->next = guestNode->data->next;
            }
            break;
        }
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::reheatFood()
{
    currentMealIndex++;
    return StatusType::SUCCESS;
}

StatusType SegmentationResort::joinTables(int tableId1, int tableId2)
{
    return StatusType::FAILURE;
}

output_t<int> SegmentationResort::joinFriend(int guestId1, int guestId2)
{
    return 0;
}

output_t<int> SegmentationResort::cleanNextRoom()
{
    if (lastRoomCleaned != nullptr) {
        //the pointer to lastRoom was updated (the room was removed) and so the current room wasn't cleaned.
        if (!lastRoomIsClean) {
            lastRoomIsClean = true;
            return StatusType::SUCCESS;
        }

        //clean current room and set to next
        lastRoomIsClean = true;
        lastRoomCleaned = lastRoomCleaned->next;
        return StatusType::SUCCESS;
    }

    //no rooms
    if (firstRoom == nullptr) {
        return StatusType::FAILURE;
    }

    lastRoomCleaned = firstRoom;
    lastRoomIsClean = true;
    //TODO: I DIDNT GET WHAT SHOULD BE RETURNED HERE, SUCCESS?
    return firstRoom->roomNumber;
}