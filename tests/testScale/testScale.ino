#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
NAU7802 mainScale;

// APPROXIMATE, if it's fluctuating around 64000, just put 64000
double bottleMass = 65000; // run program, put the "weight" value here
double unitOunce = 11250; // fill bottle with known amount, get weight, subtract bottleMass, divide by known ounces.

void setup() {
    Wire.begin();
	Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    if (mainScale.begin() == false) {
		initializeScreen();
		display.clearDisplay();
		display.print("Scale Not Detected");
		display.display();
		while (1);
	}
    mainScale.setSampleRate(NAU7802_SPS_320);
	mainScale.calibrateAFE();
	mainScale.calculateZeroOffset(64);
}

void loop() {
	initializeScreen();
    display.println("Weight: " + String(mainScale.getReading() - mainScale.getZeroOffset()));
    display.println("Water: " + String(mainScale.getReading() - mainScale.getZeroOffset() - bottleMass));
    display.println("Weight oz: " + String((mainScale.getReading() - mainScale.getZeroOffset()) / unitOunce));
    display.println("Water oz: " + String((mainScale.getReading() - mainScale.getZeroOffset() - bottleMass) / unitOunce));
    display.display();
}

void initializeScreen() {
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
}
