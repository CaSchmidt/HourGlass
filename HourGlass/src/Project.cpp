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

#include <QtXml/QDomDocument>

#include "Project.h"

#include "XML_tags.h"

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

////// XML I/O ///////////////////////////////////////////////////////////////

void output(QDomDocument *doc, QDomElement *xml_projects, const Project& project)
{
  if( doc == nullptr  ||  xml_projects == nullptr ) {
    return;
  }

  QDomElement xml_project = doc->createElement(XML_project);
  xml_project.setAttribute(XML_pid, project.id());

  QDomText xml_name = doc->createTextNode(project.name);
  xml_project.appendChild(xml_name);

  xml_projects->appendChild(xml_project);
}

void output(QDomDocument *doc, QDomElement *xml_root, const Projects& projects)
{
  if( doc == nullptr  ||  xml_root == nullptr ) {
    return;
  }

  QDomElement xml_projects = doc->createElement(XML_projects);
  xml_root->appendChild(xml_projects);

  for(const Project& project : projects) {
    output(doc, &xml_projects, project);
  }
}
