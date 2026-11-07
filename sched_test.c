#include"wilbur.h"

int main(){
  console_init();
  console_print( "Sched test" );

  uint8_t buffer[ 10240 ];
  uint8_t buffer2[10240 ];

  char filename[] = "hello2,P,R";
  uint8_t filenamelen = 10;
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

  char filename2[] = "99bugs,P,R";
  filenamelen = 10;
  filenamelo = (uint16_t)filename2;
  filenamehi = (uint16_t)filename2 >> 8;
  asm("JSR $FFBD" :: "a"(filenamelen), "x"(filenamelo), "y"(filenamehi));

  cbm_k_setlfs(2, 8, 2);

  cbm_k_open();
  cbm_k_chkin(2);
  ptr = buffer2;
  for( ; *ptr = cbm_k_getin( ), cbm_k_readst( ) == 0; ptr++ );
  cbm_k_close(2);
  cbm_k_clrch();
  if( ptr == buffer2 ) {
    PANIC("Bad command or file name");
  }

  ram_init();

  syscall_init();

  for(int i=0;i<10240;i++){
    write_ram( 1, 0x1bff + i, buffer[ i ] );
  }

  // patch out call to CHROUT
  write_ram( 1, 0x1c25, 0xea );
  write_ram( 1, 0x1c26, 0xea );
  write_ram( 1, 0x1c27, 0xea );

  for(int i=0;i<10240;i++){
    write_ram( 2, 0x1bff + i, buffer2[ i ] );
  }

  // patch out call to CHROUT
  write_ram( 2, 0x1c25, 0xea );
  write_ram( 2, 0x1c26, 0xea );
  write_ram( 2, 0x1c27, 0xea );

  sched_init();

  PANIC("");
}
