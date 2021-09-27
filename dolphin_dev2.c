/*   This file will be later merged together with dolphin.c    */
/* Encompasses function for mantaining a WebSockets connection */

#include <stdio.h>
#include <curl/curl.h>

#include "dolphin.h"
#include "curl-websocket.h"


void dolphin_on_connect(void *data, CURL *easy, const char *websocket_protocols) {
    struct dolphin* client = data;

    fprintf(stderr, "Connected to Websocket with assigned protocols: %s\n", websocket_protocols) ;

}

void
dolphin_run(struct dolphin *client)
{
    struct myapp
    struct cws_callbacks callbacks = {
        .on_connect = dolphin_on_connect
    };

}
