#include "problem13.h"
#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Location {
  int x;
  int y;
};

struct Fold {
  bool horizontal;
  int val;
};

const size_t MAX_LOCATIONS = 1000;
const size_t MAX_FOLDS = 20;
const size_t P13_MAX_LINE_LEN = 50;
const size_t MAX_TOKEN_LEN = 5;
const size_t FOLD_INFORMATION_LOCATION = 2;

AocResponse p13_process_file(FILE *file, int* solution);
AocResponse process_location_line(const char* line, struct Location* locations, size_t num_locations);
AocResponse process_fold_line(const char* line, struct Fold* folds, size_t num_folds);
void perform_fold(struct Location* locations, size_t num_locations, const struct Fold* fold, int* solution);
bool already_contained(const struct Location* locations, size_t num_locations, struct Location* current);

AocResponse problem13_solve(int *solution) {
  AocResponse response = {.code = SUCCESS};
  const char *file_name = "inputs/problem13.txt";

  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    response = (AocResponse){.code = FILE_NOT_FOUND, .reason = file_name};
    goto p13_end;
  }

  response = p13_process_file(file, solution);

p13_end:
  return response;
}

AocResponse p13_process_file(FILE *file, int* solution) {
  AocResponse response = {.code = SUCCESS};
  struct Location *locations = NULL;
  struct Fold *folds = NULL;
  size_t num_locations = 0, num_folds = 0;
  char line[P13_MAX_LINE_LEN];
  bool is_locations = true;

  locations = malloc(MAX_LOCATIONS * sizeof(struct Location));
  if (locations == NULL) {
    response = (AocResponse){ .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for locations"};
    goto read_end;
  }

  folds = malloc(MAX_FOLDS * sizeof(struct Fold));
  if (folds == NULL) {
    response = (AocResponse){ .code = MEMORY_ALLOCATION_ERROR, .reason = "Unable to allocate required memory for folds"};
    goto read_end;
  }

  while (fgets(line, P13_MAX_LINE_LEN, file)) {
    if (strlen(line) == 0 || line[0] == '\n') {
      is_locations = false;
			continue;
    }

    if (is_locations) {
      response = process_location_line(line, locations, num_locations++);
			if (!aoc_is_success(&response)) goto read_end;
    } else {
      response = process_fold_line(line, folds, num_folds++);
			if (!aoc_is_success(&response)) goto read_end;
    }
  }

	for (size_t i = 0; i < num_locations; i++) {
		printf("Location: %d %d\n", locations[i].x, locations[i].y);
	}
	printf("\n");

	perform_fold(locations, num_locations, &folds[0], solution);

read_end:
  if (locations != NULL) free(locations);  
  if (folds != NULL) free(folds);  
  return response;
}

AocResponse process_location_line(const char* line, struct Location* locations, size_t num_locations) {
  AocResponse response = {.code = SUCCESS};

	char current_token[MAX_TOKEN_LEN];
	size_t current_token_idx = 0;
	bool processing_x_val = true;

	for (size_t i = 0; i < strlen(line); i++) {
		if (line[i] == ',' || line[i] == '\n' || line[i] == '\0') {
			if (current_token_idx == 0) {
				response = (AocResponse) { .code = INVALID_INPUT, .reason = "Found unexpected empty token" };
				break;
			}

			current_token[current_token_idx] = '\0';
			int* val_to_update = processing_x_val ? &locations[num_locations].x : &locations[num_locations].y;
			response = convert_str_to_int(current_token, val_to_update, 10);
			if (!aoc_is_success(&response)) break;

			current_token_idx = 0;
			processing_x_val = !processing_x_val;
		} else {
			current_token[current_token_idx++] = line[i];
		}
	}
	
  return response;
}

AocResponse process_fold_line(const char* line, struct Fold* folds, size_t num_folds) {
  AocResponse response = {.code = SUCCESS};

	size_t current_word = 0;
	char location_token[MAX_TOKEN_LEN];
	size_t token_location = 0;

	char fold_position[MAX_TOKEN_LEN];
	size_t fold_pos_len = 0;

	for (size_t i = 0; i < strlen(line); i++) {
		if (line[i] == ' ') current_word++;
		else if (line[i] == '\n' || line[i] == '\0') {
			if (token_location == 0) {
				response = (AocResponse) { .code = INVALID_INPUT, .reason = "Unable to find fold information" };
				break;
			}
			location_token[token_location] = '\0';
			folds[num_folds].horizontal = location_token[0] == 'y';

			fold_pos_len = 0;
			for (size_t j = 2; j < strlen(location_token); j++) {
				if (location_token[j] != '\n' && location_token[j] != '\0')
					fold_position[fold_pos_len++] = location_token[j];
			}
			fold_position[fold_pos_len] = '\0';
			response = convert_str_to_int(fold_position, &folds[num_folds].val, 10);
		} else if (current_word == FOLD_INFORMATION_LOCATION) {
			location_token[token_location++] = line[i];
		}
	}


  return response;
}

void perform_fold(struct Location* locations, size_t num_locations, const struct Fold* fold, int* solution) {
	size_t num_after_fold = 0;
	size_t end_pointer = num_locations - 1;
	size_t i = 0;

	while (num_after_fold < end_pointer) {
		if (fold->horizontal && locations[i].y > fold->val) {
			locations[i].y = fold->val - (locations[i].y - fold->val);
		}	else if (!fold->horizontal && locations[i].x > fold->val) {
			locations[i].x = fold->val - (locations[i].x - fold->val);
		}

		if (already_contained(locations, num_after_fold, &locations[i])) {
			locations[i] = locations[end_pointer--];
		} else {
			num_after_fold++;
			i++;
		}
	}

	num_after_fold++;
	*solution = num_after_fold;
}

bool already_contained(const struct Location* locations, size_t num_locations, struct Location* current) {
	for (size_t i = 0; i < num_locations; i++) {
		if (current->x == locations[i].x && current->y == locations[i].y) return true;
	}

	return false;
}
