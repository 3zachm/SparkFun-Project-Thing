#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <TimeLib.h>
#include <Servo.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

// ----------------- SCREEN SETUP ----------------- //
#define SCREEN_WIDTH 128							// width of screen
#define SCREEN_HEIGHT 32 							// height of screen 
#define OLED_RESET 4								// reset pin # (-1 if sharing Arduino reset pin)
// -------------- LAZY VARIABLE USAGE ------------- //
#define rSec 60 - t									// inverse time... easily? idk im lazy
#define defaultA attach(9, 1000, 2000)				// change first parameter if using different pin #
// ------------- CONFIGURABLE OPTIONS ------------- //
const int timerMin = 1;								// start timer at X:00
const double bottleMass = 5500.0;					// empty bottle using test program
const double unitOunce = 13500.0;					// use another test program to find
const int weightMargin = 13500.0;					// minimum weight for detection
const double ounceMargin = 3.0;						// minimum fl oz to reset timer/alarm
// ------------ BITMAP IMAGE FOR LOGO ------------- //
const unsigned char PROGMEM helloWorld[] = {		// byte for byte logo construction
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0x1f, 0xf2, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfe, 0x40, 0x01, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfd, 0x27, 0xc9, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfb, 0xa7, 0xc6, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xf1, 0x4f, 0xef, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xf7, 0xbf, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xe6, 0x77, 0xef, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xe6, 0xef, 0xe7, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xe6, 0xef, 0xf7, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xce, 0xef, 0x53, 0x6f, 0xf3, 0xf8, 0xe7, 0x9e, 0x7c, 0xe4, 0xf3, 0x3c, 0x60, 0x3c, 0xf3, 0xcf, 
	0xce, 0xde, 0xbb, 0xaf, 0xe3, 0xe0, 0x67, 0x9c, 0x78, 0xe4, 0xf3, 0x1c, 0x60, 0x78, 0xf3, 0x8f, 
	0xce, 0xdd, 0xb1, 0x9f, 0xe3, 0xce, 0x67, 0x9c, 0x79, 0xe4, 0xf3, 0x18, 0xfc, 0x78, 0xf3, 0x9f, 
	0xce, 0x98, 0x75, 0x9f, 0xc9, 0xce, 0x67, 0x19, 0x39, 0xc4, 0xe3, 0x18, 0xf8, 0xf2, 0xf3, 0x9f, 
	0xce, 0x84, 0xf5, 0xbf, 0xc9, 0xce, 0x67, 0x39, 0x38, 0x0c, 0xe6, 0x10, 0xf9, 0xf6, 0x70, 0x1f, 
	0xce, 0x1e, 0xf5, 0x0b, 0x99, 0x8e, 0x67, 0x33, 0x38, 0x0c, 0xe6, 0x00, 0xf3, 0xe6, 0x70, 0x1f, 
	0xce, 0x5c, 0xf5, 0x25, 0x81, 0x98, 0x47, 0x30, 0x39, 0xc8, 0xe6, 0x44, 0xe3, 0xe0, 0x63, 0x1f, 
	0xce, 0xdf, 0xfc, 0xaf, 0x01, 0x88, 0x66, 0x20, 0x39, 0xcc, 0xc6, 0x4c, 0xc7, 0xc0, 0x67, 0x9f, 
	0xce, 0xe7, 0xf9, 0xbf, 0x38, 0xc0, 0xe0, 0x67, 0x11, 0xcc, 0x0e, 0x7c, 0xc0, 0x4e, 0x67, 0x1f, 
	0xcf, 0x7b, 0xfb, 0x6e, 0x3c, 0xe2, 0xf9, 0xc7, 0x93, 0xcf, 0x3e, 0x79, 0x80, 0x9e, 0x27, 0x3f, 
	0xcf, 0x77, 0xf4, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xcf, 0x6d, 0xf1, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xcf, 0x43, 0xcb, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xcf, 0x80, 0x82, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xe7, 0x90, 0x05, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xe7, 0xa7, 0xf9, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xf7, 0xb8, 0x06, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfb, 0x70, 0xe7, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfc, 0xf5, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
}; // https://javl.github.io/image2cpp/
// ---------- HARDWARE INITIALIZATION ------------- //
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo mainServo;									// servo constructor
NAU7802 mainScale;									// scale constructor
// ------ yea my names are terrible Im sorry ------ //
int minT = 0;										// minute counter
bool fRun = true;									// "first run" of the timer, set true to reset
bool fScale = true;									// "first run" but I was too lazy to move a code block
bool sixty = false;									// I don't remember why I needed this for the timer to work
bool tFin = false;									// timer finished
int pos = 90;										// set as the default position
bool dir = true;									// true = positive direction
double drank = 0;									// total drank
double tDrank = 0;									// temp drank 
double tWeight = 0;									// temp weight
double sDrank = 0;									// sum drank
// ------------------------------------------------ //

void setup() {
	// wire initialization
	Wire.begin();
	Wire.setClock(400000);

	// servo initailization. 
	mainServo.defaultA;
	mainServo.write(pos);
	delay(100);
	mainServo.detach();

	// logo routine
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	logoRoutine();
	if (mainScale.begin() == false) {
		initializeScreen();
		display.clearDisplay();
		display.print("Scale Not Detected");
		display.display();
		while (1);
	}
	// scale initialization
	mainScale.setSampleRate(NAU7802_SPS_320);
	mainScale.calibrateAFE();
	mainScale.calculateZeroOffset(64);
	// time
	setTime(0);
}

