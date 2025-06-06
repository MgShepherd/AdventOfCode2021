#include "problem10.h"
#include "response.h"
#include <stdio.h>
#include <string.h>

const size_t P10_MAX_LINE_LENGTH = 120;

AocResponse p10_process_lines(FILE* file, int* solution);
AocResponse p10_process_line(const char* line, char* expected_closing, int* solution);
bool is_closing_char(char element);
bool is_opening_char(char element);
int get_char_score(char element);
char get_associated_closing(char element);

AocResponse problem10_solve(int* solution) {
    AocResponse response = { .code = SUCCESS };
    const char* file_name = "inputs/problem10.txt";

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto p10_cleanup;
    }

    response = p10_process_lines(file, solution);

p10_cleanup:
    return response;
}

AocResponse p10_process_lines(FILE* file, int* solution) {
    AocResponse response = { .code = SUCCESS };

    char line[P10_MAX_LINE_LENGTH];
    char expected_closings[P10_MAX_LINE_LENGTH];

    while (fgets(line, P10_MAX_LINE_LENGTH, file)) {
        response = p10_process_line(line, expected_closings, solution);
        if (!aoc_is_success(&response)) break;
    }

    return response;
}

AocResponse p10_process_line(const char* line, char* expected_closing, int* solution) {
    AocResponse response = { .code = SUCCESS };

    int current_pointer = -1;
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\0' || line[i] == '\n') break;

        if (is_closing_char(line[i])) {
            if (current_pointer == -1 || line[i] != expected_closing[current_pointer]) {
                *solution += get_char_score(line[i]);
                break;
            }
            current_pointer--;
        } else if (is_opening_char(line[i])) {
            expected_closing[++current_pointer] = get_associated_closing(line[i]);
        } else {
            response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unexpected character found when processing line" };
            break;
        }
    }

    return response;
}

bool is_closing_char(char element) {
    return element == ')' || element == ']' || element == '}' || element == '>';
}

bool is_opening_char(char element) {
    return element == '(' || element == '[' || element == '{' || element == '<';
}

int get_char_score(char element) {
    switch (element) {
        case ')': return 3;
        case ']': return 57;
        case '}': return 1197;
        case '>': return 25137;
        default: return 0;
    }
}

char get_associated_closing(char element) {
     switch (element) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        case '<': return '>';
        default: return '.';
    }
}
