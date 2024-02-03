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
#include <QtXml/QDomDocument>

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "Global.h"
#include "ProjectModel.h"
#include "XML_tags.h"

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
  // (1) Get filename ////////////////////////////////////////////////////////

  const QString filename = getFilename();
  if( filename.isEmpty() ) {
    return;
  }
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
    _lastfilename.clear();
    return;
  }

  // (3) Create XML //////////////////////////////////////////////////////////

  QDomDocument doc;

  QDomProcessingInstruction pi = doc.createProcessingInstruction(XML_pitarget, XML_pidata);
  doc.appendChild(pi);

  QDomElement xml_root = doc.createElement(XML_HourGlass);
  doc.appendChild(xml_root);

  output(&doc, &xml_root, global.projects);

  // (4) Serialize XML ///////////////////////////////////////////////////////

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << doc.toString(2);
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
