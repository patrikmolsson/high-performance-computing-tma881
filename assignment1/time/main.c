#include <stdio.h>

long sumNFirst(long maxCap){
  long sum = 0;
  for (long i = 0; i <= maxCap; i++){
    sum += i;
  }
  return sum;
}

int main() {
  long maxCap = 1000000000;
  long sum = sumNFirst(maxCap);

  //long sum = maxCap*(maxCap+1)/2;

  printf("%li \n", sum);
  return 0;
}
