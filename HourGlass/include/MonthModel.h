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

#include <QtCore/QAbstractTableModel>

#include "Project.h"

struct Month;

class MonthModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum Columns : int {
    COL_Project = 0,
    COL_Activity,
    COL_Hours,
    Num_ItemColumns
  };

  MonthModel(QObject *parent = nullptr);
  ~MonthModel();

  void addItem(const projectid_t id);
  void clearMonth();
  int day(const int column) const;
  bool isCurrentMonth() const;
  bool isDayColumn(const int column) const;
  bool isValid() const;
  Month *month() const;
  void setMonth(Month *month);
  void updateProjects();

  int columnCount(const QModelIndex& index = QModelIndex()) const;
  QVariant data(const QModelIndex& index,
                int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const;
  int rowCount(const QModelIndex& index = QModelIndex()) const;
  bool setData(const QModelIndex& index, const QVariant& value,
               int role);

private:
  bool isDayHoursRow(const std::size_t row) const;
  bool isItemRow(const std::size_t row) const;

  Month *_month{nullptr};

signals:
  void monthChanged(const QString&);
};
