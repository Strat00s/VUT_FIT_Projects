/*  Project #3
    Path finder
    Lukas Bastyr, xbasty00
    011.12.2019

    What it is supposed to do:
    Show help, test maze and
    find a way through maze
    using right and left hand rule.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define TWN 3       // triangle wall number
#define IDN 11      // number of digits in int + 1 for a good measure
#define RPATH -1    // right path number
#define LPATH 1     // left path number
#define UPATH -1
#define DPATH 1

// The struct
typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

// Enum  for 
enum walls {bot, right, left};
        //   0     1      2

bool isborder(Map *map, int row, int cols, int border){

    // map is 0,0 based, but our coords are 1,1 based
    int map_pos = map->cols*(row-1)+(cols-1);
    switch (border){
    case bot:
        if ((map->cells[map_pos]&4) != 0){
            return true;
        }
        break;
    case right:
        if ((map->cells[map_pos]&2) != 0){
            return true;
        }
        break;
    case left:
        if ((map->cells[map_pos]&1) != 0){
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void dumper(Map *map){
    if (map->cells != NULL){
        free(map->cells);
        map->cells = NULL;
    }
}

int loader(Map *map, char *map_file){
    FILE *file = fopen(map_file, "r");
    if (file == NULL){
        return 1;
    }
    int c, dim, z;
    char tmp[IDN] = "\0";
    dim = z = c = 0;
    for (int i = 0; c != '\n';){    // kepp reading until the end of first line
        c = fgetc(file);
        if (isdigit(c)){
            tmp[i] = c;
            i++;
        }
        if ((isblank(c) || c == '\n') && i != 0){
            if (dim == 0){
                map->rows = strtol(tmp, NULL, 10);
            }
            else {
                z = strtol(tmp, NULL, 10);
            }
            dim++;
            i = 0;
        }
        if (c == '\n'){
            map->cols = strtol(tmp, NULL, 10);
            i = 0;
            break;
        }
        else if ((!isdigit(c) && !isspace(c)) || c < 0){
            fclose(file);
            return 1;
        }
    }
    if (dim != 2){
        fclose(file);
        return 1;
    }
    map->cells = malloc((map->rows*map->cols)*sizeof(unsigned char*));
    
    // load maze to cells
    int size;
    for (size = 0; feof(file) == 0;){
        c=fgetc(file);
        if (isdigit(c)){
            map->cells[size] = c;
            size++;
        }
        else if (!isdigit(c) && !isspace(c) && c != EOF){
            fclose(file);
            dumper(map);
            return 1;
        }
    }
    if (size != map->rows*map->cols){
        fclose(file);
        dumper(map);
        return 1;
    }
    fclose(file);
    return 0;
}

int tester(Map *map){
    int oden, r_start, c_start;
    r_start = c_start = 1;  // 1,1 base coordinates
    while (1){
        oden = (r_start + c_start)%2;
        for (int i = 0; i < TWN; i++){
            if (isborder(map, r_start, c_start, i)){
                if (i == bot){
                    if (!oden && r_start > 1){
                        if (!isborder(map, r_start+UPATH, c_start, i)){
                            dumper(map);
                            return 1;
                        }
                    }
                    else if (oden && r_start < map->rows){
                        if (!isborder(map, r_start+DPATH, c_start, i)){
                            dumper(map);
                            return 1;
                        }
                    }
                }
                if (i == left && c_start > 1){
                    if (!isborder(map, r_start, c_start+RPATH, right)){
                        dumper(map);
                        return 1;
                    }
                }
                if (i == right && c_start < map->cols){
                    if (!isborder(map, r_start, c_start+LPATH, left)){
                        dumper(map);
                        return 1;
                    }
                }
            }
        }
        if (r_start*c_start == map->rows*map->cols){
            break;
        }
        if (c_start%map->cols){
            c_start++;
        }
        else {
            r_start++;
            c_start = 1;
        }
    }
    return 0;
}

void helper(){
    printf("This is a proj3 help.\n");
    printf("Usage: ./proj3 [OPTION...] file...\n\n");
    printf("Available arguments:\n\n");
    printf("--help                          Show this help.\n");
    printf("--test                          Tests wheter the file is a valid maze\n");
    printf("--rpath <x_start> <y_start>     Goes trough the maze using right hand rule.\n");
    printf("--lpath <x_start> <y_start>     Goes trough the maze using left hand rule.\n");
    printf("--shortest <x_start> <y_start>  Not implement.\n");
}

int rotate(int wall, int oden, int row, int leftright, bool start){
    if (start){
        if ((row%2 && wall == bot && leftright == LPATH) || (!(row%2) && wall == bot && leftright == RPATH)){
            wall = wall;
        }
        else if (oden){
            wall = (wall+TWN+leftright)%TWN;
        }
        else {
            wall = (wall+TWN-leftright)%TWN;
        }
    }
    else{
        if (oden){
            wall = (wall+TWN-leftright)%TWN;
        }
        else {
            wall = (wall+TWN+leftright)%TWN;
        }
    }
    return wall;
}

int start_border(Map *map, int row, int col, int leftright){
    // When we are at the sides, 1,1 based coordinates
    if (col == 1){
        if (row%2){
            return (leftright == LPATH) ? bot:right;
        }
        else {
            return (leftright == RPATH) ? bot:right;
        }
    }
    else if (col == map->cols){
        if ((row*col)%2){
            return (leftright == LPATH) ? left:bot;
        }
        else {
            return (leftright == RPATH) ? left:bot;
        }
    }
    // When we are at the top/bottom, 1,1 based coordinates
    else if (row == 1 ){
        return (leftright == LPATH) ? right:left;
    }
    else if (row == map->rows){
        return (leftright == RPATH) ? right:left;
    }
    return 99;
}

int executioner(Map *map, int row, int col, int leftright){
    int move_set[2][3][2] = {
        {{-1, 0}, {0, 1}, {0, -1}}, // even position, r c incements
        {{1, 0},  {0, 1}, {0, -1}}  // odd
    };

    int oden = (row+col)%2;
    int wall = start_border(map, row, col, leftright);
    if (wall == 99){    // value can't be negative, and returns are between 0-2. So 99 shouldn't ever happen with correct coordinates 
        printf("Wrong starting position.\n");
        return 1;
    }
    // rotate on start because of my checking loop
    wall = rotate(wall, oden, row, leftright, true);

    //run for max "maze*maze" times
    for (int i = 0; i < (map->rows*map->cols)*(map->rows*map->cols); i++){
        // 1,1 based coordinates
            if (row < 1 || row-1 >= map->rows || col < 1 || col-1 >= map->cols) {
            return 0;
        }
        printf("%d,%d\n", row, col);
        oden = (row+col)%2;

        // looking for free wall, depending on oden
        // max TWN times
        for (int i = 0; i < TWN; i++) {
            wall = rotate(wall, oden, row, leftright, false);
            if (!isborder(map, row, col, wall)) {
                break;
            }
        }
        // apply moves
        row = row + move_set[oden][wall][0];
        col = col + move_set[oden][wall][1];
        wall = (TWN-wall)%3;    //flip entered border
    }
    return 0;
}

int main (int argc, char *argv[]){
    Map map;
    if (argc == 2 && strcmp(argv[1], "--help") == 0){
        helper();
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1], "--test") == 0){
        if (loader(&map, argv[2])){
            printf("Invalid\n");
            return 1;
        }
        else if (tester(&map)){
            printf("Invalid\n");
            dumper(&map);
            return 1;
        }
        else{
            printf("Valid\n");
            dumper(&map);
            return 0;
        }
    }
    else if (argc == 5 && (strcmp(argv[1], "--lpath") == 0 || strcmp(argv[1], "--rpath") == 0)){
        char *tmp_x, *tmp_y;
        int x_start = strtol(argv[2], &tmp_x, 10);
        int y_start = strtol(argv[3], &tmp_y, 10);
        if ((tmp_x[0] || tmp_y[0]) != '\0'){
            printf("Wrong argument.\n");
            return 1;
        }
        if (loader(&map, argv[4])){
            printf("Invalid\n");
            return 1;
        }
        else if (tester(&map)){
            printf("Invalid\n");
            dumper(&map);
            return 1;
        }
        int leftright;
        if (strcmp(argv[1], "--lpath") == 0){
            leftright = LPATH;
        }
        else {
            leftright = RPATH;
        }
        executioner(&map, x_start, y_start, leftright);
        dumper(&map);
        return 0;
    }
    printf("Wrong arguments.\n");
    return 1;
}