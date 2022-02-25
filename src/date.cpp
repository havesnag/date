/*
 * date.cpp
 *
 *  Created on: 2016年6月7日
 *      Author: havesnag
 */

#include "date.h"
#include <limits.h>
#include <sstream>
#include <iomanip>
#include <string.h>
using namespace std;

#ifdef PLATFORM_WINDOWS

#define localtime_r(t, tm) localtime_s(tm, t)
#define gmtime_r(t, tm) gmtime_s(tm, t)

int gettimeofday(struct timeval *tp, void *tzp) 
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = static_cast<long>(clock);
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif // PLATFORM_WINDOWS

namespace ec
{

Duration::Duration(int64 value, Period period)
{
	_value = value;
	_period = period;
}

Duration::Duration(const Duration &duration)
{
	_value = duration._value;
	_period = duration._period;
}

Duration::~Duration()
{
}

Duration Duration::clone() const
{
	return Duration(*this);
}

Duration & Duration::set(int64 value, Period period)
{
	_value = value;
	_period = period;
	return *this;
}

Duration & Duration::setValue(int64 value)
{
	_value = value;
	return *this;
}

Duration & Duration::setPeriod(Period period)
{
	_period = period;
	return *this;
}

Duration & Duration::rase()
{
	switch (_period)
	{
	case Duration::MicroSecond:
		_period = Duration::MilliSecond;
		_value /= 1000;
		break;
	case Duration::MilliSecond:
		_period = Duration::Second;
		_value /= 1000;
		break;
	case Duration::Second:
		_period = Duration::Minute;
		_value /= 60;
		break;
	case Duration::Minute:
		_period = Duration::Hour;
		_value /= 60;
		break;
	case Duration::Hour:
		_period = Duration::Day;
		_value /= 24;
		break;
	case Duration::Day:
		_period = Duration::Week;
		_value /= 7;
		break;
	case Duration::Week:
		_period = Duration::Month;
		_value /= 4;
		break;
	case Duration::Month:
		_period = Duration::Year;
		_value /= 12;
		break;
	default:
		break;
	}
	return *this;
}

Duration & Duration::down()
{
	switch (_period)
	{
	case Duration::MilliSecond:
		_period = Duration::MicroSecond;
		_value *= 1000;
		break;
	case Duration::Second:
		_period = Duration::MilliSecond;
		_value *= 1000;
		break;
	case Duration::Minute:
		_period = Duration::Second;
		_value *= 60;
		break;
	case Duration::Hour:
		_period = Duration::Minute;
		_value *= 60;
		break;
	case Duration::Day:
		_period = Duration::Hour;
		_value *= 24;
		break;
	case Duration::Week:
		_period = Duration::Day;
		_value *= 7;
		break;
	case Duration::Month:
		_period = Duration::Week;
		_value *= 4;
		break;
	case Duration::Year:
		_period = Duration::Month;
		_value *= 12;
		break;
	default:
		break;
	}
	return *this;
}

Duration & Duration::as(Period period)
{
	if (_period < period)
	{
		for (; _period < period; )
		{
			rase();
		}
	}
	else if (_period > period)
	{
		for (; _period > period; )
		{
			down();
		}
	}
	return *this;
}

int64 Duration::valueAs(Period period) const
{
	return clone().as(period).value();
}

Duration Duration::operator + (const Duration &other)
{
	return Duration(_value + other.valueAs(_period), _period);
}

Duration Duration::operator + (int64 value)
{
	return Duration(_value + value, _period);
}

Duration Duration::operator - (const Duration &other)
{
	return Duration(_value - other.valueAs(_period), _period);
}

Duration Duration::operator - (int64 value)
{
	return Duration(_value - value, _period);
}

Duration & Duration::operator += (const Duration &other)
{
	_value += other.valueAs(_period);
	return *this;
}

Duration & Duration::operator += (int64 value)
{
	_value += value;
	return *this;
}

Duration & Duration::operator -= (const Duration &other)
{
	_value -= other.valueAs(_period);
	return *this;
}

Duration & Duration::operator -= (int64 value)
{
	_value -= value;
	return *this;
}

bool Duration::operator > (const Duration & other)
{
	return (_period == other._period) ? (_value > other._value) : (_period > other._period);
}

bool Duration::operator >= (const Duration & other)
{
	return (_period == other._period) ? (_value >= other._value) : (_period > other._period);
}

bool Duration::operator == (const Duration & other)
{
	return (_period == other._period) && (_value == other._value);
}

bool Duration::operator != (const Duration & other)
{
	return (_period != other._period) && (_value != other._value);
}

bool Duration::operator < (const Duration & other)
{
	return (_period == other._period) ? (_value < other._value) : (_period < other._period);
}

bool Duration::operator <= (const Duration & other)
{
	return (_period == other._period) ? (_value <= other._value) : (_period < other._period);
}



time_t Date::localTimeZoneOffset()
{
	return localTimeZone() * -3600;
}

int Date::localTimeZone()
{
	static int tz = Date(0).hour();
	return tz;
}

bool Date::isLeapYear(int year)
{
	return (year % 4 == 0 && ((year % 400 == 0) || (year % 100 != 0)));
}

int Date::yearMonthDays(int year, int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		//Need to consider a leap year in February
		return isLeapYear(year) ? 29 : 28;
	default:
		return 0;
	}
}

