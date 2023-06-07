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

unsigned long parse_time(const char* compilationTime);
#define COMPILATION_EPOCH() (parse_time(__DATE__ " " __TIME__))

class ESP32Time {
	
	public:
		ESP32Time();
		ESP32Time(unsigned long offset);
		void setTime(unsigned long epoch = COMPILATION_EPOCH(), int ms = 0);
		void setTime(int sc, int mn, int hr, int dy, int mt, int yr, int ms = 0);
		void setTimeStruct(tm t);
		tm getTimeStruct();
		tm* get_time_struct();
		String getTime(String format);
		
		String getTime();
		String getDateTime(bool mode = false);
		String getTimeDate(bool mode = false);
		String getDate(bool mode = false);
		String getAmPm(bool lowercase = false);
		
		unsigned long getEpoch();
		unsigned long getMillis();
		unsigned long getMicros();
		static unsigned long calculateEpoch(tm t);
		int getSecond();
		int getMinute();
		int getHour(bool mode = false);
		int getDay();
		int getDayofWeek();
		int getDayofYear();
		int getMonth();
		int getYear();
		static int calculateYday(int year, int month, int day);
		
		unsigned long offset = 0;
		unsigned long getLocalEpoch();
		
	private:
		bool overflow;
};


#endif