#include <stdio.h>

#include "problem10.h"
#include "response.h"

int main() {
    unsigned long solution = 0;
    const AocResponse response = problem10_solve(&solution);
    if (!aoc_is_success(&response)) {
        aoc_display_response(&response);
        return 1;
    }

    printf("The solution to the problem is: %zu\n", solution);
    return 0;
}
