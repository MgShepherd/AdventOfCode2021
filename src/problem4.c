#include "problem4.h"
#include "response.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const size_t CALL_LINE_LENGTH = 300;
const size_t BOARD_LINE_LENGTH = 20;
const size_t MAX_CALL_VALS = 150;
const size_t MAX_CALL_SIZE = 3;
const size_t NUM_BINGO_BOARDS = 100;
const size_t BINGO_BOARD_SIZE = 5;

typedef struct {
    bool marked;
    int val;
} BingoElement;

typedef struct {
    BingoElement* elements;
    bool has_won;
} BingoBoard;

AocResponse process_call_line(FILE* file, int* calls, size_t* num_calls);
AocResponse process_boards(FILE* file, BingoBoard* boards);
AocResponse process_board(FILE* file, BingoBoard* board);
AocResponse process_board_line(const char* line, BingoElement* element_line);
AocResponse find_winning_board(const int* calls, size_t num_calls, size_t* winning_board_idx, size_t* winning_call_idx, BingoBoard* boards);
bool find_element_and_check_won(int element, BingoBoard* board);
bool check_row(size_t row_num, const BingoBoard* board);
bool check_col(size_t col_num, const BingoBoard* board);
int get_winning_score(const BingoBoard* board, int winning_element);
void destroy_boards(BingoBoard* boards);

AocResponse problem4_solve(int* solution) {
    const char* file_name = "inputs/problem4.txt";
    AocResponse response;

    BingoBoard* boards = NULL;
    int* calls = NULL;
    FILE* file = fopen(file_name, "r");

    if (file == NULL) {
        response = (AocResponse) { .code = FILE_NOT_FOUND, .reason = file_name };
        goto cleanup;
    }

    calls = (int*) malloc(MAX_CALL_VALS * sizeof(unsigned int));
    if (calls == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to reserve memory for storing bingo calls" };
        goto cleanup;
    }

    size_t num_calls;
    response = process_call_line(file, calls, &num_calls);
    if (!aoc_is_success(&response)) goto cleanup;

    boards = (BingoBoard*) malloc(NUM_BINGO_BOARDS * sizeof(BingoBoard));
    if (calls == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to reserve memory for storing bingo calls" };
        goto cleanup;
    }

    response = process_boards(file, boards);
    if (!aoc_is_success(&response)) goto cleanup;

    size_t winning_board_idx, winning_call_idx;
    response = find_winning_board(calls, num_calls, &winning_board_idx, &winning_call_idx, boards);
    if (!aoc_is_success(&response)) goto cleanup;

    *solution = get_winning_score(&boards[winning_board_idx], calls[winning_call_idx]);

cleanup:
    if (boards != NULL) destroy_boards(boards);
    if (calls != NULL) free(calls);
    if (file != NULL) fclose(file);

    return response;
}

AocResponse process_call_line(FILE* file, int* calls, size_t* num_calls) {
    AocResponse response = { .code = SUCCESS };

    char line[CALL_LINE_LENGTH];
    fgets(line, CALL_LINE_LENGTH, file);
    char current_num_str[MAX_CALL_SIZE];

    size_t line_idx = 0;
    size_t current_num_idx = 0;
    *num_calls = 0;

    while (true) {
        if (line[line_idx] == ',' || line[line_idx] == '\0' || line[line_idx] == '\n') {
            current_num_str[current_num_idx] = '\0';
            current_num_idx = 0;
            response = convert_str_to_int(current_num_str, &calls[(*num_calls)++], 10);
            if (!aoc_is_success(&response) || line[line_idx] == '\0' || line[line_idx] == '\n') {
                break;
            }
        } else {
            current_num_str[current_num_idx++] = line[line_idx];
            if (current_num_idx >= MAX_CALL_SIZE) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Bingo call bigger than expected size" };
                break;
            }
        }
        line_idx++;
    }

    return response;
}

AocResponse process_boards(FILE* file, BingoBoard* boards) {
    AocResponse response = { .code = SUCCESS };
    size_t current_board = 0;
    while (current_board < NUM_BINGO_BOARDS) {
        response = process_board(file, &boards[current_board++]);
        if (!aoc_is_success(&response)) break;
    }
    return response;
}

