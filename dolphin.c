#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>
#include <ctype.h>

#include "curl-websocket.h"
#include "debug.h"

#include "dolphin.h"

#define DOLPHIN_MAX_HEADER_PAIRS 100

struct dolphin_request_url {
  char *buf;      ///< request url buffer
  size_t length;  ///< request url string length
  size_t bufsize; ///< real size occupied in memory by buffer
};

struct dolphin_response_header {
  char *buf;      ///< response header buffer
  size_t length;  ///< response header string length
  size_t bufsize; ///< real size occupied in memory by buffer

  struct {  ///< array of header field/value pairs
    struct {
      uintptr_t idx; ///< offset index of 'buf' for the start of field or value
      size_t size;   ///< length of individual field or value
    } field, value;
  } pairs[DOLPHIN_MAX_HEADER_PAIRS];
  int size; ///< number of elements initialized in `pairs`
};

struct dolphin_response_body {
  char *buf;      ///< response body buffer
  size_t length;  ///< response body string length
  size_t bufsize; ///< real size occupied in memory by buffer
};

struct dolphin {
  char   token[256];     ///< the bot token

/* REST FIELDS */
  struct curl_slist *req_header; ///< the request header
  CURL              *ehandle;    ///< the curl's easy handle used to perform requests
  CURLM             *mhandle;    ///< the curl's multiplexer for performing non-blocking requests
  CURL              *cwshandle;  ///< the curl's web socket handler
  char errbuf[CURL_ERROR_SIZE];  ///< capture curl error messages

  struct dolphin_request_url     url;    ///< the request url

  int httpcode; ///< the response httpcode
  struct dolphin_response_header header; ///< the response header
  struct dolphin_response_body   body;   ///< the response body
};

#include "dolphin_dev1.c"
#include "dolphin_dev2.c"

struct dolphin* 
dolphin_init(const char token[])
{
  struct dolphin *new_client; ///< the client to be created

  new_client = calloc(1, sizeof *new_client);

  dolphin_adapter_init(new_client, token);

  return new_client;
}

void
dolphin_cleanup(struct dolphin *client)
{
  dolphin_adapter_cleanup(client);
  free(client);
}
