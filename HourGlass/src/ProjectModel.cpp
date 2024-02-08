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

#include "ProjectModel.h"

#include "Global.h"

////// public ////////////////////////////////////////////////////////////////

ProjectModel::ProjectModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

ProjectModel::~ProjectModel()
{
}

void ProjectModel::addProject(const QString& name)
{
  if( name.isEmpty() ) {
    return;
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  global.add(global.makeProject(name));
  endInsertRows();

  emit projectsChanged();
}

void ProjectModel::clearProjects()
{
  setProjects(Projects());
}

void ProjectModel::setProjects(Projects projects)
{
  beginResetModel();
  global.projects = std::move(projects);
  endResetModel();

  emit projectsChanged();
}

int ProjectModel::columnCount(const QModelIndex& /*index*/) const
{
  return Num_Columns;
}

QVariant ProjectModel::data(const QModelIndex& index,
                            int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  const Projects& projects = global.projects;

  if(        role == Qt::DisplayRole ) {
    const std::size_t row = index.row();
    if(        index.column() == COL_Id ) {
      return projects[row].id();
    } else if( index.column() == COL_Name ) {
      return projects[row].name;
    }

  } else if( role == Qt::EditRole ) {
    const std::size_t row = index.row();
    if( index.column() == COL_Name ) {
      return projects[row].name;
    }

  }

  return QVariant();
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  if( index.column() == COL_Name ) {
    flags.setFlag(Qt::ItemIsEditable);
  }
  return flags;
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
  if( role == Qt::DisplayRole ) {
    if( orientation == Qt::Horizontal ) {
      if(        section == 0 ) {
        return tr("ID");
      } else if( section == 1 ) {
        return tr("Name");
      }
    }
  }

  return QVariant();
}

int ProjectModel::rowCount(const QModelIndex& /*index*/) const
{
  return int(global.projects.size());
}

bool ProjectModel::setData(const QModelIndex& index, const QVariant& value,
                           int role)
{
  const QString name = value.toString();

  if( role != Qt::EditRole  ||  index.column() != COL_Name  ||  name.isEmpty() ) {
    return false;
  }

  const std::size_t row = index.row();
  global.projects[row].name = name;

  emit dataChanged(index, index);
  emit projectsChanged();

  return true;
}
