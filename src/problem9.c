#include "problem9.h"
#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t MAX_HEIGHT_MAP_DIMENSION = 100;
const size_t P9_MAX_LINE_LENGTH = 120;
const size_t NUM_BASIN_SCORES = 3;

typedef struct {
    int value;
    bool visited;
} Location;

typedef struct {
    Location* values;
    size_t width;
    size_t height;
} HeightMap;

AocResponse read_to_height_map(FILE* file, HeightMap* height_map);
AocResponse process_height_map_line(const char* line, size_t start_idx, Location* height_map_vals);
int height_map_get_value(const HeightMap* height_map, size_t x, size_t y);
void height_map_set_visited(HeightMap* height_map, size_t x, size_t y);
void height_map_cleanup(HeightMap* height_map);
Location* height_map_get_location(const HeightMap* height_map, size_t x, size_t y);
void get_largest_basin_scores(HeightMap* height_map, int* result);
bool is_low_point(const HeightMap* height_map, int x, int y);
bool height_map_get_visited(const HeightMap* height_map, size_t x, size_t y);
int compute_basin_score(HeightMap* height_map, int low_x, int low_y);
void insert_to_scores(int* scores, int score);

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

    get_largest_basin_scores(&height_map, solution);

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
    height_map->values = malloc(MAX_HEIGHT_MAP_DIMENSION *  MAX_HEIGHT_MAP_DIMENSION * sizeof(Location));

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

AocResponse process_height_map_line(const char* line, size_t start_idx, Location* height_map_vals) {
    AocResponse response = { .code = SUCCESS };

    char element[2];
    element[1] = '\0';
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\0' || line[i] == '\n') break;
        element[0] = line[i];
        height_map_vals[start_idx + i].visited = false;
        response = convert_str_to_int(element, &height_map_vals[start_idx + i].value, 10);
        if (!aoc_is_success(&response)) break;
    }

    return response;
}

int height_map_get_value(const HeightMap* height_map, size_t x, size_t y) {
    return height_map->values[(y * height_map->width) + x].value;
}

bool height_map_get_visited(const HeightMap* height_map, size_t x, size_t y) {
    return height_map->values[(y * height_map->width) + x].visited;
}

Location* height_map_get_location(const HeightMap* height_map, size_t x, size_t y) {
    return &height_map->values[(y * height_map->width) + x];
}


void height_map_set_visited(HeightMap* height_map, size_t x, size_t y) {
    height_map->values[(y * height_map->width) + x].visited = true;
}

void get_largest_basin_scores(HeightMap* height_map, int* result) {
    int scores[] = { 0, 0, 0 };
    *result = 0;

    for (size_t y = 0; y < height_map->height; y++) {
        for (size_t x = 0; x < height_map->width; x++) {
            if (is_low_point(height_map, x, y)) {
                insert_to_scores(scores, compute_basin_score(height_map, x, y));
            }
        }
    }

    printf("Largest scores are: %d, %d, %d\n", scores[0], scores[1], scores[2]);
    *result = scores[0] * scores[1] * scores[2];
}

// TODO: Change this to recursively call and move one step at a time
int compute_basin_score(HeightMap* height_map, int x, int y) {
    int current_score = 0;

    if (x < 0 || x >= height_map->width || y < 0 || y >= height_map->height || height_map_get_visited(height_map, x, y) || height_map_get_value(height_map, x, y) == 9)
        return 0;

    height_map_set_visited(height_map, x, y);
    current_score += compute_basin_score(height_map, x-1, y);
    current_score += compute_basin_score(height_map, x+1, y);
    current_score += compute_basin_score(height_map, x, y-1);
    current_score += compute_basin_score(height_map, x, y+1);

    return 1 + current_score;
}

void insert_to_scores(int* scores, int score) {
    int insert_idx = -1;
    while (insert_idx <= (int) NUM_BASIN_SCORES - 2 && score > scores[insert_idx + 1]) {
        insert_idx++;
    }
    if (insert_idx != -1) {
        int idx = insert_idx - 1;
        while (idx >= 0) {
            scores[idx] = scores[idx + 1];
            idx--;
        }
        scores[insert_idx] = score;
    }
}


bool is_low_point(const HeightMap* height_map, int x, int y) {
    const int current = height_map_get_value(height_map, x, y);
    return (x - 1 < 0 || height_map_get_value(height_map, x-1, y) > current) &&
        (x + 1 >= height_map->width || height_map_get_value(height_map, x+1, y) > current) &&
        (y - 1 < 0 || height_map_get_value(height_map, x, y-1) > current) &&
        (y + 1 >= height_map->height || height_map_get_value(height_map, x, y+1) > current);
}

void height_map_cleanup(HeightMap* height_map) {
    if (height_map->values != NULL) {
        free(height_map->values);
    }
}
