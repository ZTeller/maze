#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

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

int argCheck(int argc, char *argv[]){
    if(strcmp(argv[1],"--help")==0){
        printf("For help type '--help'\n");
        printf("To test maze validity type command like './maze --test <FILE>'\n\n");
        printf("To solve maze type command like: './maze <OPTION> <ROW> <COLUMN> <FILE>'\n\n");
        printf("Options:\n");
        printf("For right hand rule type '--rpath'\n");
        printf("For left hand rule type '--lpath'\n");
        return 0;
    }
    else if(strcmp(argv[2],"--test")==0){
        return 0; //TADY SEBUDE TESTOVAT
    }
    else if (argc != 5 || (strcmp(argv[1],"--rpath")!=0 && strcmp(argv[1],"--lpath")!=0) || isNotNumber(argv[2])  || isNotNumber(argv[3])) {
        printf("Input is not written correctly");
        return 1;
    }
    else return 2;
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
            if(len-(y)!=y){
                printf("Length of line is not what it should have been");
                return 1;
            }
            len = 0;
        }
        if(c == EOF) {
            lines++;
            break;
        }
    }
    if(lines!=x){
        printf("Lines are not what they should have been");
        return 1;
    }
    return 0;
}

int testInput(FILE *file, char* fileName){
    char size[4];
    fgets(size, sizeof(size), file);
    int x = strtok(size, " ")[0]-'0';
    int y = strtok(NULL, " ")[0]-'0';

    int map[x][y];
    int lineNum =0;
    char line[y*2+2];
    char *endP;
    if(overFlowCheck(fileName, x,y)) return 1;
    fgets(line, (int)sizeof(line), file);
    for(int j = 0; j<x; j++){
        fgets(line, (int)sizeof(line), file);
        for(int i = 0; i<y; i++){
            map[j][i] = (int)strtol(&line[i*2], &endP, 10);
            printf("%d ", map[j][i]);
        }
        printf("\n");
        lineNum++;
    }
    return 0;
}


int main(int argc, char *argv[]) {

    //Checks the input
    int arg = argCheck(argc, argv);
    if(arg<2) return arg;

    FILE *file;

    file = fopen(argv[4], "r");

    if(file == NULL){
        printf("Error opening file");
        return 1;
    }

    if(testInput(file, argv[4])) return 1;


    return 0;
}
