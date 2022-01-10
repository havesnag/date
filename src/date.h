/*
 * date.h
 *
 *  Created on: 2016年6月7日
 *      Author: havesnag
 */

#ifndef INCLUDE_EC_DATE_H_
#define INCLUDE_EC_DATE_H_

#include <time.h>
#include <string>
#include <cstdint>

#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <sys/time.h>
#endif // PLATFORM_WINDOWS

typedef int64_t int64;

namespace ec
{

class Time;
class Date;
class Duration;

/**
* @brief 表示时间段
*/
class Duration
{
public:
	/** @brief 时间类型，级别依次上升，精度依次下降 */
	enum Period
	{
		/** @brief 微秒 1/1000000秒 */
		MicroSecond = 5,
		/** @brief 毫秒 1/1000秒 */
		MilliSecond = 6,
		/** @brief 秒 */
		Second = 11,
		/** @brief 分 60秒 */
		Minute = 12,
		/** @brief 小时 3600秒 */
		Hour = 13,
		/** @brief 天 86400秒 */
		Day = 14,
		/** @brief 周 604800秒 */
		Week = 15,
		/** @brief 月 周与月相互转换比例为1:4 */
		Month = 22,
		/** @brief 年 */
		Year = 23,
	};

	Duration(int64 value = 1, Period period = Second);
	Duration(const Duration &duration);
	~Duration();

	/** @brief 克隆当前对象 */
	Duration clone() const;

	/** @brief 获取数值 */
	inline int64 value() const
	{
		return _value;
	}

	/** @brief 获取周期 */
	inline Period period() const
	{
		return _period;
	}

	/** @brief 设置时间段的数值和周期 */
	Duration & set(int64 value, Period period = Second);
	/** @brief 设置时间段的数值 */
	Duration & setValue(int64 value);
	/** @brief 设置时间段的周期 */
	Duration & setPeriod(Period period);

	/** @brief 提升级别，降低精度，这种转换方式并不是准确的，周与月转换时换1:4换算 */
	Duration & rase();
	/** @brief 降低级别，提升精度，这种转换方式并不是准确的，周与月转换时换1:4换算 */
	Duration & down();
	/** @brief 转换成指定类型的时间段 */
	Duration & as(Period period);
	/** @brief 获取值转换成某种类型后的值 */
	int64 valueAs(Period period) const;

	Duration operator + (const Duration &other);
	Duration operator + (int64 value);
	Duration operator - (const Duration &other);
	Duration operator - (int64 value);
	Duration & operator += (const Duration &other);
	Duration & operator += (int64 value);
	Duration & operator -= (const Duration &other);
	Duration & operator -= (int64 value);
	bool operator > (const Duration & other);
	bool operator >= (const Duration & other);
	bool operator == (const Duration & other);
	bool operator != (const Duration & other);
	bool operator < (const Duration & other);
	bool operator <= (const Duration & other);
private:
	int64 _value;
	Period _period;
};

/**
 * @brief 日期类
 * @details 精确到秒，Windows下仅能表示1970-01-01 00:00:00之前的日期，Linux不受此限制。
 */
class Date
{
public:
	/** @brief 返回当前系统时区，比如UTC+8的时区为8 */
	static int localTimeZone();
	/** @brief 返回当前系统时区偏移，以秒为单位，比如UTC+8的时区为-28800 */
	static time_t localTimeZoneOffset();
	/** @brief 判断是否是闰年 */
	static bool isLeapYear(int year);
	/** @brief 某年某月一共有多少天 */
	static int yearMonthDays(int year, int month);

public:
	/** @brief 以当前时间构造 */
	Date();
	/**
	 * @brief 以时间戳(秒)构造
	 * @param stamp 时间戳
	 * @param utc 是否为UTC基准时间，为false则按本地日历时间构造
	 */
	Date(time_t stamp, bool utc = false);
	/** @brief 以Time对象构造 */
	Date(const Time &time);
	/** @brief 以Date对象复制 */
	Date(const Date &other);

