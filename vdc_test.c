#include"wilbur.h"

int main(){
  console_init();

  console_print("VDC test");

  for(int i=99;;){
    char buffer[100];
    sprintf(buffer, "%d lines of bugs in the code. %d lines of bugs.", i, i);
    console_print(buffer);
    sprintf(buffer, "Fix a bug, recompile the code. %d lines of bugs in the code.", ++i);
    console_print(buffer);
  }
}
