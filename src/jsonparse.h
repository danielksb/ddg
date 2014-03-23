#ifndef DDG_JSON_PARSE_H_
#define DDG_JSON_PARSE_H_

#include "searchresult.h"
#include "http.h"

/**
 * Parses the response data structure and returns the SearchResults.
 * The caller must make sure that the SearchResults returned must
 * be freed after used.
 * In case of an error NULL is returned.
 */
SearchResult *
parse_response_data(ResponseData * responseData);

#endif

