#ifndef DDG_SEARCH_RESULT_H_
#define DDG_SEARCH_RESULT_H_

#define MAX_URL_LEN (2048)

typedef struct SearchResult_t {
    char * title;
    char * url;
    char * topic;
    struct SearchResult_t * next;
} SearchResult;

/**
 * Frees the memory of a given search result and all of its
 * following results.
 */
void
search_result_free(SearchResult * search_result);

/**
 * Prints a given list of search results to the console
 */
void
print_search_results(SearchResult * search_result);

#endif

