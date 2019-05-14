#include "common.h"
#include "func.h"
#include "struct.h"
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define C 0
#define A 1
#define X 2
#define M 3
#define P 4
#define DIR_ 1
#define FILE_ 2
#define COMPRESSED_ 3

extern list *head;
extern args *args_;

// HELPER FUNCS
static int check_ext(char *);
static int is_directory_or_file(char *);
static void add_to_archive(metadata **, char *);
static void populate_archive(int, char *);
static void add_file_stat(char *, metadata **);
static void extract_file(metadata *);
static void print_path(char *, int, int);
static void destruct_all(char *);

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

extern void parse_args(int argc, char **argv) {
  int flag, temp, file_start, i;
  args *args__ = malloc(sizeof(args));
  args_ = args__;
  args_->occurence = 0;
  args_->no_of_files = 0;

  if (argc < 4) {
    fprintf(
        stderr, "%s\n%s\n",
        "Invokation Error: Please enter your input in the following format:",
        "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory "
        "list>");
    destruct_args();
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
        fprintf(stderr, "Please pass an args_->occurence number to args \n");
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
      destruct_args();
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
    destruct_args();
    exit(EXIT_FAILURE);
  }
}

extern void destruct_args() {
  if (args_->file_list != NULL)
    free(args_->file_list);
  if (args_ != NULL)
    free(args_);
}

void destruct_all(char *message) {
  perror(message);
  destruct_args();
  destruct_struct();
  exit(EXIT_FAILURE);
}

extern void create_archive() {
  FILE *archive_fp;
  list *list_current;
  metadata_offset location = {location.offset = -1};
  int version = 0;
  int i;

  // OPEN DIRECTORY AND ARCHIVE
  archive_fp = fopen(args_->adtar_file, "wb");
  if (archive_fp == NULL) {
    destruct_all("Open <adtar_file> failed");
  }
  fwrite(&location, sizeof(metadata_offset), 1, archive_fp);
  fclose(archive_fp);
  VLOG(DEBUG, "Created file at ~/%s", args_->adtar_file);

  for (i = 0; i < args_->no_of_files; i++) {
    char *path = args_->file_list[i].name;
    switch (is_directory_or_file(path)) {
    case DIR_:
      populate_archive(DIR_, path);
      break;
    case FILE_:
      populate_archive(FILE_, path);
      break;
    default:
      perror("Encountered a non-file and non-directory");
    }
  }
  if (head == NULL) {
    fprintf(stderr, "Head of struct is NULL in create_archive\n");
    return;
  }

  if ((archive_fp = fopen(args_->adtar_file, "ab")) == NULL) {
    destruct_all("open adtar file failed");
  }

  location.offset = ftell(archive_fp);
  list_current = head;

  do {
    print_metadata(1, &list_current->metadata_);
    VLOG(DEBUG, "------------------------");
    if (fwrite(list_current->metadata_, sizeof(metadata), 1, archive_fp) != 1) {
      destruct_all("Write Metadata to file error");
    }
  } while ((list_current = get_next(&list_current)) != NULL);
  destruct_struct();
  VLOG(DEBUG, "Location set to %ld and eof at %ld", location.offset,
       ftell(archive_fp));
  fclose(archive_fp);

  if ((archive_fp = fopen(args_->adtar_file, "rb+")) == NULL) {
  }

  fwrite(&location, sizeof(metadata_offset), 1, archive_fp);
  fclose(archive_fp);
}

void populate_archive(int dir_flag, char *path) {
  DIR *dir;
  struct dirent *dirp;
  metadata *metadata_ = malloc(sizeof(metadata));
  VLOG(DEBUG, "populating archive with file %s", path);

  switch (dir_flag) {
  case DIR_:
    if ((dir = opendir(path)) == NULL) {
      free(metadata_);
      destruct_all("Open <file/directory list> failed");
    }
    // Iterate through directory and its files
    while ((dirp = readdir(dir)) != NULL) {
      // check not inside . and .. directories
      if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0) {
        char *full_path = malloc(strlen(path) + strlen(dirp->d_name) + 2);
        snprintf(full_path, strlen(path) + strlen(dirp->d_name) + 2, "%s/%s",
                 path, dirp->d_name);
        if ((dirp->d_type & DT_DIR)) {
          populate_archive(DIR_, full_path);
          free(full_path);
        } else {
          populate_archive(FILE_, full_path);
          free(full_path);
        }
      }
    }
    if (closedir(dir)) {
      free(metadata_);
      destruct_all("Close dir failed");
    }

    // ADD original DIR
    add_file_stat(path, &metadata_);
    add(&metadata_);
    break;
  case FILE_:
    add_file_stat(path, &metadata_);
    add_to_archive(&metadata_, path);
    break;
  }
}

