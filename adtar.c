#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int isDirectory(char *dir) {
   struct stat fileInfo;
   stat(dir, &fileInfo);
   if (S_ISDIR(fileInfo.st_mode)){
       return 1;
   } else{
       return 0;
   }
}

void storeFiles(FILE *file, char *baseDirectory, int fileCount){

    DIR *dir;
    struct dirent *dirp;

    // Check if directory exists
    if ((dir = opendir(baseDirectory)) == NULL){
        printf("%s %s %s\n\n", "Error:", baseDirectory, "directory does not exist");
    } else {
        printf("%s %s %s\n\n", "Success: Opened", baseDirectory, "directory");

        // Iterate through directory and its files
        while ((dirp = readdir(dir)) != NULL){
            char *base = baseDirectory;
            char *name = dirp->d_name;

            printf("%s, %s\n", base, name);
            char *concat = strcat(base, name);

            if (isDirectory(concat)) { // directory
                printf("it is");
                // storeFiles(archive, baseDir + dirp->d_name + "/", true, file_count);
            } else {
                printf("its is not");
            }
        }
    }

}

int main(int argc, char *argv[]){

    // Variables
    FILE *archive;
    char *archiveFile;
    char *baseDirectory;
    int fileCount = 0;

    if (argc < 4){
        printf("%s\n", "Please enter your input in the following format:");
        printf("%s\n\n", "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory list>");
        exit(1);
    }

    if (strcmp(argv[1], "-c") == 0){    // store
        archiveFile = argv[2];
        baseDirectory = argv[3];

        archive = fopen(archiveFile,"w");
        if (archive == NULL){
            printf("%s\n\n", "Error: Failed to open .ad archive file");
            exit(1);
        } else {
            printf("%s\n", "Success: Opened .ad archive file");
        }
        storeFiles(archive, baseDirectory, fileCount);

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

    return 0;

}
