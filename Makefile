CFLAGS = -Wall -Werror

mishell: mishell.o strvec.o
	gcc $(CFLAGS) -o mishell mishell.o strvec.o  -g

mishell.o: mishell.c strvec.c
	gcc $(CFLAGS) -c mishell.c -g

strvec: strvec.o
	gcc $(CFLAGS) -c strvec.c -g