/*
 * Clock.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: Janis Davidsons
 */

#include "Clock.h"

MCUFRIEND_kbv touchScrn;
RTC_DS3231 realTimeClock;
DateTime dateTime;

Clock::Clock(int coordX, int coordY, const GFXfont *font, Adafruit_GFX *gfx)
{
	realTimeClock.begin();
	getCurrentTime();
	setPreviousTime();
	clockX = coordX;
	clockY = coordY;
	currenFont = font;
	_screen = gfx;
}

DateTime Clock::getCurrentTime()
{
	currentTime = realTimeClock.now();
	seconds_now = currentTime.second();
	minutes_now = currentTime.minute();
	hours_now = currentTime.hour();
	return currentTime;
}

void Clock::setPreviousTime()
{
	previousTime = currentTime;
	previous_seconds = seconds_now;
	previous_minutes = minutes_now;
	previous_hours = hours_now;
}

void Clock::drawSeconds(int textSize)
{
	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char previousSeconds[] = "ss";
	previousTime.toString(previousSeconds);

	_screen->getTextBounds(previousSeconds, minutesX + minutesWidth, clockY,
						   &secondsX, &secondsY, &secondsWidth, &secondsHeight);

	_screen->fillRect(secondsX, secondsY, secondsWidth, secondsHeight,
					  BLACK);

	char seconds[] = "ss";
	currentTime.toString(seconds);
	_screen->getTextBounds(seconds, minutesX + minutesWidth, clockY, &secondsX,
						   &secondsY, &secondsWidth, &secondsHeight);

	_screen->setCursor(minutesX + minutesWidth, clockY);
	_screen->setTextColor(GREEN);
	_screen->print(seconds);

	_screen->setFont(NULL);
}

void Clock::drawMinutes(int textSize)
{
	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char clockToClear[] = "mm:ss";
	currentTime.toString(clockToClear);

	_screen->getTextBounds(clockToClear, hourX + hourWidth, clockY, &minutesX,
						   &minutesY, &minutesWidth, &minutesHeight);

	_screen->fillRect(minutesX, minutesY, minutesWidth, minutesHeight, BLACK);

	char minutes[] = "mm";
	currentTime.toString(minutes);

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

void Clock::drawHours(int textSize)
{

	_screen->setTextSize(textSize);
	_screen->setFont(currenFont);

	char clockToClear[] = "ss:bb:cc";
	currentTime.toString(clockToClear);

	_screen->getTextBounds(clockToClear, clockX, clockY, &hourX, &hourY, &hourWidth,
						   &hourHeight);

	_screen->fillRect(hourX, hourY, hourWidth, hourHeight, BLACK);

	char hours[] = "hh";
	currentTime.toString(hours);

	_screen->getTextBounds(hours, clockX, clockY, &hourX, &hourY, &hourWidth,
						   &hourHeight);

	_screen->setCursor(clockX, clockY);
	_screen->setTextColor(GREEN);
	_screen->print(hours);
	_screen->setCursor(clockX + hourWidth, clockY);
	_screen->print(":");

	_screen->getTextBounds(":", clockX + hourWidth, clockY, &hourX, &hourY, &hourWidth,
						   &hourHeight);

	_screen->setFont(NULL);

	drawMinutes(textSize);
	drawSeconds(textSize);
}

int Clock::getSecond()
{
	getCurrentTime();
	return seconds_now;
};

int Clock::getMinutes()
{
	getCurrentTime();
	return minutes_now;
};

int Clock::getHours()
{
	getCurrentTime();
	return hours_now;
};

void Clock::setFont(const GFXfont *font)
{
	currenFont = font;
}

void Clock::displayClock(int size)
{
	getCurrentTime();

	if (previous_seconds != seconds_now)
	{
		drawSeconds(size);
	}

	if (previous_minutes != minutes_now)
	{
		drawMinutes(size);
	}

	if (previous_hours != hours_now)
	{
		drawHours(size);
	}
	setPreviousTime();
}

void Clock::adjustClock()
{
	if (realTimeClock.lostPower())
	{
		realTimeClock.adjust(DateTime(F(__DATE__), F(__TIME__))); //This line sets the RTC (CLOCK MODULE) with an date & time
	}
}

void Clock::init(int size)
{
	drawHours(size);
}