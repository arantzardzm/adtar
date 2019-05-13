#include "common.h"
#include "func.h"
#include "struct.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  // DECLARE VARS
  args *args_;
  int i;

  // CHECK INVOKATION
  args_ = parse_args(argc, argv);
  if (args_->no_of_files < 1) {
    fprintf(stderr, "No files passed\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < args_->no_of_files; i++) {
    VLOG(DEBUG, "%s", args_->file_list[i].name);
  }

  switch (args_->flag) {
  case C:
    create_archive(args_->adtar_file, args_->file_list, args_->no_of_files,
                   args_->occurence);
    break;
  case A:

    break;
  case X:

    break;
  case M:

    break;
  case P:

    break;
  default:
    fprintf(stderr, "Bad Argument Found\n");
  }

  free(args_->file_list);
  free(args_);
  return EXIT_SUCCESS;
}
