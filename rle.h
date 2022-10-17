#include <stdlib.h>

char *RLECompress(char *dbuff, int dsize) {
  char *obuff;
  int obufflen = 1;
  for (int i=0; i<dsize; i++) {
    if (dbuff[i+1] == dbuff[i]) {
      for (int rle=0; rle<256; rle++) {
        if (dbuff[i+rle+1] != dbuff[i]) {
          obuff = (char*)realloc(obuff,obufflen+2);
          obuff[obufflen-1] = dbuff[i];
          obuff[obufflen] = rle;
          obufflen += 2;
        }
      }
    }
  }
  return obuff;
}

char *RLEDecompress(char *dbuff, int dsize) {
  //TODO
}
