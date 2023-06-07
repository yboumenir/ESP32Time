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

#include "ESP32Time.h"
#include "time.h"
#include <sys/time.h>

#include <ctime>
unsigned long parse_time(const char* compilationTime) {
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
    tm.tm_yday = ESP32Time::calculateYday(year, tm.tm_mon, day - 1);
 
     return ESP32Time::calculateEpoch(tm);
}

/*!
    @brief  Constructor for ESP32Time
*/
ESP32Time::ESP32Time(){
  setTime(COMPILATION_EPOCH());
}

/*!
    @brief  Constructor for ESP32Time
	@param  offest
            gmt offset in seconds
*/
ESP32Time::ESP32Time(unsigned long offset){
	this->offset = offset;
}

/*!
    @brief  set the internal RTC time
    @param  sc
            second (0-59)
    @param  mn
            minute (0-59)
    @param  hr
            hour of day (0-23)
    @param  dy
            day of month (1-31)
    @param  mt
            month (1-12)
    @param  yr
            year ie 2021
    @param  ms
            microseconds (optional)
*/
void ESP32Time::setTime(int sc, int mn, int hr, int dy, int mt, int yr, int ms) {
  // seconds, minute, hour, day, month, year $ microseconds(optional)
  // ie setTime(20, 34, 8, 1, 4, 2021) = 8:34:20 1/4/2021
  struct tm t;
  memset(&t, 0, sizeof(struct tm));
  t.tm_year = yr - 1900;    // This is year-1900, so 121 = 2021
  t.tm_mon = mt - 1;
  t.tm_mday = dy;
  t.tm_hour = hr;
  t.tm_min = mn;
  t.tm_sec = sc;
  time_t timeSinceEpoch = mktime(&t);
  setTime(timeSinceEpoch, ms);
}

/*!
    @brief  set time from struct
	@param	tm
			time struct
*/
void ESP32Time::setTimeStruct(tm t) { 
	time_t timeSinceEpoch = mktime(&t); 
	setTime(timeSinceEpoch, 0); 
}

/*!
    @brief  set the internal RTC time
    @param  epoch
            epoch time in seconds
    @param  ms
            microseconds (optional)
*/
void ESP32Time::setTime(unsigned long epoch, int ms) {
  struct timeval tv;
  memset(&tv, 0, sizeof( struct timeval));
  if (epoch > 2082758399){
	  this->overflow = true;
	  tv.tv_sec = epoch - 2082758399;  // epoch time (seconds)
  } else {
	  tv.tv_sec = epoch;  // epoch time (seconds)
  }
  
  tv.tv_usec = ms * 1000;  // Convert milliseconds to microseconds
  settimeofday(&tv, NULL);   
}

/*!
    @brief  get the internal RTC time as a tm struct
*/
tm ESP32Time::getTimeStruct(){
  struct tm timeinfo;
//   memset(&timeinfo, 0, sizeof( struct tm));
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  
//   Serial.print("The current date/time is ");
//   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time_t tt = mktime(&timeinfo);
    
  if (this->overflow){
	  tt += 63071999;
  }
  tt += offset;
  struct tm * tn = localtime(&tt);
  if (this->overflow){
	  tn->tm_year += 64;
  }
  return *tn;
}

tm* ESP32Time::get_time_struct(){
  struct tm timeinfo;
  memset(&timeinfo, 0, sizeof( struct tm));
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  
//   Serial.print("The current date/time is ");
//   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time_t tt = mktime(&timeinfo);
    
  if (this->overflow){
	  tt += 63071999;
  }
  tt += offset;
  struct tm * tn = localtime(&tt);
  if (this->overflow){
	  tn->tm_year += 64;
  }
  return tn;
}

/*!
    @brief  get the time and date as an Arduino String object
    @param  mode
            true = Long date format
			false = Short date format
*/
String ESP32Time::getDateTime(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%A, %B %d %Y %H:%M:%S", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%a, %b %d %Y %H:%M:%S", &timeinfo);
	}
	return String(s);
}

/*!
    @brief  get the time and date as an Arduino String object
    @param  mode
            true = Long date format
			false = Short date format
*/
String ESP32Time::getTimeDate(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%H:%M:%S %A, %B %d %Y", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%H:%M:%S %a, %b %d %Y", &timeinfo);
	}
	return String(s);
}

