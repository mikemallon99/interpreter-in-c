#include "repl/repl.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    FILE* file;
    if (argc > 1) {
        file = fopen(argv[1], "rb");

        if (file == NULL) {
            printf("Could not open file %s\n", argv[1]);
            return 1;
        }

        // Get the length of the file
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory for the file contents and read them into the buffer
        char* buffer = calloc(length+1, 1);

        if (buffer) {
            fread(buffer, 1, length, file);
        }

        fclose(file);

        if (buffer) {
            // Pass the file contents to the function
            execute_file(buffer);

            // Free the memory allocated for the buffer
            free(buffer);
        } else {
            printf("Could not allocate memory for file contents\n");
            return 1;
        }
    }
    else {
        start();
    }
    return 0;
}
