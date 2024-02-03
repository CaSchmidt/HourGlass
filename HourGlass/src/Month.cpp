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

#include <algorithm>

#include <QtCore/QDate>

#include "Month.h"

////// public ////////////////////////////////////////////////////////////////

Month::Month(const int year, const int month) noexcept
  : _month{month}
  , _year{year}
{
}

bool Month::isValid() const
{
  return QDate::isValid(_year, _month, 1);
}

int Month::id() const
{
  return isValid()
      ? _year*100 + _month
      : 0;
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

bool Month::operator==(const Month& other) const
{
  return isValid()  &&  other  &&  id() == other.id();
}

bool Month::operator==(const int id) const
{
  return isValid()  &&  this->id() == id;
}

////// Public ////////////////////////////////////////////////////////////////

bool addMonth(Months *list, Month month)
{
  if( list == nullptr  ||  !month  ||  isMonth(*list, month.id()) ) {
    return false;
  }

  list->push_back(std::move(month));

  return true;
}

Month *findMonth(const Months& list, const int id)
{
  const auto hit = std::find(list.cbegin(), list.cend(), id);

  return hit != list.cend()
      ? &const_cast<Month&>(*hit)
      : nullptr;
}

bool isMonth(const Months& list, const int id)
{
  return std::find(list.cbegin(), list.cend(), id) != list.cend();
}