Date::Date()
{
	_isUTC = false;
	_set(time(NULL));
}

Date::Date(time_t stamp, bool utc)
{
	_isUTC = utc;
	_set(stamp);
}

Date::Date(const Time &time)
{
	_isUTC = false;
	_set(time.stamp());
}

Date::Date(const Date &other)
{
	_isUTC = other._isUTC;
	_tm = other._tm;
}

Date::Date(int year, int month, int day, int hour, int minute, int second)
{
	_isUTC = false;
	_set(time(NULL));

	_tm.tm_year = year - 1900;
	_tm.tm_mon = month - 1;
	_tm.tm_mday = day;
	_tm.tm_hour = hour;
	_tm.tm_min = minute;
	_tm.tm_sec = second;

	_update();
}

Date::~Date()
{
}

Date Date::clone() const
{
	return Date(*this);
}

Date Date::toUTC() const
{
	return Date(stamp(), true);
}

Time Date::toTime() const
{
	return Time(*this);
}

std::string Date::toString() const
{
	std::ostringstream oss;
	oss << setfill('0') << setw(4) << year()
		<< "-" << setw(2) << month()
		<< "-" << setw(2) << day()
		<< " " << setw(2) << hour()
		<< ":" << setw(2) << minute()
		<< ":" << setw(2) << second();
	return oss.str();
}

std::string Date::format(const char * fmt) const
{
	char buf[256] = {0};
	if (0 == strftime(buf, 63, fmt, &_tm)) {
		buf[0] = '\0';
	}
	return std::string(buf);
}

time_t Date::stamp() const
{
#ifdef PLATFORM_WINDOWS
	if (_tm.tm_year > 70) // > 1970
	{
		return mktime(const_cast<struct tm *>(&_tm));
	}
	else if (70 == _tm.tm_year)
	{
		return (0 != _tm.tm_mon) ?
			mktime(const_cast<struct tm *>(&_tm)) :
			(_tm.tm_mday - 1) * 86400 + _tm.tm_hour * 3600 + _tm.tm_min * 60 + _tm.tm_sec + Date::localTimeZoneOffset();
	}
	else
	{
		return Date::localTimeZoneOffset();
	}
#else
	return mktime(const_cast<struct tm *>(&_tm));
#endif // PLATFORM_WINDOWS
}

time_t Date::utcStamp() const
{
	return stamp() - Date::localTimeZoneOffset();
}

int Date::timeZone() const
{
#ifdef PLATFORM_WINDOWS
	return _isUTC ? 0 : Date::localTimeZone();
#else
# if defined(__USE_BSD) || defined(__USE_MISC)
	return static_cast<int>(_tm.tm_gmtoff / 3600);
# else
	return static_cast<int>(_tm.__tm_gmtoff / 3600);
# endif//__USE_BSD __USE_MISC 
#endif // PLATFORM_WINDOWS
}

