#include "train.h"
#include "graphics.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>

// ----------  WALL SYSTEM  ----------
struct Wall {
    int x, y;
};
std::vector<Wall> walls;
float shakeTime = 0.0f;        // seconds left
float shakeIntensity = 12.0f;  // pixels

// ----------  WALL HELPERS  ----------
void drawBrickWall(int gx, int gy, int cellSize, int offsetY) {
    int px = gx * cellSize;
    int py = gy * cellSize + offsetY;
    Color mortar = { 160, 82, 45, 255 };   // brownish
    DrawRectangle(px, py, cellSize, cellSize, mortar);

    int rows = 4;
    int rowH = cellSize / rows;
    for (int r = 0; r < rows; ++r) {
        bool oddRow = (r & 1);
        int bricksPerRow = 3;
        int brickW = cellSize / bricksPerRow;
        for (int b = 0; b < bricksPerRow; ++b) {
            int bx = px + b * brickW + (oddRow ? brickW / 2 : 0);
            int by = py + r * rowH;
            Color brick = { 178, 34, 34, 255 };  // red brick
            DrawRectangle(bx, by, brickW - 2, rowH - 2, brick);
            DrawRectangle(bx, by, brickW - 2, 2, { 200, 70, 70, 255 }); // highlight
        }
    }
    DrawRectangleLines(px, py, cellSize, cellSize, DARKBROWN);
}

void spawnWalls(int level, int gridWidth, int gridHeight, const Train& train, int cargoX, int cargoY) {
    if (level < 5) return;
    int count = level - 4;
    for (int i = 0; i < count; ++i) {
        int wx, wy;
        do {
            wx = rand() % gridWidth;
            wy = rand() % gridHeight;
        } while (train.isOnPosition(wx, wy) || (wx == cargoX && wy == cargoY));
        walls.push_back({wx, wy});
    }
}

void stripLastCarriage(Train& train) {
    if (train.head == train.tail) return;
    TrainCarriage* cur = train.head;
    while (cur->next != train.tail) cur = cur->next;
    delete train.tail;
    train.tail = cur;
    train.tail->next = nullptr;
}