void extract_archive() {
  FILE *archive_fp;
  list *list_current;
  char buffer[512];
  metadata *metadata_;
  metadata_offset location;
  int version = 0;
  int i;

  // OPEN DIRECTORY AND ARCHIVE
  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("Open <adtar_file> failed");
  }
  if (fread(&location, sizeof(metadata_offset), 1, archive_fp) < 1) {
    perror("Reading offset from <adtar_file> failed");
    return;
  }
  fclose(archive_fp);

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("Open <adtar_file> failed");
  }

  if (fseek(archive_fp, location.offset, SEEK_SET) < 0) {
    destruct_all("fseek to metadata location on extract_archive error");
  }
  VLOG(DEBUG, "Location set to %ld and file at %ld", location.offset,
       ftell(archive_fp));

  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata_ = (metadata *)buffer;
    if (metadata_ == NULL) {
      destruct_all("Reading metadata from file to struct failed");
    }
    print_metadata(1, &metadata_);
    for (i = 0; i < args_->no_of_files; i++) {
      if (strcmp(metadata_->name, args_->file_list[i].name) == 0) {
        VLOG(DEBUG, "extracting file %s", metadata_->name);
        extract_file(metadata_);
      }
    }
  }
  fclose(archive_fp);
}

void extract_file(metadata *metadata_) {
  FILE *file;
  FILE *archive_fp;
  int i;
  char buffer[512];

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("extract file failed to read <adtar-file>");
  }
  if ((file = fopen(metadata_->name, "wb")) == NULL) {
    destruct_all("extract file failed to read file");
  }
  if (fseek(archive_fp, metadata_->offset, SEEK_SET) < 0) {
    destruct_all("Fseek to file offset in extract file failed");
  }
  VLOG(DEBUG, "--------------------");
  for (i = 0; i < metadata_->file_size; i++) {
    if (fread(buffer, sizeof(char), 1, archive_fp) > 0) {
      if (fwrite(buffer, sizeof(char), 1, file) != 1) {
        fclose(file);
        fclose(archive_fp);
        destruct_all("Write to file from <adtar-file> extraction failed");
      }
    }
  }
  fclose(file);
  fclose(archive_fp);
}

void add_file_stat(char *path, metadata **metadata__) {
  metadata *metadata_ = *metadata__;

  struct stat file_stat;
  metadata_->offset = 0;

  if (stat(path, &file_stat)) {
    perror("Failed to get file stats");
    destruct_args();
    destruct_struct();
    exit(EXIT_FAILURE);
  }
  metadata_->type = is_directory_or_file(path);
  snprintf(metadata_->name, 200, "%s", path);
  snprintf(metadata_->last_modified, 13, "%s", ctime(&file_stat.st_mtime) + 4);
  snprintf(metadata_->name, 200, "%s", path);
  metadata_->uid = file_stat.st_uid;
  metadata_->gid = file_stat.st_gid;
  metadata_->file_size = file_stat.st_size;
  metadata_->perms = file_stat.st_mode;
  // metadata_->version = meta
}

void add_to_archive(metadata **metadata_, char *path) {
  metadata *metadata__ = *metadata_;
  FILE *fp_add;
  FILE *archive_fp;
  char buffer[512];
  int size = 0;

  if ((fp_add = fopen(path, "rb")) == NULL) {
    destruct_all("Failed to open file");
  }

  if ((archive_fp = fopen(args_->adtar_file, "ab")) == NULL) {
    fclose(fp_add);
    destruct_all("Failed to archive file");
  }

  metadata__->offset = ftell(archive_fp);
  add(&metadata__);

  while ((size = fread(buffer, sizeof(char), sizeof(buffer), fp_add)) > 0) {
    if (fwrite(buffer, sizeof(char), size, archive_fp) != size) {
      perror("write to archive failed");
      exit(EXIT_FAILURE);
    }
  }

  fclose(fp_add);
  fclose(archive_fp);
}

void print_path(char *path_name, int level, int type){
    int i;
    char name[256];
    char *pos = strchr(path_name, '/');

    if (pos != NULL){
        strncpy(name, path_name, (pos-path_name)+1);
        name[(pos-path_name)+1] = '\0';


    } else {
        if (level > 0){
            printf(" |");
        }
        for (i=0; i<level; i++){
            printf("-");
        }
        if (type == DIR_){
            strncpy(name, path_name, strlen(path_name));
            name[strlen(path_name)] = '/';
            name[strlen(path_name)+1] = '\0';
            printf("%s\n", name);
        } else if (type == FILE_){
            printf("%s\n\n", path_name);
        }
    }
}

void display_hierarchy(){
    FILE *archive_fp;
    char buffer[512];
    metadata_offset location;
    metadata *metadata_;

    if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL){
        destruct_all("display hierarchy failed to read <adtar-file>");
    }
    if (fread(&location, sizeof(metadata_offset), 1, archive_fp) < 1) {
        destruct_all("Reading offset in display hierarchy from <adtar_file> failed");
        return;
    }
    fclose(archive_fp);

    if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
        destruct_all("display hierarchy failed to read <adtar_file> failed");
    }

    if (fseek(archive_fp, location.offset, SEEK_SET) < 0) {
        destruct_all("fseek to metadata location on extract_archive error");
    }

    while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
        metadata_ = (metadata *)buffer;
        if (metadata_ == NULL) {
          destruct_all("Reading metadata from file to struct failed");
        }
        print_path(metadata_->name, 0, metadata_->type);
    }
    fclose(archive_fp);
}
