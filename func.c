#include "common.h"
#include "func.h"
#include "struct.h"
#include <dirent.h>
#include <errno.h>
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
const char *modes[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

// HELPER FUNCS
static int check_ext(char *);
static int is_directory_or_file(char *);
static void add_to_archive(metadata **, char *);
static void populate_archive(int, char *);
static void add_file_stat(char *, metadata **, int, int);
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
  if (S_ISDIR(fileInfo.st_mode)) { // directory
    return DIR_;
  } else if (S_ISREG(fileInfo.st_mode)) { // file
    return FILE_;
  } else {
    return 0; // compressed
  }
}

// parse arguments typed in by the user
extern void parse_args(int argc, char **argv) {
  int flag, temp, file_start, i;
  args *args__ = malloc(sizeof(args));
  args_ = args__;
  args_->occurence = 0;
  args_->no_of_files = 0;
  args_->file_list = NULL;

  if (argc < 3) {
    fprintf(
        stderr, "%s\n%s\n",
        "Invocation Error: Please enter your input in the following format:",
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

  if (args_->flag == X || args_->flag == M || args_->flag == P) {
    file_start = 0;
    if (args_->occurence > 0) {
      if (argc < 5) {
        fprintf(
            stderr, "%s\n%s\n",
            "Invokation Error: Please enter your input in the following "
            "format:",
            "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory "
            "list>");
        destruct_args();
        exit(EXIT_FAILURE);
      }
      args_->adtar_file = argv[4];
    } else {
      args_->adtar_file = argv[2];
    }
  } else if (args_->occurence > 0) {
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
    if (argc < 4) {
      fprintf(
          stderr, "%s\n%s\n",
          "Invokation Error: Please enter your input in the following format:",
          "./adtar {-c|-a|-x|-m|-p|-o NUMBER} <archive-file> <file/directory "
          "list>");
      destruct_args();
      exit(EXIT_FAILURE);
    }
    args_->adtar_file = argv[2];
    file_start = 3;
  }
  if (file_start >= 3) {
    args_->file_list = malloc(sizeof(file_name) * (argc - file_start));
    i = 0;
    while (file_start < argc) {
      strncpy(args_->file_list[i].name, argv[file_start], 200);
      args_->file_list[file_start].name[199] = '\0';
      args_->no_of_files++;
      file_start++;
      i++;
    }
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
  int i;

  // OPEN DIRECTORY AND ARCHIVE
  archive_fp = fopen(args_->adtar_file, "wb");
  if (archive_fp == NULL) {
    destruct_all("Open <adtar_file> failed");
  }
  fwrite(&location, sizeof(metadata_offset), 1, archive_fp);
  fclose(archive_fp);
  printf("Created adtar file at ./%s\n", args_->adtar_file);
  // VLOG(DEBUG, "Created file at ~/%s", args_->adtar_file);

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
    // VLOG(DEBUG, "------------------------");
    printf("Writing file %s to adtar-file\n", list_current->metadata_->name);
    if (fwrite(list_current->metadata_, sizeof(metadata), 1, archive_fp) != 1) {
      destruct_all("Write Metadata to file error");
    }
  } while ((list_current = get_next(&list_current)) != NULL);
  destruct_struct();
  // VLOG(DEBUG, "Location set to %ld and eof at %ld", location.offset,
  // ftell(archive_fp));
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
  // VLOG(DEBUG, "populating archive with file %s", path);

  switch (dir_flag) {
  case DIR_:
    if ((dir = opendir(path)) == NULL) {
      free(metadata_);
      destruct_all("Open <file/directory list> failed");
    }
    // ADD original DIR
    add_file_stat(path, &metadata_, 0, 0);
    add(&metadata_);
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
    break;
  case FILE_:
    add_file_stat(path, &metadata_, 0, 0);
    add_to_archive(&metadata_, path);
    break;
  }
}

void append_archive() {
  FILE *archive_fp;
  // list *list_current;
  char buffer[512];
  metadata_offset location;
  // int i;

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
  // VLOG(DEBUG, "Location set to %ld and file at %ld", location.offset,
  // ftell(archive_fp));

  // READ DIRS & CREATE FOLDERS
  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata *temp;
    metadata *metadata_ = malloc(sizeof(metadata));
    temp = (metadata *)buffer;
    memcpy(metadata_, temp, sizeof(metadata));
    if (metadata_ == NULL) {
      fclose(archive_fp);
      destruct_all("Reading metadata from <adtar-file> to struct failed");
    }
    add(&metadata_);
  }

  // REMOVE METADATA FROM FILE
  if (truncate(args_->adtar_file, location.offset) != 0) {
    perror("truncate error");
    exit(EXIT_FAILURE);
  }
  fclose(archive_fp);

  create_archive();
}

void extract_archive() {
  FILE *archive_fp;
  // list *list_current;
  char buffer[512];
  metadata *metadata_;
  metadata_offset location;
  // int i;

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

  // READ DIRS & CREATE FOLDERS
  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata_ = (metadata *)buffer;
    if (metadata_ == NULL) {
      fclose(archive_fp);
      destruct_all("Reading metadata from file to struct failed");
    }
    if (metadata_->type == DIR_) {
      if (mkdir(metadata_->name, metadata_->perms) == -1) {
      }
    }
  }
  fclose(archive_fp);

  // RESET SEEK
  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("Open <adtar_file> failed");
  }

  if (fseek(archive_fp, location.offset, SEEK_SET) < 0) {
    destruct_all("fseek to metadata location on extract_archive error");
  }

  // READ FILES AND CREATE THEM
  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata_ = (metadata *)buffer;
    if (metadata_ == NULL) {
      fclose(archive_fp);
      destruct_all("Reading metadata from file to struct failed");
    }
    if (metadata_->type == FILE_) {
      // check occurence passed
      if (metadata_->max_version >= args_->occurence) {
        if (metadata_->version == args_->occurence) {
          // VLOG(DEBUG, "extracting file %s with version %d", metadata_->name,
          //      metadata_->version);
          // VLOG(DEBUG, "------------------------");
          extract_file(metadata_);
        } else {
          // VLOG(DEBUG,
          //      "searching for higher version number for file %s with version
          //      "
          //      "%d, need version %d",
          //      metadata_->name, metadata_->version, args_->occurence);
        }
      } else {
        if (metadata_->version == 0) {
          // VLOG(DEBUG,
          //      "extracting file %s with version %d, version is greater than "
          //      "max version %d",
          //      metadata_->name, metadata_->version, metadata_->max_version);
          // VLOG(DEBUG, "------------------------");
          extract_file(metadata_);
        }
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
  // VLOG(DEBUG, "--------------------");
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

void add_file_stat(char *path, metadata **metadata__, int version,
                   int max_version) {
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
  metadata_->version = version;
  metadata_->max_version = max_version;
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

void display_metadata() {
  int i, j;
  FILE *archive_fp;
  char buffer[512];
  metadata_offset location;
  metadata *metadata_;
  char permissions[10];

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("print metadata failed to read <adtar-file>");
  }
  if (fread(&location, sizeof(metadata_offset), 1, archive_fp) < 1) {
    destruct_all("Reading offset in print metadata from <adtar_file> failed");
    return;
  }
  fclose(archive_fp);

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("print metadata failed to read <adtar_file> failed");
  }

  if (fseek(archive_fp, location.offset, SEEK_SET) < 0) {
    destruct_all("fseek to metadata location on print metadata error");
  }

  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata_ = (metadata *)buffer;
    if (metadata_ == NULL) {
      destruct_all("Reading metadata from file to struct failed");
    }

    *permissions = '\0';
    // setting read, write, and execute permissions
    for (i = 2; i >= 0; i--) {
      j = (metadata_->perms >> (i * 3)) & 07;
      strcat(permissions, modes[j]);
    }
    // print permissions
    printf("%d %s %d %d% 6d %s %s\n", metadata_->type, permissions,
           metadata_->uid, metadata_->gid, metadata_->file_size,
           metadata_->last_modified, metadata_->name);
  }
  printf("\n");
  fclose(archive_fp);
}

