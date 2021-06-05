#include"test.h"

void k(){
  int *x=(int*)malloc(8*sizeof(int));
  x[9]=0;
}

int main(){
  k();
  return 0;
}
