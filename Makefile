FILE=proj2
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread -g
RM=rm -f

$(FILE) : $(FILE).c
		$(CC) $(CFLAGS) -o $(FILE) $(FILE).c

clean :
	$(RM) *.o $(FILE) 

zip:
	zip proj2.zip *.c Makefile