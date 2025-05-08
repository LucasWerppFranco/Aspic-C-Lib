#pragma once
#include "config.h"

typedef struct {
    char content[MAX_CHAR_SIZE];
} Cell;

extern Cell** map;
extern int lines, columns;

void load_map(const char* path);
void lock_map();
void clear_memory();
void show_map();
int in_bounds(int x, int y);
int is_wall(int x, int y);

