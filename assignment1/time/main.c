#include <stdio.h>

int main() {
  long maxCap = 1000000000;
  long sum;

  sum = maxCap*(maxCap+1)/2;

  printf("%li \n", sum);
  return 0;
}
