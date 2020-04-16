#include <OneWire.h>
#include <DallasTemperature.h>
#include <MCUFRIEND_kbv.h>
#include <TimerOne.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <FreeDefaultFonts.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <TouchScreen.h>
#include "RTClib.h"

#include "Clock.h"

class MCUFRIEND_kbv tft;
//create clock object
RTC_DS3231 rtc;
//AquariumClock clock(10, 30, &FreeMono24pt7b);
Clock* aquariumClock;
Clock* pageTwoClock;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

//===========================Color variables===================================================================
#define BLACK   	0x0000
#define BLUE    	0x001F
#define RED     	0xF800
#define GREEN   	0x07E0
#define CYAN    	0x07FF
#define MAGENTA 	0xF81F
#define YELLOW  	0xFFE0
#define WHITE 		0xFFFF
#define DARKBLUE 	0x0010
#define VIOLET 		0x8888
#define GRAY    	0x8410
//#define GREY   tft.color565(64, 64, 64);
#define GOLD 0xFEA0
#define BROWN 0xA145
#define SILVER 0xC618
#define LIME 0x07E0
//===========================LOAD BAR VARIABLES===============================================================
#define REDBAR_MINX 80
#define GREENBAR_MINX 130
#define BLUEBAR_MINX 180
#define BAR_MINY 30
#define BAR_HEIGHT 250
#define BAR_WIDTH 30
//temp. sensor data pin and Initialization
#define ONE_WIRE_BUS 53
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float water_temp = 0;
float last_water_temp = 20;

#define SHOW_COORDINATES_ON false  //change this to view touch point coordinates in serial monitor

int redpos = BAR_MINY + 12;
int greenpos = BAR_MINY + 12;
int bluepos = BAR_MINY + 12;
int oldrpos = redpos;
int oldgpos = greenpos;
int oldbpos = bluepos;

int currentpage;
int lastPercent = 0;
int newPercent;

//clock variables
int previous_hours, previous_minutes, previous_seconds, y_offset = 0;
//===========================FUNCTION PROTOTYPES===============================================================

void page_0();
void page_1();
void page_2();
void drawOldBar(int);
void drawBar_1(int);
void draw_water_temp();
void draw_button_list(class Adafruit_GFX_Button *page_n_buttons[]);
bool update_button_list(class Adafruit_GFX_Button *home_btn[]);
void drawScale_1();
void drawScale_2();
void drawScale_3();
void drawScale_4();
void showmsgXY(int, int, int, const GFXfont *f, int, const char *msg);
void clock(int, int, const GFXfont *f);
void aquarium_timer();
void draw_button_state(int, int, int, int, int, int, int, char *msg);
void draw_output_state(int);

//===========================Delay variables======================================================================
unsigned long timer_seoconds = 0;
const long interval_1 = 10;
//loading bar delay variables
int period = 15;
int oneSecond = 1000;
int period_3 = 1000;
unsigned long time_now;

//===========================output pin variables====================================================================
int water_pump = 39, led_relay = 51, second_relay = 49, PH_controller = 47,
		plant_light = 45, third_relay = 43, third_220 = 41, fifth_relay = 37,
		fourth_relay = 35;

bool water_pump_state = false;
bool led_relay_state = false;
bool plant_light_state = true;
bool timer_state = false;
bool plant_light_on = false;
bool led_light_on = false;
bool led_driver_on = false;
bool timer_on = true;
//===========================Aquarium timer variables=============================================================

int plant_light_ontime = 10, plant_light_offtime = 17, led_ontime = 10,
		led_offtime = 21, led_driver_ontime;

int feed_timer = 0;
bool feed_timer_on = false;
//===========================Touch Screen constants, instance of touch screen=====================================

const int XP = 6, XM = A2, YP = A1, YM = 7;
const int TS_LEFT = 974, TS_RT = 184, TS_TOP = 195, TS_BOT = 911; // changed to landscape
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//===========================Button objects======================================================================

Adafruit_GFX_Button plant_light_on_btn, next_btn, feed_btn, start_btn, temp_btn,
		level_btn, back_btn, water_pump_on_btn, led_btn_on, timer_btn_on;

