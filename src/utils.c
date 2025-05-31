#include "utils.h"

#include <stdlib.h>

AocResponse convert_str_to_int(const char* str, int* result, int base) {
    AocResponse response = { .code = SUCCESS };
    char* digit_end_pointer;
    *result = strtol(str, &digit_end_pointer, base);

    if (digit_end_pointer == str || (*digit_end_pointer != '\0' && *digit_end_pointer != '\n')) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Cannot convert line to integer" };
    }

    return response;
}
