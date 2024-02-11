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

#include <QtCore/QDate>
#include <QtCore/QLocale>
#include <QtGui/QBrush>

#include "MonthModel.h"

#include "Global.h"
#include "Month.h"

////// public ////////////////////////////////////////////////////////////////

MonthModel::MonthModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

MonthModel::~MonthModel()
{
}

void MonthModel::addItem(const projectid_t id)
{
  if( !isValid() ) {
    return;
  }

  const Project *p = global.findProject(id);
  if( p == nullptr ) {
    return;
  }

  beginInsertRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
  _month->add(Item(p->id()));
  endInsertRows();

  global.setModified();
}

void MonthModel::clearMonth()
{
  setMonth(nullptr);
}

int MonthModel::day(const int column) const
{
  return isValid()  &&  isDayColumn(column)
      ? column - Num_ItemColumns + 1
      : 0;
}

bool MonthModel::isCurrentMonth() const
{
  return isValid()
      ? _month->isMonth(QDate::currentDate())
      : false;
}

bool MonthModel::isDayColumn(const int column) const
{
  return isValid()
      ? Num_ItemColumns <= column  &&  column < Num_ItemColumns + _month->days()
      : false;
}

bool MonthModel::isValid() const
{
  return _month != nullptr;
}

Month *MonthModel::month() const
{
  return _month;
}

void MonthModel::setMonth(Month *month)
{
  if( month != nullptr  &&  !month->isValid() ) {
    return;
  }

  beginResetModel();
  _month = month;
  endResetModel();

  if( month != nullptr ) {
    emit monthChanged(_month->toLocaleString());
  } else {
    emit monthChanged(QString());
  }
}

void MonthModel::updateProjects()
{
  if( !isValid() ) {
    return;
  }

  emit dataChanged(index(0             , COL_Project),
                   index(rowCount() - 1, COL_Project));
}

int MonthModel::columnCount(const QModelIndex& /*index*/) const
{
  return isValid()
      ? Num_ItemColumns + _month->days()
      : 0;
}

QVariant MonthModel::data(const QModelIndex& index,
                          int role) const
{
  if( !isValid()  ||  !index.isValid() ) {
    return QVariant();
  }

  const int      column = index.column();
  const std::size_t row = index.row();

  if(        role == Qt::DisplayRole ) {
    if( isItemRow(row) ) {
      const Item& item = _month->items[row];

      if(        column == COL_Project ) {
        const Project *p = global.findProject(item.projectId);
        if( p != nullptr ) {
          return p->name;
        }
      } else if( column == COL_Description ) {
        return item.description;
      } else if( column == COL_Hours ) {
        return toString(item.sumHours());
      } else if( isDayColumn(column) ) {
        return toString(item.hours[column - Num_ItemColumns], true);
      }

    } else if( isDayHoursRow(row) ) {
      if( isDayColumn(column) ) {
        return toString(_month->sumDayHours(column - Num_ItemColumns));
      }

    } // row

  } else if( role == Qt::EditRole ) {
    if( isItemRow(row) ) {
      const Item& item = _month->items[row];

      if( column == COL_Description ) {
        return item.description;
      } else if( isDayColumn(column) ) {
        return toString(item.hours[column - Num_ItemColumns]);
      }
    }

  } else if( role == Qt::BackgroundRole ) {
    if(        isItemRow(row) ) {
      if(        column == COL_Hours ) {
        return QBrush(Qt::yellow);
      } else if( isDayColumn(column) ) {
        if( _month->isWeekend(day(column)) ) {
          return QBrush(Qt::cyan);
        }
      }
    } else if( isDayHoursRow(row) ) {
      if( isDayColumn(column) ) {
        return QBrush(Qt::yellow);
      }
    }

  } // Qt::ItemDataRole

  return QVariant();
}

Qt::ItemFlags MonthModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);

  if( !isValid()  ||  !index.isValid() ) {
    return flags;
  }

  const int      column = index.column();
  const std::size_t row = index.row();

  if( isItemRow(row) ) {
    if( column == COL_Description ) {
      flags |= Qt::ItemIsEditable;
    } else if( isDayColumn(column) ) {
      flags |= Qt::ItemIsEditable;
    }
  }

  return flags;
}

QVariant MonthModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
  if( !isValid() ) {
    return QVariant();
  }

  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      if(        section == COL_Project ) {
        return tr("Project");
      } else if( section == COL_Description ) {
        return tr("Description");
      } else if( section == COL_Hours ) {
        return tr("Hours");
      } else if( isDayColumn(section) ) {
        const int day = MonthModel::day(section);
        if( _month->isMonday(day) ) {
          return QStringLiteral("[%1] %2")
              .arg(_month->weekNumber(day), 2, 10, QLatin1Char('0'))
              .arg(day);
        } else if( _month->isCurrentDay(day) ) {
          return QStringLiteral(">%1<")
              .arg(day);
        } else {
          return day;
        }
      }

    } else if( orientation == Qt::Vertical ) {
      if( isItemRow(section) ) {
        return section + 1;
      }

    }

  } else if( role == Qt::ForegroundRole ) {
    if( isDayColumn(section) ) {
      if( _month->isCurrentDay(day(section)) ) {
        return QBrush(Qt::red);
      }
    }

  } // DisplayRole

  return QVariant();
}

int MonthModel::rowCount(const QModelIndex& /*index*/) const
{
  return isValid()
      ? int(_month->items.size()) + 1
      : 0;
}

bool MonthModel::setData(const QModelIndex& index, const QVariant& value,
                         int role)
{
  if( !isValid()  ||  !index.isValid() ) {
    return false;
  }

  const int      column = index.column();
  const std::size_t row = index.row();

  if( role == Qt::EditRole ) {
    if( isItemRow(row) ) {
      Item& item = _month->items[row];

      if( column == COL_Description ) {
        item.description = value.toString();

        emit dataChanged(index, index);

        global.setModified();

        return true;

      } else if( isDayColumn(column) ) {
        item.hours[column - Num_ItemColumns] = toDouble(value.toString());

        emit dataChanged(index, index);

        const QModelIndex hrsIdx = MonthModel::index(index.row(), COL_Hours);
        emit dataChanged(hrsIdx, hrsIdx);

        const QModelIndex dyHrsIdx = MonthModel::index(rowCount() - 1, column);
        emit dataChanged(dyHrsIdx, dyHrsIdx);

        global.setModified();

        return true;

      } // column

    } // row
  } // Qt::ItemRole

  return false;
}

////// private ///////////////////////////////////////////////////////////////

bool MonthModel::isDayHoursRow(const std::size_t row) const
{
  return row == _month->items.size();
}

bool MonthModel::isItemRow(const std::size_t row) const
{
  return row < _month->items.size();
}

double MonthModel::toDouble(const QString& hours)
{
  return QLocale().toDouble(hours);
}

QString MonthModel::toString(const double hours, const bool no_zero)
{
  return no_zero  &&  hours == 0
      ? QString()
      : QLocale().toString(hours, 'f', 2);
}
