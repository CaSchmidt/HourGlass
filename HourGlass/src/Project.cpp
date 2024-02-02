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

#include "Project.h"

////// public ////////////////////////////////////////////////////////////////

Project::Project(const projectid_t id, const QString& name) noexcept
  : name(name)
  , _id{id}
{
}

bool Project::isValid() const
{
  return _id != INVALID_PROJECTID  &&  !name.isEmpty();
}

projectid_t Project::id() const
{
  return _id;
}

bool Project::operator==(const projectid_t id) const
{
  return _id == id;
}

////// Public ////////////////////////////////////////////////////////////////

bool addProject(Projects *list, Project project)
{
  if( list == nullptr  ||  !project  ||  isProject(*list, project.id()) ) {
    return false;
  }

  list->push_back(std::move(project));

  return true;
}

Project findProject(const Projects& list, const projectid_t id,
                    const Project& defValue)
{
  const auto hit = std::find(list.cbegin(), list.cend(), id);

  return hit != list.cend()
      ? *hit
      : defValue;
}

bool isProject(const Projects& list, const projectid_t id)
{
  return std::find(list.cbegin(), list.cend(), id) != list.cend();
}
