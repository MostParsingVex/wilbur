#include<stdio.h>
#include"user.h"
//#include<unistd.h>

int main(){
  for(int i=99;;){
    char buffer[100];
    uint16_t len;
    len = sprintf(buffer, "%d lines of bugs in the code. %d lines of bugs.\n", i, i);
    write( 1, buffer, len );
    len = sprintf(buffer, "Fix a bug, recompile the code. %d lines of bugs in the code.\n", ++i);
    write( 1, buffer, len );
  }
}
