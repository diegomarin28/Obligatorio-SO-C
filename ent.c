#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Error: argumentos insuficientes\n");
        return 1;
    }

    if (strcmp(argv[1], "sumar") != 0) {
        fprintf(stderr, "Error: operacion invalida\n");
        return 1;
    }

    return 0;
}