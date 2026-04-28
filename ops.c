/*
 * ops.c - Operaciones aritméticas sobre Bigint: suma, resta, multiplicación.
 *
 * Separamos las operaciones de las funciones base (bigint.c) para mantener
 * el código organizado. Este archivo depende de bigint.h para conocer
 * la estructura Bigint y las funciones auxiliares.
 */

#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"


/* ════════════════════════════════════════════════════════════
 * FUNCIONES AUXILIARES INTERNAS (no se exponen en bigint.h)
 * ════════════════════════════════════════════════════════════ */

/*
 * bigint_abs_cmp: compara los valores absolutos de dos Bigint.
 * Devuelve:
 *   1  si |a| > |b|
 *  -1  si |a| < |b|
 *   0  si |a| == |b|
 *
 * Se usa en bigint_sub para saber cuál es el mayor.
 */
static int bigint_abs_cmp(const Bigint *a, const Bigint *b) {
    if (a->len != b->len) {
        return (a->len > b->len) ? 1 : -1;
    }
    /* Misma longitud: comparamos dígito a dígito desde el más significativo */
    for (int i = a->len - 1; i >= 0; i--) {
        if (a->digits[i] != b->digits[i]) {
            return (a->digits[i] > b->digits[i]) ? 1 : -1;
        }
    }
    return 0; /* son iguales */
}


/*
 * add_magnitudes: suma los valores absolutos de dos Bigint,
 * sin considerar el signo. El signo lo asigna el llamador.
 *
 * Es el algoritmo de suma a mano: dígito a dígito con acarreo.
 * Ejemplo: 999 + 1
 *   i=0: 9+1+0=10 → digits[0]=0, carry=1
 *   i=1: 9+0+1=10 → digits[1]=0, carry=1
 *   i=2: 9+0+1=10 → digits[2]=0, carry=1
 *   carry=1 → digits[3]=1, len=4
 *   Resultado: [0,0,0,1] → "1000"
 */
static Bigint *add_magnitudes(const Bigint *a, const Bigint *b) {
    int max_len = bigint_max_len(a, b);

    if (max_len + 1 > MAX_DIGITS) {
        fprintf(stderr, "Error: resultado excede el maximo de digitos\n");
        return NULL;
    }

    Bigint *result = malloc(sizeof(Bigint));
    if (result == NULL) { fprintf(stderr, "Error: sin memoria\n"); return NULL; }

    result->digits = malloc(max_len + 2); /* +2: carry + seguridad */
    if (result->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(result);
        return NULL;
    }
    result->sign = 1;

    int carry = 0;
    int i;
    for (i = 0; i < max_len; i++) {
        int sum = bigint_digit_at(a, i) + bigint_digit_at(b, i) + carry;
        result->digits[i] = sum % 10;
        carry = sum / 10;
    }

    if (carry > 0) {
        result->digits[i] = carry;
        result->len = max_len + 1;
    } else {
        result->len = max_len;
    }

    return result;
}


/*
 * sub_magnitudes: resta el valor absoluto de b al de a, asumiendo |a| >= |b|.
 * El signo lo asigna el llamador.
 *
 * Algoritmo: resta a mano con "préstamo" (borrow).
 * Ejemplo: 100 - 1
 *   i=0: 0-1 → negativo → tomamos préstamo: 10-1=9, borrow=1
 *   i=1: 0-0-1 → negativo → préstamo: 10-1=9, borrow=1
 *   i=2: 1-0-1=0
 *   Resultado: [9,9,0] → len=2 (normalizamos el 0 final) → "99"
 */
static Bigint *sub_magnitudes(const Bigint *a, const Bigint *b) {
    /* a debe ser >= b en valor absoluto */
    Bigint *result = malloc(sizeof(Bigint));
    if (result == NULL) { fprintf(stderr, "Error: sin memoria\n"); return NULL; }

    result->digits = malloc(a->len + 1);
    if (result->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(result);
        return NULL;
    }
    result->sign = 1;

    int borrow = 0; /* el "préstamo" de la resta */
    for (int i = 0; i < a->len; i++) {
        int diff = bigint_digit_at(a, i) - bigint_digit_at(b, i) - borrow;
        if (diff < 0) {
            diff += 10;  /* tomamos préstamo del dígito siguiente */
            borrow = 1;
        } else {
            borrow = 0;
        }
        result->digits[i] = (char)diff;
    }

    result->len = a->len;

    /* Normalizar: eliminar ceros a la izquierda (al final del array invertido) */
    while (result->len > 1 && result->digits[result->len - 1] == 0) {
        result->len--;
    }

    return result;
}


/* ════════════════════════════════════════════════════════════
 * OPERACIONES PÚBLICAS
 * ════════════════════════════════════════════════════════════ */

