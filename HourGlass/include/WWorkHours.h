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

#include <QtWidgets/QWidget>

#include "Month.h"

namespace Ui {
  class WWorkHours;
} // namespace Ui

class MonthModel;
class QSettings;

class WWorkHours : public QWidget {
  Q_OBJECT
public:
  WWorkHours(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~WWorkHours();

  void clear();
  void initializeUi(MonthDB months);

  MonthModel *model() const;

  void load(const QSettings& settings);
  void save(QSettings& settings) const;

public slots:
  void updateProjects();

private slots:
  void addItem();
  void addMonth();
  void fitColumns();
  void generateReport();
  void resetColumns();
  void setMonth(int index);
  void showWeek();
  void updateMonth(const QString& s);

private:
  void initHoursMenu();
  void initMonthsCombo();

  MonthModel *_model{nullptr};
  Ui::WWorkHours *ui{nullptr};
};