//===========================Setup Function=======================================================================
void setup(void) {
	time_now = millis();
	pinMode(led_relay, OUTPUT);
	pinMode(water_pump, OUTPUT);
	pinMode(PH_controller, OUTPUT);
	pinMode(plant_light, OUTPUT);

	pinMode(second_relay, OUTPUT);
	pinMode(third_relay, OUTPUT);
	pinMode(third_220, OUTPUT);
	pinMode(53, OUTPUT);
	pinMode(fifth_relay, OUTPUT);
	pinMode(fourth_relay, OUTPUT);

	digitalWrite(water_pump, LOW);
	digitalWrite(led_relay, led_relay_state);
	digitalWrite(plant_light, HIGH);
	digitalWrite(PH_controller, LOW);

	digitalWrite(second_relay, HIGH);
	digitalWrite(53, LOW);
	digitalWrite(third_relay, HIGH);
	digitalWrite(third_220, HIGH);
	digitalWrite(fifth_relay, HIGH);
	digitalWrite(fourth_relay, HIGH);

	//get time on startup
	DateTime now = rtc.now();
	rtc.begin();
	previous_hours = now.hour();
	previous_minutes = now.minute();
	previous_seconds = now.second();

	Serial.begin(9600);

	sensors.begin();					//start temperature sensor on setup
	uint16_t ID = tft.readID();
	Serial.print("TFT ID = 0x");
	Serial.println(ID, HEX);
	if (ID == 0xD3D3)
		ID = 0x9486; // write-only shield
	tft.begin(ID);
	tft.setRotation(1);            //landscape
	tft.fillScreen(BLACK);
	tft.setTextSize(3);
	tft.setTextColor(WHITE);
	tft.setCursor(50, 140);
	tft.print("Loading...");

	tft.setTextColor(tft.color565(255, 255, 0));
	tft.setCursor(30, 70);
	tft.print("By:");

	tft.setCursor(30, 100);
	tft.print("J. Davidsons");
//
//	 for (int i=0; i < 400; i++)
//	   {
//	     time_now = millis();
//	     while(millis() < time_now + period){
//	     //wait approx. [period] ms
//	     tft.fillRect(BAR_MINY - 10, BLUEBAR_MINX, i, 10, RED);
//	     }
//	   }
	Serial.print(tft.width());
	Serial.print(F(" x "));
	Serial.println(tft.height());

	page_0();
	//aquariumClock->adjustClock(2020, 04, 16, 14, 49, 0);		//This line sets the RTC (CLOCK MODULE) with an date & time
}

Adafruit_GFX_Button *page_0_btn[] = { &next_btn, &feed_btn, NULL };
Adafruit_GFX_Button *page_1_btn[] = { &next_btn, &back_btn, NULL };
Adafruit_GFX_Button *page_2_btn[] = { &water_pump_on_btn, &back_btn,
		&plant_light_on_btn, &next_btn, &led_btn_on, &timer_btn_on, NULL };

