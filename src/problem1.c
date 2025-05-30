#include "problem1.h"
#include "response.h"
#include "utils.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AocResponse process_line(const char* line, int* readings, unsigned int current_idx, unsigned int* num_increasing);
AocResponse convert_line_to_int(const char* line, int* result);
void handle_increasing_sum(const int reading, int* readings, unsigned int current_idx, unsigned int* num_increasing);

const unsigned int MAX_LINE_LENGTH = 6;
const unsigned int SLIDING_WINDOW_SIZE = 3;

AocResponse problem1_solve(unsigned int* solution) {
    const char* file_name = "inputs/problem1.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        const AocResponse response = { .code = FILE_NOT_FOUND, .reason = file_name };
        return response;
    }

    char line[MAX_LINE_LENGTH];
    int readings[] = {-1, -1, -1};
    unsigned int current_idx = 0;
    unsigned int num_increasing = 0;
    AocResponse response;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        response = process_line(line, readings, current_idx, &num_increasing);
        if(response.code != SUCCESS) {
            return response;
        }
        current_idx = (current_idx + 1) % SLIDING_WINDOW_SIZE;
    }

    *solution = num_increasing;
    return response;
}


AocResponse process_line(const char* line, int* readings, unsigned int current_idx, unsigned int* num_increasing) {
    if (strlen(line) == 0 || isspace(line[0])) {
        return (AocResponse) { .code = SUCCESS };
    }

    int reading;
    AocResponse response = convert_str_to_int(line, &reading);
    if (!aoc_is_success(&response)) {
        return response;
    }
    handle_increasing_sum(reading, readings, current_idx, num_increasing);

    response = (AocResponse) { .code = SUCCESS };
    return response;
}

void handle_increasing_sum(const int reading, int* readings, unsigned int current_idx, unsigned int* num_increasing) {
     if (readings[current_idx] == -1) {
        readings[current_idx] = reading;
        return;
    }

    int prev_sum = readings[0] + readings[1] + readings[2];
    readings[current_idx] = reading;
    int new_sum = readings[0] + readings[1] + readings[2];
    if (new_sum > prev_sum) {
        (*num_increasing)++;
    }
}
