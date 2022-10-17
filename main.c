#include "palette.h"
#include "raylib.h"
#include "find.h"
#include "rle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int px, py = 0;

#define MAXIMAGESIZE 256
#define IMAGEPIXELSIZE (256/IMAGESIZE)

char CURRENT_VERSION[] = {0x30,0x01}; //pm mM (Stored in big endian)
#define CURRENT_VERSION_SHORT 0x0130 //Mm mp

int macro[1000];
int macro_size;
bool recording;

int IMAGESIZE = 8;

unsigned char colorset[8];

unsigned char seltile = 0;

unsigned char selcolor = 0;

unsigned int move_amount = 1;
/*
unsigned char map[256][256];
unsigned char mapwx, mapwy, mapx, mapy;
*/
unsigned int tiles[257][MAXIMAGESIZE][MAXIMAGESIZE]; //Last image is clipboard image

bool mirrored_mode = false;

void handleKey(int key) {
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    switch (key) {
    case KEY_O:
      for (int x=0; x<MAXIMAGESIZE; x++) {
        for (int y=0; y<MAXIMAGESIZE; y++) {
          if (!tiles[seltile][y][x]) {
            tiles[seltile][y][x] = tiles[256][y][x];
          }
        }
      }
      break;
    case KEY_M:
      for (int x=0; x<MAXIMAGESIZE; x++) {
        for (int y=0; y<MAXIMAGESIZE; y++) {
          if (tiles[seltile][y][x]) {
            tiles[seltile][y][x] = tiles[256][y][x];
          }
        }
      }
      break;
    case KEY_D:
      for (int x=0; x<MAXIMAGESIZE; x++) {
        for (int y=0; y<MAXIMAGESIZE; y++) {
          tiles[seltile][y][x] = 0;
        }
      }
      break;
    case KEY_C:
      memcpy(tiles[256], tiles[seltile], MAXIMAGESIZE*MAXIMAGESIZE);
      break;
      case KEY_V:
      memcpy(tiles[seltile], tiles[256], MAXIMAGESIZE*MAXIMAGESIZE);
      break;
      case KEY_UP:
      seltile-=16;
      break;
    case KEY_DOWN:
      seltile+=16;
      break;
    case KEY_LEFT:
      seltile--;
      break;
    case KEY_RIGHT:
      seltile++;
      break;
    case KEY_R:;
      int old_color = tiles[seltile][py][px];
      int new_color = selcolor;
      for (int x=0; x<IMAGESIZE; x++) {
        for (int y=0; y<IMAGESIZE; y++) {
          if (tiles[seltile][y][x] == old_color) {
            printf("%d;%d;%d\n",y,x,tiles[seltile][y][x]);
            tiles[seltile][y][x] = new_color;
          }
        }
      }
      break;
  case KEY_ONE:
  case KEY_TWO:
  case KEY_THREE:
  case KEY_FOUR:
  case KEY_FIVE:
  case KEY_SIX:
  case KEY_SEVEN:
      case KEY_EIGHT:
    colorset[key-KEY_ONE] = selcolor;
    }
  } else {
  switch (key) {
  case KEY_UP:
    py-=move_amount; break;
  case KEY_DOWN:
    py+=move_amount; break;
  case KEY_LEFT:
    px-=move_amount; break;
  case KEY_RIGHT:
    px+=move_amount; break;
  case KEY_Z:
    tiles[seltile][py][px] = selcolor;
    break;
  case KEY_X:
    tiles[seltile][py][px] = 0;
    break;
  case KEY_M:
    mirrored_mode = !mirrored_mode;
    break;
  case KEY_Q:
    move_amount++;
    break;
  case KEY_W:
    move_amount--;
    break;
  case KEY_O:
    px = GetRandomValue(0,IMAGESIZE)-1;
    break;
  case KEY_R:
    px = GetRandomValue(0,IMAGESIZE)-1;
  case KEY_P:
    py = GetRandomValue(0,IMAGESIZE)-1;
    break;
  case KEY_N:
    recording = !recording;
    if (recording) macro_size=-1;
    printf("%d\n", macro_size);
    break;
  case KEY_B:
    selcolor = GetRandomValue(0, 255);
    break;
  case KEY_J:
    selcolor=(selcolor-1)%256;
    break;
  case KEY_K:
    selcolor=(selcolor+1)%256;
    break;
    
  case KEY_U: //Undo (TODO v0.14)
    break;
    
  case KEY_I: //Redo (TODO v0.14)
    break;
    
  case KEY_S:; //Save 
    FILE *fp = fopen("tileset.kops", "w");
    //printf("Compressing...\n");
    int dlen;
    //const char *cdata = CompressData(tiles, 257*MAXIMAGESIZE*MAXIMAGESIZE, &dlen);
    printf("Saving...\n");
    fwrite(&dlen, sizeof(int), 1, fp);
    fwrite(macro, 1000, 1, fp);
    fwrite(CURRENT_VERSION, 2, 1, fp);
    fwrite(tiles, sizeof(tiles), 1, fp);
    printf("Done! (File size is %ld bytes)\n", ftell(fp));
    fclose(fp);
    //free(cdata);
    break;
    
  case KEY_L:; //Load
    FILE *fpa = fopen("tileset.kops", "r");
    int datalength;
    short verbuff;
    printf("Loading...\n");
    fread(&datalength, sizeof(int), 1, fpa);
    fread(macro, 1000, 1, fpa);
    fread(&verbuff, 2, 1, fpa);
    fread(tiles, sizeof(tiles), 1, fpa);
    if (verbuff != CURRENT_VERSION_SHORT) {
      printf("WARNING: Read version %04x is different than current version %d!", verbuff, CURRENT_VERSION_SHORT);
    }
    /*printf("Allocating buffers CDATABUFF... ");
    const char *cdatab;
    printf("VERBUFF... ");
    const short verb;
    printf("DLENG and CDLEN...\n");
    int dleng, cdlen;
    printf("Reading CDLEN... ");
    fread(&cdlen, sizeof(int), 1, fpa);
    printf("Macro... ");
    fread(macro, 1000, 1, fpa);
    printf("Version... ");
    fread(&verb, 2, 1, fpa);
    printf("CDATABUFF...\n");
    fread(cdatab, cdlen, 1, fpa);
    printf("DEBUG: CDLEN = %d, Version = %04x\n", cdlen, verb);
    switch (verb) {
      case CURRENT_VERSION_SHORT:
        printf("Version is OK\n");
        break;
    default:
      printf("WARNING: Version %04x is different than current version %04x!\n", verb, CURRENT_VERSION_SHORT);
      break;
    }
    printf("Decompressing...\n");
    //const char *dcdata = DecompressData(cdatab, ftell(fpa), &dleng);
    printf("Loading...\n");
    memcpy(tiles,cdatab,cdlen);*/
    printf("Done!\n");
    break;
  
  case KEY_E: //Eyedropeer
    selcolor = tiles[seltile][py][px];
    break;
  
  case KEY_KP_ADD: //Shrink sprite
    IMAGESIZE /= 2;
    IMAGESIZE = (IMAGESIZE<8) ? 8 : IMAGESIZE;
    break;
    
  case KEY_KP_SUBTRACT: //Grow sprite
    IMAGESIZE *= 2;
    IMAGESIZE = (IMAGESIZE>MAXIMAGESIZE) ? MAXIMAGESIZE : IMAGESIZE;
    break;
  case KEY_ONE:
  case KEY_TWO:
  case KEY_THREE:
  case KEY_FOUR:
  case KEY_FIVE:
  case KEY_SIX:
  case KEY_SEVEN:
    case KEY_EIGHT:
    selcolor = colorset[key-KEY_ONE];
    break;
  case KEY_SPACE:
    printf("Executing macro big %d keys\n", macro_size);
    for (int i=0; i<macro_size; i++) {
      handleKey(macro[i]);
    }
    break;
  
  case KEY_G:
    seltile--;
    break;
  case KEY_H:
    seltile++;
    break;
  }
  }
}