/*
 * bigint_add: suma dos Bigint considerando sus signos.
 *
 * Casos:
 *   (+a) + (+b) = +(a+b)          → suma magnitudes, signo positivo
 *   (-a) + (-b) = -(a+b)          → suma magnitudes, signo negativo
 *   (+a) + (-b) con a>=b = +(a-b) → resta magnitudes, signo positivo
 *   (+a) + (-b) con a<b  = -(b-a) → resta magnitudes, signo negativo
 *   (-a) + (+b) = opuesto del caso anterior
 */
Bigint *bigint_add(const Bigint *a, const Bigint *b) {
    Bigint *result;

    if (a->sign == b->sign) {
        /* Mismo signo: sumamos magnitudes y conservamos el signo */
        result = add_magnitudes(a, b);
        if (result) result->sign = a->sign;
    } else {
        /* Distinto signo: restamos la menor de la mayor */
        int cmp = bigint_abs_cmp(a, b);
        if (cmp == 0) {
            /* Son iguales en valor absoluto → resultado es 0 */
            result = bigint_new("0");
        } else if (cmp > 0) {
            /* |a| > |b|: resultado = a - b, con el signo de a */
            result = sub_magnitudes(a, b);
            if (result) result->sign = a->sign;
        } else {
            /* |b| > |a|: resultado = b - a, con el signo de b */
            result = sub_magnitudes(b, a);
            if (result) result->sign = b->sign;
        }
    }

    return result;
}


/*
 * bigint_sub: resta dos Bigint (a - b).
 *
 * Restar b es lo mismo que sumar (-b).
 * Entonces invertimos el signo de b y llamamos a bigint_add.
 *
 * Para no modificar b (es const), creamos una copia temporal.
 */
Bigint *bigint_sub(const Bigint *a, const Bigint *b) {
    /*
     * Creamos una copia de b con el signo invertido.
     * No podemos hacer b->sign *= -1 porque b es const.
     */
    Bigint temp;
    temp.digits = b->digits; /* misma memoria de dígitos, solo cambiamos el signo */
    temp.len    = b->len;
    temp.sign   = b->sign * -1; /* invertimos el signo */

    return bigint_add(a, &temp);
}


/*
 * bigint_mul: multiplica dos Bigint.
 *
 * Algoritmo: multiplicación larga (igual que a mano en columnas).
 *
 * Ejemplo: 12 × 34
 *   digits_a = [2,1] (12 al revés)
 *   digits_b = [4,3] (34 al revés)
 *
 *   i=0 (dígito 2 de a):
 *     j=0: 2×4=8  → result[0] += 8
 *     j=1: 2×3=6  → result[1] += 6
 *   i=1 (dígito 1 de a):
 *     j=0: 1×4=4  → result[1] += 4  (posición i+j = 1)
 *     j=1: 1×3=3  → result[2] += 3
 *
 *   result = [8, 10, 3]
 *   Normalizamos acarreos:
 *     result[0]=8, result[1]=10 → result[1]=0, result[2]+=1 → result[2]=4
 *   result = [8, 0, 4] → "408"  ✓  (12 × 34 = 408)
 *
 * El signo del resultado: positivo si ambos tienen el mismo signo, negativo si no.
 */
Bigint *bigint_mul(const Bigint *a, const Bigint *b) {
    int result_len = a->len + b->len; /* el producto tiene como máximo a->len + b->len dígitos */

    if (result_len > MAX_DIGITS) {
        fprintf(stderr, "Error: resultado excede el maximo de digitos\n");
        return NULL;
    }

    Bigint *result = malloc(sizeof(Bigint));
    if (result == NULL) { fprintf(stderr, "Error: sin memoria\n"); return NULL; }

    result->digits = malloc(result_len + 1);
    if (result->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(result);
        return NULL;
    }

    /* Inicializar todos los dígitos en 0 */
    for (int k = 0; k < result_len; k++) {
        result->digits[k] = 0;
    }
    result->len  = result_len;
    result->sign = 1;

    /* Multiplicación larga: cada dígito de a por cada dígito de b */
    for (int i = 0; i < a->len; i++) {
        for (int j = 0; j < b->len; j++) {
            result->digits[i + j] += a->digits[i] * b->digits[j];
        }
    }

    /* Normalizar acarreos: si algún dígito es >= 10, propagamos */
    for (int k = 0; k < result_len - 1; k++) {
        if (result->digits[k] >= 10) {
            result->digits[k + 1] += result->digits[k] / 10;
            result->digits[k]      = result->digits[k] % 10;
        }
    }

    /* Normalizar longitud: eliminar ceros a la izquierda */
    while (result->len > 1 && result->digits[result->len - 1] == 0) {
        result->len--;
    }

    /* Signo: positivo si ambos tienen el mismo signo, negativo si no */
    if (a->sign == b->sign) {
        result->sign = 1;
    } else {
        result->sign = -1;
    }

    /* Si el resultado es 0, el signo siempre es positivo */
    if (result->len == 1 && result->digits[0] == 0) {
        result->sign = 1;
    }

    return result;
}