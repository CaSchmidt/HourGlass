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

#pragma once

#include <QtCore/QDate>
#include <QtCore/QString>

#include "Item.h"

struct Month {
  Month(const int year = 0, const int month = 0) noexcept
    : month{month}
    , year{year}
  {
  }

  bool isValid() const
  {
    return year >= 2000  &&  1 <= month  &&  month <= 12;
  }

  operator bool() const
  {
    return isValid();
  }

  int days() const
  {
    return QDate(year, month, 1).daysInMonth();
  }

  bool isMonth(const QDate& date) const
  {
    return date.year() == year  &&  date.month() == month;
  }

  QString toString() const
  {
    return QStringLiteral("%1-%2")
        .arg(year,  4, 10, QLatin1Char('0'))
        .arg(month, 2, 10, QLatin1Char('0'));
  }

  Items items;
  int month{0};
  int year{0};
};

inline bool operator<(const Month& a, const Month& b)
{
  if( a  &&  b ) {
    if(        a.year <  b.year ) {
      return true;
    } else if( a.year == b.year  &&  a.month < b.month ) {
      return true;
    }
  }

  return false;
}

inline bool operator==(const Month& a, const Month& b)
{
  return a  &&  b  &&  a.year == b.year  &&  a.month == b.month;
}
