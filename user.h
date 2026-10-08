#include<stdint.h>

void exit( int status ) {
#if 0
  asm volatile (
    "sta $0a\n" // clobber __rs1 (lsb)
    "stx $0b\n" // clobber __rs1 (msb)
    "brk\n" // exit syscall
    ".byte 0x02\n"
  );
#else
  ((void(*)(int))"\x85\x0a\x86\x0b\x00\x02")( status );
#endif
}

int write( int filedes, const char *buf, unsigned int nbyte ) {
#if 0 
  uint8_t retval_lo, retval_hi;
  asm volatile (
    "sta $0a\n" // clobber __rs1 (lsb)
    "stx $0b\n" // clobber __rs1 (msb)
    "brk\n" // write syscall
    ".byte 0x10\n"
    "lda $0a\n" // retrieve retval from __rs1 (lsb)
    "ldx $0b\n" // retrieve retval from __rs1 (msb)
    : "=a"(retval_lo), "=x"(retval_hi)
  );
  return retval_lo | retval_hi << 8;
#else
  return ((int(*)(int, const char *buf, unsigned int ))"\x85\x0a\x86\x0b\x00\x10\xa5\x0a\xa6\x0b\x60")( filedes, buf, nbyte );
#endif
}