void update() {
  int key = GetKeyPressed();
  handleKey(key);
  px %= IMAGESIZE;
  py %= IMAGESIZE;
  px = (px<0)?IMAGESIZE-1:px;
  py = (py<0)?IMAGESIZE-1:py;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &
      (GetMouseX()>255)) {
      unsigned int mx = (GetMouseX()/16)-256;
      unsigned int my = (GetMouseY()/16)-16;
      selcolor = (mx+(my*16))-0x10;
      //printf("%02x,%02x,%02x\n",(unsigned)mx,(unsigned)my,selcolor);
  }
  if (recording & (key>0)) {
    macro[macro_size] = key;
    macro_size++;
    printf("Recorded macro step %d as key %d.\n", macro_size-1, macro[macro_size-1]);
  }
}

void draw() {
	ClearBackground(WHITE);
  if (mirrored_mode) {
    for (int x=0; x<IMAGESIZE; x++) {
      for (int y=0; y<IMAGESIZE; y++) {
        unsigned char color = tiles[seltile][y][x];
        DrawRectangle(x*(IMAGEPIXELSIZE/2), y*(IMAGEPIXELSIZE/2), IMAGEPIXELSIZE/2, IMAGEPIXELSIZE/2, palette[color]);
      }
    };
    for (int x=0; x<IMAGESIZE; x++) {
      for (int y=0; y<IMAGESIZE; y++) {
        unsigned char color = tiles[seltile][y][x];
        DrawRectangle(x*(IMAGEPIXELSIZE/2)+128, y*(IMAGEPIXELSIZE/2), IMAGEPIXELSIZE/2, IMAGEPIXELSIZE/2, palette[color]);
      }
    };
    for (int x=0; x<IMAGESIZE; x++) {
      for (int y=0; y<IMAGESIZE; y++) {
        unsigned char color = tiles[seltile][y][x];
        DrawRectangle(x*(IMAGEPIXELSIZE/2), y*(IMAGEPIXELSIZE/2)+128, IMAGEPIXELSIZE/2, IMAGEPIXELSIZE/2, palette[color]);
      }
    };
    for (int x=0; x<IMAGESIZE; x++) {
      for (int y=0; y<IMAGESIZE; y++) {
        unsigned char color = tiles[seltile][y][x];
        DrawRectangle(x*(IMAGEPIXELSIZE/2)+128, y*(IMAGEPIXELSIZE/2)+128, IMAGEPIXELSIZE/2, IMAGEPIXELSIZE/2, palette[color]);
      }
    };
  } else {
	  for (int x=0; x<IMAGESIZE; x++) {
      for (int y=0; y<IMAGESIZE; y++) {
        unsigned char color = tiles[seltile][y][x];
        DrawRectangle(x*IMAGEPIXELSIZE, y*IMAGEPIXELSIZE, IMAGEPIXELSIZE, IMAGEPIXELSIZE, palette[color]);
      }
    }
  }
  for (int x=0; x<16; x++) {
    for (int y=0; y<16; y++) {
      unsigned char color = x+(y*16);
      DrawRectangle(x*16+256, y*16, 16, 16, palette[color]);
    }
  }
  if (mirrored_mode) {
    DrawRectangleLines(px*IMAGEPIXELSIZE/2,py*IMAGEPIXELSIZE/2,IMAGEPIXELSIZE/2,IMAGEPIXELSIZE/2,WHITE);
  } else {
    DrawRectangleLines(px*IMAGEPIXELSIZE,py*IMAGEPIXELSIZE,IMAGEPIXELSIZE,IMAGEPIXELSIZE,WHITE);
  }
  
  int selcolorx = selcolor%16*16;
  int selcolory = selcolor/16*16;
  DrawRectangleLines(256+selcolorx,selcolory,16,16,WHITE);
  
  for (int tilex=0; tilex<16; tilex++) {
    for (int tiley=0; tiley<16; tiley++) {
      for (int x=0; x<8; x++) {
        for (int y=0; y<8; y++) {
          DrawRectangle((x*2)+512+(tilex*16), (y*2)+(tiley*16), 2, 2, palette[tiles[tilex+(tiley*16)][y][x]]);
        }
      }
    }
  }
  DrawRectangleLines(512+(seltile%16)*16, (seltile/16)*16, 16, 16, WHITE);
  for (int i=0; i<8; i++) {
    DrawRectangle(i*96, 256, 96, 64, palette[colorset[i]]);
  }
}

int main() {
	InitWindow(768,256+64,"KOPS v0.13");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		update();
		BeginDrawing();
		draw();
		EndDrawing();
	}
}
