#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "dolphin.h"


int main(void)
{
  //char     token[128+1]="";
  char* token;
  uint64_t channel_id=0;
  setvbuf(stdout, NULL, _IONBF, 0);

  token = "ODU0OTE1NjcwOTI3NDc0NzA4.YMq4Qw.8YrgG_1pCOnIVQ8ElsNXBeLb3LQ";
  channel_id = 878024877439873055;

  // fputs("Provide a valid bot token: ", stdout);
  // scanf("%128s", token);
  // if (!*token) {
  //   perror("Not a valid token");
  //   return EXIT_FAILURE;
  // }

  // fputs("Provide a valid channel id: ", stdout);
  // scanf("%"SCNu64, &channel_id);
  // if (!channel_id) {
  //   perror("Not a valid channel id");
  //   return EXIT_FAILURE;
  // }


  struct dolphin *client = dolphin_init(token);

  char endpoint[512];
  snprintf(endpoint, sizeof(endpoint), "/channels/%"PRIu64"/messages", channel_id);
  dolphin_POST(client, endpoint, "{ \"content\":\"Hello World!\" }");

  dolphin_run(client);
  dolphin_cleanup(client);

  return EXIT_SUCCESS;
}
