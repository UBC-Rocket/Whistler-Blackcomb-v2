/*
 * Copyright (c) 2010-2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
/**
   @addtogroup util_time
   Implementations of time.h functions from the Standard C Library for
   platforms that may not support them, or for writing portable code to
   use January 1, 2000 as the epoch (as in the ZigBee specification).
   @{
   @file xbee_time.c
*/

/*** BeginHeader */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "xbee/platform.h"
#include "xbee/time.h"

#ifndef __DC__
   #define _xbee_time_debug
#elif defined XBEE_TIME_DEBUG
   #define _xbee_time_debug      __debug
#else
   #define _xbee_time_debug      __nodebug
#endif
/*** EndHeader */

/*** BeginHeader _xbee_time_jom, _xbee_time_dom */
extern const uint16_t _xbee_time_jom[2][12];
extern const uint8_t _xbee_time_dom[2][12];
/*** EndHeader */
// These declarations will cause problems in Dynamic C.  We'd like them to
// be private to this file, but the DC linker won't like that.

// Number of days since January 1st for the 1st of each month.
// _xbee_time_jom[is_leap][b]:
//    b is number of months since January
const uint16_t _xbee_time_jom[2][12] =
{
   {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},    // non-leap year
   {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}     // leap year
};

// Number of days in each month.
// _xbee_time_dom[is_leap][b]:
//    b is the number of months since January
const uint8_t _xbee_time_dom[2][12] =
{
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},        // non-leap year
   {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}         // leap year
};

/*** BeginHeader xbee_gmtime */
/*** EndHeader */
#ifdef __XBEE_PLATFORM_HCS08
   #pragma MESSAGE DISABLE C5909    // Assignment in condition is OK
#endif
/**
   @brief
   Converts a UTCTime value into a "broken-down time" (a struct tm) for
   directly accessing month, day, year, hour, minute and second, or for use
   with other functions from <time.h>.

   @param[out] dest_tm     Destination "struct tm" structure to hold the
                           broken-down time.  See time.h for details on that
                           structure.
   @param[in]  timestamp   Timestamp to convert.  Number of seconds since
                           1/1/2000 UTC.

   @return  Returns the \c dest_tm parameter.
*/
struct tm *xbee_gmtime( struct tm *dest_tm, xbee_utctime_t timestamp)
{
   struct tm t = { 0 };
   ldiv_t   lsplit;
   div_t    isplit;

   uint32_t seconds;
   uint16_t days;
   uint16_t diy;
   const uint8_t *dom;

   bool_t is_leap;

   if (dest_tm == NULL)
   {
      return NULL;
   }

   // split seconds into days and seconds
   // can't use ldiv(), since it's signed instead of unsigned
   // note that since the divisor is > 0xFFFF, days can fit in 16 bits
   seconds = timestamp % 86400;
   days = (uint16_t)(timestamp / 86400);

   ///// Populate Time Fields

   // split seconds into minutes and seconds
   lsplit = ldiv( seconds, 60);
   // split minutes into hours and minutes
   isplit = div( (int) lsplit.quot, 60);

   t.tm_sec = (int) lsplit.rem;
   t.tm_min = isplit.rem;
   t.tm_hour = isplit.quot;

   ///// Populate Date Fields

   t.tm_wday = (days + 6) % 7;
   t.tm_year = 100;
   for (;;)
   {
      // leap years are divisible by 4, except for 2100
      is_leap = (t.tm_year & 0x03) == 0 && t.tm_year != (2100 - 1900);
      diy = 365 + is_leap;
      if (days < diy)
      {
         break;
      }
      days -= diy;
      ++t.tm_year;
   };

   t.tm_isdst = 0;
   t.tm_yday = days;

   t.tm_mon = 0;
   dom = _xbee_time_dom[is_leap];
   while (days >= dom[t.tm_mon])
   {
      days -= dom[t.tm_mon];
      ++t.tm_mon;
   }
   t.tm_mday = days + 1;

   // copy back to caller and return
   *dest_tm = t;
   return dest_tm;
}
#ifdef __XBEE_PLATFORM_HCS08
   #pragma MESSAGE DEFAULT C5909    // restore C5909 (Assignment in condition)
#endif


/*** BeginHeader xbee_mktime */
/*** EndHeader */
#ifdef __XBEE_PLATFORM_HCS08
   #pragma MESSAGE DISABLE C5909    // Assignment in condition is OK
