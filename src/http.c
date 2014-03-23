#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include "http.h"

static size_t
write_callback(void *content, size_t size, size_t nmemb, void *userdata)
{
    size_t newsize = size * nmemb;

    ResponseData * response = (ResponseData *) userdata;

    response->data = (char *) realloc(response->data, response->size + newsize + 1);
    if (response->data == NULL) {
        fprintf(stderr, "http:write_callback: not enough memory (realloc failed)\n");
        return 0;
    }

    memcpy(&(response->data[response->size]), content, newsize);
    response->size += newsize;
    response->data[response->size] = 0;

    return newsize;
}

int
run_ddg_http_request(const char* query, ResponseData * data)
{
    const int MAX_URL_LEN = 2048 * sizeof(char);
    CURL *curl;
    CURLcode res;
    int returnCode;
    char *escaped_query;
    char url[MAX_URL_LEN];
    memset(url, 0, MAX_URL_LEN);

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl) {
        escaped_query = curl_easy_escape(curl, query, strlen(query));
        snprintf(url, MAX_URL_LEN, "http://api.duckduckgo.com/?q=%s&format=json", escaped_query);
        curl_free(escaped_query);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        curl_easy_setopt(curl, CURLOPT_URL, url);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            returnCode = 1;
        } else {
            returnCode = 0;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    } else {
        returnCode = 0;
        fprintf(stderr, "curl_easy_init() failed\n");
    }
    curl_global_cleanup();
    return returnCode;
}

