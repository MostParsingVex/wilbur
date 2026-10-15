#include<stdio.h>
#include<stdint.h>
#include"cbm.h"

void console_init( );

void console_print( const char *str );

void ram_init();

uint8_t read_ram( uint8_t bank, uint16_t addr );

void write_ram( uint8_t bank, uint16_t addr, uint8_t val );

#define PANIC(x) console_print("Kernel panic!"); console_print(x); for(;;);
