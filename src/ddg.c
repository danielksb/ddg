#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "jsonparse.h"
#include "searchresult.h"

#define MAX_QUERY_LEN (2048)

static void
arg_join(char *query, int argc, const char* argv[], int size)
{
  int i;
  int offset = 0;
  for (i = 1; i < argc; ++i) {
      const char* arg = argv[i];
      const int arglen = strlen(arg);
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
    SearchResult * search_result;
    ResponseData response_data;
    response_data.data = (char *) malloc(1);
    response_data.data[0] = 0;
    response_data.size = 0;

    if (argc > 1) {
        memset(query, 0, MAX_QUERY_LEN);

        arg_join(query, argc, argv, MAX_QUERY_LEN);

        if (!run_ddg_http_request(query, &response_data)) {
            returnCode = 1;
            goto cleanup;
        }
        search_result = parse_response_data(&response_data);
        if (!search_result) {
            returnCode = 1;
            goto cleanup;
        }
        print_search_results(search_result);
        search_result_free(search_result);
    } else {
        print_usage(argv);
        returnCode = 1;
    }
cleanup:
    free(response_data.data);
    return returnCode;
}

