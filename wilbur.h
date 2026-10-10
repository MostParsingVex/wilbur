#include<stdint.h>

struct console_type {
  uint8_t rows;
  uint8_t columns;
  uint8_t currentrow;
  uint8_t currentcolumn;
};

typedef struct console_type console;

void console_init( console *myconsole );

void console_print( console *myconsole, const char *str );
