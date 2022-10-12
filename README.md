# OpenPixel
A program for drawing images with your keyboard made with Raylib.

## UI
The UI is divided in 3 parts:
 1. The tile display;
 2. The palette display;
 3. The tileset display;
 
Each has a cursor that can be controlled in different ways:
 - With the keyboard (tile/tileset cursor)
 - With the mouse or keyboard (palette cursor)

The palette is made up of 256 colors (RGB332).

## Controls
These are all the keys you can use to control the cursors:
 - The arrow keys move the tile cursor (When holding the shift key they move the tileset cursor).
 - The K and L keys move the palette color left and right.
 - The G and H keys move the tileset cursor left and right.
 - The O, P, and R keys move the X, Y, or both positions of the tile cursor to a random value.
 
These commands are for drawing:
 - The Z key draws the selected color to the tile at the tile cursor.
 - The X key draws black to the tile at the tile cursor.
 - 
