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

#include <algorithm>

#include "Global.h"

////// public ////////////////////////////////////////////////////////////////

Context::Context() noexcept
  : months()
  , projects()
{
}

bool Context::add(Month m)
{
  if( !m  ||  isMonth(m.id()) ) {
    return false;
  }

  months.push_back(std::move(m));

  return true;
}

Month *Context::findMonth(const int id) const
{
  const auto hit = std::find(months.cbegin(), months.cend(), id);

  return hit != months.cend()
      ? &const_cast<Month&>(*hit)
      : nullptr;
}

bool Context::isMonth(const int id) const
{
  return std::find(months.cbegin(), months.cend(), id) != months.cend();
}

bool Context::add(Project p)
{
  if( !p  ||  isProject(p.id()) ) {
    return false;
  }

  projects.push_back(std::move(p));

  return true;
}

Project *Context::findProject(const projectid_t id) const
{
  const auto hit = std::find(projects.cbegin(), projects.cend(), id);

  return hit != projects.cend()
      ? &const_cast<Project&>(*hit)
      : nullptr;
}

bool Context::isProject(const projectid_t id) const
{
  return std::find(projects.cbegin(), projects.cend(), id) != projects.cend();
}

////// Public ////////////////////////////////////////////////////////////////

Context global;
