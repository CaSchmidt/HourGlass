/****************************************************************************
** Copyright (c) 2024, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include <QtCore/QDate>
#include <QtCore/QLocale>

#include "Month.h"

////// public ////////////////////////////////////////////////////////////////

Month::Month(const int year, const int month) noexcept
  : _month{month}
  , _year{year}
{
}

Month::Month(const QDate& date) noexcept
{
  if( date.isValid() ) {
    _year = date.year();
    _month = date.month();
  }
}

bool Month::isValid() const
{
  return QDate::isValid(_year, _month, 1);
}

monthid_t Month::id() const
{
  return isValid()
      ? make_monthid(_year, _month)
      : INVALID_MONTHID;
}

bool Month::add(Item i)
{
  if( !i ) {
    return false;
  }

  items.push_back(std::move(i));

  return true;
}

int Month::days() const
{
  return QDate(_year, _month, 1).daysInMonth();
}

bool Month::isCurrentDay(const int day) const
{
  return QDate(_year, _month, day) == QDate::currentDate();
}

bool Month::isMonday(const int day) const
{
  return QDate(_year, _month, day).dayOfWeek() == Qt::Monday;
}

bool Month::isMonth(const QDate& date) const
{
  return date.year() == _year  &&  date.month() == _month;
}

bool Month::isWeekend(const int day) const
{
  const int weekDay = QDate(_year, _month, day).dayOfWeek();

  return weekDay == Qt::Saturday  ||  weekDay == Qt::Sunday;
}

numhour_t Month::sumDayHours(const std::size_t day) const
{
  if( day >= MAX_WORKDAYS ) {
    return 0;
  }

  numhour_t result = 0;
  for(const Item& item : items) {
    result += item.hours[day];
  }

  return result;
}

QString Month::toLocaleString() const
{
  const QDate date(_year, _month, 1);
  return QLocale().toString(date, QStringLiteral("MMMM yyyy"));
}

QString Month::toString() const
{
  return QStringLiteral("%1-%2")
      .arg(_year,  4, 10, QLatin1Char('0'))
      .arg(_month, 2, 10, QLatin1Char('0'));
}

int Month::weekNumber(const int day) const
{
  return QDate(_year, _month, day).weekNumber();
}

bool Month::operator<(const Month& other) const
{
  return isValid()  &&  other  &&  id() < other.id();
}

bool Month::operator>(const Month& other) const
{
  return isValid()  &&  other  &&  id() > other.id();
}

bool Month::operator==(const Month& other) const
{
  return isValid()  &&  other  &&  id() == other.id();
}

bool Month::operator==(const monthid_t id) const
{
  return isValid()  &&  this->id() == id;
}

////// Public ////////////////////////////////////////////////////////////////

monthid_t make_monthid(const int year, const int month)
{
  return year*100 + month;
}

SplitId split_monthid(const monthid_t id)
{
  return SplitId(id/100, id%100);
}
