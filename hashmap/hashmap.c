#include <stdio.h>

void print_error(char* string) {
    printf("ERROR: %s\n", string);
}

char** read_file_by_words(FILE *fptr, size_t count) {

}

int main(int argc, char **argv)
{
    if (argc != 2) {
        print_error("no arguments provided");
        print_error("usage: ./hashmap <path>");
        return 1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "r");

    if (fptr == NULL) {
        print_error("couldn't open file");
        return 2;
    }

    char** words = read_file_by_words(fptr, 10);

    fclose(fptr);
    return 0;   
}