#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
#define defaultA attach(9, 1000, 2000) // change pin as needed

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo mainServo;

int pos = 90;
bool dir = true;

void setup() {
    Wire.begin();
	Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	mainServo.defaultA;
	mainServo.write(pos);
}

void loop() {
	initializeScreen();
    display.println("Pos: " + String(pos));
    if (dir) { display.println("Positive"); }
    if (!dir) { display.println("Negative"); }
    display.display();
	scuffedSweep();
}

void initializeScreen() {
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
}
void scuffedSweep() {
	if (pos == 180) dir = false;
	if (pos == 0) dir = true;
	if (dir) {
		pos += 1;
	}
	else {
		pos -= 1;
	}
	mainServo.write(pos);
	delay(10);
}