// ----------  GAME  ----------
enum GameState { START_MENU, PLAYING, GAME_OVER };

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Train Adventure");

    const int topBarHeight = 60;
    const int cellSize = 20;
    const int gridWidth  = screenWidth / cellSize;
    const int gridHeight = (screenHeight - topBarHeight) / cellSize;

    Train train(gridWidth / 2, gridHeight / 2);

    int dirX = 1;
    int dirY = 0;

    srand(static_cast<unsigned>(time(nullptr)));
    int cargoX = rand() % gridWidth;
    int cargoY = rand() % gridHeight;

    int level = 1;
    float speed = 5.0f;
    float timer = 0.0f;

    GameState gameState = START_MENU;

    float blinkTimer = 0.0f;
    bool showPrompt   = true;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float time      = GetTime();

        switch (gameState) {
        case START_MENU: {
            blinkTimer += deltaTime;
            if (blinkTimer >= 0.5f) { showPrompt = !showPrompt; blinkTimer = 0.0f; }

            BeginDrawing();
            ClearBackground({135, 206, 235, 255});
            drawHills(screenWidth, screenHeight, time);
            drawClouds(screenWidth, screenHeight, time);

            static float trainX = 0.0f;
            static float trainSpeed = 60.0f;
            trainX += trainSpeed * deltaTime;
            if (trainX > GetScreenWidth() || trainX < -200) trainSpeed *= -1.0f;

            DrawRectangleRounded({trainX, float(GetScreenHeight() - 100), 200, 60}, 0.2f, 6, DARKBLUE);
            DrawCircle(int(trainX + 30), GetScreenHeight() - 40, 15, BLACK);
            DrawCircle(int(trainX + 70), GetScreenHeight() - 40, 15, BLACK);
            DrawCircle(int(trainX + 150), GetScreenHeight() - 40, 15, BLACK);

            DrawText("TRAIN ADVENTURE", GetScreenWidth()/2 - MeasureText("TRAIN ADVENTURE", 40)/2, 120, 40, DARKBLUE);
            if (showPrompt) DrawText("Press ENTER to Start", GetScreenWidth()/2 - MeasureText("Press ENTER to Start", 20)/2, 200, 20, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                gameState = PLAYING;
                train       = Train(gridWidth / 2, gridHeight / 2);
                dirX = 1; dirY = 0;
                cargoX = rand() % gridWidth;
                cargoY = rand() % gridHeight;
                level  = 1;
                speed  = 5.0f;
                timer  = 0.0f;
                walls.clear();
            }
            break;
        }

        case PLAYING: {
            timer += deltaTime;

            if (IsKeyPressed(KEY_RIGHT) && !(dirX == -1 && dirY == 0)) { dirX = 1; dirY = 0; }
            else if (IsKeyPressed(KEY_LEFT)  && !(dirX == 1 && dirY == 0)) { dirX = -1; dirY = 0; }
            else if (IsKeyPressed(KEY_UP)    && !(dirX == 0 && dirY == 1)) { dirX = 0; dirY = -1; }
            else if (IsKeyPressed(KEY_DOWN)  && !(dirX == 0 && dirY == -1)) { dirX = 0; dirY = 1; }

            if (timer >= 1.0f / speed) {
                timer = 0.0f;

                int newX = (train.head->x + dirX + gridWidth)  % gridWidth;
                int newY = (train.head->y + dirY + gridHeight) % gridHeight;

                if (train.isOnPosition(newX, newY)) {
                    gameState = GAME_OVER;
                    break;
                }
                // ---- brick-wall collision ----
                bool hitWall = false;
                for (const Wall& w : walls) {
                    if (newX == w.x && newY == w.y) { hitWall = true; break; }
                }
                if (hitWall) {
                    stripLastCarriage(train);
                    shakeTime = 0.4f;
                }
                train.move(newX, newY);

                if (newX == cargoX && newY == cargoY) {
                    train.addCarriage();
                    level++;
                    speed = fminf(speed + 0.5f, 12.0f);
                    spawnWalls(level, gridWidth, gridHeight, train, cargoX, cargoY);
                    placeCargo(cargoX, cargoY, train, gridWidth, gridHeight); // ignore result
                }
            }

            // ----------  DRAW  ----------
            BeginDrawing();
            // camera shake
            if (shakeTime > 0.0f) {
                float sx = GetRandomValue(-100, 100) / 100.0f * shakeIntensity;
                float sy = GetRandomValue(-100, 100) / 100.0f * shakeIntensity;
                BeginMode2D((Camera2D){ {0,0}, {sx, sy}, 0.0f, 1.0f });
                shakeTime -= deltaTime;
            } else {
                BeginMode2D((Camera2D){ {0,0}, {0,0}, 0.0f, 1.0f });
            }

            ClearBackground({135, 206, 235, 255});
            drawHills(screenWidth, screenHeight, time);
            drawClouds(screenWidth, screenHeight, time);

            int trainLength = 0;
            for (TrainCarriage* cur = train.head; cur != nullptr; cur = cur->next) trainLength++;

            Rectangle levelBox  = {10, 10, 120, 40};
            DrawRectangleRounded(levelBox, 0.2f, 6, BLUE);
            DrawRectangleRoundedLines(levelBox, 0.2f, 6, DARKBLUE);
            DrawText("Level", levelBox.x + 15, levelBox.y + 5, 18, WHITE);
            DrawText(TextFormat("%d", level), levelBox.x + 15, levelBox.y + 22, 22, YELLOW);

            Rectangle lengthBox = {140, 10, 140, 40};
            DrawRectangleRounded(lengthBox, 0.2f, 6, BLUE);
            DrawRectangleRoundedLines(lengthBox, 0.2f, 6, DARKBLUE);
            DrawText("Length", lengthBox.x + 15, lengthBox.y + 5, 18, WHITE);
            DrawText(TextFormat("%d", trainLength), lengthBox.x + 15, lengthBox.y + 22, 22, YELLOW);

            Rectangle instrBox  = {300, 10, screenWidth - 310, 40};
            DrawRectangleRounded(instrBox, 0.2f, 6, GRAY);
            DrawRectangleRoundedLines(instrBox, 0.2f, 6, DARKGRAY);
            DrawText("Use arrow keys to drive the train", instrBox.x + 15, instrBox.y + 12, 20, BLACK);

            drawTrainTracks(gridWidth, gridHeight, cellSize, topBarHeight);
            drawCargo(cargoX, cargoY, cellSize, topBarHeight);
            for (const Wall& w : walls) drawBrickWall(w.x, w.y, cellSize, topBarHeight);
            drawTrain(train, cellSize, topBarHeight, dirX, dirY);

            EndMode2D();
            EndDrawing();
            break;
        }

        case GAME_OVER: {
            BeginDrawing();
            ClearBackground({135, 206, 235, 255});
            drawHills(screenWidth, screenHeight, time);
            drawClouds(screenWidth, screenHeight, time);

            DrawText("GAME OVER", GetScreenWidth()/2 - MeasureText("GAME OVER", 60)/2 + 3, GetScreenHeight()/2 - 45 + 3, 60, Fade(RED, 0.6f));
            DrawText("GAME OVER", GetScreenWidth()/2 - MeasureText("GAME OVER", 60)/2, GetScreenHeight()/2 - 45, 60, RED);
            DrawText("Press ENTER to Restart", GetScreenWidth()/2 - MeasureText("Press ENTER to Restart", 24)/2 + 1, GetScreenHeight()/2 + 20 + 1, 24, Fade(DARKGRAY, 0.5f));
            DrawText("Press ENTER to Restart", GetScreenWidth()/2 - MeasureText("Press ENTER to Restart", 24)/2, GetScreenHeight()/2 + 20, 24, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                train  = Train(gridWidth / 2, gridHeight / 2);
                dirX   = 1; dirY = 0;
                cargoX = rand() % gridWidth;
                cargoY = rand() % gridHeight;
                level  = 1;
                speed  = 5.0f;
                timer  = 0.0f;
                walls.clear();
                gameState = START_MENU;
            }
            break;
        }
        } // switch
    }     // while window open

    CloseWindow();
    return 0;
}