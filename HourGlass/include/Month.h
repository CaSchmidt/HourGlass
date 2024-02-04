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

#include <utility>

#include <QtCore/QString>

#include "Item.h"

class QDate;

struct Month {
  Month(const int year = 0, const int month = 0) noexcept;

  bool isValid() const;

  inline explicit operator bool() const
  {
    return isValid();
  }

  int id() const;

  bool add(Item i);
  int days() const;
  bool isCurrentDay(const int day) const;
  bool isMonday(const int day) const;
  bool isMonth(const QDate& date) const;
  bool isWeekend(const int day) const;
  // day := [0,30]
  numhour_t sumDayHours(const std::size_t day) const;
  QString toString() const;
  int weekNumber(const int day) const;

  bool operator<(const Month& other) const;
  bool operator>(const Month& other) const;
  bool operator==(const Month& other) const;
  bool operator==(const int id) const;

  Items items;

private:
  int _month{0};
  int _year{0};
};

using Months = std::vector<Month>;

int make_monthid(const int year, const int month);

std::pair<int,int> split_monthid(const int id);
