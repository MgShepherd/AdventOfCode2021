#include "problem8.h"
#include "response.h"
#include "utils.h"

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
AocResponse get_digits_appearing_in_output(char** input_digits, char** output_digits, int* solution);
void destroy_digit_array(char** digit_array, size_t len);
int get_similarity(const char* a, const char* b);

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
        get_digits_appearing_in_output(input_digits, output_digits, solution);
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

AocResponse get_digits_appearing_in_output(char** input_digits, char** output_digits, int* solution) {
    size_t digit_len = 0;
    char* digit_representations[NUM_INPUT_DIGITS];

    for (size_t i = 0; i < NUM_INPUT_DIGITS; i++) {
        digit_len = strlen(input_digits[i]);
        switch (digit_len) {
            case 2:
                digit_representations[1] = input_digits[i];
                break;
            case 3:
                digit_representations[7] = input_digits[i];
                break;
            case 4:
                digit_representations[4] = input_digits[i];
                break;
            case 7:
                digit_representations[8] = input_digits[i];
                break;
            default:
                break;
        }
    }

    for (size_t i = 0; i < NUM_INPUT_DIGITS; i++) {
        digit_len = strlen(input_digits[i]);
        if (digit_len == 6) {
            int similarity = get_similarity(digit_representations[1], input_digits[i]);
            if (similarity == 1) {
                digit_representations[6] = input_digits[i];
            } else {
                int four_similarity = get_similarity(digit_representations[4], input_digits[i]);
                if (four_similarity == 4) {
                    digit_representations[9] = input_digits[i];
                } else {
                    digit_representations[0] = input_digits[i];
                }
            }
        } else if (digit_len == 5) {
            int one_similarity = get_similarity(digit_representations[1], input_digits[i]);
            if (one_similarity == 2) {
                digit_representations[3] = input_digits[i];
            } else {
                int four_similarity = get_similarity(digit_representations[4], input_digits[i]);
                if (four_similarity == 3) {
                    digit_representations[5] = input_digits[i];
                } else {
                    digit_representations[2] = input_digits[i];
                }
            }
        }
    }

    char output_num[NUM_OUTPUT_DIGITS + 1];
    for (size_t i = 0; i < NUM_OUTPUT_DIGITS; i++) {
        for (size_t j = 0; j < NUM_INPUT_DIGITS; j++) {
            if (strlen(output_digits[i]) == strlen(digit_representations[j]) &&
                get_similarity(digit_representations[j], output_digits[i]) == strlen(output_digits[i])) {
                output_num[i] = j + '0';
                break;
            }
        }
    }
    output_num[NUM_OUTPUT_DIGITS] = '\0';
    int value;
    const AocResponse response = convert_str_to_int(output_num, &value, 10);
    if (aoc_is_success(&response)) *solution += value;
    return response;
}

int get_similarity(const char* a, const char* b) {
    int similarity = 0;
    for (size_t i = 0; i < strlen(a); i++) {
        for(size_t j = 0; j < strlen(b); j++) {
            if (b[j] == a[i]) {
                similarity++;
                break;
            }
        }
    }
    return similarity;
}

void destroy_digit_array(char** digit_array, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (digit_array[i] != NULL) {
            free(digit_array[i]);
        }
    }
}
