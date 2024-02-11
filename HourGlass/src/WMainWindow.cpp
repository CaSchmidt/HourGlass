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

#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "File_io.h"
#include "Global.h"
#include "ProjectModel.h"
#include "RecentFiles.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui{new Ui::WMainWindow}
{
  ui->setupUi(this);

  // Recent Files ////////////////////////////////////////////////////////////

  _recent = new RecentFiles(this);
  ui->recentFilesAction->setMenu(_recent->menu());

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

  connect(_recent, &RecentFiles::selected,
          this, &WMainWindow::openFile);

  // Settings ////////////////////////////////////////////////////////////////

  loadSettings();
}

WMainWindow::~WMainWindow()
{
  saveSettings();

  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::open()
{
  const QString filename = getFilename();
  if( filename.isEmpty() ) {
    return;
  }

  openFile(filename);
}

void WMainWindow::openFile(const QString& filename)
{
  if( filename.isEmpty() ) {
    return;
  }

  // (1) Read Hours file /////////////////////////////////////////////////////

  Context context;
  if( !readHoursFile(context, filename, this) ) {
    return;
  }

  _recent->add(filename);

  // (2) Update UI ///////////////////////////////////////////////////////////

  ui->hoursWidget->clear();
  ui->projectsWidget->clear();

  // TODO
  ui->projectsWidget->initializeUi(std::move(context._projects));
  ui->hoursWidget->initializeUi(std::move(context._months));
}

void WMainWindow::save()
{
  // (1) Get filename ////////////////////////////////////////////////////////

  QString filename = _recent->takeLatest();
  filename = filename.isEmpty()
      ? getFilename(true)
      : filename;
  if( filename.isEmpty() ) {
    return;
  }

  // (2) Backup Hours file ///////////////////////////////////////////////////

  backupHoursFile(filename);

  // (3) Write Hours file ////////////////////////////////////////////////////

  if( !writeHoursFile(filename, global, this) ) {
    return;
  }

  _recent->add(filename);
}

void WMainWindow::saveAs()
{
  const QString filename = getFilename(true);
  if( !filename.isEmpty() ) {
    _recent->add(filename);
    save();
  }
}

////// private ///////////////////////////////////////////////////////////////

QString WMainWindow::getFilename(const bool is_save)
{
  const QString lastfilename = _recent->latest();

  const QString dir = !lastfilename.isEmpty()
      ? QFileInfo(lastfilename).canonicalPath()
      : QString();

  const QString filename = is_save
      ? QFileDialog::getSaveFileName(this, tr("Save as"),
                                     dir, tr("HourGlass files (*.xml)"))
      : QFileDialog::getOpenFileName(this, tr("Open"),
                                     dir, tr("HourGlass files (*.xml)"));

  return filename;
}

void WMainWindow::loadSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("csLabs"),
                     QStringLiteral("HourGlass"));

  _recent->load(settings);
}

void WMainWindow::saveSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("csLabs"),
                     QStringLiteral("HourGlass"));
  settings.clear();

  _recent->save(settings);

  settings.sync();
}
