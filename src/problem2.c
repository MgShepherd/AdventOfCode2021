#include "problem2.h"
#include "response.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

const unsigned int P2_MAX_LINE_LENGTH = 12;
const unsigned int TOKENS_PER_LINE = 2;
const unsigned int MAX_TOKEN_LENGTH = 8;

typedef enum {
    FORWARD,
    DOWN,
    UP
} Direction;

typedef struct {
    Direction direction;
    int amount;
} Movement;

typedef struct {
    int horizontal;
    int depth;
    int aim;
} Position;

AocResponse convert_to_movement(const char* line, Movement* movement);
AocResponse read_next_word(const char* line, unsigned int max_len, unsigned int* start_idx, char* output);
AocResponse convert_to_direction(const char* token, Direction* direction);
void update_position(const Movement* movement, Position* position);

AocResponse problem2_solve(int* solution) {
    const char* file_name = "inputs/problem2.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
    }

    char line[P2_MAX_LINE_LENGTH];
    Movement movement;
    Position current_position = { .depth = 0, .horizontal = 0 };

    while (fgets(line, P2_MAX_LINE_LENGTH, file)) {
        AocResponse response = convert_to_movement(line, &movement);
        if (!aoc_is_success(&response)) {
            return response;
        }

        update_position(&movement, &current_position);
    }

    *solution = current_position.depth * current_position.horizontal;
    return (AocResponse) { .code = SUCCESS };
}

AocResponse convert_to_movement(const char* line, Movement* movement) {
    if (strlen(line) == 0 || isspace(line[0])) {
        return (AocResponse) { .code = SUCCESS };
    }

    char token[MAX_TOKEN_LENGTH + 1];
    unsigned int current_start_idx = 0;

    AocResponse response = read_next_word(line, MAX_TOKEN_LENGTH, &current_start_idx, token);
    if (!aoc_is_success(&response)) {
        return response;
    }

    response = convert_to_direction(token, &movement->direction);
    if (!aoc_is_success(&response)) {
        return response;
    }

    response = read_next_word(line, MAX_TOKEN_LENGTH, &current_start_idx, token);
    if (!aoc_is_success(&response)) {
        return response;
    }

    response = convert_str_to_int(token, &movement->amount);
    return response;
}

AocResponse read_next_word(const char* line, unsigned int max_len, unsigned int* start_idx, char* output) {
    unsigned int current_idx = *start_idx;
    while (current_idx < strlen(line) && line[current_idx] != ' ' && line[current_idx] != '\0' && line[current_idx] != '\n') {
        output[current_idx - *start_idx] = line[current_idx];
        current_idx++;

        if (current_idx - *start_idx >= max_len) {
            return (AocResponse) { .code = INVALID_INPUT, .reason = "Found Token longer than expected length" };
        }
    }

    if (current_idx == *start_idx) {
        return (AocResponse) { .code = INVALID_INPUT, .reason = "Empty token found" };
    }
    output[current_idx - *start_idx] = '\0';
    *start_idx = current_idx + 1;

    return (AocResponse) { .code = SUCCESS };
}

AocResponse convert_to_direction(const char* token, Direction* direction) {
    AocResponse response = { .code = SUCCESS };
    if (strcmp(token, "forward") == 0) {
        *direction = FORWARD;
    } else if (strcmp(token, "down") == 0) {
        *direction = DOWN;
    } else if (strcmp(token, "up") == 0) {
        *direction = UP;
    } else {
        response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to convert token to direction" };
    }

    return response;
}

void update_position(const Movement* movement, Position* position) {
    switch (movement->direction) {
        case FORWARD:
            position->horizontal += movement->amount;
            position->depth += position->aim * movement->amount;
            break;
        case DOWN:
            position->aim += movement->amount;
            break;
        case UP:
            position->aim -= movement->amount;
    }
}
