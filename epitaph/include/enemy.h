#pragma once
#include "player.h"

typedef struct {
    int hp;
} Enemy;

void move_enemy(Player* player);
int line_of_sight(int ex, int ey, int px, int py);
void start_combat(Player* player, Enemy* enemy);

