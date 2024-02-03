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

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "Global.h"
#include "ProjectModel.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui{new Ui::WMainWindow}
{
  ui->setupUi(this);

  // TODO ////////////////////////////////////////////////////////////////////

  ui->hoursWidget->setMonth_TODO();

  // Actions /////////////////////////////////////////////////////////////////

  ui->openAction->setShortcut(Qt::CTRL + Qt::Key_O);
  ui->saveAction->setShortcut(Qt::CTRL + Qt::Key_S);
  ui->quitAction->setShortcut(Qt::CTRL + Qt::Key_Q);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->openAction, &QAction::triggered,
          this, &WMainWindow::open);

  connect(ui->saveAction, &QAction::triggered,
          this, &WMainWindow::save);
  connect(ui->saveAsAction, &QAction::triggered,
          this, &WMainWindow::saveAs);

  connect(ui->quitAction, &QAction::triggered,
          this, &WMainWindow::close);

  connect(ui->projectsWidget->model(), &ProjectModel::projectsChanged,
          ui->hoursWidget, &WWorkHours::updateProjects);
}

WMainWindow::~WMainWindow()
{
  delete ui;
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::open()
{
}

void WMainWindow::save()
{
}

void WMainWindow::saveAs()
{
}