	/**
	 * @brief 以指定时间构造
	 * @note Windows下最小为1970-01-01 00:00:00
	 * @param year 年，取值范围[1970, )
	 * @param month 月，取值范围[1,12]
	 * @param day 日，取值范围[1,31]
	 * @param hour 时，取值范围[0,23]，默认为0
	 * @param minute 分，取值范围[0,59]，默认为0
	 * @param second 秒，取值范围[0,60]，默认为0
	 */
	Date(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

	~Date();

	/** @brief 克隆当前对象 */
	Date clone() const;
	/** @brief 转换为UTC时间 */
	Date toUTC() const;

	/** @brief 转换为Time对象 */
	Time toTime() const;
	/** @brief 转换为字符串，格式为1970-01-01 00:00:00 */
	std::string toString() const;

	/**
	 * @brief 格式化为字符串
	 * @param fmt 格式
	 * @details
	 *     %Y 用CCYY表示的年（如：2004）
	 *     %m 月份 (01-12)
	 *     %d 月中的第几天(01-31)
	 *     %H 小时, 24小时格式 (00-23)
	 *     %M 分钟(00-59)
	 *     %S 秒钟(00-59)
	 *     %X标准时间字符串（如：23:01:59）
	 *     %% 百分号
	 *
	 * @return 如果发生错误返回空字符串
	 */
	std::string format(const char * fmt = "%Y-%m-%d %H:%M:%S") const;

	/** @brief 年，[1970, ) */
	inline int year() const
	{
		return _tm.tm_year + 1900;
	}

	/** @brief 月，[1,12] */
	inline int month() const
	{
		return _tm.tm_mon + 1;
	}

	/** @brief 日，[1,31] */
	inline int day() const
	{
		return _tm.tm_mday;
	}

	/** @brief 时，[0,23] */
	inline int hour() const
	{
		return _tm.tm_hour;
	}

	/** @brief 分，[0,59] */
	inline int minute() const
	{
		return _tm.tm_min;
	}

	/** @brief 秒，[0,60] */
	inline int second() const
	{
		return _tm.tm_sec;
	}

	/** @brief 星期，[1,7] */
	inline int week() const
	{
		return (_tm.tm_wday > 0) ? _tm.tm_wday : 7;
	}

	/** @brief 是否是UTC基准时间 */
	inline bool isUTC() const
	{
		return _isUTC;
	}

	/** @brief 转换为时间戳 @note 按本地时间（时区）转换，比如在东8区(UTC+8)时1970-01-01 00:00:00为-28800 */
	time_t stamp() const;
	/** @brief 转换为UTC时间戳 @note 比如1970-01-01 00:00:00为0 */
	time_t utcStamp() const;
	/** @brief 时区，比如UTC+8的时区为8 */
	int timeZone() const;
	/** @brief 时区偏移，以秒为单位，比如UTC+8的时区为-28800 */
	time_t timeZoneOffset() const;

	/** @brief 统一设置年月日时分秒 @note 比单独设置年/月/日/时/分/秒更高效 */
	Date & set(int year, int month, int day, int hour, int minute, int second);
	/** @brief 统一设置年月日 @note 比单独设置年/月/日更高效 */
	Date & setDate(int year, int month, int day);
	/** @brief 设置年，[1970, ) @note 建议使用setDate统一设置年月日 @see setDate */
	Date & setYear(int year);
	/** @brief 设置月，[1,12] @note 建议使用setDate统一设置年月日 @see setDate */
	Date & setMonth(int month);
	/** @brief 设置日，[1,31] @note 建议使用setDate统一设置年月日 @see setDate*/
	Date & setDay(int day);
	/** @brief 设置时，[0,23] */
	Date & setHour(int hour);
	/** @brief 设置分，[0,59] */
	Date & setMinute(int minute);
	/** @brief 设置秒，[0,60] */
	Date & setSecond(int second);

	/**
	 * @brief 设置为某个时间的开始
	 * @param period 时间类型，
	 * @details
	 *      为Year时置零为一年的开始，
	 *      为Month时置零为一月的开始，
	 *      为Day时置零为一天的开始，
	 *      为Hour时置零为一小时的开始，
	 *      为Minute时置零为一分钟的开始，
	 *      为Second/MilliSecond/MicroSecond时无效果，
	 */
	Date & zeroSet(Duration::Period period);

	/** @brief 加/减 一段时间 */
	Date & add(int64 value, Duration::Period period);
	/** @brief 加/减 一段时间 */
	Date & add(const Duration & duration);
	/** @brief 加/减 年 */
	Date & addYear(int value);
	/** @brief 加/减 月 */
	Date & addMonth(int value);

	/**
	 * @brief 比较两时间之间的差异
	 * @param period 结果时间类型
	 * @param other 要比较的对象
	 * @details
	 *     为Year表示两者相差年份，不是绝对差值，2015-01-01与2014-12-30相差1年
	 *     为Month表示两者相差说数，不是绝对差值，2015-01-01与2014-12-30相差1月
	 *     为Day表示两者相差天数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1天
	 *     为Hour表示两者相差小时数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1小时
	 *     为Minute表示两者相差分钟数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1分钟
	 *     为Second表示两者相差秒数
	 *     为MilliSecond表示两者相差毫秒数，Date的精度为秒，所以只是将相差秒数*1000
	 *     为MicroSecond表示两者相差微秒数，Date的精度为秒，所以只是将相差秒数*1000000
	 * @return 返回this - other的相应差值
	 */
	int64 diff(const Date & other, Duration::Period period = Duration::Second);

	/** @brief 获取一年中的天，[1,366] */
	int getYearDay() const;
	/** @brief 距离1970-01-01 00:00:00的月数 */
	int getUTCFullMonths() const;
	/** @brief 距离1970-01-01 00:00:00的年数 */
	int getUTCFullYears() const;

	/** @brief 是否是闰年 */
	bool isLeapYear() const;
	/** @brief 是否是一月的最后一天 */
	bool isLastDayOfMonth() const;

	Date operator + (const Duration & duration);
	Date operator - (const Duration & duration);
	Duration operator - (const Date & other);
	Date & operator += (const Duration & duration);
	Date & operator -= (const Duration & duration);
	bool operator < (const Date & other);
	bool operator == (const Date & other);

protected:
	void _set(time_t stamp);
	void _update();

private:
	struct tm _tm;
	bool _isUTC;
};

/**
 * @brief 时间类
 * @details 精确到微秒，可与Date相互转换，转换为Date将损失精度到秒
 * @see Date
 */
class Time
{
public:
	Time();
	/** @brief 以时间戳构造 */
	Time(time_t stamp);
	/** @brief 以Date对象构造 */
	Time(const Date &date);
	/** @brief 以Time对象复制 */
	Time(const Time &time);
	~Time();

