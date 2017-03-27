#include <stdio.h>

int main() {
  long maxCap = 1000000000;
  long sum = 0;

  // Use loop instead of sum = maxCap*(maxCap+1)/2
  // for more interesting analysis in the assignment
  for (long i = 1; i <= maxCap; i++){
    sum += i;
  }

  printf("%li \n", sum);
  return 0;
}
