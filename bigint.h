/*
 * bigint.h - Definición de la estructura Bigint y prototipos de funciones base.
 *
 * Un archivo .h (header) es como un "contrato" — declara qué funciones existen
 * y qué tipos se usan, sin implementarlas. Así, otros archivos .c pueden
 * incluir este header y usar las funciones sin necesidad de copiar el código.
 *
 * Es equivalente a una interfaz en Java, pero más simple.
 */

/*
 * Include guard: evita que este header se incluya más de una vez en la misma
 * compilación. Si ya fue incluido, el compilador ignora todo lo que sigue.
 * Es como un "if not already imported" automático.
 */
#ifndef BIGINT_H
#define BIGINT_H

/* Tamaño máximo de dígitos que puede tener un número */
#define MAX_DIGITS 1000

/*
 * Estructura para representar un número entero de longitud arbitraria.
 *
 * Los dígitos se guardan en orden INVERTIDO: el índice 0 es las unidades,
 * el índice 1 las decenas, etc. Esto facilita la suma dígito a dígito,
 * ya que siempre arrancamos desde el índice 0 sin importar la longitud.
 *
 * Ejemplo: el número 12345 se guarda como:
 *   digits[0]=5, digits[1]=4, digits[2]=3, digits[3]=2, digits[4]=1
 *   len = 5, sign = 1
 *
 * El número -42 se guarda como:
 *   digits[0]=2, digits[1]=4, len=2, sign=-1
 */
typedef struct {
    char *digits; /* array de dígitos decimales 0..9, guardados en orden invertido */
    int   len;    /* cantidad de dígitos significativos */
    int   sign;   /* 1 = positivo, -1 = negativo */
} Bigint;

/* ── Funciones base (implementadas en bigint.c) ── */

/*
 * is_decimal_string: verifica que una cadena sea un entero válido.
 * En la Parte 2 acepta un signo opcional + o - al inicio.
 * Devuelve 1 si es válido, 0 si no.
 */
int is_decimal_string(const char *text);

/*
 * bigint_new: crea un Bigint a partir de un string como "123" o "-45".
 * Devuelve un puntero al Bigint creado, o NULL si hay error.
 * El llamador es responsable de liberar la memoria con bigint_free.
 */
Bigint *bigint_new(const char *text);

/*
 * bigint_free: libera toda la memoria de un Bigint.
 * Siempre hay que llamarla cuando el Bigint ya no se necesita.
 */
void bigint_free(Bigint *n);

/*
 * bigint_print: imprime el número en stdout en formato decimal.
 * Ejemplo: si digits=[3,2,1], len=3, sign=1 → imprime "123\n"
 * Si sign=-1 → imprime "-123\n"
 */
void bigint_print(const Bigint *n);

/*
 * bigint_max_len: devuelve la mayor longitud entre dos Bigint.
 * Se usa en las operaciones para saber hasta dónde iterar.
 */
int bigint_max_len(const Bigint *a, const Bigint *b);

/*
 * bigint_digit_at: devuelve el dígito en la posición pos.
 * Si pos >= len, devuelve 0 (como si el número tuviera ceros a la izquierda).
 * Evita salirse del array al operar con números de distinta longitud.
 */
char bigint_digit_at(const Bigint *n, int pos);

/* ── Funciones de operaciones (implementadas en ops.c) ── */

/*
 * bigint_add: suma dos Bigint y devuelve el resultado.
 * Maneja signos: si ambos son positivos suma, si tienen distinto signo resta, etc.
 */
Bigint *bigint_add(const Bigint *a, const Bigint *b);

/*
 * bigint_sub: resta dos Bigint (a - b) y devuelve el resultado.
 */
Bigint *bigint_sub(const Bigint *a, const Bigint *b);

/*
 * bigint_mul: multiplica dos Bigint y devuelve el resultado.
 */
Bigint *bigint_mul(const Bigint *a, const Bigint *b);

#endif /* BIGINT_H */