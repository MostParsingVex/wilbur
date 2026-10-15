#include"wilbur.h"

int main(){
  console_init();
  console_print( "MMU test" );

  ram_init();

  for( int i=0; i<100; i++){
    write_ram( 1, 0x8000 + i, i);
  }
  for( int i=0; i<100; i++){
    if( i != read_ram( 1, 0x8000 + i) ) {
      PANIC("memory error");
    }
  }

  PANIC("");
}
