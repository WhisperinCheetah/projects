#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


void add(char** usernames, char** passwords, size_t* size, size_t* count) {
    char* username = (char*)malloc(sizeof(char) * 100);
    char* password = (char*)malloc(sizeof(char) * 100);

    printf("Enter a username: ");
    scanf("%99s", username);

    printf("Enter a password: ");
    scanf("%99s", password);

    if (*count < *size) {
        usernames[*count] = username;
        passwords[*count] = password;

        (*count)++;
    }
    
    printf("Username %s and password %s added.\n", username, password);
    printf("=========================================\n");
}

void print_passwords(char** usernames, char** passwords, size_t count) {
    printf("You have %lu password(s)\n", count);
    for (int i = 0; i < count; i++) {
        printf("%s --- %s\n", usernames[i], passwords[i]);
    }
}


int main() {
    size_t size = 1;
    size_t count = 0;
    char** usernames = (char**)malloc(sizeof(char*) * size);
    char** passwords = (char**)malloc(sizeof(char*) * size);

    printf("Welcome to your password manager.\n");

    while (true) {
        char command[100];
        printf("Enter a command: ");
        scanf("%s",command);
        printf("You entered: %s\n\n", command);
        if (strcmp(command, "add") == 0) {
            add(usernames, passwords, &size, &count);
        } else if (strcmp(command, "print") == 0) {
            print_passwords(usernames, passwords, count);
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Command not recognised\n");
        }
        // break;
    }

    for (int i = 0; i < count; i++) {
        free(usernames[i]);
        free(passwords[i]);
    }

    free(usernames);
    free(passwords);

    return 0;
}