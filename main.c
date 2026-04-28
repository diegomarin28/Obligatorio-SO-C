/*
 * main.c - Punto de entrada del programa ent.
 *
 * Este archivo solo se encarga de:
 *   1. Leer y validar los argumentos de línea de comandos
 *   2. Llamar a la operación correspondiente
 *   3. Imprimir el resultado
 *
 * La lógica de los números grandes está en bigint.c y ops.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h" /* incluimos nuestro header para usar Bigint y todas las funciones */


/*
 * op_sumar: ejecuta la operación "sumar" con los argumentos dados.
 *
 * operandos[] es el array de strings con los números (argv desde el índice 2).
 * n es la cantidad de operandos.
 *
 * Devuelve 0 si todo salió bien, 1 si hubo error.
 */
static int op_sumar(char *operandos[], int n) {
    /* Crear el primer Bigint */
    Bigint *acumulador = bigint_new(operandos[0]);
    if (acumulador == NULL) return 1;

    /* Sumar el resto de operandos de a uno */
    for (int i = 1; i < n; i++) {
        Bigint *operando = bigint_new(operandos[i]);
        if (operando == NULL) {
            bigint_free(acumulador);
            return 1;
        }

        Bigint *nueva_suma = bigint_add(acumulador, operando);
        bigint_free(operando);
        bigint_free(acumulador);

        if (nueva_suma == NULL) return 1;
        acumulador = nueva_suma;
    }

    bigint_print(acumulador);
    bigint_free(acumulador);
    return 0;
}


/*
 * op_restar: ejecuta "restar N1 N2" (exactamente dos operandos).
 */
static int op_restar(char *operandos[], int n) {
    if (n != 2) {
        fprintf(stderr, "Error: 'restar' requiere exactamente dos operandos\n");
        return 1;
    }

    Bigint *a = bigint_new(operandos[0]);
    if (a == NULL) return 1;

    Bigint *b = bigint_new(operandos[1]);
    if (b == NULL) { bigint_free(a); return 1; }

    Bigint *resultado = bigint_sub(a, b);
    bigint_free(a);
    bigint_free(b);

    if (resultado == NULL) return 1;

    bigint_print(resultado);
    bigint_free(resultado);
    return 0;
}


/*
 * op_multiplicar: ejecuta "multiplicar N1 N2 [N3 ...]".
 * Multiplica todos los operandos de izquierda a derecha.
 */
static int op_multiplicar(char *operandos[], int n) {
    Bigint *acumulador = bigint_new(operandos[0]);
    if (acumulador == NULL) return 1;

    for (int i = 1; i < n; i++) {
        Bigint *operando = bigint_new(operandos[i]);
        if (operando == NULL) {
            bigint_free(acumulador);
            return 1;
        }

        Bigint *nuevo_producto = bigint_mul(acumulador, operando);
        bigint_free(operando);
        bigint_free(acumulador);

        if (nuevo_producto == NULL) return 1;
        acumulador = nuevo_producto;
    }

    bigint_print(acumulador);
    bigint_free(acumulador);
    return 0;
}


/*
 * main: punto de entrada.
 *
 * Estructura de argc/argv para "ent sumar 10 20 30":
 *   argc    = 5
 *   argv[0] = "ent"
 *   argv[1] = "sumar"
 *   argv[2] = "10"
 *   argv[3] = "20"
 *   argv[4] = "30"
 */
int main(int argc, char *argv[]) {

    /* 1. Verificar que haya al menos el nombre del programa y la operación */
    if (argc < 2) {
        fprintf(stderr, "Error: uso: ent <operacion> N1 N2 [...]\n");
        fprintf(stderr, "Operaciones: sumar, restar, multiplicar\n");
        return 1;
    }

    /* 2. Leer la operación */
    char *operacion = argv[1];

    /* 3. Verificar que la operación sea válida */
    int es_sumar      = strcmp(operacion, "sumar")      == 0;
    int es_restar     = strcmp(operacion, "restar")     == 0;
    int es_multiplicar = strcmp(operacion, "multiplicar") == 0;

    if (!es_sumar && !es_restar && !es_multiplicar) {
        fprintf(stderr, "Error: operacion no soportada '%s'\n", operacion);
        fprintf(stderr, "Operaciones disponibles: sumar, restar, multiplicar\n");
        return 1;
    }

    /* 4. Verificar cantidad mínima de operandos */
    /* restar necesita exactamente 2, sumar y multiplicar necesitan al menos 2 */
    int min_operandos = 2;
    if (argc - 2 < min_operandos) {
        fprintf(stderr, "Error: '%s' requiere al menos dos operandos\n", operacion);
        return 1;
    }

    /* 5. Validar cada operando */
    for (int i = 2; i < argc; i++) {
        if (!is_decimal_string(argv[i])) {
            fprintf(stderr, "Error: operando invalido '%s'\n", argv[i]);
            return 1;
        }
    }

    /* 6. Ejecutar la operación correspondiente */
    /* argv+2 es un puntero al inicio de los operandos (saltamos argv[0] y argv[1]) */
    /* argc-2 es la cantidad de operandos */
    if (es_sumar)       return op_sumar(argv + 2, argc - 2);
    if (es_restar)      return op_restar(argv + 2, argc - 2);
    if (es_multiplicar) return op_multiplicar(argv + 2, argc - 2);

    return 1; /* nunca llegamos acá, pero el compilador lo pide */
}