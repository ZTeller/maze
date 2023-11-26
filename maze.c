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

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

Map* map_const(){
    Map* map = malloc(sizeof(Map));
    map->rows = 0;
    map->cols = 0;
    map->cells = NULL;
    return map;
}

void map_dest(Map* map){
    map->rows = 0;
    map->cols = 0;
    free(map->cells);
    free(map);
}

bool isNotNumber(const char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 1;
    }
    return 0;
}

int overFlowCheck(char *fileName, int x, int y){
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

void printMap(Map *map){
    for(int i = 0; i<map->rows; i++){
        for(int j = 0; j< map->cols; j++){
            printf("%d ", map->cells[i*map->cols+j]);
        }
        printf("\n");
    }
}

bool isborder(Map *map, int r, int c, int border){
    if(map->cells[r*map->cols+c]&border){
        //printf("maps cell: [%d][%d] has border: %d\n", r+1,c+1, border);
        return true;
    }
    return false;
}

int testInput(FILE *file, char* fileName, Map *map){
    char size[4];
    fgets(size, sizeof(size), file);
    int x = strtok(size, " ")[0]-'0'; //Lines
    int y = strtok(NULL, " ")[0]-'0'; //Elements on line
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

    int lineNum =0;
    char line[y*2+2];
    char *endP;
    if(overFlowCheck(fileName, x,y)) return 1; // Checks if there are more rows/columns than expected
    fgets(line, (int)sizeof(line), file);

    for(int j = 0; j<x; j++){
        fgets(line, (int)sizeof(line), file);
        for(int i = 0; i<y; i++){
            map->cells[j*y+i] = strtol(&line[i*2], &endP, 10);
            if(map->cells[j*y+i]>7){
                printf("At least one number is larger than 7!");
            }
        }
        lineNum++;
    }
    //printMap(map);

    //Checking if borders make sense
    bool errorInBorders = false;
    for(int i = 0; i<map->rows; i++) {
        for (int j = 0; j < map->cols; j++) {
            if (j == 0) {
                if (isborder(map, i, j, 2) & !isborder(map, i, j + 1, 1)) errorInBorders=true;
            } else if (j == map->cols - 1) {
                if (isborder(map, i, j, 1) & !isborder(map, i, j - 1, 2)) errorInBorders = true;
            }else {
                if ((isborder(map, i, j, 2) & !isborder(map, i, j + 1, 1)) ||
                        (isborder(map, i, j, 1) & !isborder(map, i, j - 1, 2))) {
                    errorInBorders = true;
                }
            }

            if(i == 0 || ((i == map->rows-1) & (map->rows%2==0)))continue;
            else if(i == map->rows-1) {
                if(isborder(map, i, j, 4) & !isborder(map, i-1, j, 4)) errorInBorders = true;
            }
            else{
                if((isborder(map, i, j, 4) & !isborder(map, i+1, j, 4)) & ((i+j)%2==1) ||
                        (isborder(map, i, j, 4) & !(isborder(map, i-1, j, 4)) & ((i+j)%2 == 0))){
                    errorInBorders = true;
                }
            }

        }
    }
    if(errorInBorders){
        printf("Borders do not correlate to each other");
        return 1;
    }
    //printMap(map);
    return 0;
}

int argCheck(int argc, char *argv[], Map* map){
    if(strcmp(argv[1],"--help")==0){
        printf("For help type '--help'\n");
        printf("To test maze validity type command like './maze --test <FILE>'\n\n");
        printf("To solve maze type command like: './maze <OPTION> <ROW> <COLUMN> <FILE>'\n\n");
        printf("Options:\n");
        printf("For right hand rule type '--rpath'\n");
        printf("For left hand rule type '--lpath'\n");
        return 0;
    }
    else if(strcmp(argv[1],"--test")==0){
        FILE *file = fopen(argv[2], "r");
        if(file == NULL){
            printf("File could not been opened\n");
            return 0;
        }
        //Map *map = map_const();
        int test = testInput(file, argv[2], map);
        if(test==0) printf("Valid");
        else printf("Invalid");
        fclose(file);
        map_dest(map); //TODO
        return 0;
    }
    else if (argc != 5 || (strcmp(argv[1],"--rpath")!=0 && strcmp(argv[1],"--lpath")!=0) || isNotNumber(argv[2])  || isNotNumber(argv[3])) {
        printf("Input is not written correctly\n");
        return 0;
    }
    return 1;
}

int start_border(Map *map, int r, int c, int leftright){
    if(((r==map->rows) || (r == 1)) || ((c == map->cols) || (c == 1))){
        if(leftright == 1){//RIGHT VARIANT
            if((c == 1) & (r%2 == 1)) return 2;
            else if((c==1) & (r%2 == 0)) return 4;
            else if((c == map->cols) & ((c+r)%2==0)) return 4;
            else if((c == map->cols) & ((c+r)%2==1)) return 1;
            else if((r == 1) & ((c+r)%2==0)) return 1;
            else if((c+r)%2==1) return 2;
        }
        // Left Variant
        else{
            if((c == 1) & (r%2 == 1)) return 4; // Right if odd line
            else if((c==1) & (r%2 == 0)) return 2; // Down in equal line
            else if((c == map->cols) & ((c+r)%2==0)) return 1;
            else if((c == map->cols) & ((c+r)%2==1)) return 4;
            else if((r == 1) & ((c+r)%2==0)) return 2;
            else if((c+r)%2==1) return 1;
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
            if (border & 4) {
                if (!isborder(map, r, c, 4)) {
                    r++;
                    border = 1;
                } else {
                    border = 2;
                }
            } else if (border & 2) {
                if (!isborder(map, r, c, 2)) {
                    c++;
                } else {
                    border = 1;
                }
            } else if (border & 1) {
                if (!isborder(map, r, c, 1)) {
                    c--;
                }
                border = 4;
            }
        }
        else{ // When qual cell
            if (border & 4) {
                if (!isborder(map, r, c, 4)) {
                    r--;
                    border = 2;
                } else {
                    border = 1;
                }
            } else if (border & 1) {
                if (!isborder(map, r, c, 1)) {
                    c--;
                } else {
                    border = 2;
                }
            } else if (border & 2) {
                if (!isborder(map, r, c, 2)) {
                    c++;
                }
                border = 4;
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
            if (border & 4) {
                if (!isborder(map, r, c, 4)) {
                    r++;
                    border = 2;//1
                } else {
                    border = 1;//2
                }
            } else if (border & 2) {
                if (!isborder(map, r, c, 2)) {
                    c++;
                }
                border = 4; //1
            } else if (border & 1) {
                if (!isborder(map, r, c, 1)) {
                    c--;
                    border = 1;//there was nothing
                }
                else border = 2;//4
            }
        }
        else{ // When equal cell
            if (border & 4) {
                if (!isborder(map, r, c, 4)) {
                    r--;
                    border = 1;
                } else {
                    border = 2;
                }
            } else if (border & 1) {
                if (!isborder(map, r, c, 1)) {
                    c--;
                }
                border = 4;
            } else if (border & 2) {
                if (!isborder(map, r, c, 2)) {
                    c++;
                }
                else{
                    border = 1;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {

    Map *map = map_const();
    //Checks the input
    int arg = argCheck(argc, argv, map);
    if(arg<1) {
        printf("\n");
        return arg;
    }

    FILE *file;

    file = fopen(argv[4], "r");

    if(file == NULL){
        printf("Error opening file");
        return 1;
    }

    if(map == NULL){
        printf("Map malloc error");
        return 1;
    }
    if(testInput(file, argv[4], map)){
        printf("Invalid maze");
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
