#include "response.h"

#include <stdio.h>

void aoc_display_response(const AocResponse* response) {
    switch (response->code) {
        case FILE_NOT_FOUND:
            printf("Unable to find file: %s\n", response->reason);
            break;
        case INVALID_INPUT:
            printf("Unable to process input line due to %s\n", response->reason);
            break;
        default:
            break;
    }
}

bool aoc_is_success(const AocResponse* response) {
    return response->code == SUCCESS;
}
