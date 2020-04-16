/*
 * Clock.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: janis
 */

#include "Clock.h"
MCUFRIEND_kbv touchScrn;
const GFXfont *currenFont;
RTC_DS3231 realTimeClock;

Clock::Clock(int coordX, int coordY, const GFXfont *font, Adafruit_GFX *gfx) {

	getCurrentTime();
	getPreviousTime();
	clockX = coordX;
	clockY = coordY;
	currenFont = font;
	_screen = gfx;
	drawHours(1);
}

void Clock::getCurrentTime() {
	time = realTimeClock.now();
	seconds_now = time.second();
	minutes_now = time.minute();
	hours_now = time.hour();
}

void Clock::getPreviousTime() {
	previousTime = time;
	previous_seconds = seconds_now;
	previous_minutes = minutes_now;
	previous_hours = hours_now;
}

void Clock::drawSeconds(int textSize) {
	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char previousSeconds[] = "cc";
	previousTime.toString(previousSeconds);

	_screen->getTextBounds(previousSeconds, minutesX + minutesWidth, clockY,
			&secondsX, &secondsY, &secondsWidth, &secondsHeight);
	Serial.println(minutesWidth);

	_screen->fillRect(secondsX, secondsY, secondsWidth, secondsHeight,
			BLACK);

	char seconds[] = "cc";
	time.toString(seconds);

	_screen->getTextBounds(seconds, minutesX + minutesWidth, clockY, &secondsX,
			&secondsY, &secondsWidth, &secondsHeight);

	_screen->setCursor(minutesX + minutesWidth, clockY);
	_screen->setTextColor(GREEN);
	_screen->print(seconds);

	_screen->setFont(NULL);
}

void Clock::drawMinutes(int textSize) {
	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char clockToClear[] = "bb:cc";
	time.toString(clockToClear);

	_screen->getTextBounds(clockToClear, hourX + hourWidth, clockY, &minutesX,
			&minutesY, &minutesWidth, &minutesHeight);

	_screen->fillRect(minutesX, minutesY, minutesWidth, minutesHeight, BLACK);

	char minutes[] = "bb";
	time.toString(minutes);

	_screen->getTextBounds(minutes, hourX + hourWidth, clockY, &minutesX,
			&minutesY, &minutesWidth, &minutesHeight);

	_screen->setCursor(hourX + hourWidth, clockY);
	_screen->setTextColor(GREEN);
	_screen->print(minutes);
	_screen->setCursor(hourX + hourWidth + minutesWidth, clockY);
	_screen->print(":");

	_screen->getTextBounds(":", hourX + hourWidth + minutesWidth, clockY,
			&minutesX, &minutesY, &minutesWidth, &minutesHeight);
	_screen->setFont(NULL);

	drawSeconds(textSize);
}

void Clock::drawHours(int textSize) {

	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char clockToClear[] = "ss:bb:cc";
	time.toString(clockToClear);

	_screen->getTextBounds(clockToClear, clockX, clockY, &hourX, &hourY, &hourWidth,
			&hourHeight);

	_screen->fillRect(hourX, hourY, hourWidth, hourHeight, BLACK);

	char hours[] = "aa";
	time.toString(hours);

	_screen->getTextBounds(hours, clockX, clockY, &hourX, &hourY, &hourWidth,
			&hourHeight);

	_screen->setCursor(clockX, clockY);
	_screen->setTextColor(GREEN);
	_screen->print(hours);
	_screen->setCursor(clockX+hourWidth, clockY);
	_screen->print(":");

	_screen->getTextBounds(":", clockX+hourWidth, clockY, &hourX, &hourY, &hourWidth,
			&hourHeight);

	_screen->setFont(NULL);

	drawMinutes(textSize);
	drawSeconds(textSize);
}

int Clock::getSecond() {
	getCurrentTime();
	return seconds_now;
};

int Clock::getMinutes() {
	getCurrentTime();
	return minutes_now;
};

int Clock::getHours() {
	getCurrentTime();
	return hours_now;
};

void Clock::setFont(const GFXfont *font){
	currenFont = font;
}

void Clock::displayClock(int size =0) {

	getCurrentTime();

	//draw seconds on screen
	if (previous_seconds != seconds_now) {
		drawSeconds(size);
	}

	//draw minutes on screen
	if (previous_minutes != minutes_now) {
		drawMinutes(size);
	}

	//draw hours on screen
	if (previous_hours != hours_now) {
		drawHours(size);
	}
	getPreviousTime();
}

void Clock::adjustClock(int year,int month,int day, int hours, int minutes, int seconds){
	realTimeClock.adjust(DateTime(year,month, day, hours, minutes, seconds));		//This line sets the RTC (CLOCK MODULE) with an date & time

}

