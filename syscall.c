#include"syscall.h"
#include"wilbur.h"

#define BANK 1

void syscall_processing(){
  // 0xf900 is common to all banks
  // this assumption may not be true on other platforms
  // e.g. 6509, 65816, 45gs02
//uint8_t hw_stack_ptr = read_ram( 0/*BANK*/, 0xf900 );
  uint8_t hw_stack_ptr = *(volatile uint8_t*)0xf900;
  uint8_t bank = *(volatile uint8_t*)0xf901;
  uint16_t program_counter = read_ram( BANK, 0x100 | hw_stack_ptr + 3 + 2 ) | (uint16_t) read_ram( BANK, 0x100 | hw_stack_ptr + 3 + 3 ) << 8;
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

void null_scheduler(){}

void syscall_init(){
start_of_syscall_trampoline_asm_blob:
  asm volatile(
    "jmp skip_syscall_trampoline\n"
    "pha\n"
    "txa\n"
    "pha\n"
    "tya\n"

    "pha\n"

    "tsx\n"
    "txa\n"
    "sta $f900\n" // pointer to hardware stack to 0xf900
    "inx\n"
    "inx\n"
    "inx\n"
    "inx\n"
    "lda $100,x\n"
    "and #$10\n"
    "beq handle_irq\n"

  //"tsx\n"
  //"txa\n"
  //"sta $f900\n" // pointer to hardware stack to 0xf900
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
    ".byte 0x20\n" // jsr
  );
syscall_processing_address_label:
  asm volatile (
    ".byte 0xff,0xff\n"
  );
after_syscall_processing:
  asm volatile (
"leave_isr:\n"
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
  //"leave_isr:\n"
    "pla\n"
    "tay\n"
    "pla\n"
    "tax\n"
    "pla\n"
    "rti\n"
    "handle_irq:\n"

    "tsx\n"
    "txa\n"
    "sta $f900\n" // pointer to hardware stack to 0xf900
    "lda #$3e\n" // switch to kernel mode
    "sta $ff00\n"
    "lda #$f1\n" // zero page in bank 0
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f1\n" // hw stack in bank 0
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
  //"lda $d01a\n"
  //"ora #$02\n"
  //"sta $d01a\n"
    "lda #$01\n"
    "sta $d019\n"
    "lda $dc0d\n" // clear CIA #1
    "sta $dc0d\n"
  //"lda $dd0d\n" // clear CIA #2
  //"sta $dd0d\n"
    "lda #$00\n"
    "beq leave_isr\n"

    "skip_syscall_trampoline:\n"
  ); // no constraints
end_of_syscall_trampoline_asm_blob:

  memcpy( (uint8_t*)0xf800, &&start_of_syscall_trampoline_asm_blob + 3, &&end_of_syscall_trampoline_asm_blob - &&start_of_syscall_trampoline_asm_blob - 3 );  

  // syscall dispatch
  *(volatile uint16_t*)(0xf800 + &&syscall_processing_address_label - &&start_of_syscall_trampoline_asm_blob - 3) = (uint16_t)syscall_processing;

  // interrupt vector
  *(volatile uint8_t*)0xfffe = 0x00;
  *(volatile uint8_t*)0xffff = 0xf8;

  console_print("Installed syscall trampoline");
}

void init(){
  // put the hardware stack at 0
  write_ram( BANK, 0xf900, 0 );

  // cpu flags
  write_ram( BANK, 0x0104, 0x20/*0x24*/ );

  // start address
  write_ram( BANK, 0x0105, 0x18 );
  write_ram( BANK, 0x0106, 0x1c );

#ifdef C256
  // jump to desired bank
  *(volatile uint8_t*)0xf846 = BANK << 6 | 0x3f;

  // set zero page
  *(volatile uint8_t*)0xf832 = BANK | 0xf0;

  // set hw stack
  *(volatile uint8_t*)0xf83c = BANK | 0xf0;
#endif

  asm volatile(
  //"jmp $f827\n"
    "jmp $f831\n"
    ::: "a","x","y","c","v"
  );
}
