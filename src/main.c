#include <stdio.h>

#include "problem6.h"
#include "response.h"

int main() {
    unsigned long solution = 0;
    const AocResponse response = problem6_solve(&solution);
    if (!aoc_is_success(&response)) {
        aoc_display_response(&response);
        return 1;
    }

    printf("The solution to the problem is: %zu", solution);
    return 0;
}
