#include "problem8.h"
#include "response.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t P8_MAX_LINE_LENGTH = 200;
const size_t NUM_INPUT_DIGITS = 10;
const size_t NUM_OUTPUT_DIGITS = 4;
const size_t MAX_DIGIT_LENGTH = 8;

AocResponse process_digit_line(const char* line, int* num_appearing, char** input_digits, char** output_digits);
AocResponse process_digit_lines(FILE* file, int* solution);
AocResponse init_digit_array(char** digit_array, size_t len, size_t element_len);
void get_digits_appearing_in_output(char** output_digits, size_t len, int* solution);
void destroy_digit_array(char** digit_array, size_t len);

AocResponse problem8_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };

    const char* file_name = "inputs/problem8.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto cleanup;
    }

    response = process_digit_lines(file, solution);

  cleanup:
    if (file != NULL) fclose(file);

    return response;
}

AocResponse process_digit_lines(FILE* file, int* solution) {
    AocResponse response = { .code = SUCCESS };
    char line[P8_MAX_LINE_LENGTH];

    char* input_digits[NUM_INPUT_DIGITS];
    char* output_digits[NUM_OUTPUT_DIGITS];

    response = init_digit_array(input_digits, NUM_INPUT_DIGITS, MAX_DIGIT_LENGTH);
    if (!aoc_is_success(&response)) goto lines_cleanup;
    response = init_digit_array(output_digits, NUM_OUTPUT_DIGITS, MAX_DIGIT_LENGTH);
    if (!aoc_is_success(&response)) goto lines_cleanup;


    while (fgets(line, P8_MAX_LINE_LENGTH, file)) {
        if (strlen(line) == 0 || isspace(line[0])) {
            continue;
        }

        response = process_digit_line(line, solution, input_digits, output_digits);
        if (!aoc_is_success(&response)) goto lines_cleanup;
        get_digits_appearing_in_output(output_digits, NUM_OUTPUT_DIGITS, solution);
    }

lines_cleanup:
    destroy_digit_array(input_digits, NUM_INPUT_DIGITS);
    destroy_digit_array(output_digits, NUM_OUTPUT_DIGITS);
    return response;
}

AocResponse init_digit_array(char** digit_array, size_t len, size_t element_len) {
    AocResponse response = { .code = SUCCESS };

    for (size_t i = 0; i < len; i++) {
        digit_array[i] = (char*) malloc(element_len * sizeof(char));
        if (digit_array[i] == NULL) {
            response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for digit array" };
            break;
        }
    }
    return response;
}

AocResponse process_digit_line(const char* line, int* num_appearing, char** input_digits, char** output_digits) {
    AocResponse response = { .code = SUCCESS };

    bool processing_input = true;
    char* current_digit = input_digits[0];
    size_t current_digit_idx = 0;
    size_t current_token_idx = 0;

    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == ' ' || line[i] == '\0' || line[i] == '\n') {
            if (current_digit_idx == 0) continue;

            current_digit[current_digit_idx] = '\0';
            current_digit_idx = 0;

            if (line[i] == ' ') {
                current_token_idx++;
                if (processing_input)
                    current_digit = input_digits[current_token_idx];
                else
                    current_digit = output_digits[current_token_idx];
            }
        } else if (line[i] == '|') {
            if (!processing_input) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found more | characters than expected" };
                break;
            }
            processing_input = false;
            current_token_idx = 0;
            current_digit = output_digits[current_token_idx];
        } else {
            current_digit[current_digit_idx++] = line[i];
            if (current_digit_idx >= MAX_DIGIT_LENGTH) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found token which is longer than expected" };
                break;
            }
        }
    }

    return response;
}

void get_digits_appearing_in_output(char** output_digits, size_t len, int* solution) {
    size_t digit_len = 0;
    for (size_t i = 0; i < len; i++) {
        digit_len = strlen(output_digits[i]);
        if (digit_len == 2 || digit_len == 4 || digit_len == 3 || digit_len == 7) {
            (*solution)++;
        }
    }
}

void destroy_digit_array(char** digit_array, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (digit_array[i] != NULL) {
            free(digit_array[i]);
        }
    }
}
