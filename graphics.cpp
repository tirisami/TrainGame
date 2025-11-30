#include "graphics.h"
#include "train.h"
#include <cmath>
#include <cstdlib>
#include <vector>

void drawSmoke(int x, int y) {
    float t = GetTime() * 2;
    for (int i = 0; i < 5; i++) {
        float offsetY = fmod(t * 40 + i * 30, 60);
        float alpha = 0.5f - (offsetY / 60.0f);
        float radius = 6.0f - (offsetY / 15.0f);
        DrawCircleGradient(x + 10 + (sin(t + i) * 4), y - offsetY, radius,
                           Fade(LIGHTGRAY, alpha), Fade(WHITE, 0));
    }
}

void drawHills(int screenWidth, int screenHeight, float time) {
    Color hillColors[3] = { {50, 100, 30, 255}, {30, 80, 20, 255}, {20, 60, 15, 255} };
    int hillHeights[3] = { 150, 100, 70 };
    float hillSpeeds[3] = { 10.0f, 20.0f, 40.0f };

    for (int layer = 0; layer < 3; layer++) {
        float offset = fmod(time * hillSpeeds[layer], screenWidth + 200);
        int baseY = screenHeight - hillHeights[layer];
        int step = 100;

        for (int x = -200; x < screenWidth + 200; x += step) {
            int hillPeakX = (int)(x - offset);
            Vector2 points[3] = {
                { (float)(hillPeakX), (float)(baseY) },
                { (float)(hillPeakX + step / 2), (float)(baseY - hillHeights[layer]) },
                { (float)(hillPeakX + step), (float)(baseY) }
            };
            DrawTriangle(points[0], points[1], points[2], hillColors[layer]);
        }
    }
}

void drawClouds(int screenWidth, int screenHeight, float time) {
    float cloudSpeed = 30.0f;
    int cloudY = screenHeight / 4;

    for (int i = 0; i < 5; i++) {
        float cloudX = fmod(time * cloudSpeed + i * 150, screenWidth + 200) - 200;
        DrawEllipse((int)(cloudX), cloudY + i * 30, 60, 30, Fade(WHITE, 0.8f));
        DrawEllipse((int)(cloudX + 30), cloudY + i * 30 + 10, 70, 35, Fade(WHITE, 0.7f));
        DrawEllipse((int)(cloudX + 60), cloudY + i * 30, 50, 25, Fade(WHITE, 0.8f));
    }
}

void DrawRectangleVGradient(int x, int y, int width, int height, Color top, Color bottom) {
    for (int i = 0; i < height; i++) {
        float t = (float)i / height;
        Color color = {
            (unsigned char)(top.r + t * (bottom.r - top.r)),
            (unsigned char)(top.g + t * (bottom.g - top.g)),
            (unsigned char)(top.b + t * (bottom.b - top.b)),
            (unsigned char)(top.a + t * (bottom.a - top.a))
        };
        DrawLine(x, y + i, x + width, y + i, color);
    }
}

