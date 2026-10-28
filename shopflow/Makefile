# Makefile - compilation de ShopFlow (partie 1)
# Utilisation :  make        -> compile
#                make run    -> compile et lance
#                make clean  -> supprime les fichiers generes
CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra
OBJ    = main.o utils.o produit.o client.o initialisation.o

shopflow: $(OBJ)
	$(CC) $(CFLAGS) -o shopflow $(OBJ)

%.o: %.c types.h utils.h produit.h client.h initialisation.h
	$(CC) $(CFLAGS) -c $< -o $@

run: shopflow
	./shopflow

clean:
	rm -f $(OBJ) shopflow