//===========================Loop Function=========================================================================
void loop(void) {

		if (previous_seconds != aquariumClock->getSecond()) {
			aquarium_timer(); //call timer function to check time for relays and turn them on or off
			timer_seoconds++;
			//sensors.requestTemperatures();
			//water_temp = sensors.getTempCByIndex(0);
			time_now = millis();
	}

	//===========================feed timer=========================
	if(feed_timer_on){

		if (previous_minutes != rtc.now().minute()) {
			feed_timer++;
		}

		if (feed_timer >= 30) {
			feed_timer_on = false;
			digitalWrite(water_pump, LOW);
			water_pump_state = true;
			draw_output_state(7);
		}
	}else{

	}
//===========================First page=========================
	if (currentpage == 0) {
		if (previous_seconds != aquariumClock->getSecond()/*timer_seoconds >= interval_1*/) {
			//clock(50, 0, &FreeMono24pt7b);
			aquariumClock->displayClock(1);
			previous_seconds = aquariumClock->getSecond();
		}

		update_button_list(page_0_btn);

		if (next_btn.justPressed()) {
			page_1();

		} else if (feed_btn.justPressed()) {
			if (!feed_timer_on) {
				feed_timer_on = true;
				feed_timer = 0;
				digitalWrite(water_pump, HIGH);
				water_pump_state = false;
				draw_output_state(7);
			} else {
				feed_timer_on = false;
				feed_timer = 0;
				digitalWrite(water_pump, LOW);
				water_pump_state = true;
				draw_output_state(7);
			}
		}
	}

//===========================Second page========================
	else if (currentpage == 1) {
		update_button_list(page_1_btn);

		if (water_temp != last_water_temp) {
			draw_water_temp();

		}
		if (back_btn.justPressed()) {
			previous_hours = 0;
			previous_minutes = 0;
			previous_seconds = 0;
			page_0();

		} else if (next_btn.justPressed()) {
			previous_hours = 0;
			previous_minutes = 0;
			previous_seconds = 0;
			page_2();
		}
	}
//===========================Second page========================
	else if (currentpage == 2) {

		if (timer_seoconds >= interval_1) {
			pageTwoClock->displayClock();
			//clock(0, 2, &FreeSerifBoldItalic9pt7b);
		}

		update_button_list(page_2_btn);

		if (back_btn.justPressed()) {
			page_1();
		} else if (water_pump_on_btn.justPressed()) {		//water pump button
			if (water_pump_state) {
				draw_output_state(1);
				digitalWrite(water_pump, LOW);
				water_pump_state = false;
			} else {
				draw_output_state(1);
				digitalWrite(water_pump, HIGH);
				water_pump_state = true;
			}
		} else if (plant_light_on_btn.justPressed()) {		//plant light button
			if (!timer_on) {
				if (plant_light_state) {
					digitalWrite(plant_light, HIGH);
					plant_light_state = false;
					draw_output_state(5);
				} else {
					digitalWrite(plant_light, LOW);
					plant_light_state = true;
					draw_output_state(5);
				}
			}
		} else if (led_btn_on.justPressed()) {				//led light button
			if (!timer_on) {
				if (led_relay_state) {
					digitalWrite(led_relay, HIGH);
					led_relay_state = false;
					draw_output_state(6);
				} else {
					digitalWrite(led_relay, LOW);
					led_relay_state = true;
					draw_output_state(6);
				}
			}
		} else if (timer_btn_on.justPressed()) {		//timer on/off button
			if (timer_on) {
				timer_on = false;
				draw_output_state(0);	//timer on/off
			} else {
				timer_on = true;
				draw_output_state(0);
			}
		} else if (next_btn.justPressed()) {		//next button
			previous_hours = 0;
			previous_minutes = 0;
			previous_seconds = 0;
			page_2();
		}
	}
}
//===========================Zero page========================
void page_0(void) {
	tft.fillScreen(BLACK);
	currentpage = 0;
	next_btn.initButton(&tft, 342, 220, 120, 40, WHITE, CYAN, BLACK, "NEXT", 2);
	feed_btn.initButton(&tft, 60, 220, 120, 40, WHITE, CYAN, BLACK, "FEED", 2);
	draw_button_list(page_0_btn);
	aquariumClock = new Clock(120, 100,&FreeMono24pt7b,&tft);
}

//===========================First page========================

void page_1(void) {

	tft.fillScreen(BLACK);
	currentpage = 1;

	back_btn.initButton(&tft, 60, 220, 120, 40, WHITE, CYAN, BLACK, "BACK", 2);
	next_btn.initButton(&tft, 342, 220, 120, 40, WHITE, CYAN, BLACK, "NEXT", 2);

	draw_button_list(page_1_btn);

	drawScale_1();
	drawScale_2();
	drawScale_3();
	drawScale_4();
}

//===========================Second page========================
void page_2(void) {

	tft.fillScreen(BLACK);
	currentpage = 2;
	water_pump_on_btn.initButton(&tft, 100, 20, 180, 40, WHITE, CYAN, BLACK,
			"PUMP", 2);
	plant_light_on_btn.initButton(&tft, 100, 70, 180, 40, WHITE, CYAN, BLACK,
			"PLANT LED", 2);
	led_btn_on.initButton(&tft, 100, 120, 180, 40, WHITE, CYAN, BLACK, "LED",
			2);
	timer_btn_on.initButton(&tft, 100, 170, 180, 40, WHITE, CYAN, BLACK,
			"TIMER", 2);
	next_btn.initButton(&tft, 300, 220, 180, 40, WHITE, CYAN, BLACK, "NEXT", 2);
	back_btn.initButton(&tft, 100, 220, 180, 40, WHITE, CYAN, BLACK, "BACK", 2);

	//Drawing position of the buttons on the screen
	draw_output_state(0);

	draw_button_list(page_2_btn);
	pageTwoClock = new Clock(320,20,&FreeSerifBoldItalic9pt7b,&tft);
}

