#include "common.h"
#include "func.h"
#include "struct.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define C 0
#define A 1
#define X 2
#define M 3
#define P 4

static int check_ext(char *);
int is_directory(char *);

// ADAPTED FROM STACK OVERFLOW -
// https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
int check_ext(char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return 0;
  if (strncmp(dot, ".ad", 3) != 0)
    return 0;
  return 1;
}

int is_directory(char *dir) {
  struct stat fileInfo;
  stat(dir, &fileInfo);
  if (S_ISDIR(fileInfo.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}

extern args *parse_args(int argc, char **argv) {
  int flag, temp;
  args *args_ = malloc(sizeof(args_));
  args_->occurence = 0;

  if (argc < 4) {
    fprintf(
        stderr, "%s\n%s\n",
        "Invokation Error: Please enter your input in the following format:",
        "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory "
        "list>");
    exit(EXIT_FAILURE);
  }

  while ((flag = getopt(argc, argv, "caxmpo:")) != -1) {
    switch (flag) {
    case 'c':
      args_->flag = C;
      break;
    case 'a':
      args_->flag = A;
      break;
    case 'x':
      args_->flag = X;
      break;
    case 'm':
      args_->flag = M;
      break;
    case 'p':
      args_->flag = P;
      break;
    case 'o':
      temp = atoi(optarg);
      if (temp < 0) {
        fprintf(stderr, "Please pass an occurence number to args \n");
      }
      args_->occurence = temp;
      break;
    case '?':
    default:
      fprintf(stderr, "Bad Argument Passed\n");
    }
  }

  if (args_->occurence > 0) {
    if (argc < 6) {
      fprintf(
          stderr, "%s\n%s\n",
          "Invokation Error: Please enter your input in the following format:",
          "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory "
          "list>");
      exit(EXIT_FAILURE);
    }
    args_->adtar_file = argv[4];
    args_->directory = argv[5];
  } else {
    args_->adtar_file = argv[2];
    args_->directory = argv[3];
  }

  if (!check_ext(args_->adtar_file)) {
    fprintf(stderr, "Invokation Error: Please pass an .ad file to "
                    "<archive-file>\n");
    exit(EXIT_FAILURE);
  }
  return args_;
}

extern void create_archive(char *adtar_file, char *base_directory,
                           int occurence) {
  DIR *dir;
  struct dirent *dirp;
  FILE *archive;

  // OPEN DIRECTORY AND
  archive = fopen(adtar_file, "w");
  if (archive == NULL) {
    perror("Open <archive-file> failed");
    exit(EXIT_FAILURE);
  }

  if ((dir = opendir(base_directory)) == NULL) {
    perror("Open <directory> failed");
    exit(EXIT_FAILURE);
  } else {
    printf("%s %s %s\n", "Success: Opened", base_directory, "directory");

    // Iterate through directory and its files
    while ((dirp = readdir(dir)) != NULL) {
      char *base = base_directory;
      char *name = dirp->d_name;

      printf("%s, %s\n", base, name);
      char *concat = strcat(base, name);

      if (is_directory(concat)) { // directory
        VLOG(DEBUG, "It is a directory");
        // storeFiles(archive, baseDir + dirp->d_name + "/", true, file_count);
      } else {
        VLOG(DEBUG, "It is not a directory");
      }
    }
  }
}
