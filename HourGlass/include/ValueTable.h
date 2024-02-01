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

#pragma once

#include <type_traits>
#include <vector>

template<typename T>
class ValueTable {
public:
  using     Vector = std::vector<T>;
  using  size_type = typename Vector::size_type;
  using value_type = typename Vector::value_type;

  static_assert( std::is_unsigned_v<size_type> );

  ValueTable(const size_type rows = 0, const size_type columns = 0,
             const value_type initValue = value_type()) noexcept
  {
    resize(rows, columns, initValue);
  }

  ~ValueTable() noexcept
  {
  }

  bool isValid() const
  {
    return
        _numColumns > 0  &&
        _numRows > 0     &&
        _data.size() == _numColumns*_numRows;
  }

  operator bool() const
  {
    return isValid();
  }

  // Queries /////////////////////////////////////////////////////////////////

  inline size_type columns() const
  {
    return _numColumns;
  }

  inline size_type rows() const
  {
    return _numRows;
  }

  // Size Management /////////////////////////////////////////////////////////

  void clear()
  {
    _numColumns = _numRows = 0;
    _data.clear();
  }

  bool addRows(const size_type rows = 1,
               const value_type initValue = value_type())
  {
    return resize(_numRows + rows, _numColumns, initValue);
  }

  bool resize(const size_type rows, const size_t columns,
              const value_type initValue = value_type())
  {
    try {
      _numColumns = columns;
      _numRows = rows;
      _data.resize(_numRows*_numColumns, initValue);
    } catch(...) {
      clear();
    }

    return isValid();
  }

  // Element Access //////////////////////////////////////////////////////////

  inline value_type& operator()(const size_type i, const size_type j)
  {
    return _data[index(i, j)];
  }

  inline value_type operator()(const size_type i, const size_type j) const
  {
    return _data[index(i, j)];
  }

  // Operations //////////////////////////////////////////////////////////////

  value_type sumColumn(const size_type j) const
  {
    if( j >= _numColumns ) {
      return 0;
    }

    value_type result = 0;
    for(size_type i = 0; i < _numRows; i++) {
      result += _data[index(i, j)];
    }

    return result;
  }

  value_type sumRow(const size_type i) const
  {
    if( i >= _numRows ) {
      return 0;
    }

    value_type result = 0;
    for(size_type j = 0; j < _numColumns; j++) {
      result += _data[index(i, j)];
    }

    return result;
  }

private:
  inline size_type index(const size_type i, const size_type j) const
  {
    return i*_numColumns + j; // row-major
  }

  Vector _data;
  size_type _numColumns{0};
  size_type _numRows{0};
};