#endif
/** @internal

   Normalize \a minor such that it is in the range [0, \a count) by adding
   \a count to \a minor (and subtracting 1 from \a major) or subtracting
   \a count from \a minor (and adding 1 to \a major).
*/
void _xbee_mktime_normalize( int *major, int *minor, int count)
{
   div_t    d;

   d = div( *minor, count);
   if (d.rem < 0)
   {
      d.rem += count;
      d.quot -= 1;
   }

   *minor = d.rem;
   *major += d.quot;
}

/**
   @brief
   Convert a struct tm (from the Standard C Library's \c time.h) to a
   xbee_utctime_t type (number of seconds since Midnight January 1, 2000 UTC).

   Does NOT properly handle DST and Timezones in the struct tm.  Assumes the
   struct is in UTC.  Keep this in mind if passing a struct tm generated by
   the host's gmtime() to this function.

   @param[in]  time_rec    broken-down (componentized) calendar representation
                           of time

   @return  number of seconds since 01/01/2000 00:00:00 UTC or
            XBEE_UTCTIME_INVALID if \p time_rec is before 1/1/2000

   @todo Handle DST and time zones.
*/
xbee_utctime_t xbee_mktime( struct tm *time_rec)
{
   // Work with local copy of timeptr to reduce amount of pointer dereferencing.
   struct tm t = *time_rec;

   uint_fast8_t years;              // years since 2000
   uint16_t days;                   // days since 1/1/2000
   uint16_t yday;                   // day of the year
   uint_fast8_t daysinmonth;
   uint_fast8_t pastleapdays;
   bool_t is_leap;

   // normalize min/sec, hour/min, mday/hour and year/mon
   _xbee_mktime_normalize( &t.tm_min, &t.tm_sec, 60);    // 60 sec/min
   _xbee_mktime_normalize( &t.tm_hour, &t.tm_min, 60);   // 60 min/hour
   _xbee_mktime_normalize( &t.tm_mday, &t.tm_hour, 24);  // 24 hour/day
   _xbee_mktime_normalize( &t.tm_year, &t.tm_mon, 12);   // 12 mon/yr

   // normalize mon/mday by incrementing through the months

   // if day of month is < 1, roll back to previous month
   is_leap = (t.tm_year & 0x03) == 0 && t.tm_year != (2100 - 1900);
   while (t.tm_mday < 1)
   {
      if (t.tm_mon == 0)
      {
         // roll back January to December of previous year
         t.tm_mon = 11;
         t.tm_year -= 1;
         is_leap = (t.tm_year & 0x03) == 0 && t.tm_year != (2100 - 1900);
      }
      else
      {
         t.tm_mon -= 1;
      }
      t.tm_mday += _xbee_time_dom[is_leap][t.tm_mon];
   }

   // if day of month is > number of days in this month, roll to next month
   while (t.tm_mday > (daysinmonth = _xbee_time_dom[is_leap][t.tm_mon]))
   {
      t.tm_mday -= daysinmonth;
      if (t.tm_mon == 11)
      {
         // roll from December to January of next year
         t.tm_mon = 0;
         t.tm_year += 1;
         is_leap = (t.tm_year & 0x03) == 0 && t.tm_year != (2100 - 1900);
      }
      else
      {
         t.tm_mon += 1;
      }
   }

   // Date is normalized at this point -- year/month/day hour/minute/second

   if (t.tm_year < 100)
   {
      // year is before our epoch, return error
      return XBEE_UTCTIME_INVALID;
   }

   years = t.tm_year - 100;

   // determine day of the year using month and day of month
   yday = _xbee_time_jom[is_leap][t.tm_mon] + t.tm_mday - 1;

   // number of past leap days is:
   // the number of years / 4, plus 1 if this is not a leap year
   pastleapdays = (years >> 2) + (years & 0x03 ? 1 : 0);
   if (years > 100)           // 2100 was not a leap year, adjust accordingly
   {
      --pastleapdays;
   }

   // calculate days since 1/1/2000 (a Saturday)
   // 365 days per year since 2000, plus leap days, plus the current day of year
   days = (years * UINT16_C(365)) + pastleapdays + yday;
   t.tm_yday = yday;
   t.tm_wday = (days + 6) % 7;

   // Copy normalized struct back to caller and return seconds since 1/1/2000.
   *time_rec = t;
   return days * UINT32_C(86400)
                  + t.tm_hour * UINT32_C(3600) + t.tm_min * 60 + t.tm_sec;
}

#ifdef __XBEE_PLATFORM_HCS08
   #pragma MESSAGE DEFAULT C5909    // restore C5909 (Assignment in condition)
#endif

///@}
