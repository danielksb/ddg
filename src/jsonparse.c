#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include "jsonparse.h"

static SearchResult *
createSearchResult(const json_t * const data, const char * const topic)
{
    SearchResult * search_result;
    json_t *json_text, *json_url;
    const char *text;
    const char *url;
    int textLen;
    int urlLen;
    int topicLen;

    json_text = json_object_get(data, "Text");
    if (!json_is_string(json_text)) {
        fprintf(stderr, "error: Topic.Text is not a string\n");
        return NULL;
    }
    json_url = json_object_get(data, "FirstURL");
    if (!json_is_string(json_url)) {
        fprintf(stderr, "error: Topics.FirstURL is not a string\n");
        return NULL;
    }

    text = json_string_value(json_text);
    url = json_string_value(json_url);

    textLen = strlen(text) + 1;
    urlLen = strlen(url) + 1;
    topicLen = strlen(topic) + 1;

    search_result = (SearchResult *) malloc(sizeof(SearchResult));
    search_result->title = (char *) malloc(sizeof(char) * textLen);
    search_result->url = (char *) malloc(sizeof(char) * urlLen);
    search_result->topic = (char *) malloc(sizeof(char) * topicLen);
    search_result->next = NULL;

    strncpy(search_result->title, text, textLen);
    strncpy(search_result->url, url, urlLen);
    strncpy(search_result->topic, topic, topicLen);

    return search_result;
}

static SearchResult *
parseJsonTopics(const json_t *topics, SearchResult * search_result, const char * const topicName)
{
    int i;
    SearchResult * current = search_result;
    if (!json_is_array(topics)) {
        fprintf(stderr, "error: Topics field is not an array\n");
        return search_result;
    }

    for (i = 0; i < json_array_size(topics); ++i) {
        json_t *data, *json_subtopics;

        data = json_array_get(topics, i);

        if (!json_is_object(data)) {
            fprintf(stderr, "error: Topics[%d] is not an object\n", i);
            return search_result;
        }

        json_subtopics = json_object_get(data, "Topics");
        if (json_is_array(json_subtopics)) {
            json_t *json_topicName = json_object_get(data, "Name");
            if (json_is_string(json_topicName)) {
                const char * const subTopicName = json_string_value(json_topicName);
                if (current == NULL) {
                    search_result = parseJsonTopics(json_subtopics, current, subTopicName);
                    current = search_result;
                } else {
                    current->next = parseJsonTopics(json_subtopics, current->next, subTopicName);
                    current = current->next;
                }
            } else {
                fprintf(stderr, "error: Topics[%d] name field ist not a string.\n", i);
                return search_result;
            }
        } else {
            if (current == NULL) {
                search_result = createSearchResult(data, topicName);
                current = search_result;
            } else {
                current->next = createSearchResult(data, topicName);
                current = current->next;
            }
        }
    }
    return search_result;
}

static SearchResult *
parseJsonRoot(const json_t * const root)
{
    const json_t * topics;
    if (!json_is_object(root)) {
        fprintf(stderr, "error: root is not an object\n");
        return NULL;
    }

    topics = json_object_get(root, "RelatedTopics");

    return parseJsonTopics(topics, NULL, "Main");
}

SearchResult *
parse_response_data(ResponseData * responseData)
{
    json_error_t error;
    json_t *root;
    SearchResult * search_result;
    
    root = json_loads(responseData->data, 0, &error);

    if (root) {
        search_result = parseJsonRoot(root);
    } else {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        search_result = NULL;
    }
    json_decref(root);
    return search_result;
}

