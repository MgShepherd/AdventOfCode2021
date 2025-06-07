#include "problem12.h"
#include "response.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Cave {
    char* name;
    size_t num_links;
    struct Cave** links;
};

struct CaveSystem {
    struct Cave* caves;
    size_t num_caves;
};

const size_t MAX_NAME_LEN = 6;
const size_t MAX_NUM_LINKS = 5;
const size_t MAX_CAVES = 20;
const size_t P12_MAX_LINE_LEN = 10;

AocResponse read_file_to_cave_system(FILE* file, struct CaveSystem* cave_system, struct Cave** start_cave);
AocResponse read_line_to_cave_system(const char* line, struct CaveSystem* cave_system, struct Cave** start_cave);
AocResponse add_or_get_cave(struct CaveSystem* cave_system, const char* name, struct Cave** result);
AocResponse traverse_cave_system(const struct CaveSystem* cave_system, const struct Cave* start_cave, int* solution);
AocResponse get_possible_routes_from_cave(const struct Cave* current, const struct Cave** visited_caves, size_t num_visited, int* num_routes, bool visited_twice);
void destroy_cave_system(struct CaveSystem* cave_system);
void display_cave_system(const struct CaveSystem* cave_system);

AocResponse problem12_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem12.txt";
    struct CaveSystem cave_system;
    struct Cave* start_cave = NULL;

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto p12_end;
    }

    cave_system.caves = malloc(MAX_CAVES * sizeof(struct Cave));
    cave_system.num_caves = 0;

    if (cave_system.caves == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for caves" };
        goto p12_end;
    }

    response = read_file_to_cave_system(file, &cave_system, &start_cave);
    if (!aoc_is_success(&response)) goto p12_end;

    display_cave_system(&cave_system);

    response = traverse_cave_system(&cave_system, start_cave, solution);

p12_end:
    destroy_cave_system(&cave_system);
    if (file != NULL) fclose(file);
    return response;
}

AocResponse read_file_to_cave_system(FILE* file, struct CaveSystem* cave_system, struct Cave** start_cave) {
    AocResponse response = { .code = SUCCESS };
    char line[P12_MAX_LINE_LEN];

    while (fgets(line, P12_MAX_LINE_LEN, file)) {
        response = read_line_to_cave_system(line, cave_system, start_cave);
        if (!aoc_is_success(&response)) break;
    }

    return response;
}

AocResponse read_line_to_cave_system(const char* line, struct CaveSystem* cave_system, struct Cave** start_cave) {
    AocResponse response = { .code = SUCCESS };

    char name[MAX_NAME_LEN];
    size_t current_idx = 0;
    struct Cave* src_cave = NULL;
    struct Cave* dst_cave = NULL;

    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\n' || line[i] == '\0') break;

        if (line[i] == '-') {
            if (current_idx == 0) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found empty src cave name" };
                goto read_caves_end;
            }
            name[current_idx] = '\0';
            response = add_or_get_cave(cave_system, name, &src_cave);
            if (!aoc_is_success(&response)) goto read_caves_end;
            current_idx = 0;
        } else {
            name[current_idx++] = line[i];
            if (current_idx >= MAX_NAME_LEN) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found cave with longer name than expected" };
                goto read_caves_end;
            }
        }
    }

    if (current_idx == 0) {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found empty destination cave name" };
        goto read_caves_end;
    }
    name[current_idx] = '\0';
    response = add_or_get_cave(cave_system, name, &dst_cave);
    if (!aoc_is_success(&response)) goto read_caves_end;

    src_cave->links[src_cave->num_links++] = dst_cave;
    dst_cave->links[dst_cave->num_links++] = src_cave;

    if (strcmp(src_cave->name, "start") == 0) {
        *start_cave = src_cave;
    }

read_caves_end:
    return response;
}

AocResponse add_or_get_cave(struct CaveSystem* cave_system, const char* name, struct Cave** result) {
    AocResponse response = { .code = SUCCESS };
    char* new_name = malloc(MAX_NAME_LEN * sizeof(char));

    for (size_t i = 0; i < cave_system->num_caves; i++) {
        if (strcmp(cave_system->caves[i].name, name) == 0) {
            *result = &cave_system->caves[i];
            goto get_cave_response;
        }
    }

    memcpy(new_name, name, MAX_NAME_LEN * sizeof(char));
    struct Cave** cave_links = malloc(MAX_NUM_LINKS * sizeof(struct Cave*));
    if (cave_links == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for cave links" };
        goto get_cave_response;
    }

    cave_system->caves[cave_system->num_caves] = (struct Cave) { .name = new_name, .links = cave_links, .num_links = 0 };
    *result = &cave_system->caves[cave_system->num_caves++];

get_cave_response:
    return response;
}

void destroy_cave_system(struct CaveSystem* cave_system) {
   if (cave_system->caves != NULL) {
        for (size_t i = 0; i < cave_system->num_caves; i++) {
            free(cave_system->caves[i].links);
            free(cave_system->caves[i].name);
        }
        free(cave_system->caves);
    }
}


AocResponse traverse_cave_system(const struct CaveSystem* cave_system, const struct Cave* start_cave, int* solution) {
    AocResponse response = { .code = SUCCESS };

    const struct Cave** visited_caves = malloc(MAX_CAVES * sizeof(struct Cave*));
    if (visited_caves == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate memory to keep track of caves" };
        goto traverse_end;
    }

    response = get_possible_routes_from_cave(start_cave, visited_caves, 1, solution, false);

traverse_end:
    if (visited_caves != NULL) free(visited_caves);
    return response;
}

AocResponse get_possible_routes_from_cave(const struct Cave* current, const struct Cave** visited_caves, size_t num_visited, int* num_routes, bool visited_twice) {
    AocResponse response = { .code = SUCCESS };
    const struct Cave** new_visited = NULL;

    for (size_t i = 0; i < num_visited; i++) {
        if (visited_caves[i] == current) {
            if (visited_twice || strcmp(current->name, "start") == 0) {
                goto routes_end;
            } else {
                visited_twice = true;
            }
        }
    }

    if (strcmp(current->name, "end") == 0) {
        (*num_routes)++;
        goto routes_end;
    }

    new_visited = malloc(MAX_CAVES * sizeof(struct Cave*));
    if (new_visited == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate memory to keep track of caves" };
        goto routes_end;
    }
    memcpy(new_visited, visited_caves, num_visited * sizeof(struct Cave*));

    if (islower(current->name[0])) {
        new_visited[num_visited++] = current;
    }

    for (size_t i = 0; i < current->num_links; i++) {
        get_possible_routes_from_cave(current->links[i], new_visited, num_visited, num_routes, visited_twice);
    }

routes_end:
    if (new_visited != NULL) free(new_visited);
    return response;
}

void display_cave_system(const struct CaveSystem* cave_system) {
    for (size_t i = 0; i < cave_system->num_caves; i++) {
        printf("Cave %s: Links: ", cave_system->caves[i].name);
        for (size_t j = 0; j < cave_system->caves[i].num_links; j++) {
            printf("%s, ", cave_system->caves[i].links[j]->name);
        }
        printf("\n");
    }
}
