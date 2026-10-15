CC=~/llvm-mos/bin/mos-c128-clang
CFLAGS=-Oz -flto -fnonreentrant -Wall

all: hello_wilbur wilbur_temp wilbur vdc_test mmu_test

hello_wilbur: hello_wilbur.c user.h
	$(CC) hello_wilbur.c -o hello_wilbur $(CFLAGS)

wilbur_temp: wilbur_temp.c user.h
	$(CC) wilbur_temp.c -o wilbur_temp $(CFLAGS)

wilbur: wilbur.c wilbur.h vdc.c vdc.h mmu.c
	$(CC) wilbur.c vdc.c mmu.c -o wilbur $(CFLAGS)

vdc_test: vdc_test.c vdc.c vdc.h
	$(CC) vdc_test.c vdc.c -o vdc_test $(CFLAGS)

mmu_test: mmu_test.c vdc.c vdc.h mmu.c
	$(CC) mmu_test.c mmu.c vdc.c -o mmu_test $(CFLAGS)

clean:
	$(RM) -f *.o hello_wilbur wilbur_temp wilbur vdc_test mmu_test
