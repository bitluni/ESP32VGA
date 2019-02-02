/*
	Author: bitluni 2019	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/

#include <Arduino.h>
#include "src/ESP32Lib/src/Graphics/I2SVGA.h"
#include <soc/rtc.h>
#include "src/ESP32Lib/src/Graphics/Mesh.h"
#include "src/ESP32Lib/src/Graphics/Sprites.h"

typedef unsigned short Color;

#include "gfx/font6x8.h"
#include "gfx/thinker.h"
//#include "gfx/venus.h"
Font<Graphics<Color>> font(6, 8, font6x8::pixels);
//Mesh<Graphics<Color>> model(venus::vertexCount, venus::vertices, 0, 0, venus::triangleCount, venus::triangles, venus::triangleNormals);
Mesh<Graphics<Color>> model2(thinker::vertexCount, thinker::vertices, 0, 0, thinker::triangleCount, thinker::triangles, thinker::triangleNormals);

//Note that the I/O GPIO pads are 0-19, 21-23, 25-27, 32-39, while the output GPIOs are 0-19, 21-23, 25-27, 32-33. GPIO pads 34-39 are input-only
//0, 2, 4, 5, 12,  13, 14, 15, 16,  17, 18, 19, 21,  22, 23, 27, 32, 33
//0 (boot mode: use only as blank output)
//5 (LED)
//21 (SDA), 22(SCL)
//25, 26 (DAC)
//1(TX), 3(RX)

//pin configuration, (you could use 32 and 33, for the syncs instead)
const int redPins[] = {2, 4, 12, 13, 14};
const int greenPins[] = {15, 16, 17, 18, 19};
const int bluePins[] = {21, 22, 23, 27};
const int hsyncPin = 0;
const int vsyncPin = 5;

//VGA Device (I2S0)
I2SVGA graphics(0);

void setup()
{
  Serial.begin(115200);
  //initializing i2s vga and frame buffers
  graphics.init(graphics.MODE200x150, redPins, greenPins, bluePins, hsyncPin, vsyncPin, 16);
  //select font
  graphics.setFont(font);
}

//render 3d model
void drawModel()
{
  static Matrix perspective = Matrix::translation(graphics.xres / 2, graphics.yres / 2, 0) * Matrix::scaling(100 * graphics.pixelAspect(), 100, 100) * Matrix::perspective(90, 1, 10);
  static float u = 0;
  u += 0.02;
  Matrix rotation = Matrix::rotation(-1.7, 1, 0, 0) * Matrix::rotation(u, 0, 0, 1);
  Matrix m0 = perspective * Matrix::translation(0, 1.7 * 0, 7) * rotation * Matrix::scaling(7);
  model2.transform(m0, rotation);
  model2.drawTriangles(graphics, 31);
  graphics.flush();
}

//show output
void draw()
{
  static int lastMillis = 0;
  int t = millis();
  int fps = 1000 / (t - lastMillis);
  lastMillis = t;
  graphics.begin();
  graphics.clear(0);
  drawModel();
  graphics.setTextColor(0xffff);
  graphics.setCursor(0, 0);
  graphics.print("mem: ");
  graphics.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  graphics.print(" fps: ");
  graphics.print(fps, 10, 2);
  graphics.print(" tris/s: ");
  graphics.print(fps * model2.triangleCount);
  graphics.end();
}

//just draw
void loop()
{
  draw();
}