void print_path(char *path_name, int level, int type) {
  int i;
  char name[256];
  char new_name[256];
  char *pos = strchr(path_name, '/');

  if (pos != NULL) {
    strncpy(name, path_name, (pos - path_name) + 1);
    name[(pos - path_name) + 1] = '\0';
    memmove(new_name, path_name + (pos - path_name) + 1,
            (strlen(path_name) - strlen(name)) + 1);
    name[(pos - path_name) + 1] = '\0';
    print_path(new_name, level + 1, type);

  } else {
    if (level > 0) {
      printf(" |");
    }
    for (i = 0; i < level; i++) {
      printf("--");
    }
    // Directories
    if (type == DIR_) {
      strncpy(name, path_name, strlen(path_name));
      name[strlen(path_name)] = '/';
      name[strlen(path_name) + 1] = '\0';
      printf("%s\n", name);
    } else
        // Files
        if (type == FILE_) {
      printf("%s\n", path_name);
    }
  }
}

void display_hierarchy() {
  FILE *archive_fp;
  char buffer[512];
  metadata_offset location;
  metadata *metadata_;

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("display hierarchy failed to read <adtar-file>");
  }
  if (fread(&location, sizeof(metadata_offset), 1, archive_fp) < 1) {
    destruct_all(
        "Reading offset in display hierarchy from <adtar_file> failed");
    return;
  }
  fclose(archive_fp);

  if ((archive_fp = fopen(args_->adtar_file, "rb")) == NULL) {
    destruct_all("display hierarchy failed to read <adtar_file> failed");
  }

  if (fseek(archive_fp, location.offset, SEEK_SET) < 0) {
    destruct_all("fseek to metadata location on display hierarchy error");
  }

  while (fread(&buffer, sizeof(metadata), 1, archive_fp) == 1) {
    metadata_ = (metadata *)buffer;
    if (metadata_ == NULL) {
      destruct_all("Reading metadata from file to struct failed");
    }
    print_path(metadata_->name, 0, metadata_->type);
  }
  printf("\n");
  fclose(archive_fp);
}
