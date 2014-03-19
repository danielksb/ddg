#include "http.h"

static long bytesWritten = 0;
static size_t max_data_buffer_size = 0;

static size_t
write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t sLen = size * nmemb;

    // if this is zero, then it's done
    // we don't do any special processing on the end of the stream
    if (sLen > 0) {
        // >= to account for terminating null
        if (bytesWritten + sLen >= max_data_buffer_size) {
            fprintf(stderr, "Buffer size exceeded.\n  Buffer length: %d\n  Current length: %d\n  Bytes to write: %d\n", max_data_buffer_size, bytesWritten, sLen);
            return 0;
        }

        memcpy(&((char*)userdata)[bytesWritten], ptr, sLen);
        bytesWritten += sLen;
    }

    return sLen;
}

int
run_ddg_http_request(const char* query, char * data, size_t data_size)
{
    const int MAX_URL_LEN = 2048 * sizeof(char);
    CURL *curl;
    CURLcode res;
    int returnCode;
    char *escaped_query;
    char url[MAX_URL_LEN];
    memset(url, 0, MAX_URL_LEN);
    max_data_buffer_size = data_size;

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
    bytesWritten = 0;
    max_data_buffer_size = 0;
    return returnCode;
}

