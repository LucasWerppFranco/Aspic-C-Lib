#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char content[5]; // Para caracteres até 4 bytes + '\0'
    int number;      // Usado para armazenar o número, se houver
} Cell;

Cell** map;
int lines, columns;

typedef struct {
    char* map_name;
    char* intro_text;
    char* color_code;
    const char* file_name;
} MapConfig;

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

void load_map(const MapConfig* map_config) {
    FILE* f = fopen(map_config->file_name, "r");
    if (!f) {
        printf("Error: 1 - '%s' not found\n", map_config->file_name);
        exit(1);
    }

    fscanf(f, "%d %d\n", &lines, &columns);
    lock_map();

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            unsigned char buffer[5] = {0};
            int c = fgetc(f);

            if (c == EOF) break;

            // Detecta tamanho do caractere UTF-8
            int size = 1;
            buffer[0] = c;
            if ((c & 0xE0) == 0xC0) size = 2;
            else if ((c & 0xF0) == 0xE0) size = 3;
            else if ((c & 0xF8) == 0xF0) size = 4;

            for (int k = 1; k < size; k++) {
                buffer[k] = fgetc(f);
            }

            // Verifica se o conteúdo é um número de 1 a 9
            if (buffer[0] >= '1' && buffer[0] <= '9') {
                map[i][j].content[0] = '.';  // Exibe "." no lugar do número
                map[i][j].number = buffer[0] - '0';  // Armazena o número real
            } else {
                strncpy(map[i][j].content, (char*)buffer, 5);
                map[i][j].number = 0; // Se não for número, armazena 0
            }
        }
        fgetc(f); // Pula '\n'
    }

    fclose(f);
}

void show_map(const MapConfig* map_config) {
    printf("\033[1;37m%s\033[0m\n", map_config->intro_text);  // Texto de introdução com cor padrão

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            // Exibe o conteúdo do mapa com cores configuradas
            if (strcmp(map_config->color_code, "yellow") == 0) {
                if (strcmp(map[i][j].content, "@") == 0) {
                    printf("\033[33m%s\033[0m", map[i][j].content); // Amarelo
                } else {
                    printf("%s", map[i][j].content);
                }
            } else {
                printf("%s", map[i][j].content);
            }
        }
        printf("\n");
    }
}

int should_exit(char command) {
    return command == 'q';
}

void move_player(char* direction, const MapConfig* map_config) {
    int x = -1, y = -1;

    // encontra jogador
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
        dx = -1;  // cima
    } else if (strcmp(direction, "s") == 0 || strcmp(direction, "^[[B") == 0) {
        dx = +1;  // baixo
    } else if (strcmp(direction, "a") == 0 || strcmp(direction, "^[[D") == 0) {
        dy = -1;  // esquerda
    } else if (strcmp(direction, "d") == 0 || strcmp(direction, "^[[C") == 0) {
        dy = +1;  // direita
    } else {
        return;  // comando inválido
    }

    int new_x = x + dx;
    int new_y = y + dy;
    int beyond_x = x + 2 * dx;
    int beyond_y = y + 2 * dy;

    if (new_x < 0 || new_x >= lines || new_y < 0 || new_y >= columns) return;

    // Movimento normal
    if (strcmp(map[new_x][new_y].content, ".") == 0) {
        strcpy(map[x][y].content, ".");
        strcpy(map[new_x][new_y].content, "@");
    }
    // Empurrar caixa
    else if (strcmp(map[new_x][new_y].content, "#") == 0) {
        if (beyond_x >= 0 && beyond_x < lines && beyond_y >= 0 && beyond_y < columns) {
            if (strcmp(map[beyond_x][beyond_y].content, ".") == 0) {
                strcpy(map[beyond_x][beyond_y].content, "#");
                strcpy(map[new_x][new_y].content, "@");
                strcpy(map[x][y].content, ".");
            }
        }
    }

    // Verifica se o jogador está em cima de um número e exibe a mensagem
    if (map[new_x][new_y].number > 0) {
        printf("Você pisou no número %d!\n", map[new_x][new_y].number);
    }
}

// Função para capturar o comando, incluindo setas
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
    // Configurações do mapa 1
    MapConfig map1 = {
        .map_name = "Map 1",
        .intro_text = "Bem-vindo ao Map 1!",
        .color_code = "yellow", // Cor amarela para este mapa
        .file_name = "map.txt" // Nome do arquivo do mapa 1
    };

    // Carrega e exibe o mapa
    load_map(&map1);

    char direction[10];
    do {
        system("clear");  // Limpa a tela
        show_map(&map1);  // Exibe o mapa com configurações específicas
        printf("\nComando (w/a/s/d ou setas para mover, q para sair): \n");

        capture_input(direction);

        move_player(direction, &map1);
    } while (!should_exit(direction[0]));

    clear_memory();
    return 0;
}
