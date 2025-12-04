#ifndef TRAIN_H
#define TRAIN_H

class TrainCarriage {
public:
    int x, y;
    TrainCarriage* next;

    TrainCarriage(int x, int y);
};

class Train {
public:
    TrainCarriage* head;
    TrainCarriage* tail;

    Train(int startX, int startY);
    ~Train();

    void addCarriage();
    void move(int newX, int newY);
    bool isOnPosition(int x, int y) const;
};
bool placeCargo(int& cargoX, int& cargoY, Train& train, int gridWidth, int gridHeight);

#endif
