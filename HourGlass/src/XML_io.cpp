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

#include <QtCore/QCoreApplication>
#include <QtWidgets/QMessageBox>
#include <QtXml/QDomDocument>

#include "XML_io.h"

#include "Context.h"
#include "XML_tags.h"

////// Macros ////////////////////////////////////////////////////////////////

#define TR_CTX  "XML_io"

////// Private ///////////////////////////////////////////////////////////////

inline QString toString(const numhour_t hour)
{
  return QString::number(hour, 'f', 2);
}

template<typename T, typename REF>
using if_same_t = std::enable_if_t<std::is_same_v<T,REF>,T>;

template<typename T>
inline if_same_t<T,int> toValue(const QString& s,
                                bool *ok = nullptr, int base = 10)
{
  return s.toInt(ok, base);
}

template<typename T>
inline if_same_t<T,unsigned int> toValue(const QString& s,
                                         bool *ok = nullptr, int base = 10)
{
  return s.toUInt(ok, base);
}

template<typename T>
inline if_same_t<T,unsigned long long> toValue(const QString& s,
                                               bool *ok = nullptr, int base = 10)
{
  return s.toULongLong(ok, base);
}

template<typename T>
inline if_same_t<T,double> toValue(const QString& s,
                                   bool *ok = nullptr, int = 0)
{
  return s.toDouble(ok);
}

template<typename T>
inline T xmlAttributeValue(const QDomElement& elem, const QString& name,
                           const T& errValue = T())
{
  const QString strValue = elem.attribute(name, QString());
  if( strValue.isEmpty() ) {
    return errValue;
  }

  bool ok{false};
  const T value = toValue<T>(strValue, &ok);
  if( !ok ) {
    return errValue;
  }

  return value;
}

inline bool xmlIsTag(const QDomElement& elem, const QString& name)
{
  return !elem.isNull()  &&  elem.tagName() == name;
}

////// Private - Read Months /////////////////////////////////////////////////

bool xmlReadHours(Hours& hours, const QDomElement& xml_item)
{
  const QDomElement xml_hours = xml_item.firstChildElement(XML_hours);
  if( xml_hours.isNull() ) {
    return true; // Optional
  }

  for(QDomElement xml_day = xml_hours.firstChildElement(XML_day);
      !xml_day.isNull();
      xml_day = xml_day.nextSiblingElement(XML_day)) {
    const std::size_t did = xmlAttributeValue(xml_day, XML_did, hours.size());
    if( did >= hours.size() ) {
      return false;
    }

    bool ok{false};
    hours[did] = toValue<numhour_t>(xml_day.text(), &ok);
    if( !ok ) {
      return false;
    }
  }

  return true;
}

bool xmlReadItem(Item& item, const QDomElement xml_item)
{
  if( !xmlIsTag(xml_item, XML_item) ) {
    return true; // successfully ignored
  }

  item.projectId = xmlAttributeValue<projectid_t>(xml_item, XML_pid, INVALID_PROJECTID);
  if( item.projectId == INVALID_PROJECTID ) {
    return false;
  }

  const QDomElement xml_activity = xml_item.firstChildElement(XML_activity);
  item.activity = xml_activity.text();

  return xmlReadHours(item.hours, xml_item);
}

bool xmlReadItems(Items& items, const QDomElement& xml_month)
{
  const QDomElement xml_items = xml_month.firstChildElement(XML_items);
  if( xml_items.isNull() ) {
    return true; // Optional
  }

  for(QDomElement xml_item = xml_items.firstChildElement();
      !xml_item.isNull();
      xml_item = xml_item.nextSiblingElement()) {
    Item item;
    if( !xmlReadItem(item, xml_item) ) {
      return false;
    }

    items.push_back(std::move(item));
  }

  return true;
}

bool xmlReadMonth(Context& context, const QDomElement& xml_month)
{
  if( !xmlIsTag(xml_month, XML_month) ) {
    return true; // successfully ignored
  }

  const monthid_t mid =
      xmlAttributeValue<monthid_t>(xml_month, XML_mid, INVALID_MONTHID);
  if( mid == INVALID_MONTHID ) {
    return false;
  }

  const SplitId sid = split_monthid(mid);

  if( !context.add(Month(sid.first, sid.second)) ) {
    return false;
  }

  Month *month = context.findMonth(mid);

  return month != nullptr
      ? xmlReadItems(month->items, xml_month)
      : false;
}