time_t Date::timeZoneOffset() const
{
	return static_cast<time_t>(-3600 * timeZone());
}

Date & Date::set(int year, int month, int day, int hour, int minute, int second)
{
	_tm.tm_year = year - 1900;

	month = (month - 1) % 12;
	_tm.tm_mon = (month > 0) ? month : 0;

	day = day % 32;
	_tm.tm_mday = (day > 1) ? day : 1;

	_update();

	hour = hour % 24;
	_tm.tm_hour = (hour > 0) ? hour : 0;

	minute = minute % 60;
	_tm.tm_min = (minute > 0) ? minute : 0;

	second = second % 60;
	_tm.tm_sec = (second > 0) ? second : 0;


	return *this;
}

Date & Date::setDate(int year, int month, int day)
{
	_tm.tm_year = year - 1900;

	month = (month - 1) % 12;
	_tm.tm_mon = (month > 0) ? month : 0;

	day = day % 32;
	_tm.tm_mon = (day > 1) ? day : 1;

	_update();
	return *this;
}

Date & Date::setYear(int year)
{
	_tm.tm_year = year - 1900;
	_update();
	return *this;
}

Date & Date::setMonth(int month)
{
	month = (month - 1) % 12;
	_tm.tm_mon = (month > 0) ? month : 0;
	_update();
	return *this;
}

Date & Date::setDay(int day)
{
	day = day % 32;
	_tm.tm_mday = (day > 1) ? day : 1;
	_update();
	return *this;
}

Date & Date::setHour(int hour)
{
	hour = hour%24;
	_tm.tm_hour = (hour > 0) ? hour : 0;
	return *this;
}

Date & Date::setMinute(int minute)
{
	minute = minute%60;
	_tm.tm_min = (minute > 0) ? minute : 0;
	return *this;
}

Date & Date::setSecond(int second)
{
	second = second%60;
	_tm.tm_sec = (second > 0) ? second : 0;
	return *this;
}

Date & Date::zeroSet(Duration::Period period)
{
	switch (period)
	{
	case Duration::Minute:
		_tm.tm_sec = 0;
		break;
	case Duration::Hour:
		_tm.tm_min = 0;
		_tm.tm_sec = 0;
		break;
	case Duration::Day:
		_tm.tm_hour = 0;
		_tm.tm_min = 0;
		_tm.tm_sec = 0;
		break;
	case Duration::Week:
		_set(toTime().zeroSet(period).stamp());
		break;	
	case Duration::Month:
		_tm.tm_mday = 1;
		_tm.tm_hour = 0;
		_tm.tm_min = 0;
		_tm.tm_sec = 0;
		break;
	case Duration::Year:
		_tm.tm_mon = 0;
		_tm.tm_mday = 1;
		_tm.tm_hour = 0;
		_tm.tm_min = 0;
		_tm.tm_sec = 0;		
		break;
	default:
		break;
	}

	return *this;
}

Date & Date::add(int64 value, Duration::Period period)
{
	switch (period)
	{
	case Duration::Second:
	case Duration::Minute:
	case Duration::Hour:
	case Duration::Day:
	case Duration::Week:
		_set(toTime().add(value, period).stamp());
		break;
	case Duration::Month:
		addMonth(int(value));
		break;
	case Duration::Year:
		addMonth((int)value);
		break;
	default:
		break;
	}

	return *this;
}

Date & Date::add(const Duration & duration)
{
	return add(duration.value(), duration.period());
}

Date & Date::addYear(int value)
{
	_tm.tm_year += value;
	if (_tm.tm_year < 0)
	{
		_tm.tm_year = 0;
	}

	int monthDays = Date::yearMonthDays(year(), month()); 
	if (_tm.tm_mday > monthDays)
	{
		_tm.tm_mday = monthDays;
	}

	return *this;
}

