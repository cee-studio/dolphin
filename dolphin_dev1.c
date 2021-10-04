/* This file will be later merged together with dolphin.c */
/*   Encompasses functions for performing HTTP requests   */

#define CURLE_CHECK(client, ecode)                    \
  VASSERT_S(CURLE_OK == ecode, "(CURLE code: %d) %s", \
      ecode,                                          \
      (!client->errbuf || !*client->errbuf)           \
        ? curl_easy_strerror(ecode)                   \
        : client->errbuf)

/**
 * get http response header by lines
 * @see: https://curl.se/libcurl/c/CURLOPT_HEADERFUNCTION.html 
 */
static size_t
respheader_cb(char *buf, size_t size, size_t nmemb, void *p_userdata)
{
  size_t bufsize = size * nmemb;
  struct dolphin *client = p_userdata;

  char *ptr;
  if (!(ptr = strchr(buf, ':'))) { // returns if can't find ':' field/value delimiter
    return bufsize;
  }

  ptrdiff_t delim_idx = ptr - buf; // get ':' position
  if (!(ptr = strstr(ptr + 1, "\r\n"))) { //returns if can't find CRLF match
    return bufsize;
  }

  if (client->header.bufsize < (client->header.length + bufsize + 1)) {
    client->header.bufsize = client->header.length + bufsize + 1;
    client->header.buf = realloc(client->header.buf, client->header.bufsize);
  }
  memcpy(&client->header.buf[client->header.length], buf, bufsize);

  // get the field part of the string
  client->header.pairs[client->header.size].field.idx = client->header.length;
  client->header.pairs[client->header.size].field.size = delim_idx;

  // offsets blank characters
  size_t bufoffset=1; // starts after the ':' delimiter
  while (delim_idx + bufoffset < bufsize) {
    if (!isspace(buf[delim_idx + bufoffset]))
      break; /* EARLY BREAK (not blank character) */
    ++bufoffset;
  }

  // get the value part of the string
  client->header.pairs[client->header.size].value.idx = client->header.length + (delim_idx + bufoffset);
  client->header.pairs[client->header.size].value.size = (ptr - buf) - (delim_idx + bufoffset);

  client->header.length += bufsize;

  ++client->header.size; // update header amount of field/value header
  ASSERT_S(client->header.size < DOLPHIN_MAX_HEADER_PAIRS, "Out of bounds write attempt");

  return bufsize;
}

/**
 * get http response body in chunks
 * @see: https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
 */
static size_t
respbody_cb(char *buf, size_t size, size_t nmemb, void *p_userdata)
{
  size_t bufchunk_size = size * nmemb;
  struct dolphin *client = p_userdata;

  //increase response body memory block size only if necessary
  if (client->body.bufsize < (client->body.length + bufchunk_size + 1)) {
    client->body.bufsize = client->body.length + bufchunk_size + 1;
    client->body.buf = realloc(client->body.buf, client->body.bufsize);
  }
  memcpy(&client->body.buf[client->body.length], buf, bufchunk_size);
  client->body.length += bufchunk_size;
  client->body.buf[client->body.length] = '\0';
  return bufchunk_size;
}

static void
dolphin_adapter_init(struct dolphin *client, const char token[])
{
  char auth[128]; ///< authentication header buffer
  int ret;        ///< check return value of snprintf()

  ret = snprintf(client->token, sizeof(client->token), "%s", token);
  ASSERT_S(ret < sizeof(client->token), "Out of bounds write attempt");

  ret = snprintf(auth, sizeof(auth), "Authorization: Bot %s", token);
  ASSERT_S(ret < sizeof(auth), "Out of bounds write attempt");

  client->req_header = curl_slist_append(NULL, auth);
  curl_slist_append(client->req_header, "User-Agent: Dolphin (https://github.com/cee-studio/dolphin)");
  curl_slist_append(client->req_header, "Content-Type: application/json");
  curl_slist_append(client->req_header, "Accept: application/json");

  client->ehandle = curl_easy_init();
  //set error buffer for capturing CURL error descriptions
  curl_easy_setopt(client->ehandle, CURLOPT_ERRORBUFFER, client->errbuf);
  //set ptr to request header we will be using for API communication
  curl_easy_setopt(client->ehandle, CURLOPT_HTTPHEADER, client->req_header);
  //enable follow redirections
  curl_easy_setopt(client->ehandle, CURLOPT_FOLLOWLOCATION, 1L);
  //set response body callback
  curl_easy_setopt(client->ehandle, CURLOPT_WRITEFUNCTION, &respbody_cb);
  //set ptr to response body to be filled at callback
  curl_easy_setopt(client->ehandle, CURLOPT_WRITEDATA, client);
  //set response header callback
  curl_easy_setopt(client->ehandle, CURLOPT_HEADERFUNCTION, &respheader_cb);
  //set ptr to response header to be filled at callback
  curl_easy_setopt(client->ehandle, CURLOPT_HEADERDATA, client);
}

static void
dolphin_adapter_cleanup(struct dolphin *client)
{
  if (client->url.buf)
    free(client->url.buf);
  if (client->body.buf)
    free(client->body.buf);
  if (client->header.buf)
    free(client->header.buf);
  curl_slist_free_all(client->req_header);
  curl_easy_cleanup(client->ehandle); 
}

static void
set_url(struct dolphin *client, const char endpoint[])
{
  size_t url_len = sizeof(DOLPHIN_BASE_URL);
  url_len += 1 + strlen(endpoint);

  if (url_len > client->url.bufsize) {
    void *tmp = realloc(client->url.buf, url_len);
    ASSERT_S(NULL != tmp, "Couldn't increase buffer's length");
    client->url.buf = tmp;
    client->url.bufsize = url_len;
  }
  memcpy(client->url.buf, DOLPHIN_BASE_URL, sizeof(DOLPHIN_BASE_URL));

  int offset = sizeof(DOLPHIN_BASE_URL) - 1;
  offset += snprintf(client->url.buf + offset, client->url.bufsize - offset, "%s", endpoint);
  ASSERT_S(offset < client->url.bufsize, "Out of bounds write attempt");

  CURLcode ecode = curl_easy_setopt(client->ehandle, CURLOPT_URL, client->url.buf);
  CURLE_CHECK(client, ecode);

  client->url.length = offset;
}

static void
perform_request(struct dolphin *client)
{
  CURLcode ecode;
  
  ecode = curl_easy_perform(client->ehandle);

  //get response's code
  ecode = curl_easy_getinfo(client->ehandle, CURLINFO_RESPONSE_CODE, &client->httpcode);
  CURLE_CHECK(client, ecode);
}

static void
reset_adapter(struct dolphin *client)
{
  client->body.length = 0;
  client->header.length = 0;
  client->header.size = 0;
  *client->errbuf = '\0';
}

void
dolphin_POST(struct dolphin *client, const char endpoint[], const char body[])
{
  // resets any preexisting CUSTOMREQUEST
  curl_easy_setopt(client->ehandle, CURLOPT_CUSTOMREQUEST, NULL);
  if (NULL == body) {
    body = "";
  }

  set_url(client, endpoint); //set the request url

  curl_easy_setopt(client->ehandle, CURLOPT_POST, 1L);
  curl_easy_setopt(client->ehandle, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(client->ehandle, CURLOPT_POSTFIELDSIZE, strlen(body));

  perform_request(client);

  reset_adapter(client); // reset for next iteration
}
