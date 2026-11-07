#include"wilbur.h"
void sched_init(){
  console_print("Round robin scheduler");
}

void sched_start(){
  // put the hardware stack at 0
  write_ram( 1, 0xf900, 0 );

  // cpu flags
  write_ram( 1, 0x0101, 0x24 );

  // start address
  write_ram( 1, 0x0102, 0x18 );
  write_ram( 1, 0x0103, 0x1c );

#if 1
  // jump to desired bank
  *(volatile uint8_t*)0xf83c = 1 << 6 | 0x3f;
#endif

  // put the hardware stack at 0
  write_ram( 2, 0xf900, 0 );

  // cpu flags
  write_ram( 2, 0x0101, 0x24 );

  // start address
  write_ram( 2, 0x0102, 0x18 );
  write_ram( 2, 0x0103, 0x1c );

  asm volatile(
    "jmp $f827\n"
    ::: "a","x","y","c","v"
  );
}
