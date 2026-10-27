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
      PANIC("Write/read above 0x01ff failed");
    }
  }

  console_print("Write/read above 0x01ff passed");
#if 1
  for( int i=0; i<100; i++){
    write_ram( 1, 0x002 + i, i);
  }
  for( int i=0; i<100; i++){
    if( i != read_ram( 1, 0x002 + i) ) {
      PANIC("Zero page write/read failed");
    }
  }

  console_print("Zero page write/read passed");
#endif
  for( int i=0; i<100; i++){
    write_ram( 1, 0x100 + i, i);
  }
  for( int i=0; i<100; i++){
    if( i != read_ram( 1, 0x100 + i) ) {
      PANIC("HW stack write/read failed");
    }
  }

  console_print("HW stack write/read passed");

  for( int i=0; i<100; i++){
    write_ram( 1, 0x002 + i, i);
  }
  for( int i=0; i<100; i++){
    if( i != read_ram( 1, 0x002 + i) ) {
      PANIC("Zero page write/read failed");
    }
  }

  console_print("Zero page write/read passed");

  for( int i=0; i<1000; i++){
    write_ram( 1, 0x002 + i, i);
  }
  for( int i=0; i<1000; i++){
    if( (uint8_t)i != read_ram( 1, 0x002 + i) ) {
      PANIC("Zero page write/read failed");
    }
  }

  console_print("Zero page + HW stack + above 0x01ff write/read passed");

  PANIC("Memory tests passed");
}
