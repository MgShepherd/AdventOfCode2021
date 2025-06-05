#include "problem9.h"
#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t MAX_HEIGHT_MAP_DIMENSION = 100;
const size_t P9_MAX_LINE_LENGTH = 120;

typedef struct {
    int* values;
    size_t width;
    size_t height;
} HeightMap;

AocResponse read_to_height_map(FILE* file, HeightMap* height_map);
AocResponse process_height_map_line(const char* line, size_t start_idx, int* height_map_vals);
int height_map_get(const HeightMap* height_map, size_t x, size_t y);
void height_map_cleanup(HeightMap* height_map);
void get_total_risk_level(const HeightMap* height_map, int* result);
bool is_low_point(const HeightMap* height_map, int x, int y);

AocResponse problem9_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem9.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto cleanup;
    }

    HeightMap height_map;
    response = read_to_height_map(file, &height_map);
    if (!aoc_is_success(&response)) goto cleanup;

    get_total_risk_level(&height_map, solution);

cleanup:
    height_map_cleanup(&height_map);
    if (file != NULL) fclose(file);
    return response;
}

AocResponse read_to_height_map(FILE* file, HeightMap* height_map) {
    AocResponse response = { .code = SUCCESS };
    char line[P9_MAX_LINE_LENGTH];

    height_map->width = 0;
    height_map->height = 0;
    height_map->values = malloc(MAX_HEIGHT_MAP_DIMENSION *  MAX_HEIGHT_MAP_DIMENSION * sizeof(int));

    if (height_map->values == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for height map" };
        goto height_map_end;
    }

    while (fgets(line, P9_MAX_LINE_LENGTH, file)) {
        if (height_map->width == 0) {
            height_map->width = strlen(line) - 1;
        }

        response = process_height_map_line(line, height_map->height * height_map->width, height_map->values);
        if (!aoc_is_success(&response)) goto height_map_end;
        (height_map->height)++;
    }

height_map_end:
    return response;
}

AocResponse process_height_map_line(const char* line, size_t start_idx, int* height_map_vals) {
    AocResponse response = { .code = SUCCESS };

    char element[2];
    element[1] = '\0';
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\0' || line[i] == '\n') break;
        element[0] = line[i];
        response = convert_str_to_int(element, &height_map_vals[start_idx + i], 10);
        if (!aoc_is_success(&response)) break;
    }

    return response;
}

int height_map_get(const HeightMap* height_map, size_t x, size_t y) {
    return height_map->values[(y * height_map->width) + x];
}

void get_total_risk_level(const HeightMap* height_map, int* result) {
    *result = 0;
    for (size_t y = 0; y < height_map->height; y++) {
        for (size_t x = 0; x < height_map->width; x++) {
            if (is_low_point(height_map, x, y)) {
                *result += 1 + height_map_get(height_map, x, y);
            }
        }
    }
}

bool is_low_point(const HeightMap* height_map, int x, int y) {
    const int current = height_map_get(height_map, x, y);
    return (x - 1 < 0 || height_map_get(height_map, x-1, y) > current) &&
        (x + 1 >= height_map->width || height_map_get(height_map, x+1, y) > current) &&
        (y - 1 < 0 || height_map_get(height_map, x, y-1) > current) &&
        (y + 1 >= height_map->height || height_map_get(height_map, x, y+1) > current);
}

void height_map_cleanup(HeightMap* height_map) {
    if (height_map->values != NULL) {
        free(height_map->values);
    }
}
