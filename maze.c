/*
 * Name: Maze
 * Author: Zdenek Teller
 * Login: xtellez00
 * Date: 22.11.2023
 * Purpose: Second VUT project (Triangular maze pathfinder)
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define BORDER_LEFT 1
#define BORDER_RIGHT 2
#define BORDER_HORIZONTAL 4

typedef struct { // Basic Map structure
    int rows;
    int cols;
    unsigned char *cells;
} Map;

//Creates default instance of Map
Map* map_const(){
    Map* map = malloc(sizeof(Map));
    map->rows = 0;
    map->cols = 0;
    map->cells = NULL;
    return map;
}

void map_dest(Map* map){ // Destroys map and frees memory
    map->rows = 0;
    map->cols = 0;
    free(map->cells);
    free(map);
}

bool isNotNumber(const char number[]) // Function to check if variable is type int
{
    int i = 0;

    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 1;
    }
    return 0;
}

int overFlowCheck(char *fileName, int x, int y){ // Checks if the array is overflowing anywhere
    FILE *file2 = fopen(fileName, "r");
    int lines = 0, len = 0, c;
    char useless[4];
    fgets( useless,sizeof(useless),file2);
    fgetc(file2);
    while(true)
    {
        c = fgetc( file2 );
        //printf("%c", c);
        len++;
        if(c == '\n' ){
            lines++;
            if(!((len-y==y)|| (len == 1))){
                //printf("Length of line is not what it should have been len=%d y=%d\n", len, y);
                fclose(file2);
                return 1;
            }
            len = 0;
        }
        else if(c == EOF) {
            //lines++;
            break;
        }
    }
    fclose(file2);
    if(lines!=x){
        //printf("Lines are not what they should have been lines=%d x=%d", lines, x);
        return 1;
    }
    return 0;
}

void printMap(Map *map){ // Prints Map into stdout. Can be used for debugging
    for(int i = 0; i<map->rows; i++){
        for(int j = 0; j< map->cols; j++){
            printf("%d ", map->cells[i*map->cols+j]);
        }
        printf("\n");
    }
}

bool isborder(Map *map, int r, int c, int border){ // Checks if border is present
    if(map->cells[r*map->cols+c]&border){
        //printf("maps cell: [%d][%d] has border: %d\n", r+1,c+1, border);
        return true;
    }
    return false;
}

int testInput(FILE *file, char* fileName, Map *map){ // Validation of maze array

    char size[4];
    fgets(size, sizeof(size), file); // Reads first line with rows and cols

    int x = strtok(size, " ")[0]-'0'; //Lines
    int y = strtok(NULL, " ")[0]-'0'; //Cols
    map->rows = x;
    map->cols = y;
    unsigned char* cells = realloc(map->cells, sizeof(int)*(x*y));
    if(cells == NULL){
        printf("Map realloc fail");
        free(cells);
        return 1;
    }
    map->cells = cells;
    //free(cells);

    char line[y*2+2];
    char *endP;
    if(overFlowCheck(fileName, x,y)) return 1; // Checks if there are more rows/columns than expected
    fgets(line, (int)sizeof(line), file);

    // Loads values into map.cells
    for(int j = 0; j<x; j++){
        fgets(line, (int)sizeof(line), file);
        for(int i = 0; i<y; i++){
            map->cells[j*y+i] = strtol(&line[i*2], &endP, 10);
            if((map->cells[j*y+i]>7)){
                //printf("At least one number is larger than 7 or smaller than 0!");
                return 1;
            }
        }
    }
    //printMap(map);

    //Checking if borders make sense
    bool errorInBorders = false;
    for(int i = 0; i<map->rows; i++) {
        for (int j = 0; j < map->cols; j++) {
            if (j == 0) { // Left and right neighbour (exceptions for first and last value)
                if (isborder(map, i, j, BORDER_RIGHT) & !isborder(map, i, j + 1, BORDER_LEFT)) errorInBorders=true;
            } else if (j == map->cols - 1) {
                if (isborder(map, i, j, BORDER_LEFT) & !isborder(map, i, j - 1, BORDER_RIGHT)) errorInBorders = true;
            }else {
                if ((isborder(map, i, j, BORDER_RIGHT) & !isborder(map, i, j + 1, BORDER_LEFT)) ||
                        (isborder(map, i, j, BORDER_LEFT) & !isborder(map, i, j - 1, BORDER_RIGHT))) {
                    errorInBorders = true;
                }
            }
            // Top and bottom layer (exception for out of maze facing cells)
            if(((i == 0) & ((i+j)%2==0) ) || ((i == map->rows-1) & ((i+j)%2==1)))continue;
            else if(i == 0){
                if(isborder(map, i,j,BORDER_HORIZONTAL) & !isborder(map, i+1, j, BORDER_HORIZONTAL)) errorInBorders = true;
            }
            else if(i == map->rows-1) {
                if(isborder(map, i, j, BORDER_HORIZONTAL) & !isborder(map, i-1, j, BORDER_HORIZONTAL)) errorInBorders = true;
            }
            else{
                if((isborder(map, i, j, BORDER_HORIZONTAL) & !isborder(map, i+1, j, BORDER_HORIZONTAL)) & ((i+j)%2==1) ||
                        (isborder(map, i, j, BORDER_HORIZONTAL) & !(isborder(map, i-1, j, BORDER_HORIZONTAL)) & ((i+j)%2 == 0))){
                    errorInBorders = true;
                }
            }

        }
    }
    if(errorInBorders){
        //printf("Borders do not correlate to each other");
        return 1;
    }
    //printMap(map);
    return 0;
}

int argCheck(int argc, char *argv[], Map* map){
    if(strcmp(argv[1],"--help")==0){ // Help content
        printf("\nFor help type '--help'\n");
        printf("To test maze validity type command like './maze --test <FILE>'\n\n");
        printf("To solve maze type command like: './maze <OPTION> <ROW> <COLUMN> <FILE>'\n\n");
        printf("Options:\n");
        printf("For right hand rule type '--rpath'\n");
        printf("For left hand rule type '--lpath'\n\n");
        printf("Good luck with solving ;) - Zdenek Teller\n");
        return 0;
    }
    else if(strcmp(argv[1],"--test")==0){ // Test option (Opens file -> tests map -> evaluates -> finishes)
        FILE *file = fopen(argv[2], "r");
        if(file == NULL){
            printf("File could not been opened\n");
            return 1;
        }
        //Map *map = map_const();
        int test = testInput(file, argv[2], map);
        if(test==0) printf("Valid");
        else printf("Invalid");
        fclose(file);
        map_dest(map); //TODO
        return 0;
    }
    // Else checks if all needed arguments are present
    else if (argc != 5 || (strcmp(argv[1],"--rpath")!=0 && strcmp(argv[1],"--lpath")!=0) || isNotNumber(argv[2])  || isNotNumber(argv[3])) {
        printf("Input is not written correctly\n");
        return 1;
    }
    return 2;
}

int start_border(Map *map, int r, int c, int leftright){
    if(((r==map->rows) || (r == 1)) || ((c == map->cols) || (c == 1))){ // Checks if we are on the border of the maze
        if(leftright == 1){//RIGHT VARIANT
            if((c == 1) & (r%2 == 1)) return BORDER_RIGHT;
            else if((c==1) & (r%2 == 0)) return BORDER_HORIZONTAL;
            else if((c == map->cols) & ((c+r)%2==0)) return BORDER_HORIZONTAL;
            else if((c == map->cols) & ((c+r)%2==1)) return BORDER_LEFT;
            else if((r == 1) & ((c+r)%2==0)) return BORDER_LEFT;
            else if((c+r)%2==1) return BORDER_RIGHT;
        }
        // Left Variant
        else{
            if((c == 1) & (r%2 == 1)) return BORDER_HORIZONTAL; // Right if odd line
            else if((c==1) & (r%2 == 0)) return BORDER_RIGHT; // Down in equal line
            else if((c == map->cols) & ((c+r)%2==0)) return BORDER_LEFT;
            else if((c == map->cols) & ((c+r)%2==1)) return BORDER_HORIZONTAL;
            else if((r == 1) & ((c+r)%2==0)) return BORDER_RIGHT;
            else if((c+r)%2==1) return BORDER_LEFT;
        }
    }
    return 0;
}

void rpath(Map* map, int border, int r, int c){
    //printMap(map);
    r--;
    c--;
    int lastR = 0;
    int lastC = 0;
    //while(true){
    while((r>=0) & (c >= 0) & (r< map->rows) & (c<map->cols))
    {
        if(!((r==lastR) & (c==lastC))){
            printf("%d,%d\n", r+1, c+1);
        }
        lastC = c;
        lastR = r;
        if((r+c)%2!=0) { //When odd cell
            if (border & BORDER_HORIZONTAL) {
                if (!isborder(map, r, c, BORDER_HORIZONTAL)) {
                    r++;
                    border = BORDER_LEFT;
                } else {
                    border = BORDER_RIGHT;
                }
            } else if (border & BORDER_RIGHT) {
                if (!isborder(map, r, c, BORDER_RIGHT)) {
                    c++;
                } else {
                    border = BORDER_LEFT;
                }
            } else if (border & BORDER_LEFT) {
                if (!isborder(map, r, c, BORDER_LEFT)) {
                    c--;
                }
                border = BORDER_HORIZONTAL;
            }
        }
        else{ // When equal cell
            if (border & BORDER_HORIZONTAL) {
                if (!isborder(map, r, c, BORDER_HORIZONTAL)) {
                    r--;
                    border = BORDER_RIGHT;
                } else {
                    border = BORDER_LEFT;
                }
            } else if (border & BORDER_LEFT) {
                if (!isborder(map, r, c, BORDER_LEFT)) {
                    c--;
                } else {
                    border = BORDER_RIGHT;
                }
            } else if (border & BORDER_RIGHT) {
                if (!isborder(map, r, c, BORDER_RIGHT)) {
                    c++;
                }
                border = BORDER_HORIZONTAL;
            }
        }
    }
}

void lpath(Map* map, int border, int r, int c){
    //printMap(map);
    r--;
    c--;
    int lastR = 0;
    int lastC = 0;
    while((r>=0) & (c >= 0) & (r< map->rows) & (c<map->cols))
    {
        if(!((r==lastR) & (c==lastC))){
            printf("%d,%d\n", r+1, c+1);
        }
        lastC = c;
        lastR = r;
        if((r+c)%2!=0) { //When odd cell
            if (border & BORDER_HORIZONTAL) {
                if (!isborder(map, r, c, BORDER_HORIZONTAL)) {
                    r++;
                    border = BORDER_RIGHT;//1
                } else {
                    border = BORDER_LEFT;//2
                }
            } else if (border & BORDER_RIGHT) {
                if (!isborder(map, r, c, BORDER_RIGHT)) {
                    c++;
                }
                border = BORDER_HORIZONTAL; //1
            } else if (border & BORDER_LEFT) {
                if (!isborder(map, r, c, BORDER_LEFT)) {
                    c--;
                    border = BORDER_LEFT;//there was nothing
                }
                else border = BORDER_RIGHT;//4
            }
        }
        else{ // When equal cell
            if (border & BORDER_HORIZONTAL) {
                if (!isborder(map, r, c, BORDER_HORIZONTAL)) {
                    r--;
                    border = BORDER_LEFT;
                } else {
                    border = BORDER_RIGHT;
                }
            } else if (border & BORDER_LEFT) {
                if (!isborder(map, r, c, BORDER_LEFT)) {
                    c--;
                }
                border = BORDER_HORIZONTAL;
            } else if (border & BORDER_RIGHT) {
                if (!isborder(map, r, c, BORDER_RIGHT)) {
                    c++;
                }
                else{
                    border = BORDER_LEFT;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {

    Map *map = map_const();
    //Checks the input
    int arg = argCheck(argc, argv, map);
    if(arg<2) {
        printf("\n");
        return arg;
    }

    FILE *file;
    //Opening of file
    file = fopen(argv[4], "r");

    if(file == NULL){ // Checks result od file opening
        printf("Error opening file");
        return 1;
    }

    if(map == NULL){ // Checks if map initialized correctly
        printf("Map malloc error");
        return 1;
    }
    if(testInput(file, argv[4], map)){ // Tests if maze is correct
        printf("Invalid");
        map_dest(map);
        return 1;
    }
    //printMap(map);

    int leftright;
    char *endP;
    if(strcmp(argv[1], "--rpath")==0) leftright = 1;
    else{
        leftright = 0;
    }
    int start = start_border(map, (int)strtol(argv[2], &endP, 10), (int)strtol(argv[3], &endP, 10), leftright);
    if(start == 0){
        printf("Invalid starting point!");
        map_dest(map);
        return 1;
    }
    //else printf("Starting point: %d\n", start);



    //printMap(&map);
    int r = (int)strtol(argv[2], &endP, 10);
    int c = (int)strtol(argv[3], &endP, 10);
    if(leftright == 1){
        rpath(map, start, r, c);
    }else{
        lpath(map, start, r, c);
    }
    //printf("\n");
    map_dest(map);
    fclose(file);
    return 0;
    //test2
}
