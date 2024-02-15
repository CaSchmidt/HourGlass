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

#include <QtWidgets/QComboBox>

#include "ProjectDelegate.h"

#include "Global.h"

////// public ////////////////////////////////////////////////////////////////

ProjectDelegate::ProjectDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

ProjectDelegate::~ProjectDelegate()
{
}

QWidget *ProjectDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem& /*option*/,
                                       const QModelIndex& /*index*/) const
{
  QComboBox *combo = new QComboBox(parent);
  combo->clear();
  combo->setFrame(false);
  combo->setMaxVisibleItems(MAX_VISIBLE);

  const ProjectIDs projects = global.listProjects();
  for(const projectid_t id : projects) {
    const Project *p = global.findProject(id);
    if( p == nullptr ) {
      continue;
    }

    combo->addItem(p->name, p->id());
  }

  return combo;
}

void ProjectDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex& index) const
{
  QComboBox *combo = dynamic_cast<QComboBox*>(editor);

  const projectid_t id = index.model()->data(index, Qt::EditRole).value<projectid_t>();

  int at = -1;
  for(int i = 0; i < combo->count(); i++) {
    if( combo->itemData(i).value<projectid_t>() == id ) {
      at = i;
      break;
    }
  }

  if( at < 0 ) {
    return;
  }

  combo->setCurrentIndex(at);
}

void ProjectDelegate::setModelData(QWidget *editor,
                                   QAbstractItemModel *model,
                                   const QModelIndex& index) const
{
  QComboBox *combo = dynamic_cast<QComboBox*>(editor);

  const projectid_t id = combo->currentData().value<projectid_t>();

  model->setData(index, id, Qt::EditRole);
}

void ProjectDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem& option,
                                           const QModelIndex& /*index*/) const
{
  editor->setGeometry(option.rect);
}
