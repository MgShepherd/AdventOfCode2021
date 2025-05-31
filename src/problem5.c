#include "problem5.h"
#include "response.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const size_t GRID_DIMENSION_SIZE = 1000;
const size_t SEGMENT_LINE_LENGTH = 20;
const size_t P5_MAX_TOKEN_LENGTH = 10;
const size_t MAX_LOCATION_LENGTH = 5;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position start;
    Position end;
} Segment;

void insert_horizontal_line(int* grid, int* overlapping_count, int y, int start_x, int end_x);
void insert_vertical_line(int* grid, int* overlapping_count, int x, int start_y, int end_y);
void insert_diagonal_line(int* grid, int* overlapping_count, const Position* start, const Position *end);
AocResponse fill_grid(int* grid, FILE* file, int* overlapping_count);
AocResponse read_line_to_segment(Segment* segment, const char* line);
AocResponse read_token_to_position(Position* position, const char* token);

AocResponse problem5_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem5.txt";
    int* grid = NULL;

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto cleanup;
    }

    grid = (int*) calloc(GRID_DIMENSION_SIZE * GRID_DIMENSION_SIZE, sizeof(int));
    if (grid == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for grid" };
        goto cleanup;
    }

    response = fill_grid(grid, file, solution);
    if (!aoc_is_success(&response)) goto cleanup;

cleanup:
    if (grid != NULL) free(grid);
    if (file != NULL) fclose(file);

    return response;
}

AocResponse fill_grid(int* grid, FILE* file, int* overlapping_count) {
    AocResponse response = { .code = SUCCESS };

    char line[SEGMENT_LINE_LENGTH];
    Segment segment;
    while (fgets(line, SEGMENT_LINE_LENGTH, file)) {
        if (strlen(line) == 0 || isspace(line[0])) {
            continue;
        }

        response = read_line_to_segment(&segment, line);
        if (!aoc_is_success(&response)) break;

        if (segment.start.x == segment.end.x) {
            insert_vertical_line(grid, overlapping_count, segment.start.x, segment.start.y, segment.end.y);
        } else if (segment.start.y == segment.end.y) {
            insert_horizontal_line(grid, overlapping_count, segment.start.y, segment.start.x, segment.end.x);
        } else {
            insert_diagonal_line(grid, overlapping_count, &segment.start, &segment.end);
        }
    }

    return response;
}

void insert_horizontal_line(int* grid, int *overlapping_count, int y, int start_x, int end_x) {
    const int start = start_x < end_x ? start_x : end_x;
    const int end = start_x < end_x ? end_x : start_x;

    for (size_t i = start; i <= end; i++) {
        grid[(y * GRID_DIMENSION_SIZE) + i]++;
        if (grid[(y * GRID_DIMENSION_SIZE) + i] == 2) {
            (*overlapping_count)++;
        }
    }
}

void insert_vertical_line(int* grid, int *overlapping_count, int x, int start_y, int end_y) {
    const int start = start_y < end_y ? start_y : end_y;
    const int end = start_y < end_y ? end_y : start_y;

    for (size_t i = start; i <= end; i++) {
        grid[(i * GRID_DIMENSION_SIZE) + x]++;
        if (grid[(i * GRID_DIMENSION_SIZE) + x] == 2) {
            (*overlapping_count)++;
        }
    }
}

void insert_diagonal_line(int* grid, int* overlapping_count, const Position* start, const Position* end) {
    const int x_direction = start->x - end->x > 0 ? -1 : 1;
    const int y_direction = start->y - end->y > 0 ? -1 : 1;

    Position current = { .x = start->x, .y = start->y };
    while (true) {
        grid[(current.y * GRID_DIMENSION_SIZE) + current.x]++;
        if (grid[(current.y * GRID_DIMENSION_SIZE) + current.x] == 2) {
            (*overlapping_count)++;
        }
        if (current.x == end->x && current.y == end->y) {
            break;
        }
        current.x += x_direction;
        current.y += y_direction;
    }
}

AocResponse read_line_to_segment(Segment* segment, const char* line) {
    AocResponse response = { .code = SUCCESS };
    Position* current_position = &segment->start;
    char current_token[P5_MAX_TOKEN_LENGTH];
    size_t token_idx = 0;
    bool all_tokens_found = false;

    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '-' || line[i] == '>') {
            continue;
        } else if (line[i] == '\0' || line[i] == ' ' || line[i] == '\n') {
            if (token_idx == 0) continue;

            current_token[token_idx] = '\0';
            response = read_token_to_position(current_position, current_token);
            if (!aoc_is_success(&response)) break;

            if (current_position == &segment->start) {
                current_position = &segment->end;
                token_idx = 0;
            } else {
                all_tokens_found = true;
                break;
            }
        } else {
            current_token[token_idx++] = line[i];
        }
    }

    if (!all_tokens_found && response.code == SUCCESS) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to parse expected number of segments from line" };
    }

    return response;
}

AocResponse read_token_to_position(Position* position, const char* token) {
    AocResponse response = { .code = SUCCESS };

    int* current_component = &position->x;

    char current_location[MAX_LOCATION_LENGTH];
    size_t current_location_idx = 0;
    bool found_all_components = false;

    for (size_t i = 0; i < strlen(token) + 1; i++) {
        if (i == strlen(token) || token[i] == ',' || token[i] == '\0' || token[i] == '\n' || token[i] == ' ') {
            if (current_location_idx == 0) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found unexpected empty token\n" };
                break;
            }

            current_location[current_location_idx] = '\0';
            response = convert_str_to_int(current_location, current_component, 10);
            if (!aoc_is_success(&response)) break;

            if (current_component == &position->x) {
                current_component = &position->y;
                current_location_idx = 0;
            } else {
                found_all_components = true;
                break;
            }
        } else {
            current_location[current_location_idx++] = token[i];
        }
    }

    if (!found_all_components && response.code == SUCCESS) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to convert token into correct number of positions" };
    }

    return response;
}
