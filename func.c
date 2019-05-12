#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern int isDirectory(char *dir) {
  struct stat fileInfo;
  stat(dir, &fileInfo);
  if (S_ISDIR(fileInfo.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}

extern void storeFiles(FILE *file, char *baseDirectory, int fileCount) {

  DIR *dir;
  struct dirent *dirp;

  // Check if directory exists
  if ((dir = opendir(baseDirectory)) == NULL) {
    printf("%s %s %s\n\n", "Error:", baseDirectory, "directory does not exist");
  } else {
    printf("%s %s %s\n\n", "Success: Opened", baseDirectory, "directory");

    // Iterate through directory and its files
    while ((dirp = readdir(dir)) != NULL) {
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
