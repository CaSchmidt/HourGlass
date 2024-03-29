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

#include "WProjects.h"
#include "ui_WProjects.h"

#include "ProjectModel.h"

#include "Global.h"

////// public ////////////////////////////////////////////////////////////////

WProjects::WProjects(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui{new Ui::WProjects}
{
  ui->setupUi(this);

  // Data Model //////////////////////////////////////////////////////////////

  _model = new ProjectModel(ui->tableView); // takes ownership!
  ui->tableView->setModel(_model);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addButton, &QPushButton::clicked,
          this, &WProjects::addProject);

  connect(ui->nameEdit, &QLineEdit::returnPressed,
          ui->addButton, &QPushButton::click);
}

WProjects::~WProjects()
{
  delete ui;
}

void WProjects::clear()
{
  _model->clearProjects();
}

void WProjects::initializeUi(ProjectDB projects)
{
  _model->setProjects(std::move(projects));
}

ProjectModel *WProjects::model() const
{
  return _model;
}

////// private slots /////////////////////////////////////////////////////////

void WProjects::addProject()
{
  _model->addProject(ui->nameEdit->text());
  ui->nameEdit->clear();
}
