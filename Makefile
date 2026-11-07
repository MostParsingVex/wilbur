CC=~/llvm-mos/bin/mos-c128-clang
CFLAGS=-Oz -flto -fnonreentrant -Wall -DC256

all: hello_wilbur hello2 99bugs wilbur_temp wilbur vdc_test mmu_test syscall_test sched_test

hello_wilbur: hello_wilbur.c user.h
	$(CC) hello_wilbur.c -o hello_wilbur $(CFLAGS)

hello2: hello2.c user.h
	$(CC) hello2.c -o hello2 $(CFLAGS)

99bugs: 99bugs.c user.h
	$(CC) 99bugs.c -o 99bugs $(CFLAGS)

wilbur_temp: wilbur_temp.c user.h
	$(CC) wilbur_temp.c -o wilbur_temp $(CFLAGS)

wilbur: wilbur.c wilbur.h vdc.c vdc.h mmu.c syscall.c
	$(CC) wilbur.c vdc.c mmu.c -o wilbur $(CFLAGS)

vdc_test: vdc_test.c vdc.c vdc.h
	$(CC) vdc_test.c vdc.c -o vdc_test $(CFLAGS)

mmu_test: mmu_test.c vdc.c vdc.h mmu.c
	$(CC) mmu_test.c mmu.c vdc.c -o mmu_test $(CFLAGS)

syscall_test: syscall_test.c vdc.c vdc.h mmu.c syscall.c
	$(CC) syscall_test.c mmu.c vdc.c syscall.c -o syscall_test $(CFLAGS)

sched_test: sched_test.c sched.c vdc.c vdc.h mmu.c syscall.c
	$(CC) sched_test.c sched.c mmu.c vdc.c syscall.c -o sched_test $(CFLAGS)

clean:
	$(RM) -f *.o hello_wilbur hello2 99bugs wilbur_temp wilbur vdc_test mmu_test syscall_test sched_test
