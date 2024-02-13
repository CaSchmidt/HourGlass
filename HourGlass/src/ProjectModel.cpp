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

  beginResetModel();
  global.add(global.makeProject(name));
  _projects = global.listProjects();
  endResetModel();

  emit projectsChanged();
}

void ProjectModel::clearProjects()
{
  setProjects(ProjectDB());
}

void ProjectModel::setProjects(ProjectDB projects)
{
  beginResetModel();
  global.set(std::move(projects));
  _projects = global.listProjects();
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

  const int column = index.column();

  const Project *p = project(index.row());
  if( p == nullptr ) {
    return QVariant();
  }

  if(        role == Qt::DisplayRole ) {
    if(        column == COL_Id ) {
      return p->id();
    } else if( column == COL_Name ) {
      return p->name;
    } else if( column == COL_Annotation ) {
      return p->annotation;
    }

  } else if( role == Qt::EditRole ) {
    if(        column == COL_Name ) {
      return p->name;
    } else if( column == COL_Annotation ) {
      return p->annotation;
    }

  } // Qt::ItemDataRole

  return QVariant();
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  if(        index.column() == COL_Name ) {
    flags.setFlag(Qt::ItemIsEditable);
  } else if( index.column() == COL_Annotation ) {
    flags.setFlag(Qt::ItemIsEditable);
  }
  return flags;
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
  if( role == Qt::DisplayRole ) {
    if( orientation == Qt::Horizontal ) {
      if(        section == COL_Id ) {
        return tr("ID");
      } else if( section == COL_Name ) {
        return tr("Name");
      } else if( section == COL_Annotation ) {
        return tr("Annotation");
      }
    }
  }

  return QVariant();
}

int ProjectModel::rowCount(const QModelIndex& /*index*/) const
{
  return int(_projects.size());
}

bool ProjectModel::setData(const QModelIndex& index, const QVariant& value,
                           int role)
{
  if( !index.isValid()  ||  role != Qt::EditRole ) {
    return false;
  }

  const int column = index.column();

  Project *p = project(index.row());
  if( p == nullptr ) {
    return false;
  }

  if( column == COL_Name ) {
    const QString name = value.toString();

    if( !name.isEmpty() ) {
      p->name = name;

      emit dataChanged(index, index);
      emit projectsChanged();

      global.setModified();

      return true;
    }

  } else if( column == COL_Annotation ) {
    const QString annotation = value.toString();

    p->annotation = annotation;

    emit dataChanged(index, index);

    global.setModified();

    return true;

  } // column

  return false;
}

////// private ///////////////////////////////////////////////////////////////

Project *ProjectModel::project(const int row) const
{
  return 0 <= row  &&  row < rowCount()
      ? global.findProject(_projects[std::size_t(row)])
      : nullptr;
}