//===========================Function returns,sets pressed X and Y coordinates=====================================

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void) {
	TSPoint p = ts.getPoint();
	pinMode(YP, OUTPUT);      //restore shared pins
	pinMode(XM, OUTPUT);
	digitalWrite(YP, HIGH);   //because TFT control pins
	digitalWrite(XM, HIGH);
	bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
	if (pressed) {
		pixel_x = map(p.y, TS_LEFT, TS_RT, 0, tft.width()); //changed to portrait
		pixel_y = map(p.x, TS_TOP, TS_BOT, 0, tft.height()); //changed to portrait
		if (SHOW_COORDINATES_ON) {
			Serial.print("X = ");
			Serial.print(pixel_x); //if set true, prints touch coordinates on Serial.print
			Serial.print("\tY = ");
			Serial.println(pixel_y); //if set true, prints touch coordinates on Serial.print
		}
	}
	return pressed;
}

/* update the state of a button and redraw as required
 *
 * main program can use isPressed(), justPressed() etc
 */
bool update_button(Adafruit_GFX_Button *b, bool down) {
	b->press(down && b->contains(pixel_x, pixel_y));
	if (b->justReleased())
		b->drawButton(false);
	if (b->justPressed())
		b->drawButton(true);
	return down;
}

/* most screens have different sets of buttons
 * life is easier if you process whole list in one go
 */
bool update_button_list(Adafruit_GFX_Button **pb) {
	bool down = Touch_getXY();
	for (int i = 0; pb[i] != NULL; i++) {
		update_button(pb[i], down);
	}
	return down;
}

void draw_button_list(Adafruit_GFX_Button **pb) {
	for (int i = 0; pb[i] != NULL; i++) {
		pb[i]->drawButton(false);
	}
}

//===============================Function to draw button state=====================================
void draw_button_state(int x, int y, int width, int height, int sz,
		int text_color, int box_color, char *msg) {
	tft.fillRect(x, y, width, height, box_color);
	tft.drawRect(x, y, width, height, WHITE);
	//tft.setFont(f);
	tft.setCursor(x + 22, y + 12);
	tft.setTextColor(BLACK);
	tft.setTextSize(sz);
	tft.print(msg);
	tft.setFont(NULL);
}

//===============================Function to draw characters on tft=====================================
void showmsgXY(int x, int y, int sz, const GFXfont *f, int color,
		const char *msg) {
	//int16_t x1, y1;
	//uint16_t wid, ht;
	//tft.drawFastHLine(0, y, tft.width(), WHITE);
	tft.setFont(f);
	tft.setCursor(x, y);
	tft.setTextColor(color);
	tft.setTextSize(sz);
	tft.print(msg);
	tft.setFont(NULL);
}

//===============================Function to draw numbers on tft=====================================
void showNumXY(int x, int y, int sz, const GFXfont *f, int color,
		const int number) {
	//int16_t x1, y1;
	//uint16_t wid, ht;
	//tft.drawFastHLine(0, y, tft.width(), WHITE);
	tft.setFont(f);
	tft.setCursor(x, y);
	tft.setTextColor(color);
	tft.setTextSize(sz);
	tft.print(number);
	tft.setFont(NULL);
}

//===========================Function to redraw previous bar =============================================
void drawOldBar(int oldPer) {
	tft.fillRect(128, 20 + (150 - oldPer), 10, oldPer, GREEN);
}

//========================================================================================================

//===========================FIRST SCALE===================================================================
void drawScale_1() {

	tft.drawFastVLine(50, 20, 150, WHITE);
	tft.drawFastHLine(42, 20, 8, WHITE);
	tft.drawFastHLine(45, 57.5, 5, WHITE);
	tft.drawFastHLine(42, 95, 8, WHITE);
	tft.drawFastHLine(45, 132.5, 5, WHITE);
	tft.drawFastHLine(42, 170, 8, WHITE);
	tft.setTextColor(WHITE);

	showmsgXY(0, 25, 1, &FreeSerifBoldItalic9pt7b, WHITE, "100%");
	showmsgXY(10, 100, 1, &FreeSerifBoldItalic9pt7b, WHITE, "50%");
	showmsgXY(25, 170, 1, &FreeSerifBoldItalic9pt7b, WHITE, "0%");
	showmsgXY(45, 15, 1, &FreeSmallFont, YELLOW, "LEVEL");

}

