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

#include <QtCore/QObject>
#include <QtWidgets/QMessageBox>
#include <QtXml/QDomDocument>

#include "XML_io.h"

#include "Context.h"
#include "XML_tags.h"

////// Private - Read ////////////////////////////////////////////////////////

bool xmlReadMonth(Context *context, const QDomElement& xml_month)
{
  if( context == nullptr  ||  xml_month.isNull() ) {
    return false;
  }

  if( xml_month.tagName() != XML_month ) {
    return true; // successfully ignored
  }

  const QString idstr = xml_month.attribute(XML_mid);
  if( idstr.isEmpty() ) {
    return false;
  }

  bool ok{false};
  const int id = idstr.toInt(&ok);
  if( !ok ) {
    return false;
  }

  const auto mid = split_monthid(id);

  if( !context->add(Month(mid.first, mid.second)) ) {
    return false;
  }

  return true;
}

bool xmlReadMonths(Context *context, const QDomElement& xml_root)
{
  if( context == nullptr  ||  xml_root.isNull() ) {
    return false;
  }

  const QDomElement xml_months = xml_root.firstChildElement(XML_months);
  if( xml_months.isNull() ) {
    return false;
  }

  for(QDomElement xml_month = xml_months.firstChildElement();
      !xml_month.isNull();
      xml_month = xml_month.nextSiblingElement()) {
    if( !xmlReadMonth(context, xml_month) ) {
      return false;
    }
  }

  return true;
}

bool xmlReadProject(Context *context, const QDomElement& xml_project)
{
  if( context == nullptr  ||  xml_project.isNull() ) {
    return false;
  }

  if( xml_project.tagName() != XML_project ) {
    return true; // successfully ignored
  }

  const QString idstr = xml_project.attribute(XML_pid);
  if( idstr.isEmpty() ) {
    return false;
  }

  bool ok{false};
  const projectid_t id = idstr.toUInt(&ok);
  if( !ok ) {
    return false;
  }

  const QString name = xml_project.text().trimmed();

  if( !context->add({id, name}) ) {
    return false;
  }

  return true;
}

bool xmlReadProjects(Context *context, const QDomElement& xml_root)
{
  if( context == nullptr  ||  xml_root.isNull() ) {
    return false;
  }

  const QDomElement xml_projects = xml_root.firstChildElement(XML_projects);
  if( xml_projects.isNull() ) {
    return false;
  }

  for(QDomElement xml_project = xml_projects.firstChildElement();
      !xml_project.isNull();
      xml_project = xml_project.nextSiblingElement()) {
    if( !xmlReadProject(context, xml_project) ) {
      return false;
    }
  }

  return true;
}

////// Private - Write ///////////////////////////////////////////////////////

void xmlWriteMonth(QDomDocument *doc, QDomElement *xml_months, const Month& month)
{
  if( doc == nullptr  ||  xml_months == nullptr ) {
    return;
  }

  QDomElement xml_month = doc->createElement(XML_month);
  xml_month.setAttribute(XML_mid, month.id());

  xml_months->appendChild(xml_month);
}

void xmlWriteMonths(QDomDocument *doc, QDomElement *xml_root, const Months& months)
{
  if( doc == nullptr  ||  xml_root == nullptr ) {
    return;
  }

  QDomElement xml_months = doc->createElement(XML_months);
  xml_root->appendChild(xml_months);

  for(const Month& month : months) {
    xmlWriteMonth(doc, &xml_months, month);
  }
}

void xmlWriteProject(QDomDocument *doc, QDomElement *xml_projects, const Project& project)
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

void xmlWriteProjects(QDomDocument *doc, QDomElement *xml_root, const Projects& projects)
{
  if( doc == nullptr  ||  xml_root == nullptr ) {
    return;
  }

  QDomElement xml_projects = doc->createElement(XML_projects);
  xml_root->appendChild(xml_projects);

  for(const Project& project : projects) {
    xmlWriteProject(doc, &xml_projects, project);
  }
}

////// Public ////////////////////////////////////////////////////////////////

bool xmlRead(Context *context, const QString& xmlContent, QWidget *parent)
{
  if( context == nullptr ) {
    return false;
  }

  context->clear();

  QDomDocument doc;

  QString errmsg;
  int line, column;
  if( !doc.setContent(xmlContent, &errmsg, &line, &column) ) {
    QMessageBox::critical(parent, QObject::tr("Error"),
                          QObject::tr("XML(%1,%2):\n\"%3\"")
                          .arg(line)
                          .arg(column)
                          .arg(errmsg));
    return false;
  }

  const QDomElement xml_root = doc.firstChildElement(XML_HourGlass);
  if( xml_root.isNull() ) {
    return false;
  }

  if( !xmlReadProjects(context, xml_root) ) {
    return false;
  }

  if( !xmlReadMonths(context, xml_root) ) {
    return false;
  }

  return true;
}

QString xmlWrite(const Context& context, QWidget * /*parent*/)
{
  QDomDocument doc;

  QDomProcessingInstruction pi = doc.createProcessingInstruction(XML_pitarget, XML_pidata);
  doc.appendChild(pi);

  QDomElement xml_root = doc.createElement(XML_HourGlass);
  doc.appendChild(xml_root);

  xmlWriteProjects(&doc, &xml_root, context.projects);
  xmlWriteMonths(&doc, &xml_root, context.months);

  return doc.toString(2);
}
