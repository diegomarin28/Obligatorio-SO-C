# Makefile para el proyecto ent
#
# Uso:
#   make          → compila todo y genera el ejecutable "ent"
#   make clean    → borra los archivos .o y el ejecutable

# ── Variables ──────────────────────────────────────────────
# CC: el compilador a usar
CC = gcc

# CFLAGS: opciones de compilación
#   -Wall    → activa la mayoría de los warnings (errores potenciales)
#   -Wextra  → activa warnings extra
#   -std=c11 → usamos el estándar C11
#   -g       → incluye información de debugging (útil para encontrar errores)
CFLAGS = -Wall -Wextra -std=c11 -g

# TARGET: nombre del ejecutable final
TARGET = ent

# OBJ: lista de archivos objeto (.o) que necesitamos
# Cada .c se compila a un .o, y después se unen para formar el ejecutable
OBJ = main.o bigint.o ops.o


# ── Regla principal ─────────────────────────────────────────
# "all" es el objetivo por defecto — lo que se construye cuando escribís "make"
all: $(TARGET)

# Para construir "ent", necesitamos todos los .o
# El comando los une (linkea) en un único ejecutable
$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)


# ── Reglas de compilación ───────────────────────────────────
# Cada .c se compila por separado a un .o
# La opción -c significa "compilar pero no linkear todavía"
#
# Importante: main.c depende de bigint.h
# Si cambia bigint.h, make sabe que tiene que recompilar main.o
main.o: main.c bigint.h
	$(CC) $(CFLAGS) -c main.c

bigint.o: bigint.c bigint.h
	$(CC) $(CFLAGS) -c bigint.c

ops.o: ops.c bigint.h
	$(CC) $(CFLAGS) -c ops.c


# ── Limpieza ────────────────────────────────────────────────
# "make clean" borra los archivos generados
# El -f en rm significa "no falles si el archivo no existe"
clean:
	rm -f $(OBJ) $(TARGET)