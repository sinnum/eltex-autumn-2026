#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void run_producer();
void run_consumer();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <producer|consumer>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "producer") == 0) {
        run_producer();
    } else if (strcmp(argv[1], "consumer") == 0) {
        run_consumer();
    } else {
        fprintf(stderr, "Invalid mode. Use 'producer' or 'consumer'.\n");
        return 1;
    }

    return 0;
}