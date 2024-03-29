#include "common.h"
#include "func.h"
#include "struct.h"
#include <stdlib.h>

list *head;
args *args_;

int main(int argc, char **argv) {
  // DECLARE VARS

  // CHECK INVOCATION
  parse_args(argc, argv);
  if ((args_->flag == A || args_->flag == C) && args_->no_of_files < 1) {
    fprintf(stderr, "No files passed\n");
    destruct_args();
    exit(EXIT_FAILURE);
  }

  switch (args_->flag) {
  case C:
    create_archive();
    break;
  case A:
    append_archive();
    break;
  case X:
    extract_archive();
    break;
  case M:
    display_metadata();
    break;
  case P:
    display_hierarchy();
    break;
  default:
    fprintf(stderr, "Bad Argument Found\n");
  }

  destruct_args();
  return EXIT_SUCCESS;
}
