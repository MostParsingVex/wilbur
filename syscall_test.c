#include"wilbur.h"

#define BANK 1

int main(){
  console_init();
  console_print( "Syscall test" );

  uint8_t buffer[ 10240 ];

  char filename[] = "99bugs,P,R";//"hello_wilbur,P,R";
  uint8_t filenamelen = 10;//16;
  uint8_t filenamelo = (uint16_t)filename;
  uint8_t filenamehi = (uint16_t)filename >> 8;
  asm("JSR $FFBD" :: "a"(filenamelen), "x"(filenamelo), "y"(filenamehi));

  cbm_k_setlfs(2, 8, 2);

  cbm_k_open();
  cbm_k_chkin(2);
  uint8_t*ptr = buffer;
  for( ; *ptr = cbm_k_getin( ), cbm_k_readst( ) == 0; ptr++ );
  cbm_k_close(2);
  cbm_k_clrch();
  if( ptr == buffer ) {
    PANIC("Bad command or file name");
  }  

  ram_init();

  syscall_init();

  for(int i=0;i<10240;i++){
    write_ram( BANK, 0x1bff + i, buffer[ i ] );
  }

  // patch out call to CHROUT
  write_ram( BANK, 0x1c25, 0xea );
  write_ram( BANK, 0x1c26, 0xea );
  write_ram( BANK, 0x1c27, 0xea );

  init();

  PANIC("");
}