void loop() { 
	initializeScreen();
	display.setCursor(40, 10);
	if (!tFin) {
		// switch between oz statistics and timer
		if ((second() > 5 && second() < 13) || (second() > 35 && second() < 43)) {
			display.setCursor(0, 10);
			display.setTextSize(1);
			display.println(formatDouble(waterWeightOz(), 1) + "oz remaining");
			display.print(formatDouble((drank / unitOunce), 1) + "oz drank");
		}
		else {
			display.print(constructTime());
		}
		display.display();
		int tTime = second();
		// if the bottle is picked up, pause execution and wait for weight
		if (!weightDetection()) {
			display.clearDisplay();
			display.setCursor(40, 10);
			display.setTextSize(1);
			while (!weightDetection()) {
				display.drawLine(0, 0, display.width()-1, 0, SSD1306_WHITE);
				display.drawLine(0, display.height()-1, display.width()-1, display.height()-1, SSD1306_WHITE);
				display.setCursor(0, 10);
				display.print("Timer paused");
				setTime(tTime);
				display.display();
				delay(500);
			}
			delay(1500);
			// check if some water is drank or bottle is refilled (assume all water was drank and give credit)
			checkDrink();
			// if there's a change in weight that's not negative, store it for next use.
			if (tDrank > 0 || fScale) {
				tWeight = waterWeight();
				fScale = false;
			}
		}
		
	}
	else {
		tripAlarm();
		if (!weightDetection()) {
			while (!weightDetection()) {
				initializeScreen();
				display.print("Refill or Drink");
				display.display();
			}
			// wastes time but makes error in weight calculation less likely
			delay(2000);
			checkDrink();
			delay(2000);
		}
	}
}

void checkDrink() {
	tDrank = tWeight - waterWeight();
	if (tDrank >= (unitOunce/2)) {
		//testingDrank();
		drank += tDrank;
		sDrank += tDrank;
		displaytDrank();
		if (sDrank > (ounceMargin * unitOunce)) {
			fRun = true;
			fScale = true;
			sDrank = 0;
			tFin = false;
		}
	}
	else if (tDrank < -(unitOunce/2)) {
		drank += tWeight;
		fRun = true;
		fScale = true;
		sDrank = 0;
		tFin = false;
		refill();
		delay(2000);
	}
}

void refill() {
	initializeScreen();
	display.setTextSize(2);
	display.setCursor(13, 10);
	display.println("Refilled!");
	display.display();
}

void initializeScreen() {
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.drawLine(0, 0, display.width()-1, 0, SSD1306_WHITE);
	display.drawLine(0, display.height()-1, display.width()-1, display.height()-1, SSD1306_WHITE);
}

// probably the worst written part of this, but it works and I don't want to rewrite it
String constructTime() {
	if (sixty) {
		delay(1001);
		minT++;
		sixty = false;
	}
	int t = second();
	String separator = ":";
	String displaySec = String(rSec);
	String displayMin = String(timerMin - minT);
	if (fRun) {
		displayMin = timerMin;
		minT = 0;
		setTime(0);
		fRun = false;
	}
	if (rSec < 10) {
		separator += "0";
	}
	if (rSec == 60) {
		displaySec = "00";
		sixty = true;
	}
	if (rSec == 60 && minT == timerMin) {
		tFin = true;
		setTime(0);
	}
	return displayMin + separator + displaySec;
}

bool weightDetection() { 
	if (mainScale.getZeroOffset()*(-1) < mainScale.getReading()*(-1) - weightMargin) {
		return true;
	}
	return false;
}

void tripAlarm() {
	mainServo.defaultA;
	while (weightDetection()) {
		alarm();
	}
	// attach/detach to prevent cheaper servos from "vibrating" when the exact value can't be reached
	mainServo.detach();
}

void alarm() {
	display.clearDisplay();
	display.setCursor(40, 10);
	if (dir) {
		display.print("Drink");
	}
	else {
		display.print("    ");
		setTime(0);
	}
		display.display();
	if (pos == 180) dir = false;
	if (pos == 0) dir = true;
	if (dir) {
		pos += 15;
	}
	else {
		pos -= 15;
	}
	mainServo.write(pos);
	delay(50);
}

double waterWeightOz() {
	return (mainScale.getReading()*(-1) - mainScale.getZeroOffset()*(-1) - bottleMass) / unitOunce;
}

double waterWeight() {
	return (mainScale.getReading()*(-1) - mainScale.getZeroOffset()*(-1) - bottleMass);
}

String formatDouble(double val, int precision){
	String temp = String(val);
	return temp.substring(0, temp.indexOf('.') + precision + 1);
}

void displaytDrank() {
	initializeScreen();
	display.setCursor(0, 10);
	display.setTextSize(1);
	display.println(formatDouble((tDrank / unitOunce), 1) + "oz drank!");
	display.print(formatDouble((drank / unitOunce), 1) + "oz total");
	display.display();
	delay(3500);
}

void logoRoutine() {
	display.clearDisplay();
	for (int y = 64; y >= 0; y--) {
		display.clearDisplay();
		display.drawBitmap(0, y, helloWorld, 128, 64, SSD1306_WHITE);
		display.display();
	}
	delay(3000);
	display.display();
}