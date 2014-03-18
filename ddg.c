#include <stdio.h>
#include <curl/curl.h>
#include <jansson.h>
#include <string.h>

#define BUF_SIZE (24*1024)
#define MAX_URL_LEN (2048)

long bytesWritten = 0;

static size_t
write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t sLen = size * nmemb;

    // if this is zero, then it's done
    // we don't do any special processing on the end of the stream
    if (sLen > 0) {
        // >= to account for terminating null
        if (bytesWritten + sLen >= BUF_SIZE) {
            fprintf(stderr, "Buffer size exceeded.\n  Buffer length: %d\n  Current length: %d\n  Bytes to write: %d\n", BUF_SIZE, bytesWritten, sLen);
            return 0;
        }

        memcpy(&((char*)userdata)[bytesWritten], ptr, sLen);
        bytesWritten += sLen;
    }

    return sLen;
}

int
loadJsonFromRequestData(char *data)
{
    json_error_t error;
    json_t *root;
    int returnCode;
    
    root = json_loads(data, 0, &error);

    if (root) {
        returnCode = parseJsonRoot(root);
    } else {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        returnCode = 1;
    }
    json_decref(root);
    return returnCode;
}

int
parseJsonTopics(const json_t *topics)
{
    int i;
    int returnCode = 0;
    if (!json_is_array(topics)) {
        fprintf(stderr, "error: Topics field is not an array\n");
        return 1;
    }

    for (i = 0; i < json_array_size(topics); ++i) {
        json_t *data, *json_text, *json_url, *json_subtopics;
        const char *text;
        const char *url;

        data = json_array_get(topics, i);

        if (!json_is_object(data)) {
            fprintf(stderr, "error: Topics[%d] is not an object\n", i);
            return 1;
        }

        json_subtopics = json_object_get(data, "Topics");
        if (json_is_array(json_subtopics)) {
            json_t *json_topicName = json_object_get(data, "Name");
            if (json_is_string(json_topicName)) {
                printf("== %s == \n\n", json_string_value(json_topicName));
            }
            returnCode = parseJsonTopics(json_subtopics);
        } else {
            json_text = json_object_get(data, "Text");
            if (!json_is_string(json_text)) {
                fprintf(stderr, "error: Topics[%d].Text is not a string\n", i);
                continue;
            }
            json_url = json_object_get(data, "FirstURL");
            if (!json_is_string(json_url)) {
                fprintf(stderr, "error: Topics[%d].FirstURL is not a string\n", i);
                continue;
            }

            text = json_string_value(json_text);
            url = json_string_value(json_url);
            printf("%s\n(%s)\n\n", text, url);
        }
    }
    return returnCode;
}

int
parseJsonRoot(const json_t *root)
{
    json_t *topics;
    if (!json_is_object(root)) {
        fprintf(stderr, "error: root is not an object\n");
        return 1;
    }

    topics = json_object_get(root, "RelatedTopics");

    printf("== Main == \n\n");
    return parseJsonTopics(topics);
}

void
arg_join(char *query, int argc, const char* argv[], int size)
{
  int i;
  int offset = 0;
  for (i = 1; i < argc; ++i) {
      const char* arg = argv[i];
      const arglen = strlen(arg);
      strncpy(query+offset, arg, arglen);
      offset += arglen;
      query[offset] = ' ';
      offset += sizeof(char);
      if (offset >= size) {
          break;
      }
  }
}

void
print_usage(const char* argv[])
{
    printf("Usage: %s QUERY\n", argv[0]);
}

void
set_url(char *url, CURL *curl, int argc, const char* argv[])
{
    char *query = (char * )malloc(MAX_URL_LEN);
    char *escaped_query;
    memset(query, 0, MAX_URL_LEN);
    arg_join(query, argc, argv, MAX_URL_LEN);
    escaped_query = curl_easy_escape(curl, query, strlen(query));
    snprintf(url, MAX_URL_LEN, "http://api.duckduckgo.com/?q=%s&format=json", escaped_query);
    curl_free(escaped_query);
    free(query);
}

int
main(int argc, const char* argv[])
{
    CURL *curl;
    CURLcode res;
    char data[BUF_SIZE];
    char url[MAX_URL_LEN];
    int returnCode = 0;
    memset(data, 0, BUF_SIZE);
    memset(url, 0, MAX_URL_LEN);

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    if (curl) {
        if (argc > 1) {
            set_url(url, curl, argc, argv);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
            curl_easy_setopt(curl, CURLOPT_URL, url);

            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                returnCode = loadJsonFromRequestData(data);
                //printf("%s\n", data);
            } else {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                returnCode = 1;
            }
        } else {
            print_usage(argv);
            returnCode = 1;
        }
        curl_easy_cleanup(curl);
    }
    return returnCode;
}
