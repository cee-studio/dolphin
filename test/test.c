#include <stdio.h>
#include <stdlib.h>

#include "dolphin.h"


int main(void)
{
  struct dolphin *client = dolphin_init("hello");
  dolphin_cleanup(client);

  return EXIT_SUCCESS;
}
