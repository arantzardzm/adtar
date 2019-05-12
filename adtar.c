#include "common.h"
#include "func.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // DECLARE VARS
  FILE *archive;
  char *archiveFile;
  char *baseDirectory;
  int fileCount = 0;

  if (argc < 4) {
    printf("%s\n", "Please enter your input in the following format:");
    printf("%s\n\n", "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> "
                     "<file/directory list>");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "-c") == 0) { // store
    archiveFile = argv[2];
    baseDirectory = argv[3];

    archive = fopen(archiveFile, "w");
    if (archive == NULL) {
      printf("%s\n\n", "Error: Failed to open .ad archive file");
      exit(1);
    } else {
      printf("%s\n", "Success: Opened .ad archive file");
    }
    storeFiles(archive, baseDirectory, fileCount);

  } else if (strcmp(argv[1], "-a") == 0) { // append

  } else if (strcmp(argv[1], "-x") == 0) { // extract
    if (strcmp(argv[2], "-o") == 0) {      // extract file number -o
      int file_number = atoi(argv[3]);

    } else {
      int file_number = 0;
    }

  } else if (strcmp(argv[1], "-m") == 0) { // print

  } else if (strcmp(argv[1], "-p") == 0) { // display

  } else {
    printf("%s\n\n", "Please enter a valid flag value.");
    exit(1);
  }

  return 0;
}
