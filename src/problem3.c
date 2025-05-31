#include "problem3.h"
#include "response.h"
#include "utils.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

AocResponse process_lines(FILE* file, int* solution);
AocResponse filter_lines(char** inputs, size_t current_length, size_t char_idx, bool is_oxygen);
AocResponse get_rating(char** inputs, bool is_oxygen, int* rating);
void destroy_inputs(char** inputs);

const size_t LINE_LENGTH = 12;
const size_t NUM_LINES = 1000;

AocResponse problem3_solve(int* solution) {
    const char* file_name = "inputs/problem3.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
    }

    process_lines(file, solution);
    fclose(file);

    return (AocResponse) { .code = SUCCESS };
}

AocResponse process_lines(FILE* file, int* solution) {
    AocResponse response = { .code = SUCCESS };
    char** inputs = (char**) malloc(NUM_LINES * sizeof(char*));

    char line[LINE_LENGTH + 1];
    size_t idx = 0;
    while (fgets(line, LINE_LENGTH + 1, file)) {
        if (strlen(line) == 0 || isspace(line[0])) {
            continue;
        }
        char* current_line = (char*) malloc((LINE_LENGTH + 1) * sizeof(char));
        strcpy(current_line, line);
        inputs[idx++] = current_line;
    }

    int oxy_rating, co2_rating;
    response = get_rating(inputs, true, &oxy_rating);
    if (!aoc_is_success(&response)) {
        return response;
    }

    response = get_rating(inputs, false, &co2_rating);
    if (!aoc_is_success(&response)) {
        return response;
    }

    *solution = oxy_rating * co2_rating;
    destroy_inputs(inputs);
    return response;
}

AocResponse get_rating(char** inputs, bool is_oxygen, int* rating) {
    AocResponse response = filter_lines(inputs, NUM_LINES, 0, is_oxygen);
    if (!aoc_is_success(&response)) {
        return response;
    }

    response = convert_str_to_int(inputs[0], rating, 2);
    if (!aoc_is_success(&response)) {
        return response;
    }
    return response;
}

AocResponse filter_lines(char** inputs, size_t current_length, size_t char_idx, bool is_oxygen) {
    if (char_idx >= LINE_LENGTH) {
        return (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to find single rating value before out of bounds" };
    }

    int frequency = 0;
    for (size_t i = 0; i < current_length; i++) {
        if (inputs[i][char_idx] == '1') {
            frequency++;
        } else {
            frequency--;
        }
    }

    char keep_char;
    if (is_oxygen) {
        keep_char = frequency >= 0 ? '1' : '0';
    } else {
        keep_char = frequency >= 0 ? '0' : '1';
    }

    size_t current_idx = 0;
    size_t end_idx = current_length - 1;

    while(current_idx <= end_idx) {
        if (inputs[current_idx][char_idx] != keep_char) {
            char* temp = inputs[current_idx];
            inputs[current_idx] = inputs[end_idx];
            inputs[end_idx] = temp;
            end_idx--;
        } else {
            current_idx++;
        }
    }

    if (end_idx <= 0) {
        return (AocResponse) { .code = SUCCESS };
    }
    return filter_lines(inputs, end_idx + 1, char_idx + 1, is_oxygen);
}

void destroy_inputs(char** inputs) {
    for (size_t i = 0; i < NUM_LINES; i++) {
        free(inputs[i]);
    }
    free(inputs);
}
