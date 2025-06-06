#include "problem10.h"
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t P10_MAX_LINE_LENGTH = 120;
const size_t MAX_NUM_SCORES = 100;

AocResponse p10_process_lines(FILE* file, unsigned long* solution);
AocResponse p10_process_line(const char* line, char* expected_closing, unsigned long* scores, size_t* scores_len);
bool is_closing_char(char element);
bool is_opening_char(char element);
int get_char_score(char element);
char get_associated_closing(char element);
void insert_score(unsigned long* scores, size_t* scores_len, unsigned long current_score);

AocResponse problem10_solve(unsigned long* solution) {
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

AocResponse p10_process_lines(FILE* file, unsigned long* solution) {
    AocResponse response = { .code = SUCCESS };

    char line[P10_MAX_LINE_LENGTH];
    char expected_closings[P10_MAX_LINE_LENGTH];
    size_t scores_len = 0;
    unsigned long* scores = malloc(MAX_NUM_SCORES * sizeof(unsigned long));

    if (scores == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate memory for storing scores" };
        goto p10_process_lines_end;
    }


    while (fgets(line, P10_MAX_LINE_LENGTH, file)) {
        response = p10_process_line(line, expected_closings, scores, &scores_len);
        if (!aoc_is_success(&response)) break;
    }
    *solution = scores[scores_len / 2];

p10_process_lines_end:
    if (scores != NULL) free(scores);
    return response;
}

AocResponse p10_process_line(const char* line, char* expected_closing, unsigned long* scores, size_t* scores_len) {
    AocResponse response = { .code = SUCCESS };


    int current_pointer = -1;
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\0' || line[i] == '\n') break;

        if (is_closing_char(line[i])) {
            if (current_pointer == -1 || line[i] != expected_closing[current_pointer]) {
                goto p10_process_line_end;
            }
            current_pointer--;
        } else if (is_opening_char(line[i])) {
            expected_closing[++current_pointer] = get_associated_closing(line[i]);
        } else {
            response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unexpected character found when processing line" };
            goto p10_process_line_end;
        }
    }

    unsigned long current_score = 0;
    while (current_pointer != -1) {
        current_score *= 5;
        current_score += get_char_score(expected_closing[current_pointer]);
        current_pointer--;
    }
    insert_score(scores, scores_len, current_score);

p10_process_line_end:
    return response;
}

void insert_score(unsigned long* scores, size_t* scores_len, unsigned long current_score) {
    if (current_score == 0) return;

    for (size_t i = 0; i < *scores_len; i++) {
        if (current_score < scores[i]) {
            for (size_t j = *scores_len + 1; j > i; j--) {
                scores[j] = scores[j - 1];
            }
            scores[i] = current_score;
            (*scores_len)++;
            return;
        }
    }

    scores[(*scores_len)++] = current_score;
}

bool is_closing_char(char element) {
    return element == ')' || element == ']' || element == '}' || element == '>';
}

bool is_opening_char(char element) {
    return element == '(' || element == '[' || element == '{' || element == '<';
}

int get_char_score(char element) {
    switch (element) {
        case ')': return 1;
        case ']': return 2;
        case '}': return 3;
        case '>': return 4;
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
