/*
 * bigint.c - Implementación de las funciones base del Bigint.
 *
 * Este archivo contiene todo lo que tiene que ver con crear, destruir
 * e imprimir un Bigint. Las operaciones matemáticas están en ops.c.
 *
 * Para usar estas funciones desde otro archivo, hay que incluir bigint.h.
 */

#include <stdio.h>   /* printf, fprintf */
#include <stdlib.h>  /* malloc, free */
#include <string.h>  /* strlen */
#include "bigint.h"  /* nuestra propia definición — se usa "" en vez de <> para headers locales */


/*
 * is_decimal_string: verifica que una cadena sea un entero válido.
 *
 * En la Parte 2 aceptamos un signo + o - opcional al inicio.
 * Ejemplos válidos:   "123", "-45", "+7", "0"
 * Ejemplos inválidos: "", "12x", "3.14", "+", "-", NULL
 *
 * Devuelve 1 si es válido, 0 si no.
 */
int is_decimal_string(const char *text) {
    if (text == NULL || text[0] == '\0') return 0;

    int start = 0; /* índice desde donde empiezan los dígitos */

    /* Aceptamos un signo opcional al inicio */
    if (text[0] == '+' || text[0] == '-') {
        start = 1;
    }

    /* Después del signo tiene que haber al menos un dígito */
    if (text[start] == '\0') return 0;

    /* Todos los caracteres restantes deben ser dígitos */
    for (int i = start; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') return 0;
    }
    return 1;
}


/*
 * bigint_new: crea un Bigint a partir de un string.
 *
 * Pasos:
 *   1. Detectar el signo (+ o -)
 *   2. Validar longitud máxima
 *   3. Reservar memoria para la estructura y el array de dígitos
 *   4. Copiar los dígitos al revés
 *   5. Normalizar (eliminar ceros a la izquierda)
 *
 * Devuelve puntero al Bigint, o NULL si hay error.
 */
Bigint *bigint_new(const char *text) {
    int sign = 1;   /* por defecto positivo */
    int start = 0;  /* índice donde empiezan los dígitos */

    /* Detectar signo */
    if (text[0] == '-') {
        sign = -1;
        start = 1;
    } else if (text[0] == '+') {
        sign = 1;
        start = 1;
    }

    /* n = cantidad de dígitos (sin el signo) */
    int n = (int)strlen(text) - start;

    /* Validar longitud máxima */
    if (n > MAX_DIGITS) {
        fprintf(stderr, "Error: numero demasiado largo (max %d digitos)\n", MAX_DIGITS);
        return NULL;
    }

    /* Reservar memoria para la estructura */
    Bigint *b = malloc(sizeof(Bigint));
    if (b == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        return NULL;
    }

    /* Reservar memoria para el array de dígitos */
    b->digits = malloc(n + 1); /* +1 de seguridad */
    if (b->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(b);
        return NULL;
    }

    b->sign = sign;

    /*
     * Copiar los dígitos al revés.
     * text = "12345" (start=0) → digits[0]=5, digits[1]=4, ...
     * text = "-42"   (start=1) → digits[0]=2, digits[1]=4
     */
    int j = 0;
    for (int i = (int)strlen(text) - 1; i >= start; i--) {
        b->digits[j++] = text[i] - '0'; /* char '5' → número 5 */
    }
    b->len = n;

    /* Normalización: eliminar ceros a la izquierda (al final del array invertido) */
    while (b->len > 1 && b->digits[b->len - 1] == 0) {
        b->len--;
    }

    /* El cero siempre es positivo, independientemente del signo ingresado */
    if (b->len == 1 && b->digits[0] == 0) {
        b->sign = 1;
    }

    return b;
}


/*
 * bigint_free: libera la memoria de un Bigint.
 *
 * Hay que liberar en orden: primero el array interno (digits),
 * después la estructura. Si lo hacemos al revés perdemos la
 * referencia a digits y no podemos liberarlo.
 */
void bigint_free(Bigint *n) {
    if (n == NULL) return;
    free(n->digits); /* primero el array interno */
    free(n);         /* después la estructura */
}


/*
 * bigint_print: imprime el número en stdout.
 *
 * Los dígitos están al revés, entonces recorremos de atrás para adelante.
 * Si el número es negativo, primero imprimimos el signo '-'.
 */
void bigint_print(const Bigint *n) {
    if (n == NULL) return;

    /* Imprimir el signo negativo si corresponde */
    if (n->sign == -1) {
        printf("-");
    }

    /* Imprimir dígitos de más significativo a menos significativo */
    for (int i = n->len - 1; i >= 0; i--) {
        printf("%d", (int)n->digits[i]);
    }
    printf("\n");
}


/*
 * bigint_max_len: devuelve la mayor longitud entre dos Bigint.
 * Operador ternario: condicion ? valor_si_true : valor_si_false
 */
int bigint_max_len(const Bigint *a, const Bigint *b) {
    return (a->len > b->len) ? a->len : b->len;
}


/*
 * bigint_digit_at: devuelve el dígito en la posición pos.
 * Si pos está fuera del rango, devuelve 0 (como cero a la izquierda).
 * Esto evita salirse del array cuando los operandos tienen distinta longitud.
 */
char bigint_digit_at(const Bigint *n, int pos) {
    if (pos >= n->len) return 0;
    return n->digits[pos];
}