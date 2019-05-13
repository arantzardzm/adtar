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
  enum flag_type flag;
  int occurence;
  char *directory;
  char *adtar_file;
} args;

extern args *parse_args(int, char **);
extern void create_archive(char *, char *, int);

#endif // FUNC_H_