AocResponse process_board(FILE* file, BingoBoard* board) {
    AocResponse response = { .code = SUCCESS };

    board->has_won = false;
    board->elements = (BingoElement*) malloc(BINGO_BOARD_SIZE * BINGO_BOARD_SIZE * sizeof(BingoElement));
    if (board->elements == NULL) {
        response = (AocResponse) { .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for bingo board" };
    }

    char line[BOARD_LINE_LENGTH];
    size_t line_idx = 0;
    while (line_idx < BINGO_BOARD_SIZE) {
        if (fgets(line, BOARD_LINE_LENGTH, file) == NULL) {
            response = (AocResponse) { .code = INVALID_INPUT, .reason = "Reached end of file before finishing processing boards" };
            break;
        }

        if (strlen(line) < 2) {
            continue;
        }

        response = process_board_line(line, &board->elements[line_idx * BINGO_BOARD_SIZE]);
        if (!aoc_is_success(&response)) break;
        line_idx++;
    }

    return response;
}

AocResponse process_board_line(const char* line, BingoElement* element_line) {
    AocResponse response = { .code = SUCCESS };
    char current_element[MAX_CALL_SIZE];
    size_t element_idx = 0;
    size_t current_element_idx = 0;

    for (size_t i = 0; i < BOARD_LINE_LENGTH; i++) {
        if ((line[i] == ' ' || line[i] == '\0' || line[i] == '\n') && current_element_idx != 0) {
            current_element[current_element_idx] = '\0';
            response = convert_str_to_int(current_element, &element_line[element_idx++].val, 10);
            if (!aoc_is_success(&response) || element_idx == BINGO_BOARD_SIZE) break;
            current_element_idx = 0;
        } else if (line[i] != ' ' && line[i] != '\0' && line[i] != '\n') {
            current_element[current_element_idx++] = line[i];
            if (current_element_idx >= MAX_CALL_SIZE) {
                response = (AocResponse) { .code = INVALID_INPUT, .reason = "Bingo board element larger than expected" };
                break;
            }
        }
    }

    return response;
}

AocResponse find_winning_board(const int* calls, size_t num_calls, size_t* winning_board_idx, size_t* winning_call_idx, BingoBoard* boards) {
    AocResponse response = { .code = SUCCESS };
    size_t won_boards = 0;

    for (size_t i = 0; i < num_calls; i++) {
        for (size_t j = 0; j < NUM_BINGO_BOARDS; j++) {
            if (boards[j].has_won) continue;
            boards[j].has_won = find_element_and_check_won(calls[i], &boards[j]);
            if (boards[j].has_won) {
                won_boards++;
                if (won_boards == NUM_BINGO_BOARDS) {
                    *winning_call_idx = i;
                    *winning_board_idx = j;
                    goto end_func;
                }
            }
        }
    }

end_func:
    return response;
}

bool find_element_and_check_won(int element, BingoBoard* board) {
    for (size_t y = 0; y < BINGO_BOARD_SIZE; y++) {
        for (size_t x = 0; x < BINGO_BOARD_SIZE; x++) {
            if (board->elements[(y * BINGO_BOARD_SIZE) + x].val == element) {
                board->elements[(y * BINGO_BOARD_SIZE) + x].marked = true;

                if (check_row(y, board) || check_col(x, board)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool check_row(size_t row_num, const BingoBoard* board) {
    for (size_t i = 0; i < BINGO_BOARD_SIZE; i++) {
        if (!board->elements[(row_num * BINGO_BOARD_SIZE) + i].marked) return false;
    }
    return true;
}

bool check_col(size_t col_num, const BingoBoard* board) {
    for (size_t i = 0; i < BINGO_BOARD_SIZE; i++) {
        if (!board->elements[(i * BINGO_BOARD_SIZE) + col_num].marked) return false;
    }
    return true;
}

int get_winning_score(const BingoBoard* board, int winning_element) {
    int sum_unmarked = 0;
    for (size_t y = 0; y < BINGO_BOARD_SIZE; y++) {
        for (size_t x = 0; x < BINGO_BOARD_SIZE; x++) {
            if (!board->elements[(y * BINGO_BOARD_SIZE) + x].marked) {
               sum_unmarked += board->elements[(y * BINGO_BOARD_SIZE) + x].val;
            }
        }
    }
    return sum_unmarked * winning_element;
}

void destroy_boards(BingoBoard* boards) {
    for (size_t i = 0; i < NUM_BINGO_BOARDS; i++) {
        if (boards[i].elements != NULL) {
            free(boards[i].elements);
        }
    }
    free(boards);
}

