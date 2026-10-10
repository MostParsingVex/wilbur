/*
  MOS8563 VDC driver
*/

#include"vdc.h"
#include"wilbur.h"

uint8_t read_reg_vdc( uint8_t addr ) {
  *(volatile uint8_t*)0xd600 = addr;
  for( ;0x80 & ~*(volatile uint8_t*)0xd600; );
  return *(volatile uint8_t*)0xd601;
}

void write_reg_vdc( uint8_t addr, uint8_t val ) {
  *(volatile uint8_t*)0xd600 = addr;
  for( ;0x80 & ~*(volatile uint8_t*)0xd600; );
  *(volatile uint8_t*)0xd601 = val;
}

void console_init( ) {
  write_reg_vdc( 18, 0x00 );
  write_reg_vdc( 19, 0x00 );
  for( int i = 0; i < 2000; i++ ) {
    write_reg_vdc( 31, 0x20 );
  }
  write_reg_vdc( 18, 0x08 );
  write_reg_vdc( 19, 0x00 );
  for( int i = 0; i < 2000; i++ ) {
    write_reg_vdc( 31, 0x8e );
  }
  write_reg_vdc( 18, 0x00 );
  write_reg_vdc( 19, 0x00 );
#if 1
  char drivername[] = "MOS Technology 8563 VDC driver";
  for( int i = 0; i < strlen( drivername ); i++ ) {
    write_reg_vdc( 31, drivername[i] >= 'a' ? drivername[i] - 'a' + 1 : drivername[i] );
  }
  write_reg_vdc( 18, 0x00 );
  write_reg_vdc( 19, 0x50 );
#else
  for( int i = 0; i < 256; i++ ) {
    write_reg_vdc( 31, i );
  }
#endif
}

void console_print( const char *str ) {
  uint16_t addr = read_reg_vdc( 19 ) | read_reg_vdc( 18 ) << 8;
  if( addr > 2000 ) {
    // blit
    // dest
    write_reg_vdc( 18, 0x00 );
    write_reg_vdc( 19, 0x00 );
    // src
    write_reg_vdc( 32, 0x00 );
    write_reg_vdc( 33, 0x50 );
    uint8_t temp = 0x80 | read_reg_vdc( 24 );
    write_reg_vdc( 24, temp );
    for( int i = 0; i < 10; i++ ) {
      write_reg_vdc( 30, 192 );
    }
    write_reg_vdc( 18, 0x07 );
    write_reg_vdc( 19, 0x80 );
    temp = 0x7f & read_reg_vdc( 24 );
    write_reg_vdc( 24, temp );
    write_reg_vdc( 31, 0x20 );
    write_reg_vdc( 30, 80 );
  }
  for( int i = 0; i < strlen( str ); i++ ) {
    write_reg_vdc( 31, str[i] >= 'a' ? str[i] - 'a' + 1 : str[i] );
  }
  addr += 80;
  write_reg_vdc( 18, addr / 256 );
  write_reg_vdc( 19, addr % 256 );
}
