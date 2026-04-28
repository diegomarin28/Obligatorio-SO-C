
/*
 * ent.c - Sumador de enteros de longitud arbitraria
 */

#include <stdio.h> /* printf, fprintf (entrada/salida)  */
#include <stdlib.h> /*  malloc, free (memoria dinámica) */
#include <string.h> /*  strlen, strcmp, strcpy (manejo de strings) */

/* Tamaño máximo de dígitos que puede tener un número, me lo pide el obligatorio */
#define MAX_DIGITS 1000

/* Estructura para representar un número entero de longitud arbitraria.
 * Guardamos los dígitos en orden invertido para que el indice 0 sea el dígito de las unidades, el índice 1 el de las decenas, etc.
 * Ejemplo: el número 12345 se guarda como:
 *   digits[0]=5, digits[1]=4, digits[2]=3, digits[3]=2, digits[4]=1
 *   len = 5
 * Porque la suma se hace de derecha a izquierda (de las unidades hacia arriba), y si los dígitos están al revés, el índice 0 siempre es las unidades — mucho más fácil de programar.
 * Ademas, si sumo distintas longitudes de numeros deberia alinear antes de sumar, esto me lo complica, tmb el no saber cual es mi ultimo digito del array y deberia recorrer siempre todo el array para sumar, innecesario
 */

typedef struct {
    char *digits; /* array de dígitos decimales 0..9, guardados en orden invertido , char representa un numero del -128 al 127, es el mas chico posible, viene bien pq usamos del 0 al 9 para los digitos*/
    int   len;    /* cantidad de dígitos significativos */
    int   sign;   /* 1 = positivo, -1 = negativo (siempre 1 en este caso) */
} Bigint;


/*
 * is_decimal_string: verifica que una cadena sea un entero no negativo válido.
 * Devuelve 1 si la cadena solo tiene dígitos '0'-'9' y no está vacía.
 * Devuelve 0 en cualquier otro caso (vacía, letras, signos, puntos, etc.).
 */
int is_decimal_string(const char *text) {
    if (text == NULL || text[0] == '\0') return 0;

    for (int i = 0; text[i] != '\0'; i++) { /* desde que i=0 hasta que i sea el indice terminador*/
        /* Si algún carácter no es dígito, rechazamos */
        if (text[i] < '0' || text[i] > '9') return 0;
    }
    return 1;
}


/*
 * bigint_new: crea un Bigint a partir de una cadena de texto.
 *
 * Ejemplo: bigint_new("00123") crea un Bigint con dígitos [3,2,1], len=3
 * (se ignoran los ceros a la izquierda y se guarda al revés).
 *
 * Devuelve un puntero( eso lo da el *) al Bigint creado, o NULL si hay error.
 */
Bigint *bigint_new(const char *text) {
    int n = (int)strlen(text); /* strlen es como len pero de un string, a "0" devuelve 1, a "" devuelve 0, a "123" devuelve 3.., lo quiero en int, por eso el int antes, para poder compararlo y eso, pongo int en ambos lados para q coincidan  */

    /* Validar longitud máxima */
    if (n > MAX_DIGITS) {
        fprintf(stderr, "Error: numero demasiado largo (max %d digitos)\n", MAX_DIGITS);
        return NULL;
    }

    /*Malloc es una funcion que pide memoria al sistema operativo en tiempo de ejecucion. Hay q liberarla manualmente con free, sino la libero hay memory leak */

    /* Reservar memoria para la estructura , en C tengo q pedir memoria*/
    Bigint *b = malloc(sizeof(Bigint)); /* el *b siginifica q b es un puntero, Pedís al sistema operativo memoria suficiente para guardar un Bigint. sizeof calcula cuántos bytes ocupa la estructura. malloc devuelve la dirección de esa memoria — que guardás en b.*/
    if (b == NULL) {
        fprintf(stderr, "Error: sin memoria\n"); /* stderr es la salida de errores*/
        return NULL;
    }

    /* Reservar memoria para el array de dígitos */
    b->digits = malloc(n + 1); /* b es un puntero a un Bigint. Para acceder a sus campos usás -> en vez de . y +1 por si el resultado de una suma necesita un dígito más, tipo 99 + 1, por eso pido memoria para n+1 bytes */
    if (b->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(b); 
        return NULL; /* si no puedo reservar memoria para los dígitos, libero la estructura que ya había reservado antes, para no tener fugas de memoria (si no lo hago pierdo esa memoria hasta q el programa termine), y devuelvo NULL para indicar el error */
    }

    b->sign = 1; /* siempre positivo en esta versión, el número guardado en b es positivo */

    /*
     * Copiamos los dígitos al revés.
     * text = "12345" → digits[0]=5, digits[1]=4, digits[2]=3, ...
     */
    int j = 0;
    for (int i = n - 1; i >= 0; i--) {
        b->digits[j++] = text[i] - '0'; /*j++ — usa j primero y después lo incrementa, convertimos char '5' al número 5, text[i] - '0' — convierte el carácter '3' (que en ASCII es 51) al número 3 restando '0' (que es 48). Da 51 - 48 = 3 */
    }
    b->len = n; /* inicialmente, len es n, pero después lo normalizamos para eliminar ceros a la izquierda */

    /* Normalización: eliminar ceros a la izquierda (que en nuestro array son al final) */
    while (b->len > 1 && b->digits[b->len - 1] == 0) { /* es len -1 pq un len 4 llega hasta digits[3] pq cuenta el 0]*/
        b->len--;
    }

    return b; /* devuelvo el puntero al Bigint creado */
}


