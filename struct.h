#ifndef STRUCT_H_
#define STRUCT_H_

#include <sys/stat.h>

typedef struct {
  char name[250];         // file name
  long offset;            // offset used for location of file
  int uid;                // user id
  int gid;                // group id
  int file_size;          // file size
  char last_modified[13]; // time of last modification
  mode_t perms;           // permission
  char type;              // file type
  int compressed;         // compressed file
} metadata;

typedef struct {
  metadata *metadata_;
  struct list *next;
} list;

extern int add(list **, metadata *);
extern list *get_next(list **);
extern int next_is_empty(list **);
extern metadata get_metadata(list **);
extern void destruct(list **);

#endif // STRUCT_H_
