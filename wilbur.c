#include"wilbur.h"

void sycall_handler(){
  
}

int main(){
  console_init();
  console_print( "Wilbur kernel" );

  ram_init();

  uint8_t buffer[ 1024 ];

  char filename[] = "hello_wilbur,P,R";
  uint8_t filenamelen = 16;
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


  PANIC("");
}