	/** @brief 克隆当前对象 */
	Time clone() const;

	/** @brief 转换成本地日历时间的Date对象 */
	Date toDate() const;
	/** @brief 转换成UTC基准时间的Date对象 */
	Date utcDate() const;

	/** @brief 获取秒数，等同于时间戳 */
	inline time_t seconds() const
	{
		return _tv.tv_sec;
	}

	/** @brief 获取微秒数, [0,1000000) @details 微秒部分小于一秒 */
	inline long microSeconds() const
	{
		return _tv.tv_sec;
	}

	/** @brief 获取毫秒时间戳 */
	inline int64 milliStamp() const
	{
		return _tv.tv_sec * 1000 + _tv.tv_usec / 1000;
	}

	/** @brief 获取微秒时间戳 */
	inline int64 microStamp() const
	{
		return _tv.tv_sec * 1000000 + _tv.tv_usec;
	}

	/** @brief 获取时间戳 */
	inline time_t stamp() const
	{
		return _tv.tv_sec;
	}

	/** @brief 获取UTC时间戳 */
	time_t utcStamp() const;

	/** @brief 设置秒数和微秒数 */
	Time & set(time_t seconds, long microSeconds = 0);
	/** @brief 获取微秒数, [0,1000) */
	Time & setSeconds(time_t seconds);
	/** @brief 获取微秒数, [0,1000000) */
	Time & setMicroSeconds(long microSeconds);

