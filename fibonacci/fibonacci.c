#include <stdio.h>
#include <stdlib.h>

int recursive(int n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return recursive(n-1) + recursive(n-2);
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Expected 1 argument but got %d", argc);
    return 1;
  }

  int n = atoi(argv[1]);
  
  printf("%dth fibonacci number is: %d", n, recursive(n));
  return 0;
}
