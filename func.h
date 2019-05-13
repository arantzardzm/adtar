#ifndef FUNC_H_
#define FUNC_H_

enum flag_type {
  C,
  A,
  X,
  M,
  P,
};

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

extern args *parse_args(int, char **);
extern void create_archive(char *, file_name *, int, int);

#endif // FUNC_H_
