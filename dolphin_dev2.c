/*   This file will be later merged together with dolphin.c    */
/* Encompasses function for mantaining a WebSockets connection */

#include <stdio.h>
#include <curl/curl.h>

#include "dolphin.h"
#include "curl-websocket.h"

#define WAIT_MS 1000


void dolphin_on_connect(void *data, CURL *easy, const char *websocket_protocols) {
    fprintf(stderr, "Connected to Websocket with assigned protocols: %s\n", websocket_protocols) ;
}

void
dolphin_start(struct dolphin *client)
{
    /* Keep running until all transfers have completed */
    while(1) {
        int events = 0;
        int in_progress = 0;

        /* Start transfers, and check if any have have completed  */
        curl_multi_perform(client->mhandle, &in_progress);
        curl_multi_wait(client->mhandle, NULL, 0, WAIT_MS, &events);

        /* There are no longer any active transfers, and their outputs
         * must be handled. */
        if(in_progress == 0) {
            int messages = 0;
            struct CURLMsg *curl_message = curl_multi_info_read(client->mhandle, &messages);

            if(curl_message != NULL) {
                CURLcode error_code = curl_message->data.result;

                if(error_code == CURLE_OK) {
                    break;
                }

                /* Handle whatever error code appeared */
                printf("Something went wrong!\n");
                break;
            }
            else {
                /* No connection could be established */
                printf("No connection could be established. =(\n");
            }
        }
    }

    /* If this point is reached, then the WebSocket connection has been
     * concluded organically. All went well, and it can now be discarded,
     * and the state of the client reset. */
    curl_multi_remove_handle(client->mhandle, client->cwshandle);

    if(client->cwshandle != NULL) {
        free(client->cwshandle);
        client->cwshandle = NULL;
    }
}

void
dolphin_run(struct dolphin *client)
{
    /* The callbacks for handling websocket events */
    struct cws_callbacks callbacks = {
        .on_connect = dolphin_on_connect
    };

    /* Setup the cURL websockets handler */
    client->cwshandle = cws_new(DOLPHIN_WSS_URL, "", &callbacks);
    curl_easy_setopt(client->cwshandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(client->cwshandle, CURLOPT_VERBOSE, 1L);

    /* Setup and configure the multiplexer, which is used for handling
     * cURL requests in a non-blocking fashion so we do not clog the
     * event-loop. */
    client->mhandle = curl_multi_init();
    curl_multi_add_handle(client->mhandle, client->cwshandle);

    dolphin_start(client);
}
