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
#define DIR_ 1
#define FILE_ 2

list *head;
list *list_current;

// HELPER FUNCS
static int check_ext(char *);
static int is_directory_or_file(char *);
static void add_to_archive(metadata **, char *, char *);
static void populate_archive(int, char *, char *);

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

int is_directory_or_file(char *dir) {
  struct stat fileInfo;
  stat(dir, &fileInfo);
  if (S_ISDIR(fileInfo.st_mode)) {
    return DIR_;
  } else if (S_ISREG(fileInfo.st_mode)) {
    return FILE_;
  } else {
    return 0;
  }
}

extern args *parse_args(int argc, char **argv) {
  int flag, temp, file_start, i;
  args *args_ = malloc(sizeof(args_));
  args_->occurence = 0;
  args_->no_of_files = 0;

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
    file_start = 5;
  } else {
    args_->adtar_file = argv[2];
    file_start = 3;
  }
  args_->file_list = malloc(sizeof(file_name) * (argc - file_start));
  i = 0;
  while (file_start < argc) {
    strncpy(args_->file_list[i].name, argv[file_start], 200);
    args_->file_list[file_start].name[199] = '\0';
    args_->no_of_files++;
    file_start++;
    i++;
  }

  if (!check_ext(args_->adtar_file)) {
    fprintf(stderr, "Invokation Error: Please pass an .ad file to "
                    "<archive-file>\n");
    exit(EXIT_FAILURE);
  }
  return args_;
}

extern void create_archive(char *adtar_file, file_name *file_list,
                           int no_of_files, int occurence) {
  FILE *archive_fp;
  list *archive_head;
  long location = -1;
  int version = 0;
  int i;

  // OPEN DIRECTORY AND ARCHIVE
  archive_fp = fopen(adtar_file, "w");
  if (archive_fp == NULL) {
    perror("Open <adtar_file> failed");
    exit(EXIT_FAILURE);
  }
  fprintf(archive_fp, "%ld\n", location);
  fclose(archive_fp);
  VLOG(DEBUG, "Created file at ~/%s", adtar_file);

  head = list_current;

  for (i = 0; i < no_of_files; i++) {
    char *path = file_list[i].name;
    switch (is_directory_or_file(path)) {
    case DIR_:
      VLOG(DEBUG, "%s is a directory", path);
      break;
    case FILE_:
      VLOG(DEBUG, "%s is a file", path);
      populate_archive(FILE_, path, adtar_file);
      break;
    default:
      VLOG(DEBUG, "What is this %s", path);
    }
  }
  if ((archive_fp = fopen(adtar_file, "ab")) == NULL) {
    perror("open adtar file failed");
    exit(EXIT_FAILURE);
  }

  location = ftell(archive_fp);
  do {
    if (fwrite(list_current, sizeof(metadata), 1, archive_fp) != 1) {
      perror("Write Metadata to file error");
      exit(EXIT_FAILURE);
    }
  } while ((list_current = get_next(&list_current)) != NULL);
  free(list_current);
  free(head);
  fclose(archive_fp);

  if ((archive_fp = fopen(adtar_file, "r+")) == NULL) {
    perror("Open adtar file failed");
    exit(EXIT_FAILURE);
  }
  fprintf(archive_fp, "%ld\n", location);
  fclose(archive_fp);
}

void populate_archive(int dir_flag, char *path, char *adtar_file) {
  char *full_path = basename(path);
  DIR *dir;
  struct dirent *dirp;
  metadata *metadata_ = malloc(sizeof(metadata));
  switch (dir_flag) {
  case DIR_:
    if ((dir = opendir(path)) == NULL) {
      perror("Open <file/directory list> failed");
      exit(EXIT_FAILURE);
    }
    // Iterate through directory and its files
    while ((dirp = readdir(dir)) != NULL) {
    }
    break;
  case FILE_:
    get_file_stat(path, &metadata_);
    strncat(metadata_->name, full_path, 200);
    metadata_->name[199] = '\0';
    add_to_archive(&metadata_, full_path, adtar_file);
    break;
  }
}

void add_to_archive(metadata **metadata_, char *path, char *archive_file) {
  metadata *metadata__ = *metadata_;
  FILE *fp_add;
  FILE *archive_fp;
  char buffer[512];
  int size = 0;

  if ((fp_add = fopen(path, "rb")) == NULL) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  if ((archive_fp = fopen(archive_file, "ab")) == NULL) {
    perror("Failed to archive file");
    exit(EXIT_FAILURE);
  }

  metadata__->offset = ftell(archive_fp);
  add(&list_current, &metadata__);

  while ((size = fread(buffer, sizeof(char), sizeof(buffer), fp_add)) > 0) {
    if (fwrite(buffer, sizeof(char), size, archive_fp) != size) {
      perror("write to archive failed");
      exit(EXIT_FAILURE);
    }
  }

  fclose(fp_add);
  fclose(archive_fp);
}
