#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h> 

#include "dolphin.h"


struct dolphin {
/* DISCORD-SPECIFIC FIELDS */
  char token[256]; ///< the bot token

/* CONNECTION-SPECIFIC FIELDS */
  struct curl_slist *req_header; ///< the request header
  CURL *ehandle;                 ///< the curl's easy handle used to perform requests
  struct logconf conf;           ///< for logging
  char errbuf[CURL_ERROR_SIZE];  ///< capture curl error messages
};


struct dolphin* 
dolphin_init(const char token[])
{
  struct dolphin *new_client; ///< the client to be created
  int ret; ///< check return value of snprintf()
  char auth[128]; ///< authentication header buffer


  new_client = calloc(1, sizeof *new_client);
  logconf_setup(&new_client->conf, "DISCORD", NULL);

  ret = snprintf(new_client->token, sizeof(new_client->token), "%s", token);
  ASSERT_S(ret < sizeof(new_client->token), "Out of bounds write attempt");

  ret = snprintf(auth, sizeof(auth), "Authorization: Bot %s", token);
  ASSERT_S(ret < sizeof(auth), "Out of bounds write attempt");

  new_client->req_header = curl_slist_append(NULL, auth);

  return new_client;
}

void
dolphin_cleanup(struct dolphin *client)
{
  logconf_cleanup(&client->conf);
  curl_slist_free_all(client->req_header);
  free(client);
}
