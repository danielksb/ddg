#ifndef DDG_HTTP_H_
#define DDG_HTTP_H_

#include <stdlib.h>

typedef struct {
    char * data;
    size_t size;
} ResponseData;

/**
 * Sends a search query to ddg and writes the result into ResponseData. This function will block
 * until all bytes are written.
 * @return 1 on success, 0 otherwise
 */
int
run_ddg_http_request(const char* query, ResponseData * data);

#endif
