#include <stdlib.h>

int findel(char *elarr, size_t elnum, size_t elsize, char *elsrc) {
  int eqel;
  for (int i=0; i<elnum; i++) {
    eqel=0;
    for (int j=0; j<elsize; j++) {
      if (elsrc[j] == elarr[j]) {
        eqel++;
      }
    }
    if (eqel == elsize) {
      return i;
    }
  }
  return -1;
}
