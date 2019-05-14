#include "common.h"
#include "struct.h"
#include <stdlib.h>
#include <sys/stat.h>

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
  char buf[1024];

  snprintf(buf, 1024,
           "File Name: %s\noffset:%ld\nuser id:%d\tgroup id: %d\nfile size: "
           "%d\nLast "
           "modified %s",
           metadata_->name, metadata_->offset, metadata_->uid, metadata_->gid,
           metadata_->file_size, metadata_->last_modified);
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

void destruct() {
  list *this_ = head;
  list *temp;
  while (!next_is_empty(&this_)) {
    temp = this_;
    this_ = get_next(&this_);
    free(temp->metadata_);
    free(temp);
  }
  free(this_);
}