void drawTrain(Train& train, int cellSize, int offsetY, int dirX, int dirY) {
    TrainCarriage* current = train.head;
    int index = 0;

    float wheelRotation = GetTime() * 12.0f;

    while (current != nullptr) {
        int px = current->x * cellSize;
        int py = current->y * cellSize + offsetY;

        DrawCircle(px + cellSize / 2, py + cellSize - 3, cellSize / 3, Fade(BLACK, 0.2f));

        if (index == 0) {
            DrawRectangleVGradient(px, py, cellSize, cellSize, DARKBLUE, (Color){ 30, 60, 140, 255 });
            DrawRectangleVGradient(px + cellSize / 4, py + cellSize / 4, cellSize / 2, cellSize / 3, SKYBLUE, (Color){ 100, 180, 255, 255 });
            drawSmoke(px + cellSize / 2, py);

            Color noseColor = (Color){ 20, 40, 90, 255 };
            if (dirX == 1)
                DrawTriangle((Vector2){(float)(px + cellSize), (float)py},
                             (Vector2){(float)(px + cellSize), (float)(py + cellSize)},
                             (Vector2){(float)(px + cellSize + 6), (float)(py + cellSize / 2)}, noseColor);
            else if (dirX == -1)
                DrawTriangle((Vector2){(float)px, (float)py},
                             (Vector2){(float)px, (float)(py + cellSize)},
                             (Vector2){(float)(px - 6), (float)(py + cellSize / 2)}, noseColor);
            else if (dirY == -1)
                DrawTriangle((Vector2){(float)px, (float)py},
                             (Vector2){(float)(px + cellSize), (float)py},
                             (Vector2){(float)(px + cellSize / 2), (float)(py - 6)}, noseColor);
            else if (dirY == 1)
                DrawTriangle((Vector2){(float)px, (float)(py + cellSize)},
                             (Vector2){(float)(px + cellSize), (float)(py + cellSize)},
                             (Vector2){(float)(px + cellSize / 2), (float)(py + cellSize + 6)}, noseColor);

            DrawRectangle(px + cellSize / 2 - 3, py - 6, 6, 6, (Color){ 50, 80, 140, 255 });
        } else {
            DrawRectangleVGradient(px + 2, py + 4, cellSize - 4, cellSize - 8, BLUE, (Color){ 20, 50, 130, 255 });

            Vector2 center1 = { (float)(px + 6), (float)(py + cellSize - 3) };
            Vector2 center2 = { (float)(px + cellSize - 6), (float)(py + cellSize - 3) };
            int wheelRadius = 4;

            for (int i = 0; i < 4; i++) {
                float angle = wheelRotation + i * 3.14159f / 2;
                Vector2 offset = { cosf(angle) * wheelRadius, sinf(angle) * wheelRadius };
                DrawLineEx(center1, (Vector2){center1.x + offset.x, center1.y + offset.y}, 2, BLACK);
                DrawLineEx(center2, (Vector2){center2.x + offset.x, center2.y + offset.y}, 2, BLACK);
            }

            DrawCircle(center1.x, center1.y, wheelRadius, DARKGRAY);
            DrawCircle(center2.x, center2.y, wheelRadius, DARKGRAY);
            DrawCircleLines(center1.x, center1.y, wheelRadius, BLACK);
            DrawCircleLines(center2.x, center2.y, wheelRadius, BLACK);
        }

        current = current->next;
        index++;
    }
}

void drawTrainTracks(int gridWidth, int gridHeight, int cellSize, int offsetY) {
    Color railColor = (Color){ 70, 70, 70, 255 };
    Color tieColor = (Color){ 120, 70, 40, 255 };
    int railThickness = 5;
    int tieThickness = 8;

    for (int x = 0; x <= gridWidth; x++) {
        int pxLeft = x * cellSize - railThickness / 2;
        if (pxLeft < 0) pxLeft = 0;
        int pxRight = x * cellSize + cellSize - railThickness / 2;

        DrawRectangle(pxLeft, offsetY, railThickness, gridHeight * cellSize, railColor);
        if (x < gridWidth)
            DrawRectangle(pxRight, offsetY, railThickness, gridHeight * cellSize, railColor);
    }

    for (int y = 0; y <= gridHeight; y++) {
        int py = y * cellSize + offsetY - tieThickness / 2;
        DrawRectangle(0, py, gridWidth * cellSize, tieThickness, tieColor);
    }
}

void drawCargo(int cargoX, int cargoY, int cellSize, int offsetY) {
    int x = cargoX * cellSize;
    int y = cargoY * cellSize + offsetY;

    DrawRectangleVGradient(x, y, cellSize, cellSize, ORANGE, (Color){ 190, 120, 30, 255 });
    DrawRectangleLines(x, y, cellSize, cellSize, (Color){ 120, 70, 15, 255 });

    DrawRectangle(x + 3, y + 6, cellSize - 6, 4, (Color){ 150, 90, 20, 255 });
    DrawRectangle(x + 3, y + cellSize / 2, cellSize - 6, 4, (Color){ 150, 90, 20, 255 });

    DrawCircle(x + 4, y + 4, 2, BLACK);
    DrawCircle(x + cellSize - 4, y + 4, 2, BLACK);
    DrawCircle(x + 4, y + cellSize - 4, 2, BLACK);
    DrawCircle(x + cellSize - 4, y + cellSize - 4, 2, BLACK);
}

bool placeCargo(int& cargoX, int& cargoY, Train& train, int gridWidth, int gridHeight) {
    std::vector<std::pair<int,int>> freeSpots;

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            if (!train.isOnPosition(x, y)) {
                freeSpots.emplace_back(x, y);
            }
        }
    }

    if (freeSpots.empty()) {
        return false;  // no place for cargo, game over condition
    }

    int idx = rand() % freeSpots.size();
    cargoX = freeSpots[idx].first;
    cargoY = freeSpots[idx].second;
    return true;
}