Date & Date::addMonth(int value)
{
	_tm.tm_mon += value;
	_tm.tm_year += _tm.tm_mon / 12;
	_tm.tm_mon = _tm.tm_mon % 12;
	if (_tm.tm_mon < 0)
	{
		_tm.tm_year -= 1;
		_tm.tm_mon += 12;
	}

	int monthDays = Date::yearMonthDays(year(), month());
	if (_tm.tm_mday > monthDays)
	{
		_tm.tm_mday = monthDays;
	}

	return *this;
}

int64 Date::diff(const Date & other, Duration::Period period)
{
	switch (period)
	{
	case Duration::MicroSecond:
		return static_cast<int64>((stamp() - other.stamp()) * 1000000);
	case Duration::MilliSecond:
		return static_cast<int64>((stamp() - other.stamp()) * 1000);
	case Duration::Second:
		return static_cast<int64>(stamp() - other.stamp());
	case Duration::Minute:
		return static_cast<int64>(stamp() / 60 - other.stamp() / 60);
	case Duration::Hour:
		return static_cast<int64>(stamp() / 3600 - other.stamp() / 3600);
	case Duration::Day:
	case Duration::Week:
		return toTime().diff(other.toTime(), period);
	case Duration::Month:
		return year() * 12 + month() - other.year() * 12  - other.month();
	case Duration::Year:
		return year() - other.year();
	default:
		return 0;
	}
}

int Date::getYearDay() const
{
	return _tm.tm_yday + 1;
}

int Date::getUTCFullMonths() const
{
	return (year() - 1970) * 12 + month() - 1;
}

int Date::getUTCFullYears() const
{
	return year() - 1970;
}

bool Date::isLeapYear() const
{
	return Date::isLeapYear(year());
}

bool Date::isLastDayOfMonth() const
{
	return day() >= Date::yearMonthDays(year(), month());
}

Date Date::operator + (const Duration & duration)
{
	return clone().add(duration.value(), duration.period());
}

Date Date::operator - (const Duration & duration)
{
	return clone().add(-duration.value(), duration.period());
}

Duration Date::operator - (const Date & other)
{
	return Duration(static_cast<int64>(stamp() - other.stamp()));
}

Date & Date::operator += (const Duration & duration)
{
	return add(duration.value(), duration.period());
}

Date & Date::operator -= (const Duration & duration)
{
	return add(-duration.value(), duration.period());
}

bool Date::operator < (const Date & other)
{
	if (_tm.tm_year != other._tm.tm_year)
	{
		return _tm.tm_year < other._tm.tm_year;
	}

	if (_tm.tm_mon != other._tm.tm_mon)
	{
		return _tm.tm_mon < other._tm.tm_mon;
	}

	if (_tm.tm_mday != other._tm.tm_mday)
	{
		return _tm.tm_mday < other._tm.tm_mday;
	}

	if (_tm.tm_hour != other._tm.tm_hour)
	{
		return _tm.tm_hour < other._tm.tm_hour;
	}

	if (_tm.tm_min != other._tm.tm_min)
	{
		return _tm.tm_min < other._tm.tm_min;
	}

	return _tm.tm_sec < other._tm.tm_sec;
}

bool Date::operator == (const Date & other)
{
	return (_tm.tm_year == other._tm.tm_year) &&
			(_tm.tm_mon == other._tm.tm_mon) &&
			(_tm.tm_mday == other._tm.tm_mday) &&
			(_tm.tm_hour == other._tm.tm_hour) &&
			(_tm.tm_min == other._tm.tm_min) &&
			(_tm.tm_sec == other._tm.tm_sec);
}