//===========================SECOND SCALE===================================================================

void drawScale_2() {
	tft.drawFastVLine(137, 20, 150, WHITE);
	tft.drawFastHLine(129, 20, 8, WHITE);
	tft.drawFastHLine(132, 57.5, 5, WHITE);
	tft.drawFastHLine(129, 95, 8, WHITE);
	tft.drawFastHLine(132, 132.5, 5, WHITE);
	tft.drawFastHLine(129, 170, 8, WHITE);
	tft.setTextColor(WHITE);

	showmsgXY(95, 25, 1, &FreeSerifBoldItalic9pt7b, WHITE, "50*");
	showmsgXY(95, 100, 1, &FreeSerifBoldItalic9pt7b, WHITE, "25*");
	showmsgXY(105, 170, 1, &FreeSerifBoldItalic9pt7b, WHITE, "0*");
	showmsgXY(125, 15, 1, &FreeSmallFont, YELLOW, "TEMP 1");

}

//===========================THIRD SCALE===================================================================

void drawScale_3() {
	tft.drawFastVLine(224, 20, 150, WHITE);
	tft.drawFastHLine(216, 20, 8, WHITE);
	tft.drawFastHLine(219, 57.5, 5, WHITE);
	tft.drawFastHLine(216, 95, 8, WHITE);
	tft.drawFastHLine(219, 132.5, 5, WHITE);
	tft.drawFastHLine(216, 170, 8, WHITE);
	tft.setTextColor(WHITE);

	showmsgXY(182, 25, 1, &FreeSerifBoldItalic9pt7b, WHITE, "50*");
	showmsgXY(185, 100, 1, &FreeSerifBoldItalic9pt7b, WHITE, "25*");
	showmsgXY(195, 170, 1, &FreeSerifBoldItalic9pt7b, WHITE, "0*");
	showmsgXY(212, 15, 1, &FreeSmallFont, YELLOW, "TEMP 2");

}

//===========================FOURTH SCALE===================================================================

void drawScale_4() {
	tft.drawFastVLine(311, 20, 150, WHITE);
	tft.drawFastHLine(303, 20, 8, WHITE);
	tft.drawFastHLine(306, 57.5, 5, WHITE);
	tft.drawFastHLine(303, 95, 8, WHITE);
	tft.drawFastHLine(306, 132.5, 5, WHITE);
	tft.drawFastHLine(303, 170, 8, WHITE);
	tft.setTextColor(WHITE);

	showmsgXY(269, 25, 1, &FreeSerifBoldItalic9pt7b, WHITE, "40*");
	showmsgXY(269, 100, 1, &FreeSerifBoldItalic9pt7b, WHITE, "30*");
	showmsgXY(269, 170, 1, &FreeSerifBoldItalic9pt7b, WHITE, "20*");
	showmsgXY(299, 15, 1, &FreeSmallFont, YELLOW, "WATER TEMP");

}
//===========================function to draw WATER TEMPERATURE BAR on screen===================================================================
void draw_water_temp() {

	tft.fillRect(320, 175, 40, 20, BLACK);
	showNumXY(320, 190, 1, &FreeSerifBoldItalic9pt7b, GREEN, water_temp);
	showmsgXY(340, 190, 1, &FreeSerifBoldItalic9pt7b, GREEN, "*C");

	int new_temp = map(water_temp, 20, 40, 0, 150);
	int last_temp = map(last_water_temp, 20, 40, 0, 150);
	if (water_temp < last_water_temp) {
		tft.fillRect(315, 170 - last_temp, 30, last_temp - new_temp, BLACK);
	} else {
		tft.fillRect(315, 170 - new_temp, 30, new_temp - last_temp, GREEN);
	}
	last_water_temp = water_temp;
}

