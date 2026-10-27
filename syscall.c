#include"syscall.h"
#include"wilbur.h"

void syscall_processing(){
//console_print("caught syscall");

  uint8_t hw_stack_ptr = read_ram( 1, 0xf900 );
  uint16_t program_counter = read_ram( 1, 0x100 | hw_stack_ptr + 1 ) | (uint16_t) read_ram( 1, 0x100 | hw_stack_ptr + 2 ) << 8;
  uint8_t syscall_number = read_ram( 1, program_counter - 1 );
  char buffer[ 100 ] = "CAUGHT SYSCALL ";
  buffer[ 15 ] = "0123456789ABCDEF"[syscall_number >> 4];
  buffer[ 16 ] = "0123456789ABCDEF"[syscall_number & 0xf];
  console_print( buffer );

  for(;;);
}

void syscall_init(){
start_of_syscall_trampoline_asm_blob:
  asm volatile(
    "jmp skip_syscall_trampoline\n"
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
    "rti\n"
    "skip_syscall_trampoline:\n"
  ); // no constraints
end_of_syscall_trampoline_asm_blob:

  memcpy( (uint8_t*)0xf800, &&start_of_syscall_trampoline_asm_blob + 3, &&end_of_syscall_trampoline_asm_blob - &&start_of_syscall_trampoline_asm_blob - 3 );  

  // syscall dispatch
//*(uint16_t*)0xf80e = (uint16_t)syscall_processing;
//*(uint16_t*)0xf80d = (uint16_t)syscall_processing;
  *(uint16_t*)0xf81f = (uint16_t)syscall_processing;

  // interrupt vector
  *(uint8_t*)0xfffe = 0x00;
  *(uint8_t*)0xffff = 0xf8;

  console_print("Installed syscall trampoline");
}

void init(){
  // put the hardware stack at 0
  write_ram( 1, 0xf900, 0 );

  // cpu flags
  write_ram( 1, 0x0100, 0x24 );

  // start address
  write_ram( 1, 0x0101, 0x18 );
  write_ram( 1, 0x0102, 0x1c );

  asm volatile(
    "jmp $f821\n"
    ::: "a","x","y","c","v"
  );
}