bool xmlReadMonths(Context& context, const QDomElement& xml_root)
{
  const QDomElement xml_months = xml_root.firstChildElement(XML_months);
  if( xml_months.isNull() ) {
    return true; // Optional
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

////// Private - Read Projects ///////////////////////////////////////////////

bool xmlReadProject(Context& context, const QDomElement& xml_project)
{
  if( !xmlIsTag(xml_project, XML_project) ) {
    return true; // successfully ignored
  }

  const projectid_t id =
      xmlAttributeValue<projectid_t>(xml_project, XML_pid, INVALID_PROJECTID);
  if( id == INVALID_PROJECTID ) {
    return false;
  }

  const QDomElement xml_name = xml_project.firstChildElement(XML_name);
  if( xml_name.isNull() ) {
    return false;
  }

  const QString name = xml_name.text().trimmed();

  const QDomElement xml_annotation = xml_project.firstChildElement(XML_annotation);
  const QString annotation = xml_annotation.text().trimmed();

  if( !context.add({id, name, annotation}) ) {
    return false;
  }

  return true;
}

bool xmlReadProjects(Context& context, const QDomElement& xml_root)
{
  const QDomElement xml_projects = xml_root.firstChildElement(XML_projects);
  if( xml_projects.isNull() ) {
    return true; // Optional
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

////// Private - Write Months ////////////////////////////////////////////////

void xmlWriteHours(QDomDocument& doc, QDomElement& xml_item, const Item& item)
{
  constexpr numhour_t ZERO = 0;

  if( item.sumHours() == ZERO ) {
    return; // Optional
  }

  QDomElement xml_hours = doc.createElement(XML_hours);
  xml_item.appendChild(xml_hours);

  for(std::size_t i = 0; i < item.hours.size(); i++) {
    if( item.hours[i] == ZERO ) {
      continue;
    }

    QDomElement xml_day = doc.createElement(XML_day);
    xml_day.setAttribute(XML_did, i);
    xml_hours.appendChild(xml_day);

    QDomText xml_text = doc.createTextNode(toString(item.hours[i]));
    xml_day.appendChild(xml_text);
  }
}

void xmlWriteItem(QDomDocument& doc, QDomElement& xml_items, const Item& item)
{
  QDomElement xml_item = doc.createElement(XML_item);
  xml_item.setAttribute(XML_pid, item.projectId);
  xml_items.appendChild(xml_item);

  QDomElement xml_activity = doc.createElement(XML_activity);
  xml_item.appendChild(xml_activity);

  QDomText xml_text = doc.createTextNode(item.activity);
  xml_activity.appendChild(xml_text);

  xmlWriteHours(doc, xml_item, item);
}

void xmlWriteItems(QDomDocument& doc, QDomElement& xml_month, const Items& items)
{
  if( items.empty() ) {
    return; // Optional
  }

  QDomElement xml_items = doc.createElement(XML_items);
  xml_month.appendChild(xml_items);

  for(const Item& item : items) {
    xmlWriteItem(doc, xml_items, item);
  }
}

void xmlWriteMonth(QDomDocument& doc, QDomElement& xml_months, const Month& month)
{
  QDomElement xml_month = doc.createElement(XML_month);
  xml_month.setAttribute(XML_mid, month.id());
  xml_months.appendChild(xml_month);

  xmlWriteItems(doc, xml_month, month.items);
}

void xmlWriteMonths(QDomDocument& doc, QDomElement& xml_root, const Context& context)
{
  const MonthIDs months = context.listMonths();
  if( months.empty() ) {
    return; // Optional
  }

  QDomElement xml_months = doc.createElement(XML_months);
  xml_root.appendChild(xml_months);

  for(const monthid_t id : months) {
    const Month *m = context.findMonth(id);
    if( m == nullptr ) {
      continue;
    }

    xmlWriteMonth(doc, xml_months, *m);
  }
}

////// Private - Write Projects //////////////////////////////////////////////

void xmlWriteProject(QDomDocument& doc, QDomElement& xml_projects, const Project& project)
{
  QDomElement xml_project = doc.createElement(XML_project);
  xml_project.setAttribute(XML_pid, project.id());

  QDomElement xml_name = doc.createElement(XML_name);
  xml_name.appendChild(doc.createTextNode(project.name));
  xml_project.appendChild(xml_name);

  QDomElement xml_annotation = doc.createElement(XML_annotation);
  xml_annotation.appendChild(doc.createTextNode(project.annotation));
  xml_project.appendChild(xml_annotation);

  xml_projects.appendChild(xml_project);
}

void xmlWriteProjects(QDomDocument& doc, QDomElement& xml_root, const Context& context)
{
  const ProjectIDs projects = context.listProjects();
  if( projects.empty() ) {
    return; // Optional
  }

  QDomElement xml_projects = doc.createElement(XML_projects);
  xml_root.appendChild(xml_projects);

  for(const projectid_t id : projects) {
    const Project *p = context.findProject(id);
    if( p == nullptr ) {
      continue;
    }

    xmlWriteProject(doc, xml_projects, *p);
  }
}

////// Public ////////////////////////////////////////////////////////////////

bool xmlRead(Context& context, const QString& xmlContent, QWidget *parent)
{
  context.clear();

  QDomDocument doc;

  QString errmsg;
  int line, column;
  if( !doc.setContent(xmlContent, &errmsg, &line, &column) ) {
    QMessageBox::critical(parent, QCoreApplication::translate(TR_CTX, "Error"),
                          QCoreApplication::translate(TR_CTX, "XML(%1,%2):\n\"%3\"")
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

  xmlWriteProjects(doc, xml_root, context);
  xmlWriteMonths(doc, xml_root, context);

  return doc.toString(2);
}
