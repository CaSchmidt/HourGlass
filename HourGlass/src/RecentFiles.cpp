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

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtWidgets/QMenu>

#include "RecentFiles.h"

////// Macros ////////////////////////////////////////////////////////////////

#define SETTINGS_GROUP  QStringLiteral("RecentFiles")
#define SETTINGS_VALUE  QStringLiteral("%1/filename%2")

////// public ////////////////////////////////////////////////////////////////

RecentFiles::RecentFiles(QWidget *parent)
  : QObject(parent)
{
  _menu = new QMenu(parent);
  clear();

  connect(_menu, &QMenu::aboutToShow,
          this, &RecentFiles::makeMenu);
}

RecentFiles::~RecentFiles()
{
  delete _menu;
}

QString RecentFiles::latest() const
{
  return !_recent.isEmpty()
      ? _recent.first()
      : QString();
}

QMenu *RecentFiles::menu()
{
  return _menu;
}

QString RecentFiles::takeLatest()
{
  return !_recent.isEmpty()
      ? _recent.takeFirst()
      : QString();
}

void RecentFiles::load(const QSettings& settings)
{
  clear();

  for(int i = 0; i < MAX_RECENT; i++) {
    const QString filename = settings.value(SETTINGS_VALUE
                                            .arg(SETTINGS_GROUP)
                                            .arg(i)).toString();
    if( filename.isEmpty() ) {
      break;
    }

    _recent.append(filename);
  }
}

void RecentFiles::save(QSettings& settings) const
{
  settings.remove(SETTINGS_GROUP);

  for(int i = 0; i < _recent.size(); i++) {
    const QString filename = _recent.at(i);
    if( filename.isEmpty() ) {
      continue;
    }

    settings.setValue(SETTINGS_VALUE
                      .arg(SETTINGS_GROUP)
                      .arg(i), filename);
  }
}

////// public slots //////////////////////////////////////////////////////////

void RecentFiles::add(const QString& filename)
{
  if( filename.isEmpty() ) {
    return;
  }

  _recent.prepend(filename);
  _recent.removeDuplicates();

  while( _recent.size() > MAX_RECENT ) {
    _recent.removeLast();
  }
}

void RecentFiles::clear()
{
  _recent.clear();
}

void RecentFiles::removeLatest()
{
  if( !_recent.isEmpty() ) {
    _recent.removeFirst();
  }
}

////// private slots /////////////////////////////////////////////////////////

void RecentFiles::makeMenu()
{
  _menu->clear();

  if( _recent.isEmpty() ) {
    return;
  }

  for(const QString& filename : qAsConst(_recent)) {
    addAction(filename);
  }

  _menu->addSeparator();

  QAction *action = _menu->addAction(tr("Clear"));
  connect(action, &QAction::triggered,
          this, &RecentFiles::clear);
}

void RecentFiles::triggerRecent()
{
  const QAction *action = dynamic_cast<const QAction*>(sender());
  if( action == nullptr ) {
    return;
  }

  const QString filename = action->data().toString();
  if( filename.isEmpty() ) {
    return;
  }

  if( _recent.removeAll(filename) < 1 ) {
    return;
  }

  emit selected(filename);
}

////// private ///////////////////////////////////////////////////////////////

void RecentFiles::addAction(const QString& filename)
{
  if( filename.isEmpty() ) {
    return;
  }

  QAction *action = _menu->addAction(QFileInfo(filename).fileName());
  action->setData(filename);
  action->setToolTip(filename);

  connect(action, &QAction::triggered,
          this, &RecentFiles::triggerRecent);
}