void Date::_set(time_t stamp)
{
#ifdef PLATFORM_WINDOWS
	if (stamp >= 0)
	{
		_isUTC ? gmtime_r(&stamp, &_tm) : localtime_r(&stamp, &_tm);
	}
	else
	{
		if (_isUTC)
		{
			stamp = 0;
			gmtime_r(&stamp, &_tm);
		}
		else
		{
			time_t zoneOffset = Date::timeZoneOffset();
			if (zoneOffset >= 0)
			{
				stamp = 0;
				localtime_r(&stamp, &_tm);
			}
			else
			{
				stamp = (stamp < zoneOffset) ? 0 : stamp - zoneOffset;
				localtime_r(&stamp, &_tm);
				int hour = static_cast<int>(zoneOffset / 3600);
				_tm.tm_hour += hour;
			}
		}
	}
#else
	_isUTC ? gmtime_r(&stamp, &_tm) : localtime_r(&stamp, &_tm);
#endif // PLATFORM_WINDOWS
}

void Date::_update()
{
	int monthDays = Date::yearMonthDays(year(), month());
	if (_tm.tm_mday > monthDays)
	{
		_tm.tm_mday = monthDays;
	}

	_set(stamp());
}


Time::Time()
{
	gettimeofday(&_tv, NULL);
}

Time::Time(time_t stamp)
{
	set(stamp);
}

Time::Time(const Date &date)
{
	set(date.stamp());
}

Time::Time(const Time &time)
{
	_tv = time._tv;
}

Time::~Time()
{
}

Time Time::clone() const
{
	return Time(*this);
}

Date Time::toDate() const
{
	return Date(*this);
}

Date Time::utcDate() const
{
	return Date(stamp(), true);
}


time_t Time::utcStamp() const
{
	return _tv.tv_sec - Date::localTimeZoneOffset();
}

Time & Time::set(time_t seconds, long microSeconds)
{
	_tv.tv_sec = static_cast<long>(seconds);
	if (microSeconds < 0)
	{
		microSeconds = 0;
	}
	else if (microSeconds >= 1000000)
	{
		microSeconds = 1000000 - 1;
	}
	return *this;
}

Time & Time::setSeconds(time_t seconds)
{
	_tv.tv_sec = static_cast<long>(seconds);
	return *this;
}

Time & Time::setMicroSeconds(long microSeconds)
{
	if (microSeconds < 0)
	{
		microSeconds = 0;
	}
	else if (microSeconds >= 1000000)
	{
		microSeconds = 1000000 - 1;
	}

	_tv.tv_usec = microSeconds;
	return *this;
}

Time & Time::zeroSet(Duration::Period period)
{
	switch (period)
	{
	case Duration::MilliSecond:
		_tv.tv_usec = _tv.tv_usec * 1000 * 1000;
		break;
	case Duration::Second:
		_tv.tv_usec = 0;
		break;
	case Duration::Minute:
		_tv.tv_sec = _tv.tv_sec / 60 * 60;
		_tv.tv_usec = 0;
		break;
	case Duration::Hour:
		_tv.tv_sec = _tv.tv_sec / 3600 * 3600;
		_tv.tv_usec = 0;
		break;
	case Duration::Day:
	case Duration::Week:
	case Duration::Month:
	case Duration::Year:
		setSeconds(toDate().zeroSet(period).stamp());
		break;
	default:
		break;
	}

	return *this;
}

Time & Time::add(int64 value, Duration::Period period)
{
	switch (period)
	{
	case Duration::MicroSecond:
		addMicroSecond(long(value));
		break;
	case Duration::MilliSecond:
		addMilliSecond(long(value));
		break;
	case Duration::Second:
		addSecond(int(value));
		break;
	case Duration::Minute:
		addMinute(int(value));
		break;
	case Duration::Hour:
		addHour(int(value));
		break;
	case Duration::Day:
		addDay(int(value));
		break;
	case Duration::Week:
		addWeek(int(value));
		break;
	case Duration::Month:
	case Duration::Year:
		setSeconds(toDate().add(value, period).stamp());
		break;
	default:
		break;
	}

	return *this;
}

Time & Time::add(const Duration & duration)
{
	return add(duration.value(), duration.period());
}

Time & Time::addWeek(int value)
{
	_tv.tv_sec += value * 3600 * 24 * 7;
	return *this;
}

