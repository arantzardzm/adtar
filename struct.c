#include "common.h"
#include "struct.h"
#include <sys/stat.h>

void add(list **this, metadata *metadata_) {
  list *this_ = *this;
  // allocate memory
  list *new_node = malloc(sizeof(list));
  if (new_node == NULL) {
    perror("Add Next Failed on Malloc");
    exit(EXIT_FAILURE);
  }
  // loop till next is not empty
  while (!next_is_empty(this_)) {
    this_ = get_next(this_);
  }
  new_node->metadata = metadata_;
  new_node->next = NULL;
  this_->next = new_node;
}

list *get_next(list **this) {
  list *this_ = *this;
  return this_->next;
}
metadata get_metadata(list **this) {
  list *this_ = *this;
  return this_->metadata;
}
int next_is_empty(list **this) {
  list *this_ = *this;
  return (this_->next == NULL);
}

void destruct(list **this) {
  list *this_ = *this;
  list *temp;
  while (!next_is_empty(this_)) {
    temp = this_;
    this_ = get_next(this_);
    free(temp->metadata);
    free(temp);
  }
  free(this_);
}
