#include "problem6.h"
#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AocResponse process_file(unsigned long* ages, FILE* file);
AocResponse convert_line_to_ages(unsigned long* ages, const char* line);
void simulate_cycles(unsigned long* ages);
unsigned long get_total_fish(unsigned long* ages);

const size_t NUM_LIFECYCLE_VALUES = 9;
const size_t P6_MAX_LINE_LENGTH = 1000;
const size_t MAX_AGE_LENGTH = 2;
const size_t NUM_CYCLES = 256;
const size_t RESTART_CYCLE_AGE = 6;

AocResponse problem6_solve(unsigned long* solution) {
    AocResponse response = { .code = SUCCESS };

    const char* file_name = "inputs/problem6.txt";
    unsigned long* ages = NULL;

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto cleanup;
    }

    ages = (unsigned long*) calloc(NUM_LIFECYCLE_VALUES, sizeof(unsigned long));
    if (ages == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate memory required for storing ages" };
        goto cleanup;
    }

    response = process_file(ages, file);
    if (!aoc_is_success(&response)) goto cleanup;

    simulate_cycles(ages);

    *solution = get_total_fish(ages);

cleanup:
    if (ages != NULL) free(ages);
    if (file != NULL) fclose(file);

    return response;
}

AocResponse process_file(unsigned long* ages, FILE* file) {
    AocResponse response = { .code = SUCCESS };

    char line[P6_MAX_LINE_LENGTH];
    if (fgets(line, P6_MAX_LINE_LENGTH, file) == NULL) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to read first line of input file" };
    } else {
        response = convert_line_to_ages(ages, line);
    }

    return response;
}

AocResponse convert_line_to_ages(unsigned long* ages, const char* line) {
    AocResponse response = { .code = SUCCESS };

    char current_age[MAX_AGE_LENGTH];
    size_t current_age_idx = 0;
    int idx_to_add = 0;
    size_t i = 0;

    while (true) {
        if (line[i] == ',' || line[i] == '\0' || line[i] == '\n') {
            if (current_age_idx == 0) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found empty age value" };
                break;
            }

            current_age[current_age_idx] = '\0';
            current_age_idx = 0;
            response = convert_str_to_int(current_age, &idx_to_add, 10);
            if (!aoc_is_success(&response)) break;
            ages[idx_to_add]++;

            if (line[i] == '\0' || line[i] == '\n') break;
        } else {
            current_age[current_age_idx++] = line[i];
        }
        i += 1;
    }

    return response;
}

void simulate_cycles(unsigned long* ages) {
    for(size_t i = 0; i < NUM_CYCLES; i++) {
        const unsigned long num_fish_to_add = ages[0];
        for (size_t j = 0; j < NUM_LIFECYCLE_VALUES - 1; j++) {
            ages[j] = ages[j + 1];
        }
        ages[NUM_LIFECYCLE_VALUES - 1] = num_fish_to_add;
        ages[RESTART_CYCLE_AGE] += num_fish_to_add;
    }
}

unsigned long get_total_fish(unsigned long* ages) {
    unsigned long total_fish = 0;
    for (size_t i = 0; i < NUM_LIFECYCLE_VALUES; i++) {
        total_fish += ages[i];
    }
    return total_fish;
}