////===========================CLOCK FUNCTION===================================================================
//void clock(int x, int current_page, const GFXfont *f) {
//	int seconds_now = rtc.now().second();		//get current time
//	int minutes_now = rtc.now().minute();
//	int hours_now = rtc.now().hour();
//
//	if (current_page == 0) {
//		bool shift_rigth_sec = false;
//		bool shift_rigth_min = false;
//		bool shift_rigth_hour = false;
//
//		int seconds_offset = 25;
//		int minutes_offset = 25;
//		int hours_offset = 25;
//		showmsgXY(110 + x, 130, 1, f, GREEN, ":");
//		showmsgXY(180 + x, 130, 1, f, GREEN, ":");
//
//		//Check if time number is below 10. If so then add 0 and shift right.
//		if (seconds_now <= 9 && previous_seconds != seconds_now) {
//			tft.fillRect(200 + x, 100, 60, 40, BLACK);
//			seconds_offset = 25;
//			showNumXY(200 + x, 130, 1, f, GREEN, 0);
//			shift_rigth_sec = true;
//		} else {
//			seconds_offset = 0;
//		}
//
//		if (minutes_now <= 9 && previous_minutes != minutes_now) {
//			tft.fillRect(130 + x, 100, 60, 40, BLACK);
//			minutes_offset = 25;
//			showNumXY(130 + x, 130, 1, f, GREEN, 0);
//			shift_rigth_min = true;
//		} else {
//			minutes_offset = 0;
//		}
//
//		if (hours_now <= 9 && previous_hours != hours_now) {
//			tft.fillRect(60 + x, 100, 60, 40, BLACK);
//			hours_offset = 25;
//			showNumXY(60 + x, 130, 1, f, GREEN, 0);
//			shift_rigth_hour = true;
//		} else {
//			hours_offset = 0;
//		}
//
//		//draw seconds on screen
//		if (previous_seconds != seconds_now) {
//			if (!shift_rigth_sec) {
//				tft.fillRect(200 + x, 100, 60, 40, BLACK);
//			}
//			showNumXY(200 + x + seconds_offset, 130, 1, f, GREEN, seconds_now);
//			previous_seconds = seconds_now;
//
//			//draw minutes on screen
//			if (previous_minutes != minutes_now) {
//				if (!shift_rigth_min) {
//					tft.fillRect(130 + x, 100, 60, 40, BLACK);
//				}
//				showNumXY(130 + x + minutes_offset, 130, 1, f, GREEN,
//						minutes_now);
//				previous_minutes = minutes_now;
//
//				//draw hours on screen
//				if (previous_hours != hours_now) {
//					if (!shift_rigth_hour) {
//						tft.fillRect(60 + x, 100, 60, 40, BLACK);
//					}
//					showNumXY(60 + x + hours_offset, 130, 1, f, GREEN,
//							hours_now);
//					previous_hours = hours_now;
//				}
//			}
//		}
//	} else if (current_page == 2) {
//		bool shift_rigth_sec = false;
//		bool shift_rigth_min = false;
//		bool shift_rigth_hour = false;
//
//		int seconds_offset = 10;
//		int minutes_offset = 10;
//		int hours_offset = 10;
//		showmsgXY(335 + x, 20, 1, f, GREEN, ":");
//		showmsgXY(370 + x, 20, 1, f, GREEN, ":");
//		//Check if time number is below 10. If so then add 0 and shift right.
//		if (seconds_now <= 9 && previous_seconds != seconds_now) {
//			tft.fillRect(380 + x, 5, 20, 20, BLACK);
//			seconds_offset = 10;
//			showNumXY(380 + x, 20, 1, f, GREEN, 0);
//			shift_rigth_sec = true;
//		} else {
//			seconds_offset = 0;
//		}
//
//		if (minutes_now <= 9 && previous_minutes != minutes_now) {
//			tft.fillRect(345 + x, 5, 20, 20, BLACK);
//			minutes_offset = 10;
//			showNumXY(345 + x, 20, 1, f, GREEN, 0);
//			shift_rigth_min = true;
//		} else {
//			minutes_offset = 0;
//		}
//
//		if (hours_now <= 9 && previous_hours != hours_now) {
//			tft.fillRect(310 + x + hours_offset, 5, 20, 20, BLACK);
//			hours_offset = 10;
//			showNumXY(310 + x, 20, 1, f, GREEN, 0);
//			shift_rigth_hour = true;
//		} else {
//			hours_offset = 0;
//		}
//
//		//draw seconds on screen
//		if (previous_seconds != seconds_now) {
//			if (!shift_rigth_sec) {
//				tft.fillRect(380 + x, 5, 20, 20, BLACK);
//			}
//			showNumXY(380 + x + seconds_offset, 20, 1, f, GREEN, seconds_now);
//			previous_seconds = seconds_now;
//
//			//draw minutes on screen
//			if (previous_minutes != minutes_now) {
//				if (!shift_rigth_min) {
//					tft.fillRect(345 + x, 5, 20, 20, BLACK);
//
//				}
//				showNumXY(345 + x + minutes_offset, 20, 1, f, GREEN,
//						minutes_now);
//				previous_minutes = minutes_now;
//
//				//draw hours on screen
//				if (previous_hours != hours_now) {
//					if (!shift_rigth_hour) {
//						tft.fillRect(310 + x + hours_offset, 5, 20, 20, BLACK);
//					}
//					showNumXY(310 + x + hours_offset, 20, 1, f, GREEN,
//							hours_now);
//					previous_hours = hours_now;
//				}
//			}
//		}
//	}
//}
void aquarium_timer() {

	if (timer_on) {
//	//TIMER FOR PLANT GROW LED LIGHT
//		if (previous_hours >= plant_light_ontime
//				&& previous_hours < plant_light_offtime) {//check if timer is within interval
//			if (!plant_light_state) {					//check if light is off
//				digitalWrite(plant_light, LOW);
//				if (currentpage == 2 && !plant_light_state) {
//					draw_output_state(3);
//				}
//				plant_light_state = true;
//			}
//		} else {
//			if (plant_light_state) {
//				digitalWrite(plant_light, HIGH);
//				if (currentpage == 2 && plant_light_state) {
//					draw_output_state(3);
//				}
//				plant_light_state = false;
//			}
//		}
		//TIMER FOR LED BACKGROUND LIGHT
		if (previous_hours >= led_ontime && previous_hours < led_offtime) {	//check if timer is within interval

			if (!led_relay_state) {
				digitalWrite(led_relay, LOW);
				if (currentpage == 2 && !led_relay_state) {
					draw_output_state(4);
				}
				led_relay_state = true;
			}
		} else {
			if (led_relay_state) {
				digitalWrite(led_relay, HIGH);
				if (currentpage == 2 && led_relay_state) {
					draw_output_state(4);
				}
				led_relay_state = false;
			}
		}
	}
}

