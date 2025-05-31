#include "problem3.h"
#include "response.h"
#include "utils.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

AocResponse process_lines(FILE* file, int* solution);
void fill_zeroes(int* input_array, const size_t length);
AocResponse update_bit_vals(const char* line, int* bit_vals);
AocResponse get_rate(const int* bit_vals, bool is_gamma, int* rate);

const size_t LINE_LENGTH = 12;

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
    int bit_vals[LINE_LENGTH];
    char line[LINE_LENGTH + 1];
    fill_zeroes(bit_vals, LINE_LENGTH);
    AocResponse response = { .code = SUCCESS };

    while (fgets(line, LINE_LENGTH + 1, file)) {
        if (strlen(line) == 0 || isspace(line[0])) {
            continue;
        }

        response = update_bit_vals(line, bit_vals);
        if (!aoc_is_success(&response)) {
            return response;
        }
    }

    int gamma_rate, epsilon_rate;
    get_rate(bit_vals, true, &gamma_rate);
    get_rate(bit_vals, false, &epsilon_rate);

    *solution = gamma_rate * epsilon_rate;
    printf("Epsilon Rate: %d, Gamma Rate: %d\n", epsilon_rate, gamma_rate);

    return response;
}

void fill_zeroes(int* input_array, const size_t length) {
    for (size_t i = 0; i < LINE_LENGTH; i++) {
        input_array[i] = 0;
    }
}

AocResponse update_bit_vals(const char* line, int* bit_vals) {
    for (size_t i = 0; i < LINE_LENGTH; i++) {
        if (line[i] == '0') {
            bit_vals[i] -= 1;
        } else if (line[i] == '1') {
            bit_vals[i] += 1;
        } else {
            return (AocResponse) { .code = INVALID_INPUT, .reason = "Encountered character that was not 1 or 0" };
        }
    }

    return (AocResponse) { .code = SUCCESS };
}

AocResponse get_rate(const int* bit_vals, bool is_gamma, int* rate) {
    char binary_string[LINE_LENGTH + 1];
    for (size_t i = 0; i < LINE_LENGTH; i++) {
        if (bit_vals[i] >= 0) {
            binary_string[i] = is_gamma ? '1' : '0';
        } else {
            binary_string[i] = is_gamma ? '0' : '1';
        }
    }
    binary_string[LINE_LENGTH] = '\0';

    return convert_str_to_int(binary_string, rate, 2);
}
