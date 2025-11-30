#pragma once
#include "raylib.h"
#include "train.h"

void drawSmoke(int x, int y);
void drawHills(int screenWidth, int screenHeight, float time);
void drawClouds(int screenWidth, int screenHeight, float time);
void DrawRectangleVGradient(int x, int y, int width, int height, Color top, Color bottom);

void drawTrain(Train& train, int cellSize, int offsetY, int dirX, int dirY);
void drawTrainTracks(int gridWidth, int gridHeight, int cellSize, int offsetY);
void drawCargo(int cargoX, int cargoY, int cellSize, int offsetY);

