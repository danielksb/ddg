#ifndef DDG_HTTP_H_
#define DDG_HTTP_H_

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

/**
 * Sends a search query to ddg and writes the result into data. This function will block
 * until all bytes are written.
 * @return 1 on success, 0 otherwise
 */
int
run_ddg_http_request(const char* query, char * data, size_t data_size);

#endif
