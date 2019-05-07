#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){

    if (argc < 4){
        printf("%s\n", "Please enter your input in the following format:");
        printf("%s\n\n", "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory list>");
        exit(1);
    }

    if (strcmp(argv[1], "-c") == 0){    // store

    } else if (strcmp(argv[1], "-a") == 0){    // append

    } else if (strcmp(argv[1], "-x") ==  0){    // extract
        if (strcmp(argv[2], "-o") == 0){    // extract file number -o
            int file_number = atoi(argv[3]);

        } else {
            int file_number = 0;
        }

    } else if (strcmp(argv[1], "-m") == 0){    // print

    } else if (strcmp(argv[1], "-p") == 0){    // display

    } else {
        printf("%s\n\n", "Please enter a valid flag value.");
        exit(1);
    }

}
