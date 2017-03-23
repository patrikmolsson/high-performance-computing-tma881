#include <stdio.h>

void main() {
  long maxCap = 1000000000;
  long sum;

  sum = maxCap*(maxCap+1)/2;

  printf("%li", sum);
}
