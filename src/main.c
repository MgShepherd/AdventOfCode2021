#include <stdio.h>

#include "problem7.h"
#include "response.h"

int main() {
    int solution = 0;
    const AocResponse response = problem7_solve(&solution);
    if (!aoc_is_success(&response)) {
        aoc_display_response(&response);
        return 1;
    }

    printf("The solution to the problem is: %d\n", solution);
    return 0;
}
