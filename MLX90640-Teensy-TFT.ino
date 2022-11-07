//Thanks to Adafruit for their code at https://github.com/adafruit/Adafruit_MLX90640/blob/master/examples/MLX90640_arcadaCam/MLX90640_arcadaCam.ino
//Revised to make work with a ST7735 display 1.8" TFT and a Teensy 4.1 board

#define TFT_SCLK 13  // SCLK can also use pin 14
#define TFT_MOSI 11  // MOSI can also use pin 7
#define TFT_CS   10  // CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23
#define TFT_DC    9  //  but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22
#define TFT_RST   8  // RST can use any pin
#define SD_CS     4  // CS for SD card, can use any pin
#include <Adafruit_GFX.h>
#include <ST7735_t3.h>
#include <SPI.h>
#include <Adafruit_MLX90640.h>
ST7735_t3 disp = ST7735_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#define RGB(r,g,b) (b<<11|g<<6|r)

float pi = 3.1415926f;

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

//low range of the sensor (this will be blue on the screen)
int MINTEMP = 20;

//high range of the sensor (this will be red on the screen)
int MAXTEMP = 30;

//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};

uint16_t displayPixelWidth, displayPixelHeight;

void setup() {
  Serial.begin(115200);
  
  //pineMode(SD_CS, INPUT_PULLUP); //im not using this pin
  disp.initR(INITR_BLACKTAB);
  disp.fillScreen(RGB(0,0,0));
  disp.fillScreen(ST7735_BLACK);
  disp.setRotation(3);    //rotate display by 270 degrees
  disp.setTextWrap(true);
  displayPixelWidth = 5; //arcada.display->width() / 32;
  displayPixelHeight = 5; //arcada.display->width() / 32; //Keep pixels square 

  delay(100);

  Serial.println("Adafruit MLX90640 Camera");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT);
  mlx.setRefreshRate(MLX90640_8_HZ);
  Wire.setClock(1000000); // max 1 MHz
}

void loop() {
  int locx = 0; 
  int locy = 0;
  int hightemp = 0;
  int lowtemp = 100;
  uint32_t timestamp = millis();
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  }

  int colorTemp;
  for (uint8_t h=0; h<24; h++) {
    for (uint8_t w=0; w<32; w++) {
      float t = frame[h*32 + w];
      // Serial.print(t, 1); Serial.print(", ");
      if (t < lowtemp) {
        lowtemp = t;  //store the lowest temperature
      }
      if (t > hightemp) {
        hightemp = t; //store the high temperature
        locx = 31-w;   // save location of highest temp
        locy = h;
      }    
           
      uint8_t colorIndex = map(t, MINTEMP, MAXTEMP, 0, 255);
      
      colorIndex = constrain(colorIndex, 0, 255);
      //draw the pixels!
      disp.fillRect(displayPixelWidth * (31-w), displayPixelHeight * h,
                               displayPixelHeight, displayPixelWidth, 
                               camColors[colorIndex]); //have to transverse di
    }
  }
  Serial.print((millis()-timestamp) / 2); Serial.println(" ms per frame (2 frames per display)");
  Serial.print(2000.0 / (millis()-timestamp)); Serial.println(" FPS (2 frames per display)");

  disp.fillRect(0,120,160,8,0x0000);//draw rectangle over old values 
  disp.setCursor(0, 120);
  disp.setTextColor(0x07FF); //cyan
  disp.print("LOW TEMP: ");
  disp.println(lowtemp, DEC);
  disp.setTextColor(0xF800);
  disp.setCursor(72, 120);
  disp.print(" HIGH TEMP: ");
  disp.println(hightemp, DEC);

  //MAXTEMP = max(hightemp, 35); //keep noise in displayed colorization down when temps overall are low
  MAXTEMP = hightemp;
  MINTEMP = lowtemp; //MINTEMP and MAXTEMP define parameters for lookup tables colors
  disp.drawFastVLine((locx *5), (locy *5)-24, 48, ST7735_RED); //displays pixel location of highest temperature
  disp.drawFastHLine((locx *5)-24, (locy *5), 48, ST7735_RED);
  Serial.print("MAXTEMP: ");Serial.println(MAXTEMP, DEC);
  Serial.print("MINTEMP: ");Serial.println(MINTEMP, DEC);
  Serial.print("LocX: ");Serial.println(locx, DEC);
  Serial.print("LocY: ");Serial.println(locy, DEC); 
}
