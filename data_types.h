struct Table;

struct Guest {
    int GuestID;
    int roomNum;
    Table* activeTable;
    Guest* next;
    Guest* previous;
    int lastMeal;

    Guest(int GuestID, int roomNum, Table* activeTable = nullptr, Guest* next = nullptr, Guest* previous = nullptr, int lastMeal = -1) :
    GuestID(GuestID), roomNum(roomNum), activeTable(activeTable), next(next), previous(previous), lastMeal(lastMeal){}
};

struct Table {
    int tableID;
    int capacity;
    int currentCount;
    AVL<int, Guest*> Guests;
    Guest* head;

    Table(int tableID, int capacity) :
    tableID(tableID), capacity(capacity), currentCount(0), head(nullptr){}
};

struct Room {
    int roomNumber;
    Room* next;

    Room(int roomNumber) : roomNumber(roomNumber), next(nullptr){}
};