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
    rooms.insert(roomNum, newRoom);
    guests.insert(geustId, newGuest);
    return StatusType::SUCCESS;
}

StatusType SegmentationResort::checkOut(int geustId)
{
    if (geustId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    bool guestNotExist = guests.find(geustId) == nullptr;
    if (guestNotExist) {
        return StatusType::FAILURE;
    }

    auto guest = guests.find(geustId)->data;
    bool guestInDiningRoom = guest->activeTable != nullptr;
    if (guestInDiningRoom) {
        return StatusType::FAILURE;
    }

    rooms.remove(guest->roomNum);
    guests.remove(geustId);

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::addTable(int tableId, int capacity)
{
    return StatusType::FAILURE;
}

StatusType SegmentationResort::removeTable(int tableId)
{
    return StatusType::FAILURE;
}

StatusType SegmentationResort::enterDiningRoom(int guestId, int tableId)
{
    return StatusType::FAILURE;
}

StatusType SegmentationResort::leaveDiningRoom(int guestId, int tableId)
{
    return StatusType::FAILURE;
}

StatusType SegmentationResort::reheatFood()
{
    return StatusType::FAILURE;
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
    return 0;
}