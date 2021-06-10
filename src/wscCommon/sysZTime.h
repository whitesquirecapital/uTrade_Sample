#pragma once

#include <time.h>
#include <stdint.h>
#include <netinet/in.h>

#include <date/tz.h>
#include <iostream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#define ONE_SEC 1000000000LL
#define NANO_SECONDS_IN_24_HOURS 86400000000000LL

namespace wsc
{

	class Time
	{

	public:
		// CAUTION: Never override time by calling below function in live environment. Only meant for simulation.
		static void setTimestampUnsafeForLive(int64_t timestamp){
			Time::virtualTimestamp = timestamp;
		}

		static int64_t getTimestamp()
		{

			return Time::virtualTimestamp != 0L ? Time::virtualTimestamp : Time::getSystemTimestamp();
			// return Time::getSystemTimestamp();
		}

		//
		// Using CLOCK_REALTIME. For more details on time related system calls please refer:
		// http://juliusdavies.ca/posix_clocks/clock_realtime_linux_faq.html
		//
		// Format: Epoch 1970: GTM: Nano Seconds
		//
		// CAUTION: Working on the last digit of int64_t max. Avoid adding multiple timestamps. Though few hundred years still left.
		//          int64_t max	     : 9223372036854775807
		//          current timestamp: 1512113667000000000
		//
		// NOTE: Almost always used through getTimestamp() call. Use direclty only for latency profiling.
		//
		static int64_t getSystemTimestamp()
		{

			timespec ts;
			::clock_gettime(CLOCK_REALTIME, &ts);
			return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
		}

		// Input Format: Epoch 1970: GTM: Nano Seconds
		// Output Example: "IST 2017-12-06 11:54:26.262681300"
		static void printTimestamp(std::ostream &os, int64_t timestamp)
		{

			std::time_t t = timestamp / 1000000000LL;
			// os << std::put_time(std::localtime(&t), "%Z %Y-%m-%d %T.") << std::setfill('0') << std::setw(9) << std::max(timestamp % 1000000000LL, 0LL);
			os << std::localtime(&t)->tm_zone << " " << 1900 + std::localtime(&t)->tm_year << "-"
			   << 1 + std::localtime(&t)->tm_mon << "-" << std::localtime(&t)->tm_mday << " "
			   << std::localtime(&t)->tm_hour << ":" << std::localtime(&t)->tm_min << ":"
			   << std::localtime(&t)->tm_sec 
			   << std::setfill('0') << std::setw(9) << std::max(timestamp % 1000000000LL, 0LL);
		}

		static date::year_month_day parseYYYYMMDD(int date)
		{

			date::year_month_day ymd;

			std::istringstream ss(boost::lexical_cast<std::string>(date));
			ss >> date::parse("%Y%m%d", ymd);

			return ymd;
		}

		static int getYYYYMMDD(date::year_month_day ymd)
		{

			std::ostringstream ss;
			ss << date::format("%Y%m%d", ymd);
			return std::stoi(ss.str());
		}

		static date::year_month_day getYearMonthDay(int64_t timestamp, const date::time_zone *tz)
		{

			auto tp = date::sys_seconds{std::chrono::seconds{timestamp / 1000000000L}};
			auto zt = date::make_zoned(tz, tp);

			return date::year_month_day{date::floor<date::days>(zt.get_local_time())};
		}

		static date::time_of_day<std::chrono::milliseconds> parseHHMMSSm(std::string hhmmss, bool hasColons)
		{

			std::chrono::milliseconds msecs;
			if (!hasColons)
			{
				std::size_t len = hhmmss.length();
				std::size_t f = hhmmss.find_first_of(".");
				if (f != std::string::npos)
					len = f;
				hhmmss = std::string(6 - len, '0') + hhmmss;
			}
			std::istringstream ss(hhmmss);
			ss >> date::parse(hasColons ? "%H:%M:%S" : "%H%M%S", msecs);
			if (!ss)
				throw std::string("Error: Time::parseHHMMSSm(): failed to parse: ") + hhmmss;
			date::time_of_day<std::chrono::milliseconds> tod{msecs};
			return tod;
		}

		static std::string getHHMMSSm(date::time_of_day<std::chrono::milliseconds> tod, bool withColons, bool withMilli)
		{

			std::ostringstream ss;
			if (withMilli)
				ss << date::format(withColons ? "%H:%M:%S" : "%H%M%S", tod.to_duration());
			else
			{
				ss << date::format(withColons ? "%H:%M:%S" : "%H%M%S", date::floor<std::chrono::seconds>(tod.to_duration()));
			}
			return ss.str();
		}

		static date::time_of_day<std::chrono::milliseconds> getTimeOfDayInMilliSec(int64_t timestamp, const date::time_zone *tz)
		{

			auto tp = date::sys_time<std::chrono::milliseconds>{std::chrono::milliseconds{timestamp / 1000000L}};
			auto zt = date::make_zoned(tz, tp);

			auto lt = zt.get_local_time();
			auto ld = date::floor<date::days>(lt);
			date::time_of_day<std::chrono::milliseconds> tod{lt - ld};
			return tod;
		}

		static int64_t getTimestamp(date::year_month_day ymd, date::time_of_day<std::chrono::milliseconds> tod, const date::time_zone *tz)
		{

			date::zoned_time<std::chrono::milliseconds> zt{tz, date::local_days{ymd} + tod.hours() + tod.minutes() + tod.seconds() + tod.subseconds()};
			return zt.get_sys_time().time_since_epoch().count() * 1000000LL;
		}

		static int64_t getDayStartTimestamp(date::year_month_day ymd, const date::time_zone *tz)
		{

			date::zoned_seconds zt{tz, date::local_days{ymd}};
			return zt.get_sys_time().time_since_epoch().count() * 1000000000L;
		}

		static const date::time_zone *getTimezoneIST()
		{
			return date::locate_zone("Asia/Kolkata");
		}
		static const date::time_zone *getTimezoneUTC()
		{
			return date::locate_zone("Etc/UTC");
		}

	private:
		static char cacheLineSeparator1[64];
		static int64_t virtualTimestamp;
		static char cacheLineSeparator2[64];
       };

}
