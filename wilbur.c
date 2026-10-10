#include"wilbur.h"

int main(){
  console myconsole;
  console_init( &myconsole );
  console_print( &myconsole, "Wilbur kernel" );

  console_print( &myconsole, "" );
  console_print( &myconsole, "Kernel panic!" );
}
