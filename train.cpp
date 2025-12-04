#include "train.h"

TrainCarriage::TrainCarriage(int x, int y) : x(x), y(y), next(nullptr) {}

Train::Train(int startX, int startY) {
    head = new TrainCarriage(startX, startY);
    tail = head;
}

Train::~Train() {
    TrainCarriage* current = head;
    while (current) {
        TrainCarriage* next = current->next;
        delete current;
        current = next;
    }
}

void Train::addCarriage() {
    TrainCarriage* newCarriage = new TrainCarriage(tail->x, tail->y);
    tail->next = newCarriage;
    tail = newCarriage;
}

void Train::move(int newX, int newY) {
    int prevX = head->x;
    int prevY = head->y;

    head->x = newX;
    head->y = newY;

    TrainCarriage* current = head->next;
    while (current) {
        int tempX = current->x;
        int tempY = current->y;

        current->x = prevX;
        current->y = prevY;

        prevX = tempX;
        prevY = tempY;

        current = current->next;
    }
}

bool Train::isOnPosition(int x, int y) const {
    TrainCarriage* current = head;
    while (current) {
        if (current->x == x && current->y == y)
            return true;
        current = current->next;
    }
    return false;
}
