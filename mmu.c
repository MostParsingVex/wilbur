#include"wilbur.h"
#include"mmu.h"

void ram_init() {
  console_print( "MOS Technology 8722 MMU driver" );

  // disable interrupts
  asm volatile(
    "sei\n" // disable interrupts
    "lda #$3e\n"
    "sta $ff00\n" // bank 0, ram up to 0xd000
    "lda #$09\n"
    "sta $d506\n" // 4k of shared ram at 0xf000
    ::: "a"
  );

  start_of_asm_blob:

  asm volatile (
    "jmp skip_trampoline\n"
    "lda #$f1\n"  // jsr to 0xf001
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f1\n"
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
    "lda #$7f\n"
    "sta $ff00\n" // bank 1, turn off rom and io
    "lda $ffff\n" // put address at 0xf01b
    "sta $f000\n" // stash byte at 0xf000
    "lda #$3e\n"
    "sta $ff00\n" // bank 0 turn off rom but leave io
    "lda #$f0\n"
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f0\n"
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
    "rts\n"
    "lda #$f1\n"  // jsr to 0xf03a
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f1\n"
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"
    "lda #$7f\n"
    "sta $ff00\n"
    "lda $f000\n"
    "sta $ffff\n" // put address at 0xf057
    "lda #$3e\n"
    "sta $ff00\n"
    "lda #$f0\n"
    "sta $d508\n"
    "lda #$00\n"
    "sta $d507\n"
    "lda #$f0\n"
    "sta $d50a\n"
    "lda #$01\n"
    "sta $d509\n"

    "rts\n"
    "skip_trampoline:\n"
  ); // no constraints

  end_of_asm_block:
  memcpy((uint8_t*)0xf001, (uint8_t*)&&start_of_asm_blob + 3, (uint8_t*)&&end_of_asm_block - (uint8_t*)&&start_of_asm_blob - 3);
  console_print("Installed MMU trampolines");
}

uint8_t read_ram( uint8_t bank, uint16_t addr ) {
  *(volatile uint16_t*)0xf01b = addr;
  asm volatile( "jsr $f001\n" ::: "a","x","y","c","v" );
  return *(volatile uint8_t*)0xf000;
}

void write_ram( uint8_t bank, uint16_t addr, uint8_t val ) {
  *(volatile uint16_t*)0xf057 = addr;
  *(volatile uint8_t*)0xf000 = val;
  asm volatile( "jsr $f03a\n" ::: "a","x","y","c","v" );
}
