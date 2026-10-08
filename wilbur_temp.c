#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include"cbm.h"

int main(int argc, char**argv, char**envp){
  asm(
    "jmp skip_bunch_of_stuff\n"
    ".octa 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n"
    ".octa 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n"
    "skip_bunch_of_stuff:"
  );

  *(int**)0x316 = (int*)&&brk_handler;

//asm("brk");

  char filename[] = "hello_wilbur,P,R";
  uint8_t filenamelen = 16;
  uint8_t filenamelo = (uint16_t)filename;
  uint8_t filenamehi = (uint16_t)filename >> 8;
  asm("JSR $FFBD" :: "a"(filenamelen), "x"(filenamelo), "y"(filenamehi));

  cbm_k_setlfs(2, 8, 2);

  cbm_k_open();
  cbm_k_chkin(2);
  uint8_t*ptr = (uint8_t*)0x1bff;
  for( ; *ptr = cbm_k_getin( ), cbm_k_readst( ) == 0; ptr++ );
  cbm_k_close(2);
  cbm_k_clrch();
  if( ptr == (uint8_t*)0x1bff ) {
    puts("Bad command or file name");
    for(;;);
  }

  asm( "jmp $1c0d" );

#if 0
brk_handler:
  unsigned char stack_ptr;
  asm volatile( "tsx\n" : "=x"(stack_ptr) );
  uint8_t* arg_ptr = (uint8_t*)(0x100 | (int)stack_ptr);
//printf("BRK handler %02x\n", **arg_ptr );
  printf("backtrace %d %04x %02x %02x %02x %02x %d\n", 42, (int)arg_ptr, (int)*arg_ptr, (int)arg_ptr[1], (int)arg_ptr[2], (int)arg_ptr[3], stack_ptr);
  for(;;);
#else
brk_handler:
  asm volatile(
    "cli\n"
    "ldx #$00\n"
    "label3:\n"
    "lda label1,x\n"
    "beq label2\n"
    "jsr $ffd2\n"
    "inx\n"
    "jmp label3\n"
    "label2:\n"
    "tsx\n"

    "txa\n"
    "clc\n"
    "adc #$06\n"
    "tax\n"

    "lda $100,x\n"
    "sec\n"
    "sbc #$01\n"
    "sta label6\n"
    "inx\n"
    "lda $100,x\n"
    "sta label7\n"

    ".byte 0xad\n"
    "label6:\n"
    ".byte 0x00\n"
    "label7:\n"
    ".byte 0x00\n"

    "tax\n"
    "and #$f0\n"
    "lsr\n"
    "lsr\n"
    "lsr\n"
    "lsr\n"
    "tay\n"
    "lda label4,y\n"
    "jsr $ffd2\n"
    "txa\n"
    "and #$0f\n"
    "tay\n"
    "lda label4,y\n"
    "jsr $ffd2\n"

    "lda #$d\n"
    "jsr $ffd2\n"

    "txa\n"
    "cmp #$10\n" // write()
    "beq label_write\n"
    "cmp #$02\n" // exit()
    "beq label_exit\n"
    "ldx #$0\n"
    "label_unk:\n"
    "lda label_unk_string,x\n"
    "beq label_continue\n"
    "jsr $ffd2\n"
    "inx\n"
    "jmp label_unk\n"
    "label_write:\n"
    "ldx #$0\n"
    "label_write2:\n"
    "lda label_write_string,x\n"
    "beq label_write_continue\n"
    "jsr $ffd2\n"
    "inx\n"
    "jmp label_write2\n"
    "label_write_continue:\n"
    "ldy #$0\n"
    "label_write3:\n"
    "cpy $e\n"
    "beq label_write_done\n"
    "lda ($c),y\n"
    "jsr $ffd2\n"
    "iny\n"
    "jmp label_write3\n"

    "label_write_done:\n"
    "lda #$d\n"
    "jsr $ffd2\n"
    "pla\n"
    "pla\n"
    "pla\n"
    "pla\n"
    "rti\n"

    "label_exit:\n"
    "ldx #$0\n"
    "label_exit2:\n"
    "lda label_exit_string,x\n"
    "beq label_exit_continue\n"
    "jsr $ffd2\n"
    "inx\n"
    "jmp label_exit2\n"

    "label_exit_continue:\n"
    "label_continue:\n" // temporary
    "label5:\n"
    "jmp label5\n"


    "label1:\n"
    ".asciz \"BRK handler \"\n"
    "label4:\n"
    ".ascii \"0123456789abcdef\"\n"
    "label_write_string:\n"
    ".asciz \"write() syscall\\r\"\n"
    "label_exit_string:\n"
    ".asciz \"exit() syscall\\r\"\n"
    "label_unk_string:\n"
    ".asciz \"unknown syscall\\r\"\n"
  );
#endif
}