/*!
    @brief  get the time as an Arduino String object
*/
String ESP32Time::getTime(){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	strftime(s, 50, "%H:%M:%S", &timeinfo);
	return String(s);
}

/*!
    @brief  get the time as an Arduino String object with the specified format
	@param	format
			time format 
			http://www.cplusplus.com/reference/ctime/strftime/
*/
String ESP32Time::getTime(String format){
	struct tm timeinfo = getTimeStruct();
	char s[128];
	char c[128];
	format.toCharArray(c, 127);
	strftime(s, 127, c, &timeinfo);
	return String(s);
}

/*!
    @brief  get the date as an Arduino String object
    @param  mode
            true = Long date format
			false = Short date format
*/
String ESP32Time::getDate(bool mode){
	struct tm timeinfo = getTimeStruct();
	char s[51];
	if (mode)
	{
		strftime(s, 50, "%A, %B %d %Y", &timeinfo);
	}
	else
	{
		strftime(s, 50, "%a, %b %d %Y", &timeinfo);
	}
	return String(s);
}

/*!
    @brief  get the current milliseconds as unsigned long
*/
unsigned long ESP32Time::getMillis(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec/1000;
}

/*!
    @brief  get the current microseconds as unsigned long
*/
unsigned long ESP32Time::getMicros(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec;
}

/*!
    @brief  get the current epoch seconds as unsigned long
*/
unsigned long ESP32Time::getEpoch(){
    struct tm *timeinfo = get_time_struct();
    if(timeinfo != nullptr)
	    return calculateEpoch(*timeinfo);
	 else
	    return 0;
}

int ESP32Time::calculateYday(int year, int month, int day) {
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

unsigned long  ESP32Time::calculateEpoch(tm t) {
    unsigned long epoch = t.tm_sec + t.tm_min*60 + t.tm_hour*3600 + t.tm_yday*86400 +
     (t.tm_year-70)*31536000 + ((t.tm_year-69)/4)*86400 -
     ((t.tm_year-1)/100)*86400 + ((t.tm_year+299)/400)*86400;
     return epoch;
}

/*!
    @brief  get the current epoch seconds as unsigned long from the rtc without offset
*/
unsigned long ESP32Time::getLocalEpoch(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long epoch = tv.tv_sec;
	if (this->overflow){
		epoch += 63071999 + 2019686400;
	}
	return epoch;
}

/*!
    @brief  get the current seconds as int
*/
int ESP32Time::getSecond(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_sec;
}

/*!
    @brief  get the current minutes as int
*/
int ESP32Time::getMinute(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_min;
}

/*!
    @brief  get the current hour as int
	@param	mode
			true = 24 hour mode (0-23)
			false = 12 hour mode (0-12)
*/
int ESP32Time::getHour(bool mode){
	struct tm timeinfo = getTimeStruct();
	if (mode)
	{
		return timeinfo.tm_hour;
	}
	else
	{
		int hour = timeinfo.tm_hour;
		if (hour > 12)
		{
			return timeinfo.tm_hour-12;
		}
		else
		{
			return timeinfo.tm_hour;
		}
	}
}

/*!
    @brief  return current hour am or pm
	@param	lowercase
			true = lowercase
			false = uppercase
*/
String ESP32Time::getAmPm(bool lowercase){
	struct tm timeinfo = getTimeStruct();
	if (timeinfo.tm_hour >= 12)
	{
		if (lowercase)
		{
			return "pm";
		}
		else
		{
			return "PM";
		}
	}
	else
	{
		if (lowercase)
		{
			return "am";
		}
		else
		{
			return "AM";
		}
	}
}

/*!
    @brief  get the current day as int (1-31)
*/
int ESP32Time::getDay(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_mday;
}

/*!
    @brief  get the current day of week as int (0-6)
*/
int ESP32Time::getDayofWeek(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_wday;
}

/*!
    @brief  get the current day of year as int (0-365)
*/
int ESP32Time::getDayofYear(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_yday;
}

/*!
    @brief  get the current month as int (0-11)
*/
int ESP32Time::getMonth(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_mon;
}

/*!
    @brief  get the current year as int
*/
int ESP32Time::getYear(){
	struct tm timeinfo = getTimeStruct();
	return timeinfo.tm_year+1900;
}
