#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include "http.h"

#define MAX_QUERY_LEN (2048)

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

static void
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

int
main(int argc, const char* argv[])
{
    int returnCode = 0;
    char query[MAX_QUERY_LEN];
    ResponseData response_data;
    response_data.data = (char *) malloc(1);
    response_data.data[0] = 0;
    response_data.size = 0;

    if (argc > 1) {
        memset(query, 0, MAX_QUERY_LEN);

        arg_join(query, argc, argv, MAX_QUERY_LEN);

        if (run_ddg_http_request(query, &response_data)) {
            returnCode = loadJsonFromRequestData(response_data.data);
            //printf("%s\n", data);
        } else {
            returnCode = 1;
        }
    } else {
        print_usage(argv);
        returnCode = 1;
    }
    free(response_data.data);
    return returnCode;
}

