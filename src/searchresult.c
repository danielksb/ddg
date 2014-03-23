#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "searchresult.h"

void
search_result_free(SearchResult * search_result)
{
    SearchResult * next;
    if (search_result != NULL) {
        next = search_result->next;
        if (search_result->title) {
            free(search_result->title);
        }
        if (search_result->url) {
            free(search_result->url);
        }
        if (search_result->topic) {
            free(search_result->topic);
        }
        free(search_result);
        search_result_free(next);
    }
}

void
print_search_results(SearchResult * search_result)
{
    SearchResult * current = search_result;
    char topic[512];
    memset(topic, 0, 512);
    while (current) {
        if (strncmp(topic, current->topic, 512) != 0) {
            printf("== %s ==\n\n", current->topic);
            strncpy(topic, current->topic, 512);
        }
        printf("%s\n(%s)\n\n", current->title, search_result->url);
        current = current->next;
    }
}

