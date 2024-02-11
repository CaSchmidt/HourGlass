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
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtWidgets/QMessageBox>

#include "File_io.h"

#include "Context.h"
#include "XML_io.h"

////// Macros ///////////////////////////////////////////////////////////////

#define BACKUP_DIR  QStringLiteral("bakhours")

#define TR_CTX  "File_io"

////// Public ////////////////////////////////////////////////////////////////

bool backupHoursFile(const QString& filename)
{
  const QFileInfo info(filename);
  if( !info.exists() ) {
    return true; // nothing to backup!
  }

  const QDir dir = info.canonicalPath();
  dir.mkdir(BACKUP_DIR);

  const QDir bakdir = dir.absoluteFilePath(BACKUP_DIR);
  if( !bakdir.exists() ) {
    return false;
  }

  const QString time = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"));
  const QString bakfilename = QStringLiteral("%1-%2.%3")
      .arg(info.completeBaseName(), time, info.suffix());

  return QFile::copy(filename, bakdir.absoluteFilePath(bakfilename));
}

bool readHoursFile(Context& context, const QString& filename, QWidget *parent)
{
  context.clear();

  const QString fileInfo = QFileInfo(filename).fileName();

  // (1) Open file for reading ///////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QFile::ReadOnly) ) {
    QMessageBox::critical(parent, QCoreApplication::translate(TR_CTX, "Error"),
                          QCoreApplication::translate(TR_CTX, "Unable to open file \"%1\"!")
                          .arg(fileInfo));
    return false;
  }

  // (2) Deserialize XML /////////////////////////////////////////////////////

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  const QString xmlContent = stream.readAll();
  file.close();

  // (3) Parse XML ///////////////////////////////////////////////////////////

  if( !xmlRead(context, xmlContent, parent) ) {
    QMessageBox::critical(parent, QCoreApplication::translate(TR_CTX, "Error"),
                          QCoreApplication::translate(TR_CTX, "Unable to read XML file \"%1\"!")
                          .arg(fileInfo));
    return false;
  }

  // (4) Validate Context ////////////////////////////////////////////////////

  if( !context ) {
    QMessageBox::critical(parent, QCoreApplication::translate(TR_CTX, "Error"),
                          QCoreApplication::translate(TR_CTX, "Invalid context! (\"%1\")")
                          .arg(fileInfo));
    return false;
  }

  // Done! ///////////////////////////////////////////////////////////////////

  return true;
}

bool writeHoursFile(const QString& filename, const Context& context, QWidget *parent)
{
  // (1) Open file for writing ///////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QFile::WriteOnly) ) {
    QMessageBox::critical(parent, QCoreApplication::translate(TR_CTX, "Error"),
                          QCoreApplication::translate(TR_CTX, "Unable to save file \"%1\"!")
                          .arg(QFileInfo(filename).fileName()));
    return false;
  }

  // (2) Create XML //////////////////////////////////////////////////////////

  const QString xmlContent = xmlWrite(context, parent);

  // (3) Serialize XML ///////////////////////////////////////////////////////

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream << xmlContent;
  stream.flush();

  // Done! ///////////////////////////////////////////////////////////////////

  file.close();

  return true;
}
