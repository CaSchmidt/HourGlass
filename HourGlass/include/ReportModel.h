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

#include <vector>

#include <QtCore/QAbstractTableModel>

#include "Month.h"

using ReportEntry = std::pair<projectid_t,numhour_t>;
using Report      = std::vector<ReportEntry>;

class ReportModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum Column : int {
    COL_Id = 0,
    COL_Name,
    COL_Annotation,
    COL_Hours,
    Num_Columns
  };

  ReportModel(QObject *parent = nullptr);
  ~ReportModel();

  void setMonth(const Month *month);

  int columnCount(const QModelIndex& index) const;
  QVariant data(const QModelIndex& index,
                int role) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const;
  int rowCount(const QModelIndex& index) const;

private:
  using size_type = std::size_t;

  bool isProjectRow(const int row) const;
  bool isSumRow(const int row) const;

  Report _report;
};
