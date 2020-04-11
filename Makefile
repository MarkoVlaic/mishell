CFLAGS = -Wall -Werror

mishell: mishell.o strvec.o job.o shell_state.o job.h strvec.h shell_state.h
	gcc $(CFLAGS) -o mishell mishell.o strvec.o job.o shell_state.o -g

mishell.o: mishell.c strvec.c
	gcc $(CFLAGS) -c mishell.c -g

strvec.o: strvec.c
	gcc $(CFLAGS) -c strvec.c -g

job.o: job.c
	gcc $(CFLAGS) -c job.c -g

shell_state.o: shell_state.c
	gcc $(CFLAGS) -c shell_state.c -g