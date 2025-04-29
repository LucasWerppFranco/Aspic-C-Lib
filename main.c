#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char content[5]; 
} Cell;

Cell** map;
int lines, columns;

void lock_map() {
    map = malloc(sizeof(Cell*) * lines);
    if (map == NULL) {
        printf("Erro ao alocar mapa.\n");
        exit(1);
    }

    for (int i = 0; i < lines; i++) {
        map[i] = malloc(sizeof(Cell) * columns);
        if (map[i] == NULL) {
            printf("Erro ao alocar linha %d.\n", i);
            exit(1);
        }
    }
}

void clear_memory() {
    for (int i = 0; i < lines; i++) {
        free(map[i]);
    }
    free(map);
}

void load_map() {
    FILE* f = fopen("map.txt", "r");
    if (!f) {
        printf("Error: 1 - 'map.txt' not found\n");
        exit(1);
    }

    fscanf(f, "%d %d\n", &lines, &columns);
    lock_map();

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            unsigned char buffer[5] = {0};
            int c = fgetc(f);

            if (c == EOF) break;

            int size = 1;
            buffer[0] = c;
            if ((c & 0xE0) == 0xC0) size = 2;
            else if ((c & 0xF0) == 0xE0) size = 3;
            else if ((c & 0xF8) == 0xF0) size = 4;

            for (int k = 1; k < size; k++) {
                buffer[k] = fgetc(f);
            }

            strncpy(map[i][j].content, (char*)buffer, 5);
        }
        fgetc(f); 
    }

    fclose(f);
}

void show_map() {
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%s", map[i][j].content);         
    }
      printf("\n");
    }
}

int should_exit(char command) {
    return command == 'q';
}

void move_player(char* direction) {
    int x = -1, y = -1;

    for (int i = 0; i < lines && x == -1; i++) {
        for (int j = 0; j < columns; j++) {
            if (strcmp(map[i][j].content, "@") == 0) {
                x = i;
                y = j;
                break;
            }
        }
    }

    int dx = 0, dy = 0;
    if (strcmp(direction, "w") == 0 || strcmp(direction, "^[[A") == 0) {
        dx = -1;  
    } else if (strcmp(direction, "s") == 0 || strcmp(direction, "^[[B") == 0) {
        dx = +1;  
    } else if (strcmp(direction, "a") == 0 || strcmp(direction, "^[[D") == 0) {
        dy = -1;  
    } else if (strcmp(direction, "d") == 0 || strcmp(direction, "^[[C") == 0) {
        dy = +1;  
    } else {
        return;  
    }

    int new_x = x + dx;
    int new_y = y + dy;
    int beyond_x = x + 2 * dx;
    int beyond_y = y + 2 * dy;

    if (new_x < 0 || new_x >= lines || new_y < 0 || new_y >= columns) return;

    if (strcmp(map[new_x][new_y].content, ".") == 0) {
        strcpy(map[x][y].content, ".");
        strcpy(map[new_x][new_y].content, "@");
    }
    else if (strcmp(map[new_x][new_y].content, "#") == 0) {
        if (beyond_x >= 0 && beyond_x < lines && beyond_y >= 0 && beyond_y < columns) {
            if (strcmp(map[beyond_x][beyond_y].content, ".") == 0) {
                strcpy(map[beyond_x][beyond_y].content, "#");
                strcpy(map[new_x][new_y].content, "@");
                strcpy(map[x][y].content, ".");
            }
        }
    }
}

void capture_input(char* direction) {
    int c = fgetc(stdin);

    if (c == '\033') { 
        fgetc(stdin);  
        c = fgetc(stdin); 

        switch(c) {
            case 'A': strcpy(direction, "^[[A"); break;  
            case 'B': strcpy(direction, "^[[B"); break;  
            case 'C': strcpy(direction, "^[[C"); break;  
            case 'D': strcpy(direction, "^[[D"); break;  
        }
    } else {
        direction[0] = c;
        direction[1] = '\0';
    }
}

int main() {
    load_map();

    char direction[10];
    do {
        show_map();  

        capture_input(direction);

        move_player(direction);
    } while (!should_exit(direction[0]));

    clear_memory();
    return 0;
}

