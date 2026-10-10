CC=~/llvm-mos/bin/mos-c128-clang
CFLAGS=-Oz -flto -fnonreentrant -Wall

all: hello_wilbur wilbur_temp wilbur

hello_wilbur: hello_wilbur.c user.h
	$(CC) hello_wilbur.c -o hello_wilbur $(CFLAGS)

wilbur_temp: wilbur_temp.c user.h
	$(CC) wilbur_temp.c -o wilbur_temp $(CFLAGS)

wilbur: wilbur.c wilbur.h vdc.c vdc.h
	$(CC) wilbur.c vdc.c -o wilbur $(CFLAGS)

clean:
	$(RM) -f *.o hello_wilbur wilbur_temp wilbur