void draw_output_state(int index) {

	switch (index) {
	case 0:
		if (!water_pump_state) {
			draw_button_state(200, 0, 70, 40, 2, BLACK, GREEN, "ON");//water pump
		} else {
			draw_button_state(200, 0, 70, 40, 2, BLACK, GRAY, "OFF");
		}

		if (timer_on) {
			draw_button_state(200, 150, 70, 40, 2, BLACK, GREEN, "ON");	//timer
		} else {
			draw_button_state(200, 150, 70, 40, 2, BLACK, GRAY, "OFF");
		}

		if (plant_light_state) {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GREEN, "ON");//plant light
		} else {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GRAY, "OFF");
		}

		if (led_relay_state) {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GREEN, "ON");	//led light
		} else {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 1:
		if (water_pump_state) {
			draw_button_state(200, 0, 70, 40, 2, BLACK, GREEN, "ON");//water pump
		} else {
			draw_button_state(200, 0, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 2:
		if (timer_on) {
			draw_button_state(200, 150, 70, 40, 2, BLACK, GREEN, "ON");	//timer
		} else {
			draw_button_state(200, 150, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 3:
		if (!plant_light_state) {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GREEN, "ON");//plant light from timer change
		} else {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 4:
		if (!led_relay_state) {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GREEN, "ON");	//led light from timer change
		} else {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 5:
		if (plant_light_state) {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GREEN, "ON");//plant light from button press
		} else {
			draw_button_state(200, 50, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 6:
		if (led_relay_state) {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GREEN, "ON");	//led light from button press
		} else {
			draw_button_state(200, 100, 70, 40, 2, BLACK, GRAY, "OFF");
		}
		break;
	case 7:
		//!water_pump_state ? draw_button_state(130, 200, 70, 40, 2, BLACK, GREEN, "ON"):tft.fillRect(130, 200, 70, 40, BLACK);

		if (feed_timer_on) {
			draw_button_state(130, 200, 70, 40, 2, BLACK, GREEN, "ON");	//led light from button press
		} else {
			tft.fillRect(130, 200, 70, 40, BLACK);
		}
		break;

	default:
		break;
	}
}
