#include "problem1.h"
#include "response.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AocResponse process_line(const char* line, int* prev_reading, unsigned int* num_increasing);

const unsigned int MAX_LINE_LENGTH = 5;

AocResponse problem1_solve(unsigned int* solution) {
    const char* file_name = "inputs/problem1.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        const AocResponse response = { .code = FILE_NOT_FOUND, .reason = file_name };
        return response;
    }

    char line[MAX_LINE_LENGTH];
    int prev_reading = -1;
    unsigned int num_increasing = 0;
    AocResponse response;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        response = process_line(line, &prev_reading, &num_increasing);
        if(response.code != SUCCESS) {
            return response;
        }
    }

    *solution = num_increasing;
    return response;
}

AocResponse process_line(const char* line, int* prev_reading, unsigned int* num_increasing) {
    if (strlen(line) == 0 || isspace(line[0])) {
        const AocResponse response = { .code = SUCCESS };
        return response;
    }

    char* digit_end_pointer;
    long reading = strtol(line, &digit_end_pointer, 10);

    if (digit_end_pointer == line || (*digit_end_pointer != '\0' && *digit_end_pointer != '\n')) {
        const AocResponse response = { .code = INVALID_INPUT, .reason = "Cannot convert line to integer" };
        return response;
    }

    if (*prev_reading != -1 && reading > *prev_reading) {
        *num_increasing += 1;
    }

    *prev_reading = reading;
    const AocResponse response = { .code = SUCCESS };
    return response;
}