	/** 
	 * @brief 设置为某个时间的开始
	 * @param period 时间类型，
	 * @details
	 *     为Year时置零为一年的开始，
	 *	   为Month时置零为一月的开始，
	 *	   为Day时置零为一天的开始，
	 *	   为Hour时置零为一小时的开始，
	 *	   为Minute时置零为一分钟的开始，
	 *	   为Second时置零为一秒的开始，
	 *	   为MilliSecond时置零为一毫秒的开始，
	 *	   为MicroSecond时置零为一微秒的开始
	 */
	Time & zeroSet(Duration::Period period);

	/** @brief 加/减 一段时间 */
	Time & add(int64 value, Duration::Period period = Duration::Period::MilliSecond);
	/** @brief 加/减 一段时间 */
	Time & add(const Duration & duration);
	/** @brief 加/减 周 */
	Time & addWeek(int value);
	/** @brief 加/减 天 */
	Time & addDay(int value);
	/** @brief 加/减 时 */
	Time & addHour(int value);
	/** @brief 加/减 分 */
	Time & addMinute(int value);
	/** @brief 加/减 秒 */
	Time & addSecond(long value);
	/** @brief 加/减 毫秒 */
	Time & addMilliSecond(long value);
	/** @brief 加/减 微秒 */
	Time & addMicroSecond(long value);

	/**
	 * @brief 比较两时间之间的差异
	 * @param period 结果时间类型
	 * @param other 要比较的对象
	 * @details
	 *     为Year表示两者相差年份，不是绝对差值，2015-01-01与2014-12-30相差1年
	 *     为Month表示两者相差说数，不是绝对差值，2015-01-01与2014-12-30相差1月
	 *     为Day表示两者相差天数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1天
	 *     为Hour表示两者相差小时数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1小时
	 *     为Minute表示两者相差分钟数，不是绝对差值，2015-01-01 23:59:59与2015-01-02 00:00:00相差1分钟
	 *     为Second表示两者相差秒数
	 *     为MilliSecond表示两者相差毫秒数
	 *     为MicroSecond表示两者相差微秒数
	 * @return 返回this - other的相应差值
	 */
	int64 diff(const Time & other, Duration::Period period = Duration::Second);

	/** @brief 距离1970-01-01 00:00:00的微秒数 */
	int64 getUTCFullMicroSeconds() const;
	/** @brief 距离1970-01-01 00:00:00的毫秒数 */
	int64 getUTCFullMilliSeconds() const;
	/** @brief 距离1970-01-01 00:00:00的秒数 @note 与getUTCStamp()含义相同 @see getUTCStamp */
	time_t getUTCFullSeconds() const;
	/** @brief 距离1970-01-01 00:00:00的分钟 */
	int getUTCFullMinutes() const;
	/** @brief 距离1970-01-01 00:00:00的小时 */
	int getUTCFullHours() const;
	/** @brief 距离1970-01-01 00:00:00的天数 */
	int getUTCFullDays() const;
	/** @brief 距离1970-01-01 00:00:00的周数 @note 从0开始，1970-01-01 00:00:00为第0周星期4 */
	int getUTCFullWeeks() const;

	Time operator + (const Duration & duration);
	Time operator - (const Duration & duration);
	Duration operator - (const Time & other);
	Time & operator += (const Duration & duration);
	Time & operator -= (const Duration & duration);
	bool operator < (const Time & other);
	bool operator = (const Time & other);

private:
	struct timeval _tv;
};

} /* namespace ec */

#endif /* INCLUDE_EC_DATE_H_ */
