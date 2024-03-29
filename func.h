#ifndef FUNC_H_
#define FUNC_H_

// flags to be used in adtar (compress, append, extract, print, display)
enum flag_type {
  C,
  A,
  X,
  M,
  P,
};

typedef struct {
  long offset;
} metadata_offset;

typedef struct {
  char name[200];
} file_name;

typedef struct {
  enum flag_type flag;
  int occurence;
  file_name *file_list;
  int no_of_files;
  char *adtar_file;
} args;

// read, write, and execute mode declaration
extern const char *modes[];

// external functions for adtar implementation
extern void parse_args(int, char **);
extern void create_archive();
extern void append_archive();
extern void extract_archive();
extern void destruct_args();
extern void display_metadata();
extern void display_hierarchy();

#endif // FUNC_H_
