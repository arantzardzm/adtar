#include "common.h"
#include "struct.h"
#include <stdlib.h>
#include <sys/stat.h>

#define DIR_ 1
#define FILE_ 2
#define COMPRESSED_ 3

extern list *head;

void add(metadata **metadata__) {
  metadata *metadata_ = *metadata__;
  list *new_node = malloc(sizeof(list));
  // allocate memory
  if (head == NULL) {
    new_node->metadata_ = metadata_;
    new_node->next = NULL;
    head = new_node;
    return;
  }
  list *this_ = head;
  if (new_node == NULL) {
    perror("Add Next Failed on Malloc");
    exit(EXIT_FAILURE);
  }
  // loop till next is not empty
  while (!next_is_empty(&this_)) {
    this_ = get_next(&this_);
  }
  new_node->metadata_ = metadata_;
  new_node->next = NULL;
  this_->next = new_node;
}

list *get_next(list **this) {
  list *this_ = *this;
  return this_->next;
}
metadata *get_metadata(list **this) {
  list *this_ = *this;
  return this_->metadata_;
}

void print_metadata(int debug, metadata **metadata__) {
  metadata *metadata_ = *metadata__;
  char *file_type;
  char buf[1024];

  if (metadata_ == NULL) {
    fprintf(stderr, "Bad print_metadata called, metadata passed is NULL\n");
    return;
  }

  switch (metadata_->type) {
  case DIR_:
    file_type = "-d";
    break;
  case FILE_:
    file_type = "-f";
    break;
  case COMPRESSED_:
    file_type = "-c";
    break;
  default:
    fprintf(stderr, "Bad file type found %d\n", metadata_->type);
    return;
  }

  snprintf(buf, 1024,
           "File Name: %s\noffset:%ld\nuser id:%d\tgroup id: %d\nfile size: "
           "%d\nLast "
           "modified %s\n%s--%d",
           metadata_->name, metadata_->offset, metadata_->uid, metadata_->gid,
           metadata_->file_size, metadata_->last_modified, file_type,
           metadata_->perms);
  if (debug) {
    VLOG(DEBUG, "%s", buf);
  } else {
    printf("%s\n", buf);
  }
}

int next_is_empty(list **this) {
  list *this_ = *this;
  return (this_->next == NULL);
}

void destruct_struct() {
  VLOG(DEBUG, "destructing");
  if (head == NULL)
    return;
  list *this_ = head;
  list *temp;
  while (!next_is_empty(&this_)) {
    temp = this_;
    this_ = get_next(&this_);
    free(temp->metadata_);
    free(temp);
  }
  VLOG(DEBUG, "out");
  free(this_->metadata_);
  free(this_);
}
