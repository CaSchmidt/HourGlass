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

#include "Context.h"

////// Private ///////////////////////////////////////////////////////////////

static_assert( std::is_unsigned_v<std::size_t> );

inline bool operator<(const ProjectDB::value_type& a, const ProjectDB::value_type& b)
{
  return a.second.id() < b.second.id();
}

////// public ////////////////////////////////////////////////////////////////

Context::Context() noexcept
  : _months()
  , _projects()
{
  clear();
}

bool Context::isValid() const
{
  for(const auto& v : _months) {
    for(const Item& item : v.second.items) {
      if( !isProject(item.projectId) ) {
        return false;
      }
    } // For each Item
  } // For each Month

  return true;
}

void Context::clear()
{
  _months.clear();
  _projects.clear();
  clearModified();
}

////// public - Modification State ///////////////////////////////////////////

void Context::clearModified()
{
  _is_modified = false;
}

bool Context::isModified() const
{
  return _is_modified;
}

void Context::setModified()
{
  _is_modified = true;
}

////// public - Month ////////////////////////////////////////////////////////

bool Context::add(Month m)
{
  if( !m  ||  isMonth(m.id()) ) {
    return false;
  }

  const auto result = _months.emplace(m.id(), std::move(m));

  setModified();

  return isMonth(result.first->second.id());
}

Month *Context::findMonth(const monthid_t id) const
{
  const auto hit = _months.find(id);

  return hit != _months.cend()
      ? &const_cast<Month&>(hit->second)
      : nullptr;
}

bool Context::isMonth(const monthid_t id) const
{
  return _months.contains(id);
}

MonthIDs Context::listMonths() const
{
  if( _months.empty() ) {
    return MonthIDs();
  }

  MonthIDs result;

  result.reserve(_months.size());
  for(const auto& v : _months) {
    result.push_back(v.second.id());
  }

  std::sort(result.begin(), result.end(), std::greater<monthid_t>());

  return result;
}

void Context::set(MonthDB months)
{
  _months = std::move(months);
}

////// public - Project //////////////////////////////////////////////////////

bool Context::add(Project p)
{
  if( !p  ||  isProject(p.id()) ) {
    return false;
  }

  const auto result = _projects.emplace(p.id(), std::move(p));

  setModified();

  return isProject(result.first->second.id());
}

Project *Context::findProject(const projectid_t id) const
{
  const auto hit = _projects.find(id);

  return hit != _projects.cend()
      ? &const_cast<Project&>(hit->second)
      : nullptr;
}

bool Context::isProject(const projectid_t id) const
{
  return _projects.contains(id);
}

ProjectIDs Context::listProjects() const
{
  if( _projects.empty() ) {
    return ProjectIDs();
  }

  ProjectIDs result;

  result.reserve(_projects.size());
  for(const auto& v : _projects) {
    result.push_back(v.second.id());
  }

  std::sort(result.begin(), result.end());

  return result;
}

Project Context::makeProject(const QString& name) const
{
  constexpr projectid_t ONE = 1;

  const auto hit = std::max_element(_projects.cbegin(), _projects.cend());

  const projectid_t newId = hit != _projects.cend()
      ? hit->second.id() + ONE
      : ONE;

  return Project(newId, name);
}

void Context::set(ProjectDB projects)
{
  _projects = std::move(projects);
}
