#include "common.h"
#include "func.h"
#include "struct.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  // DECLARE VARS
  args *args_;

  // CHECK INVOKATION
  args_ = parse_args(argc, argv);
  switch (args_->flag) {
  case C:
    create_archive(args_->adtar_file, args_->directory, args_->occurence);
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

  free(args_);
  return EXIT_SUCCESS;
}