/*
 * bigint_free: libera la memoria de un Bigint.
 * Siempre hay que liberar tanto el array de dígitos como la estructura.
 */
void bigint_free(Bigint *n) { /*n es un puntero al Bigint que le pasás */
    if (n == NULL) return;
    free(n->digits); /* primero liberamos el array interno */
    free(n);         /* después la estructura , hay dos free pq hice dos malloc*/
}


/*
 * bigint_print: imprime el número en decimal en stdout.
 * Recordamos que los dígitos están al revés, entonces imprimimos
 * desde el último dígito hasta el primero.
 */
void bigint_print(const Bigint *n) {
    if (n == NULL) return;
    /* Imprimimos desde el dígito más significativo (al final del array) hacia atrás */
    for (int i = n->len - 1; i >= 0; i--) {
        printf("%d", (int)n->digits[i]); /*printf con %d imprime un número entero. El (int) convierte el char a int para que %d lo imprima bien como número y no como carácter. Sin el cast podría dar resultados raros */
    } /*printf siempre recibe primero un string con el formato, y después los valores. Los %d (numero entero 3), %c (caracter '3'), %s (para strings) son los "huecos" donde se insertan los valores */
    printf("\n");
}


/*
 * bigint_max_len: devuelve la mayor longitud entre dos Bigint.
 * Se usa en bigint_add para saber hasta dónde iterar.
 */
int bigint_max_len(const Bigint *a, const Bigint *b) {
    return (a->len > b->len) ? a->len : b->len; /* operador ternario: si a->len > b->len, devuelve a->len, sino devuelve b->len, devuelve el mas grande de los dos */
}


/*
 * bigint_digit_at: devuelve el dígito en la posición pos.
 * Si pos >= len (el número no tiene tantos dígitos), devuelve 0.
 * Esto es muy útil en la suma para no salirse del array. Con bigint_digit_at(b, 1) devuelve 0 de forma segura, por ej si sumo numeros de distintos digitos, para no llegar a un indice que no existe, y que me de error, con esta funcion me devuelve 0, como si el numero tuviera ceros a la izquierda, por ejemplo al sumar 123 + 45, para el 45 en la posicion 2 (centenas) no tiene digito, entonces devuelve 0, y puedo seguir sumando sin problemas.
 */
char bigint_digit_at(const Bigint *n, int pos) {
    if (pos >= n->len) return 0;
    return n->digits[pos];
}


/*
 * bigint_add: suma dos Bigint y devuelve el resultado como un nuevo Bigint.
 *
 * Algoritmo (igual que sumar a mano en columnas, de derecha a izquierda):
 *   - En cada posición, sumamos digit_a + digit_b + carry
 *   - Si el resultado >= 10, escribimos resultado-10 y llevamos carry=1
 *   - Si no, escribimos el resultado y carry=0
 *   - Al terminar, si queda carry, agregamos un dígito más
 *
 * Devuelve un nuevo Bigint (hay que liberarlo después), o NULL si hay error.
 */
