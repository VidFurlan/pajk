#include <Arduino.h>
#include <SD.h>
#include <Servo.h>
#include <TFT.h>
#include <SPI.h>

#define PIN_SPI_CS 4

#define TFT_CS          10
#define TFT_DC          8
#define TFT_RST         9
#define TFT_MOSI        11
#define TFT_SCLK        13

#define DISP_W          128
#define DISP_H          160

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

File bmpFile;
File gcodeFile;

//Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, TFT_CS, TFT_DC, TFT_RST);
TFT TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

const float LEN = 7; // Length of link in cm
const float DIST = 4; // Distance between origins in cm
float x, y, dl, dr, hl, hr;
float pl1, pl2, pr1, pr2;
int tl, tr;

Servo ml, mr;

struct MoveData{
	float x;
	float y;
};

float angle_left(float x, float y) {
  
  dl = sqrt((x * x) + (y * y)); // Distance between origin left and (x,y) point
  hl = sqrt((LEN * LEN) - ((dl / 2) * (dl / 2))); // Height of the triangle
  
  pl1 = atan2(hl, dl / 2); // Phi 1, angle between d and axis
  pl2 = atan2(y, x); // Phi 2, angle between d and l 
  
  tl = (pl1 + pl2) * 180 / PI; // Angle between axis and l

  return tl;
}

/// @brief Function to calculate the angle of the right motor
/// @return Returns the angle of the right motor 
float angle_right(float x, float y) {
  x -= DIST;

  
  dr = sqrt((x * x) + (y * y)); // Distance between origin right and (x,y) point
  hr = sqrt((LEN * LEN) - ((dr / 2) * (dr / 2))); // Height of the triangle

  pr1 = atan2(hr, dr / 2); // Phi 1, angle between d and axis
  pr2 = atan2(y, x); // Phi 2, angle between d and l 

  tr = (pr2 - pr1) * 180 / PI;

  return tr;
}

void setup_servos(){
  ml.attach(7);
  mr.attach(2);
  Serial.begin(115200);

  ml.write(90);
  mr.write(90);
  delay(1000);

  ml.write(angle_left(2, 5));
  mr.write(angle_left(2, 5));
}

void draw_image() {
    TFTscreen.background(BLACK);
    TFTscreen.fill(255, 255, 255);
    
	while(bmpFile.available()){
        uint16_t coords[2];
        bmpFile.read(&coords, sizeof(coords));
          
        TFTscreen.drawPixel(coords[0], coords[1], 255);
    }
}

void setup_file_stream(){
	if(!SD.begin(PIN_SPI_CS)){
		Serial.println("Failed to open SD card");
	}

	bmpFile = SD.open("/test/test.bmp", FILE_READ); //this one is opened just fine 
	gcodeFile = SD.open("/test/test.pajk", FILE_READ);//file is there but can't be opened

	if(!(bmpFile.available() && gcodeFile.available())){
		Serial.println("Failed to open some of files on the SD card, try reuploading them");
	}
	Serial.flush();//cause it won't print otherwise look i dont fucking know

// Serial.println(gcodeFile.available());
}

void setup_display(){
    TFTscreen.begin();
}

void setup(){
	Serial.begin(115200);

	setup_servos();
	setup_file_stream();
	setup_display();
	draw_image();

}

void stream_move_data(){
	if(!gcodeFile.available()){
		return;
	}

	MoveData dat;
	gcodeFile.read(&dat, sizeof(MoveData));

	Serial.println(dat.x, dat.y);


//	ml.write(angle_left(dat.x, dat.y));
//	mr.write(angle_right(dat.x, dat.y));

	delay(50);
}

void loop(){
	stream_move_data();
}