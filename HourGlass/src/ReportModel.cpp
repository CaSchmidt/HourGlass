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

#include <unordered_map>

#include <QtGui/QBrush>

#include "ReportModel.h"

#include "Global.h"
#include "View.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  using ReportMap = std::unordered_map<projectid_t,numhour_t>;

  Report generateReport(const Month& month)
  {
    if( !month ) {
      return Report();
    }

    // (1) Sum each Item's hours /////////////////////////////////////////////

    ReportMap map;
    for(const Item& item : month.items) {
      if( !map.contains(item.projectId) ) {
        map[item.projectId] = numhour_t{0};
      }

      map[item.projectId] += item.sumHours();
    } // Item

    // (2) unordered_map<> to vector<> ///////////////////////////////////////

    Report result;
    result.reserve(map.size());
    for(const auto& v : map) {
      result.emplace_back(v.first, v.second);
    }

    // (3) Sort by Project's ID //////////////////////////////////////////////

    std::sort(result.begin(), result.end());

    return result;
  }

  auto lambda_sum = [](const numhour_t& lhs, const ReportEntry& rhs) -> numhour_t
  {
    return lhs + rhs.second;
  };

  numhour_t sum(const Report& report)
  {
    return std::accumulate(report.cbegin(), report.cend(),
                           numhour_t{0}, lambda_sum);
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

ReportModel::ReportModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

ReportModel::~ReportModel()
{
}

void ReportModel::setMonth(const Month *month)
{
  if( month == nullptr ) {
    return;
  }

  beginResetModel();
  _report = priv::generateReport(*month);
  endResetModel();
}

int ReportModel::columnCount(const QModelIndex& /*index*/) const
{
  return Num_Columns;
}

QVariant ReportModel::data(const QModelIndex& index,
                           int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  const int column = index.column();
  const int    row = index.row();

  if( role == Qt::DisplayRole ) {
    if(        isProjectRow(row) ) {
      const ReportEntry& entry = _report[size_type(row)];

      const Project *p = global.findProject(entry.first);
      if( p == nullptr ) {
        return QVariant();
      }

      if(        column == COL_Id ) {
        return entry.first;

      } else if( column == COL_Name ) {
        return p->name;

      } else if( column == COL_Annotation ) {
        return p->annotation;

      } else if( column == COL_Hours ) {
        return View::toString(entry.second);

      } // column

    } else if( isSumRow(row) ) {
      if(        column == COL_Annotation ) {
        return tr("Sum");

      } else if( column == COL_Hours ) {
        return priv::sum(_report);

      } // column

    } // row

  } else if( role == Qt::BackgroundRole ) {
    if( column == COL_Hours ) {
      return QBrush(Qt::yellow);

    } // column

  } else if( role == Qt::TextAlignmentRole ) {
    if( isSumRow(row) ) {
      if( column == COL_Annotation ) {
        const Qt::Alignment alignment = Qt::AlignRight | Qt::AlignVCenter;

        return QVariant(alignment);

      } // column

    } // row

  } // Qt::ItemDataRole

  return QVariant();
}

QVariant ReportModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      if(        section == COL_Id ) {
        return tr("ID");
      } else if( section == COL_Name ) {
        return tr("Name");
      } else if( section == COL_Annotation ) {
        return tr("Annotation");
      } else if( section == COL_Hours ) {
        return tr("Hours");
      }

    } else if( orientation == Qt::Vertical ) {
      if( isProjectRow(section) ) {
        return section + 1;
      }

    } // Qt::Orientation

  } // Qt::ItemDataRole

  return QVariant();
}

int ReportModel::rowCount(const QModelIndex& /*index*/) const
{
  return int(_report.size()) + 1;
}

////// private ///////////////////////////////////////////////////////////////

bool ReportModel::isProjectRow(const int row) const
{
  return size_type(row) < _report.size();
}

bool ReportModel::isSumRow(const int row) const
{
  return size_type(row) == _report.size();
}