Time & Time::addDay(int value)
{
	_tv.tv_sec += value * 3600 * 24;
	return *this;
}

Time & Time::addHour(int value)
{
	_tv.tv_sec += value * 3600;
	return *this;
}

Time & Time::addMinute(int value)
{
	_tv.tv_sec += value * 60;
	return *this;
}

Time & Time::addSecond(long value)
{
	_tv.tv_sec += value;
	return *this;
}

Time & Time::addMilliSecond(long value)
{
	_tv.tv_usec += value * 1000;
	_tv.tv_sec += _tv.tv_usec / 1000000;
	_tv.tv_usec %= 1000000;
	if (_tv.tv_usec < 0)
	{
		_tv.tv_sec -= 1;
		_tv.tv_usec += 1000000;
	}
	return *this;
}

Time & Time::addMicroSecond(long value)
{
	_tv.tv_usec += value;
	_tv.tv_sec += _tv.tv_usec / 1000000;
	_tv.tv_usec %= 1000000;
	if (_tv.tv_usec < 0)
	{
		_tv.tv_sec -= 1;
		_tv.tv_usec += 1000000;
	}
	return *this;
}

int64 Time::diff(const Time & other, Duration::Period period)
{
	switch (period)
	{
	case Duration::MicroSecond:
		return static_cast<int64>(microStamp() - other.microStamp());
	case Duration::MilliSecond:
		return static_cast<int64>(milliStamp() - other.milliStamp());
	case Duration::Second:
		return static_cast<int64>(stamp() - other.stamp());
	case Duration::Minute:
		return static_cast<int64>(stamp() / 60 - other.stamp() / 60);
	case Duration::Hour:
		return static_cast<int64>(stamp() / 3600 - other.stamp() / 3600);
	case Duration::Day:
		return static_cast<int64>(getUTCFullDays() - other.getUTCFullDays());
	case Duration::Week:
		return static_cast<int64>(getUTCFullWeeks() - other.getUTCFullWeeks());
	case Duration::Month:
	case Duration::Year:
		return toDate().diff(other.toDate(), period);
	default:
		return 0;
	}
}

int64 Time::getUTCFullMicroSeconds() const
{
	return microStamp() + Date::localTimeZoneOffset() * 1000000;
}

int64 Time::getUTCFullMilliSeconds() const
{
	return milliStamp() + Date::localTimeZoneOffset() * 1000;
}

time_t Time::getUTCFullSeconds() const
{
	return seconds();
}

int Time::getUTCFullMinutes() const
{
	return static_cast<int>((utcStamp() / 60));
}

int Time::getUTCFullHours() const
{
	return static_cast<int>((utcStamp() / 3600));
}

int Time::getUTCFullDays() const
{
	return static_cast<int>((utcStamp() / 86400));
}

int Time::getUTCFullWeeks() const
{
	int days = getUTCFullDays() + 4;
	int weeks = (days - 1) / 7;
	int weekDay = days % 7;
	if (weekDay < 0) {
		weeks -= 1;
	}
	return weeks;
}

Time Time::operator + (const Duration & duration)
{
	return clone().add(duration.value(), duration.period());
}

Time Time::operator - (const Duration & duration)
{
	return clone().add(-duration.value(), duration.period());
}

Duration Time::operator - (const Time & other)
{
	return Duration(static_cast<int64>(stamp() - other.stamp()));
}

Time & Time::operator += (const Duration & duration)
{
	return add(duration.value(), duration.period());
}

Time & Time::operator -= (const Duration & duration)
{
	return add(-duration.value(), duration.period());
}

bool Time::operator < (const Time & other)
{
	if (_tv.tv_sec != other._tv.tv_sec)
	{
		return _tv.tv_sec < other._tv.tv_sec;
	}

	return _tv.tv_usec < other._tv.tv_usec;

}

bool Time::operator = (const Time & other)
{
	return (_tv.tv_sec == other._tv.tv_sec) && (_tv.tv_usec == other._tv.tv_usec);
}

} /* namespace ec */
