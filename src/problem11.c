#include "problem11.h"
#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t GRID_DIMENSION = 10;
const int FLASH_POINT = 9;
const size_t NUM_STEPS = 100;

typedef struct {
    int last_flashed_step;
    int value;
} Octopus;

typedef struct {
    int x;
    int y;
} GridLocation;

AocResponse convert_file_to_grid(Octopus* grid, FILE* file);
AocResponse process_grid_line(Octopus* grid, size_t start_pos, const char* line);
AocResponse perform_steps(Octopus* grid, int num_steps, int* solution);
void perform_step(Octopus* grid, GridLocation* flash_locations, int current_step, int* total_flashes);
void perform_flash(Octopus* grid, GridLocation* flash_locations, const GridLocation* current_loc, size_t* flash_loc_len, int current_step);

AocResponse problem11_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem11.txt";
    Octopus* grid = NULL;

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto p11_cleanup;
    }

    grid = malloc(GRID_DIMENSION * GRID_DIMENSION * sizeof(Octopus));
    response = convert_file_to_grid(grid, file);
    if (!aoc_is_success(&response)) goto p11_cleanup;

    response = perform_steps(grid, NUM_STEPS, solution);

p11_cleanup:
    if (grid != NULL) free(grid);
    if (file != NULL) fclose(file);
    return response;
}

AocResponse convert_file_to_grid(Octopus* grid, FILE* file) {
    AocResponse response = { .code = SUCCESS };

    char line[GRID_DIMENSION + 2];
    size_t idx = 0;
    while (fgets(line, GRID_DIMENSION + 1, file)) {
        if (strlen(line) == 0 || line[0] == '\n') continue;

        response = process_grid_line(grid, idx * GRID_DIMENSION, line);
        if (!aoc_is_success(&response)) break;
        idx++;
    }

    return response;
}

AocResponse process_grid_line(Octopus* grid, size_t start_pos, const char* line) {
    AocResponse response = { .code = SUCCESS };

    char element[2];
    element[1] = '\0';
    for (size_t i = 0; i < strlen(line); i++) {
        element[0] = line[i];
        response = convert_str_to_int(element, &grid[start_pos + i].value, 10);
        grid[start_pos + i].last_flashed_step = -1;
        if (!aoc_is_success(&response)) break;
    }

    return response;
}

AocResponse perform_steps(Octopus* grid, int num_steps, int* solution) {
    AocResponse response = { .code = SUCCESS };

    GridLocation* flash_locations = malloc(GRID_DIMENSION * GRID_DIMENSION * sizeof(GridLocation));

    if (flash_locations == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for flash locations" };
        goto steps_cleanup;
    }

    for (int i = 0; i < num_steps; i++) {
        perform_step(grid, flash_locations, i, solution);
    }

steps_cleanup:
    if (flash_locations != NULL) free(flash_locations);
    return response;
}

void perform_step(Octopus* grid, GridLocation* flash_locations, int current_step, int* total_flashes) {
    size_t flash_locs_len = 0;
    for (size_t y = 0; y < GRID_DIMENSION; y++) {
        for (size_t x = 0; x < GRID_DIMENSION; x++) {
            grid[(y * GRID_DIMENSION) + x].value++;
            if (grid[(y * GRID_DIMENSION) + x].value > FLASH_POINT) {
                flash_locations[flash_locs_len++] = (GridLocation) { .x = x, .y = y };
            }
        }
    }

    size_t current_loc_idx = 0;
    while (current_loc_idx < flash_locs_len) {
        perform_flash(grid, flash_locations, &flash_locations[current_loc_idx++], &flash_locs_len, current_step);
    }
    (*total_flashes) += current_loc_idx;
    for (size_t i = 0; i < flash_locs_len; i++) {
        grid[(flash_locations[i].y * GRID_DIMENSION) + flash_locations[i].x].value = 0;
    }
}

void perform_flash(Octopus* grid, GridLocation* flash_locations, const GridLocation* current_loc, size_t* flash_loc_len, int current_step) {
    grid[(current_loc->y * GRID_DIMENSION) + current_loc->x].last_flashed_step = current_step;

    for (int y = current_loc->y-1; y <= current_loc->y+1; y++) {
        if (y < 0 || y >= GRID_DIMENSION) continue;

        for (int x = current_loc->x-1; x <= current_loc->x+1; x++) {
            if (x < 0 || x >= GRID_DIMENSION) continue;
            grid[(y * GRID_DIMENSION) + x].value++;

            if (grid[(y * GRID_DIMENSION) + x].last_flashed_step < current_step && grid[(y * GRID_DIMENSION) + x].value == FLASH_POINT + 1) {
                flash_locations[(*flash_loc_len)++] = (GridLocation) { .x = x, .y = y };
            }
        }
    }
}
