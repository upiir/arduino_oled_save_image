// simple project using Arduino UNO and 128x64 OLED Display to display gauge
// created by upir, 2023
// youtube channel: https://www.youtube.com/upir_upir

// YOUTUBE VIDEO: https://youtu.be/Ft2pRMVm44E
// SOURCE FILES: https://github.com/upiir/arduino_oled_save_image
// WOKWI: https://wokwi.com/projects/371410215597990913

// Links from the video:
// 128x64 SSD1306 OLED Display: https://s.click.aliexpress.com/e/_DCKdvnh
// Arduino UNO: https://s.click.aliexpress.com/e/_AXDw1h
// Arduino breadboard prototyping shield: https://s.click.aliexpress.com/e/_ApbCwx
// Andonstar Microscope on Amazon: https://bit.ly/3BA7LEW 
// Andonstar Microscope on AliExpress: https://s.click.aliexpress.com/e/_DEYzpEb
// U8g2 screenshot function: https://github.com/olikraus/u8g2/wiki/u8g2reference#writebufferxbm
// Image2cpp (convert array to image): https://javl.github.io/image2cpp/
// Photopea (online graphics editor like Photoshop): https://www.photopea.com/
// Split files with PowerShell: https://stackoverflow.com/questions/1001776/how-can-i-split-a-text-file-using-powershell
// Processing application: https://processing.org/
// IrfanView: https://www.irfanview.com/


// Related videos with Arduino UNO and 128x64 OLED screen:
// U8g vs U8g2: https://youtu.be/K5e0lFRvZ2E
// Arduino OLED menu: https://youtu.be/HVHVkKt-ldc
// Arduino Parking Sensor - https://youtu.be/sEWw087KOj0
// Turbo pressure gauge with Arduino and OLED display - https://youtu.be/JXmw1xOlBdk
// Arduino Car Cluster with OLED Display - https://youtu.be/El5SJelwV_0
// Knob over OLED Display - https://youtu.be/SmbcNx7tbX8
// Arduino + OLED = 3D ? - https://youtu.be/kBAcaA7NAlA
// Arduino OLED Gauge - https://youtu.be/xI6dXTA02UQ
// Smaller & Faster Arduino - https://youtu.be/4GfPQoIRqW8



#include "U8g2lib.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);  // [full framebuffer, size = 1024 bytes], note that taking screenshow with u8g2 only works with fullscreen buffer

int progress = 0; // progress in degrees
int progress_inc = 2; // progress increment
char string_buffer[10];   // helper buffer for converting numbers into c-style strings
float adjusted_progress;  // adjusted progress for smoother animation

void setup() {
  u8g2.begin();  // begin u8g2
  Serial.begin(9600); // begin serial communication
}

// those two functions (clamp and smoothstep) are used to add some easing to the animations of the main value
float clamp(float x, float lowerlimit = 0.0f, float upperlimit = 1.0f) {
  if (x < lowerlimit) return lowerlimit;
  if (x > upperlimit) return upperlimit;
  return x;
}

float smoothstep(float x, float edge0 = 0.0f, float edge1 = 1.0f) {
  // Scale, and clamp x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0));

  return x * x * (3.0f - 2.0f * x);
}



void loop() {

  u8g2.clearBuffer();                    // clear buffer for storing display content in RAM
  u8g2.setDrawColor(1);                  // set color to white
  u8g2.setFont(u8g2_font_profont11_tr);  // set font for small digits

  // draw two big semicircles for gauge outline
  u8g2.drawCircle(64, 60, 59, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  u8g2.drawCircle(64, 60, 57, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);


  int endx = round(-cos(radians(adjusted_progress)) * 54);
  int endy = round(-sin(radians(adjusted_progress)) * 54);

  int startx_off = round(-cos(radians(adjusted_progress + 90)) * 3);
  int starty_off = round(-sin(radians(adjusted_progress + 90)) * 3);


  // tickmarks with labels
  for (int i = 0; i <= 180; i = i + 45) {
    int tick_x1 = 64 + round(-cos(radians(i)) * 50);
    int tick_y1 = 60 + round(-sin(radians(i)) * 50);
    int tick_x2 = 64 + round(-cos(radians(i)) * 54);
    int tick_y2 = 60 + round(-sin(radians(i)) * 54);
    u8g2.drawLine(tick_x1, tick_y1, tick_x2, tick_y2); // draw tickmark

    int label_x = 64 + round(-cos(radians(i)) * 41);
    int label_y = 60 + round(-sin(radians(i)) * 41) + 4;
    itoa(round((i / 180.0) * 100), string_buffer, 10);  // convert integer to c style string
    int string_width = u8g2.getStrWidth(string_buffer);
    u8g2.drawStr(label_x - string_width / 2, label_y, string_buffer);
  }

  // draw big value
  u8g2.setFont(u8g2_font_profont22_tn);  // font for the big value
  itoa(round(adjusted_progress / 1.8), string_buffer, 10);  // convert integer to c style string
  int string_width = u8g2.getStrWidth(string_buffer);       // get string width
  u8g2.drawStr(64 - string_width / 2, 40, string_buffer);   // draw big value

  // small label
  u8g2.setFont(u8g2_font_profont10_tr);
  strcpy(string_buffer, "POWER");
  string_width = u8g2.getStrWidth(string_buffer);
  u8g2.drawStr(64 - string_width / 2, 48, string_buffer);

  // draw black triangle below the needle to cover the background
  u8g2.setDrawColor(0); // black color
  u8g2.drawTriangle(64 + startx_off * 1.8, 60 + starty_off * 1.8,
                    64 - startx_off * 1.8, 60 - starty_off * 1.8,
                    64 + endx * 1.05, 60 + endy * 1.05);
  u8g2.setDrawColor(1); // white color

  // draw the needle
  u8g2.drawLine(64 + startx_off, 60 + starty_off, 64 + endx, 60 + endy);
  u8g2.drawLine(64 - startx_off, 60 - starty_off, 64 + endx, 60 + endy);

  // draw the center piece of the needle - cirle
  u8g2.drawDisc(64, 60, 8);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(64, 60, 7);

  u8g2.sendBuffer();  // send buffer from RAM to display controller


  // uncomment the line below to send the buffer content over the serial port
  // note that it will slow down rendering a lot
  // but you can use this to produce PNG images from the OLED display
  //u8g2.writeBufferXBM(Serial);      // Write XBM image to serial out


  // increment/decrement the progress value
  progress = progress + progress_inc;
  if (progress >= 180 && progress_inc > 0) {
    progress = 180;
    progress_inc = progress_inc * -1;
  } else if (progress <= 0 && progress_inc < 0) {
    progress = 0;
    progress_inc = progress_inc * -1;
  }

  // adjust the progress with smoothstep function to ease the animation
  adjusted_progress = (progress / 180.0) * 100.0;
  adjusted_progress = smoothstep(adjusted_progress, 0, 100) * 180;
}
