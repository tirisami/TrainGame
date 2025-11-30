#include "train.h"
#include "graphics.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>


enum GameState { START_MENU, PLAYING, GAME_OVER };

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Train Adventure");

    const int topBarHeight = 60;
    const int cellSize = 20;
    const int gridWidth = screenWidth / cellSize;
    const int gridHeight = (screenHeight - topBarHeight) / cellSize;

    Train train(gridWidth / 2, gridHeight / 2);

    int dirX = 1;
    int dirY = 0;

    srand(time(nullptr));
    int cargoX = rand() % gridWidth;
    int cargoY = rand() % gridHeight;

    int level = 1;
    float speed = 5.0f;
    float timer = 0.0f;

    GameState gameState = START_MENU;

    float blinkTimer = 0.0f;
    bool showPrompt = true;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float time = GetTime();

        switch (gameState) {
            case START_MENU: {
                blinkTimer += deltaTime;
                if (blinkTimer >= 0.5f) {
                    showPrompt = !showPrompt;
                    blinkTimer = 0.0f;
                }

                BeginDrawing();
                ClearBackground((Color){ 135, 206, 235, 255 }); // Sky Blue

                drawHills(screenWidth, screenHeight, time);
                drawClouds(screenWidth, screenHeight, time);

                static float trainX = 0;
                static float trainSpeed = 60.0f;
                trainX += trainSpeed * deltaTime;
                if (trainX > GetScreenWidth() || trainX < -200) trainSpeed *= -1;

                DrawRectangleRounded((Rectangle){ (float)trainX, (float)(GetScreenHeight() - 100), 200.0f, 60.0f }, 0.2f, 6, DARKBLUE);
                DrawCircle((int)(trainX + 30), GetScreenHeight() - 40, 20, Fade(BLACK, 0.3f));
                DrawCircle((int)(trainX + 70), GetScreenHeight() - 40, 20, Fade(BLACK, 0.3f));
                DrawCircle((int)(trainX + 150), GetScreenHeight() - 40, 20, Fade(BLACK, 0.3f));

                DrawCircle((int)(trainX + 30), GetScreenHeight() - 40, 15, BLACK);
                DrawCircle((int)(trainX + 70), GetScreenHeight() - 40, 15, BLACK);
                DrawCircle((int)(trainX + 150), GetScreenHeight() - 40, 15, BLACK);

                DrawCircleGradient((int)(trainX + 180), GetScreenHeight() - 100, 25, Fade(LIGHTGRAY, 0.6f), Fade(WHITE, 0));
                DrawCircleGradient((int)(trainX + 210), GetScreenHeight() - 130, 18, Fade(LIGHTGRAY, 0.4f), Fade(WHITE, 0));
                DrawCircleGradient((int)(trainX + 230), GetScreenHeight() - 160, 12, Fade(LIGHTGRAY, 0.2f), Fade(WHITE, 0));

                DrawText("TRAIN ADVENTURE", GetScreenWidth() / 2 - MeasureText("TRAIN ADVENTURE", 40) / 2 + 2, 122, 40, Fade(DARKBLUE, 0.6f));
                DrawText("TRAIN ADVENTURE", GetScreenWidth() / 2 - MeasureText("TRAIN ADVENTURE", 40) / 2, 120, 40, LIGHTGRAY);

                if (showPrompt) {
                    DrawText("PRESS ENTER TO START", GetScreenWidth() / 2 - MeasureText("PRESS ENTER TO START", 20) / 2, 300, 20, Fade(WHITE, 0.5f));
                }
                EndDrawing();

                if (IsKeyPressed(KEY_ENTER)) {
                    gameState = PLAYING;
                    train = Train(gridWidth / 2, gridHeight / 2);
                    dirX = 1;
                    dirY = 0;
                    level = 1;
                    speed = 5.0f;
                    timer = 0.0f;
                    cargoX = rand() % gridWidth;
                    cargoY = rand() % gridHeight;
                }
                break;
            }

                        case PLAYING: {
                timer += deltaTime;

                if (IsKeyPressed(KEY_RIGHT) && !(dirX == -1 && dirY == 0)) {
                    dirX = 1; dirY = 0;
                } else if (IsKeyPressed(KEY_LEFT) && !(dirX == 1 && dirY == 0)) {
                    dirX = -1; dirY = 0;
                } else if (IsKeyPressed(KEY_UP) && !(dirX == 0 && dirY == 1)) {
                    dirX = 0; dirY = -1;
                } else if (IsKeyPressed(KEY_DOWN) && !(dirX == 0 && dirY == -1)) {
                    dirX = 0; dirY = 1;
                }

                if (timer >= 1.0f / speed) {
                    timer = 0;

                    int newX = train.head->x + dirX;
                    int newY = train.head->y + dirY;

                    if (newX < 0) newX = gridWidth - 1;
                    else if (newX >= gridWidth) newX = 0;

                    if (newY < 0) newY = gridHeight - 1;
                    else if (newY >= gridHeight) newY = 0;

                    if (train.isOnPosition(newX, newY)) {
                        gameState = GAME_OVER;
                    } else {
                        train.move(newX, newY);

                        if (newX == cargoX && newY == cargoY) {
                            train.addCarriage();

                            int trainLength = 0;
                            for (TrainCarriage* cur = train.head; cur != nullptr; cur = cur->next) {
                                trainLength++;
                            }

                            if (trainLength % 3 == 0) {
                                level++;
                                speed = (speed + 0.5f > 15.0f) ? 15.0f : speed + 0.5f;
                            }

                            if (!placeCargo(cargoX, cargoY, train, gridWidth, gridHeight)) {
                                gameState = GAME_OVER;
                            }
                        }
                    }
                }

                BeginDrawing();
                ClearBackground((Color){ 135, 206, 235, 255 });

                drawHills(screenWidth, screenHeight, time);
                drawClouds(screenWidth, screenHeight, time);

                /* =====  TOP-BAR UI  ===== */
                int trainLength = 0;
                for (TrainCarriage* cur = train.head; cur != nullptr; cur = cur->next) {
                    trainLength++;
                }

                // Level box
                Rectangle levelBox = {10, 10, 120, 40};
                DrawRectangleRounded(levelBox, 0.2f, 6, BLUE);
                DrawRectangleRoundedLines(levelBox, 0.2f, 6, DARKBLUE);
                DrawText("Level", levelBox.x + 15, levelBox.y + 5, 18, WHITE);
                DrawText(TextFormat("%d", level), levelBox.x + 15, levelBox.y + 22, 22, YELLOW);

                // Length box
                Rectangle lengthBox = {140, 10, 140, 40};
                DrawRectangleRounded(lengthBox, 0.2f, 6, BLUE);
                DrawRectangleRoundedLines(lengthBox, 0.2f, 6, DARKBLUE);
                DrawText("Length", lengthBox.x + 15, lengthBox.y + 5, 18, WHITE);
                DrawText(TextFormat("%d", trainLength), lengthBox.x + 15, lengthBox.y + 22, 22, YELLOW);

                // Instruction box
                Rectangle instrBox = {300, 10, screenWidth - 310, 40};
                DrawRectangleRounded(instrBox, 0.2f, 6, GRAY);
                DrawRectangleRoundedLines(instrBox, 0.2f, 6, DARKGRAY);
                DrawText("Use arrow keys to drive the train", instrBox.x + 15, instrBox.y + 12, 20, BLACK);

                /* =====  GAME WORLD  ===== */
                drawTrainTracks(gridWidth, gridHeight, cellSize, topBarHeight);
                drawCargo(cargoX, cargoY, cellSize, topBarHeight);
                drawTrain(train, cellSize, topBarHeight, dirX, dirY);

                EndDrawing();
                break;
            }

            case GAME_OVER: {
                blinkTimer += deltaTime;
                if (blinkTimer >= 0.5f) {
                    showPrompt = !showPrompt;
                    blinkTimer = 0.0f;
                }

                BeginDrawing();
                ClearBackground((Color){ 20, 20, 20, 255 });
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, 120, 40, RED);
                DrawText(TextFormat("YOUR SCORE: %d", level), screenWidth / 2 - MeasureText("YOUR SCORE: 00", 30) / 2, 200, 30, WHITE);

                if (showPrompt) {
                    DrawText("PRESS ENTER TO RESTART", screenWidth / 2 - MeasureText("PRESS ENTER TO RESTART", 20) / 2, 300, 20, Fade(WHITE, 0.5f));
                }

                EndDrawing();

                if (IsKeyPressed(KEY_ENTER)) {
                    gameState = START_MENU;
                }
                break;
            }
        }
    }

    CloseWindow();
    return 0;
}
