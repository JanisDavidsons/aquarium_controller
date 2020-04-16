/*
 * Clock.h
 *
 *  Created on: Mar 21, 2020
 *      Author: janis
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <MCUFRIEND_kbv.h>
#include <RTClib.h>
#include <Fonts/FreeMono24pt7b.h>

class Clock {

private:
	int clockX;
	int clockY;

	int16_t hourX, hourY;
	uint16_t hourWidth, hourHeight;

	int16_t minutesX, minutesY;
	uint16_t minutesWidth, minutesHeight;

	int16_t secondsX, secondsY;
	uint16_t secondsWidth, secondsHeight;

	int16_t firstColonX, firstColonY;
	uint16_t firstColonWidth, firstColonHeight;

	int16_t secondColonX, secondColonY;
	uint16_t secondColonWidth, secondColonHeight;

	Adafruit_GFX *_screen;
	GFXfont *currenFont;
#define BLACK 		0x0000
#define GREEN   	0x07E0
#define RED     	0xF800

	DateTime time;
	DateTime previousTime;
	int seconds_now;  //rtc.now().second();		//get current time
	int minutes_now;  //rtc.now().minute();
	int hours_now;  //rtc.now().hour();

	char *seconds_now_char;  //rtc.now().second();		//get current time
	char *minutes_now_char;  //rtc.now().minute();
	char *hours_now_char;  //rtc.now().hour();

	bool shift_rigth_seconds = false;
	bool shift_rigth_min = false;
	bool shift_rigth_hour = false;

	int previous_hours;
	int previous_minutes;
	int previous_seconds;

	int seconds_offset = 25;
	int minutes_offset = 25;
	int hours_offset = 25;

public:
	Clock(int coordX, int coordY, const GFXfont *font, Adafruit_GFX *gfx);
	//virtual ~Clock();

	void getCurrentTime();
	void getPreviousTime();
	void displayClock(int size = 0);
	void setFont(const GFXfont *font);

	int getSecond();
	int getMinutes();
	int getHours();
	void adjustClock(int,int,int,int,int,int);

private:
	void drawSeconds(int);
	void drawMinutes(int);
	void drawHours(int);

};

#endif /* CLOCK_H_ */
