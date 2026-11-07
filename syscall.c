#include"syscall.h"
#include"wilbur.h"

#define BANK 2

void syscall_processing(){
  uint8_t hw_stack_ptr = read_ram( BANK, 0xf900 );
  uint16_t program_counter = read_ram( BANK, 0x100 | hw_stack_ptr + 2 ) | (uint16_t) read_ram( BANK, 0x100 | hw_stack_ptr + 3 ) << 8;
  uint8_t syscall_number = read_ram( BANK, program_counter - 1 );
#if 0
  char buffer[ 100 ] = "CAUGHT SYSCALL ";
  buffer[ 15 ] = "0123456789ABCDEF"[syscall_number >> 4];
  buffer[ 16 ] = "0123456789ABCDEF"[syscall_number & 0xf];
#else
  char buffer[ 100 ];
  sprintf( buffer, "Caught syscall 0x%02x", syscall_number );
#endif
  console_print( buffer );

  switch( syscall_number ) {
    // exit
    case 0x02:
      {
        uint16_t retval = read_ram( BANK, 0x000a ) | (uint16_t)read_ram( BANK, 0x000b ) << 8;
        char buffer[ 100 ];
        sprintf( buffer, "exit() argument 0x%02x", retval );
        console_print( buffer );
        PANIC("called exit()" );
      }
    // write
    case 0x10:
      {
        uint16_t len  = read_ram( BANK, 0x000e ) | (uint16_t)read_ram( BANK, 0x000f ) << 8;
        uint16_t addr = read_ram( BANK, 0x000c ) | (uint16_t)read_ram( BANK, 0x000d ) << 8;
        for( int i = 0; i < len; i++ ) {
          int temp = read_ram( BANK, addr + i );
          console_putchar( temp );
        }
      }
      break;
    default:
      console_print( "Unknown syscall" );
      break;
  }
}

void syscall_init(){
start_of_syscall_trampoline_asm_blob:
  asm volatile(
    "jmp skip_syscall_trampoline\n"
    "pla\n"
    "pha\n"
    "and #$10\n"
    "beq leave_isr\n"

    "tsx\n"
    "txa\n"
    "sta $f900\n" // stack hardware stack pointer to 0xf900
    "lda #$3e\n" // switch to kernel mode
    "sta $ff00\n"
    "lda #$f0\n" // zero page in bank 0
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f0\n" // hw stack in bank 0
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
    "jsr $ffff\n"
    "lda #$f1\n" // zero page in bank n
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f1\n" //hw stack in bank n
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
    "lda #$7f\n" // switch to user mode
    "sta $ff00\n"
    "lda $f900\n"
    "tax\n"
    "txs\n" // restore hardware stack pointer
    "leave_isr:\n"
    "rti\n"
    "skip_syscall_trampoline:\n"
  ); // no constraints
end_of_syscall_trampoline_asm_blob:

  memcpy( (uint8_t*)0xf800, &&start_of_syscall_trampoline_asm_blob + 3, &&end_of_syscall_trampoline_asm_blob - &&start_of_syscall_trampoline_asm_blob - 3 );  

  // syscall dispatch
  *(uint16_t*)0xf825 = (uint16_t)syscall_processing;

  // interrupt vector
  *(uint8_t*)0xfffe = 0x00;
  *(uint8_t*)0xffff = 0xf8;

  console_print("Installed syscall trampoline");
}

void init(){
  // put the hardware stack at 0
  write_ram( BANK, 0xf900, 0 );

  // cpu flags
  write_ram( BANK, 0x0101, 0x20/*0x24*/ );

  // start address
  write_ram( BANK, 0x0102, 0x18 );
  write_ram( BANK, 0x0103, 0x1c );

#ifdef C256
  // jump to desired bank
  *(volatile uint8_t*)0xf83c = BANK << 6 | 0x3f;
#endif

  asm volatile(
    "jmp $f827\n"
    ::: "a","x","y","c","v"
  );
}