Bigint *bigint_add(const Bigint *a, const Bigint *b) {
    int max_len = bigint_max_len(a, b); /*hasta aca itero en los dos, si hay uno mas chico, bigint_digit_at este me genera 0 ahi para q no de error */

    /* El resultado puede tener como máximo max_len+1 dígitos (por el acarreo final) */
    if (max_len + 1 > MAX_DIGITS) {
        fprintf(stderr, "Error: resultado excede el maximo de digitos\n");
        return NULL;
    }

    /* Reservar memoria para el resultado */
    Bigint *result = malloc(sizeof(Bigint));
    if (result == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        return NULL;
    }

    result->digits = malloc(max_len + 2); /* +2: uno por el carry, otro de seguridad */
    if (result->digits == NULL) {
        fprintf(stderr, "Error: sin memoria\n");
        free(result);
        return NULL;
    }
    result->sign = 1;

    int carry = 0; /* el "me llevo" de la suma */
    int i;

    for (i = 0; i < max_len; i++) {
        int sum = bigint_digit_at(a, i) + bigint_digit_at(b, i) + carry;
        result->digits[i] = sum % 10; /* dígito que queda */
        carry = sum / 10;             /* 0 o 1 (el "me llevo") */
    }

    /* Si quedó un acarreo al final, es un dígito más */
    if (carry > 0) {
        result->digits[i] = carry;
        result->len = max_len + 1;
    } else {
        result->len = max_len;
    }

    return result;
}


/*
 * FUNCIÓN PRINCIPAL
 *
 * argc = cantidad de argumentos (incluyendo el nombre del programa)
 * argv = array de strings con los argumentos
 *
 * Ejemplo: "ent sumar 10 20 30"
 *   argc = 5
 *   argv[0] = "ent"
 *   argv[1] = "sumar"
 *   argv[2] = "10"
 *   argv[3] = "20"
 *   argv[4] = "30"
 */
int main(int argc, char *argv[]) {

    /* 1. Validar cantidad de argumentos */
    /* Necesitamos al menos: ent sumar N1 N2 → 4 argumentos */
    if (argc < 2) { /*Primero verificás que argv[1] existe (argc < 2), asi dsp puedo verificar lo de sumar, pq sino existe y verifico sumar , crash*/
        fprintf(stderr, "Error: uso: ent sumar N1 N2 [N3 ...]\n");
        return 1;
    }

    /* 2. Validar que la operación sea "sumar" */
    if (strcmp(argv[1], "sumar") != 0) {
        fprintf(stderr, "Error: operacion no soportada '%s'. Use 'sumar'.\n", argv[1]);
        return 1;
    }

    /* Ahora que sabemos que es "sumar", verificamos que haya al menos dos operandos */
    if (argc < 4) {
        fprintf(stderr, "Error: 'sumar' requiere al menos dos operandos\n");
        return 1;
    }

    /* Validar todos primero antes de crear ningún Bigint */
    for (int i = 2; i < argc; i++) {
        if (!is_decimal_string(argv[i])) {
            fprintf(stderr, "Error: operando invalido '%s'\n", argv[i]);
            return 1;
        }
    }

    /* Crear el primer Bigint */
    Bigint *acumulador = bigint_new(argv[2]); /*argv[2] es el primer número. Lo convertís a Bigint y lo guardás en acumulador. Si falla, salís.*/
    if (acumulador == NULL) return 1;

    /* Sumar el resto de operandos de a uno */
    for (int i = 3; i < argc; i++) { /*Arranco en el segundo operando, el primero ya lo guarde , suma el acumulador mas el proximo, 
Para ./ent sumar 10 20 30: acumulador = 10
i=3: acumulador = 10 + 20 = 30
i=4: acumulador = 30 + 30 = 60 */
        Bigint *operando = bigint_new(argv[i]);
        if (operando == NULL) {
            bigint_free(acumulador);
            return 1;
        }

        Bigint *nueva_suma = bigint_add(acumulador, operando);
        bigint_free(operando);  /* ya no necesitamos el operando */
        bigint_free(acumulador); /* ya no necesitamos el acumulador anterior */

        if (nueva_suma == NULL) return 1;
        acumulador = nueva_suma; /* el resultado es el nuevo acumulador */
    }

    /* 3. Imprimir resultado en stdout (solo el número, nada más) */
    bigint_print(acumulador);

    /* 4. Liberar memoria */
    bigint_free(acumulador);

    return 0;
}