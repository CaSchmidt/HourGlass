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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtXml/QDomDocument>

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "Global.h"
#include "ProjectModel.h"
#include "XML_io.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui{new Ui::WMainWindow}
{
  ui->setupUi(this);

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

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::open()
{
  // (1) Get filename ////////////////////////////////////////////////////////

  const QString filename = getFilename();
  if( filename.isEmpty() ) {
    return;
  }

  _lastfilename = filename;
  const QString fileInfo = QFileInfo(_lastfilename).fileName();

  // (2) Open file for reading ///////////////////////////////////////////////

  QFile file(_lastfilename);
  if( !file.open(QFile::ReadOnly) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to open file \"%1\"!")
                          .arg(fileInfo));
    _lastfilename.clear();
    return;
  }

  // (3) Deserialize XML /////////////////////////////////////////////////////

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  const QString xmlContent = stream.readAll();
  file.close();

  // (4) Parse XML ///////////////////////////////////////////////////////////

  Context context;
  if( !xmlRead(&context, xmlContent, this) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to read XML file \"%1\"!")
                          .arg(fileInfo));
    _lastfilename.clear();
    return;
  }

  // (5) Update UI ///////////////////////////////////////////////////////////

  ui->hoursWidget->clear();
  ui->projectsWidget->clear();

  ui->projectsWidget->initializeUi(std::move(context.projects));
  ui->hoursWidget->initializeUi(std::move(context.months));

  // Done! ///////////////////////////////////////////////////////////////////
}

void WMainWindow::save()
{
  // (1) Get filename ////////////////////////////////////////////////////////

  _lastfilename = _lastfilename.isEmpty()
      ? getFilename(true)
      : _lastfilename;
  if( _lastfilename.isEmpty() ) {
    return;
  }

  // (2) Open file for writing ///////////////////////////////////////////////

  QFile file(_lastfilename);
  if( !file.open(QFile::WriteOnly) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Unable to save file \"%1\"!")
                          .arg(QFileInfo(_lastfilename).fileName()));
    _lastfilename.clear();
    return;
  }

  // (3) Create XML //////////////////////////////////////////////////////////

  const QString xmlContent = xmlWrite(global, this);

  // (4) Serialize XML ///////////////////////////////////////////////////////

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << xmlContent;
  stream.flush();

  // Done! ///////////////////////////////////////////////////////////////////

  file.close();
}

void WMainWindow::saveAs()
{
  const QString filename = getFilename(true);
  if( !filename.isEmpty() ) {
    _lastfilename = filename;
    save();
  }
}

////// private ///////////////////////////////////////////////////////////////

QString WMainWindow::getFilename(const bool is_save)
{
  const QString dir = !_lastfilename.isEmpty()
      ? QFileInfo(_lastfilename).canonicalPath()
      : QString();

  const QString filename = is_save
      ? QFileDialog::getSaveFileName(this, tr("Save as"),
                                     dir, tr("HourGlass files (*.xml)"))
      : QFileDialog::getOpenFileName(this, tr("Open"),
                                     dir, tr("HourGlass files (*.xml)"));

  return filename;
}
