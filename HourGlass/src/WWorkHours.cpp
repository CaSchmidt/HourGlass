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

#include <QtWidgets/QAction>

#include "WWorkHours.h"
#include "ui_WWorkHours.h"

#include "Global.h"
#include "MonthModel.h"

////// public ////////////////////////////////////////////////////////////////

WWorkHours::WWorkHours(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui{new Ui::WWorkHours}
{
  ui->setupUi(this);

  // Calendar ////////////////////////////////////////////////////////////////

  ui->dateEdit->setDate(QDate::currentDate());

  // Data Model //////////////////////////////////////////////////////////////

  _model = new MonthModel(ui->hoursView);
  ui->hoursView->setModel(_model);

  // Hours View Actions //////////////////////////////////////////////////////

  initHoursMenu();

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addItemButton, &QPushButton::clicked,
          this, &WWorkHours::addItem);

  // TODO ////////////////////////////////////////////////////////////////////

  _month = Month(2024, 2);
}

WWorkHours::~WWorkHours()
{
  delete ui;
}

void WWorkHours::clear()
{
  ui->monthCombo->clear();
  ui->projectCombo->clear();
  _model->clearMonth();
}

void WWorkHours::initializeUi(Months months)
{
  global.months = std::move(months);
}

MonthModel *WWorkHours::model() const
{
  return _model;
}

void WWorkHours::setMonth_TODO()
{
  _model->setMonth(&_month);
}

////// public slots //////////////////////////////////////////////////////////

void WWorkHours::updateProjects()
{
  // Projects Combo //////////////////////////////////////////////////////////

  ui->projectCombo->clear();
  for(const Project& p : global.projects) {
    ui->projectCombo->addItem(p.name, p.id());
  }

  // Data Model //////////////////////////////////////////////////////////////

  _model->updateProjects();
}

////// private slots /////////////////////////////////////////////////////////

void WWorkHours::addItem()
{
  const projectid_t id = ui->projectCombo->currentData().value<projectid_t>();
  _model->addItem(id);
}

void WWorkHours::resizeColumns()
{
  QHeaderView *view = ui->hoursView->horizontalHeader();
  if( view == nullptr ) {
    return;
  }
  view->resizeSections(QHeaderView::ResizeToContents);
}

void WWorkHours::showMonth()
{
  QHeaderView *view = ui->hoursView->horizontalHeader();
  if( view == nullptr ) {
    return;
  }

  for(int i = MonthModel::Num_ItemColumns; i < _model->columnCount(); i++) {
    view->showSection(i);
  }
}

void WWorkHours::showWeek()
{
  QHeaderView *view = ui->hoursView->horizontalHeader();
  if( view == nullptr  ||  !_model->isValid() ) {
    return;
  }

  const int currentWeek = QDate::currentDate().weekNumber();
  const Month *month = _model->month();

  for(int i = MonthModel::Num_ItemColumns; i < _model->columnCount(); i++) {
    if( month->weekNumber(_model->day(i)) == currentWeek ) {
      view->showSection(i);
    } else {
      view->hideSection(i);
    }
  }
}

////// private ///////////////////////////////////////////////////////////////

void WWorkHours::initHoursMenu()
{
  QAction *action = nullptr;

  action = new QAction(tr("Resize columns"), ui->hoursView);
  connect(action, &QAction::triggered,
          this, &WWorkHours::resizeColumns);
  ui->hoursView->addAction(action);

  action = new QAction(tr("Show month"), ui->hoursView);
  connect(action, &QAction::triggered,
          this, &WWorkHours::showMonth);
  ui->hoursView->addAction(action);

  action = new QAction(tr("Show week"), ui->hoursView);
  connect(action, &QAction::triggered,
          this, &WWorkHours::showWeek);
  ui->hoursView->addAction(action);

  ui->hoursView->setContextMenuPolicy(Qt::ActionsContextMenu);
}
