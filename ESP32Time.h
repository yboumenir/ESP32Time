/*
   MIT License

  Copyright (c) 2021 Felix Biego

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ESP32TIME_H
#define ESP32TIME_H

#include <Arduino.h>
#include <ctime>

static int calculate_yday(int year, int month, int day) {
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int yday = 0;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29;  // February has 29 days in a leap year
    }
    for (int i = 0; i < month - 1; i++) {
        yday += daysInMonth[i];
    }
    yday += day;
    return yday;
}
static unsigned long parse_time(const char* compilationTime) {
	const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    struct std::tm tm = {};
    memset(&tm, 0, sizeof(struct tm));
    char month[4];
    int day, year, hour, minute, second;

    sscanf(compilationTime, "%s %d %d %d:%d:%d",
           month, &day, &year, &hour, &minute, &second);

	tm.tm_mon = ((strstr(months, month) - months) / 3) + 1;
    tm.tm_mday = day;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_yday = calculate_yday(year, tm.tm_mon, day - 1);
 
	unsigned long epoch = tm.tm_sec + tm.tm_min*60 + tm.tm_hour*3600 + tm.tm_yday*86400 +
     (tm.tm_year-70)*31536000 + ((tm.tm_year-69)/4)*86400 -
     ((tm.tm_year-1)/100)*86400 + ((tm.tm_year+299)/400)*86400;
     
     return epoch;
}

#define COMPILATION_EPOCH() (parse_time(__DATE__ " " __TIME__))

class ESP32Time {
	
	public:
		ESP32Time();
		ESP32Time(unsigned long offset);
		void setTime(unsigned long epoch = COMPILATION_EPOCH(), int ms = 0);
		void setTime(int sc, int mn, int hr, int dy, int mt, int yr, int ms = 0);
		void setTimeStruct(tm t);
		tm getTimeStruct();
		String getTime(String format);
		
		String getTime();
		String getDateTime(bool mode = false);
		String getTimeDate(bool mode = false);
		String getDate(bool mode = false);
		String getAmPm(bool lowercase = false);
		
		unsigned long getEpoch();
		unsigned long getMillis();
		unsigned long getMicros();
		int getSecond();
		int getMinute();
		int getHour(bool mode = false);
		int getDay();
		int getDayofWeek();
		int getDayofYear();
		int getMonth();
		int getYear();
		
		unsigned long offset = 0;
		unsigned long getLocalEpoch();
		
	private:
		bool overflow;
};


#endif