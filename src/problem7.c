#include "problem7.h"
#include "response.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

const size_t MAX_TOKEN_SIZE = 5;
const size_t P7_MAX_LINE_LENGTH = 4000;
const size_t MAX_POSITIONS = 1000;

AocResponse convert_to_positions(const char* line, int* positions, size_t* num_positions, int* max_position);
int find_optimal_alignment(const int* positions, size_t num_positions, int max_position);

AocResponse problem7_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem7.txt";
    int* positions = NULL;
    char line[P7_MAX_LINE_LENGTH];

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto p7_cleanup;
    }

    positions = (int*) calloc(MAX_POSITIONS, sizeof(int));
    if (positions == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for positions" };
        goto p7_cleanup;
    }

    if (fgets(line, P7_MAX_LINE_LENGTH, file) == NULL) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to load data from input file" };
        goto p7_cleanup;
    }

    size_t num_positions = 0;
    int max_position = 0;
    response = convert_to_positions(line, positions, &num_positions, &max_position);
    if (!aoc_is_success(&response)) goto p7_cleanup;

    *solution = find_optimal_alignment(positions, num_positions, max_position);

p7_cleanup:
    if (positions != NULL) free(positions);
    if (file != NULL) fclose(file);
    return response;
}

AocResponse convert_to_positions(const char* line, int* positions, size_t* num_positions, int* max_position) {
    AocResponse response = { .code = SUCCESS };

    char current_token[MAX_TOKEN_SIZE];
    size_t current_token_idx = 0;
    int position = 0;
    size_t i = 0;

    while (true) {
        if (line[i] == ',' || line[i] == '\0' || line[i] == '\n') {
            if (current_token_idx == 0) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found empty age value" };
                break;
            }

            current_token[current_token_idx] = '\0';
            current_token_idx = 0;
            response = convert_str_to_int(current_token, &position, 10);
            if (position > *max_position) *max_position = position;
            positions[(*num_positions)++] = position;

            if (!aoc_is_success(&response)) break;

            if (line[i] == '\0' || line[i] == '\n') break;
        } else {
            current_token[current_token_idx++] = line[i];
        }
        i += 1;
    }

    return response;
}

int find_optimal_alignment(const int* positions, size_t num_positions, int max_position) {
  int optimal = -1;
  int current = 0;

  for (int i = 0; i <= max_position; i++) {
      current = 0;
      for (size_t j = 0; j < num_positions; j++) {
          current += abs(positions[j] - i);
      }
      if (current < optimal || optimal == -1) optimal = current;
  }

  return optimal;
}
