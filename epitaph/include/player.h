#pragma once
#include "map.h"

typedef struct {
    int hp;
} Player;

void move_player(char* direction);
void capture_input(char* direction);

