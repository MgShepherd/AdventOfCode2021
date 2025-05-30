#ifndef ERROR_H_
#define ERROR_H_

#include <stdbool.h>

typedef enum {
  SUCCESS,
  FILE_NOT_FOUND,
  INVALID_INPUT
} AocResponseCode;

typedef struct {
    AocResponseCode code;
    const char* reason;
} AocResponse;

void aoc_display_response(const AocResponse* response);
bool aoc_is_success(const AocResponse* response);

#endif // ERROR_H_
