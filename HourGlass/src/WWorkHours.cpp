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

void WWorkHours::setMonth_TODO()
{
  _model->setMonth(&_month);
}

////// public slots //////////////////////////////////////////////////////////

void WWorkHours::updateProjects()
{
  if( Global::projects == nullptr ) {
    return;
  }

  const ProjectModel *projects = Global::projects;

  // Projects Combo //////////////////////////////////////////////////////////

  ui->projectCombo->clear();
  for(std::size_t i = 0; i < projects->projectCount(); i++) {
    const Project p = projects->projectAt(i);
    ui->projectCombo->addItem(p.name, p.id);
  }

  // Data Model //////////////////////////////////////////////////////////////

  _model->updateProjects();

  // Hours View Actions //////////////////////////////////////////////////////

  {
    QAction *action = nullptr;

    action = new QAction(tr("Resize days"), ui->hoursView);
    connect(action, &QAction::triggered,
            this, &WWorkHours::resizeDays);
    ui->hoursView->addAction(action);
  }

  ui->hoursView->setContextMenuPolicy(Qt::ActionsContextMenu);
}

////// private slots /////////////////////////////////////////////////////////

void WWorkHours::addItem()
{
  const projectid_t id = ui->projectCombo->currentData().value<projectid_t>();
  _model->addItem(id);
}

void WWorkHours::resizeDays()
{
  QHeaderView *view = ui->hoursView->horizontalHeader();
  if( view == nullptr ) {
    return;
  }
  view->resizeSections(QHeaderView::ResizeToContents);
}
