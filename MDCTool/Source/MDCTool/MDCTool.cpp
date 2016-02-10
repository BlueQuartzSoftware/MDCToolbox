/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MDCTool.h"

#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QFile>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMessageBox>

#include "PreviewTableModel.h"
#include "PreviewTableItem.h"

#include "D3DInfoPage.h"
#include "GatherDataPage.h"
#include "InputDataPage.h"
#include "MDCConfiguration.h"

#include <iostream>

QString MDCTool::m_LastOpenDialogFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MDCTool::MDCTool(QWidget* parent) :
  QWizard(parent),
  m_CurrentPageId(0)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MDCTool::~MDCTool()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCTool::setupGui()
{
  resize(900, 700);

  setOption(QWizard::NoBackButtonOnStartPage);
  setOption(QWizard::NoCancelButton);

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

#if defined (Q_OS_MAC)
  setWizardStyle(QWizard::MacStyle);
#else
  setWizardStyle(QWizard::ModernStyle);
#endif

  PreviewTableModel* model = PreviewTableModel::Instance();
  model->insertColumn(0, QModelIndex());
  model->insertColumn(1, QModelIndex());
  model->insertColumn(2, QModelIndex());
  model->setHeaderData(0, Qt::Horizontal, "Conditions");
  model->setHeaderData(1, Qt::Horizontal, "Raw Image File Path");
  model->setHeaderData(2, Qt::Horizontal, "Output");

  D3DInfoPage* d3dInfoPage = new D3DInfoPage(this);
  setPage(0, d3dInfoPage);

  /* We have to create these next two pages here, because we don't know how many configurations 
     we will have, and the wizard needs to have more than one page at this point so that it shows
     a "Next" button instead of a "Finish" button on the first page.  At this point, we are assuming
     that we will have at least one configuration. */
  //GatherDataPage* gatherPage = new GatherDataPage(this);
  //setPage(1, gatherPage);

  //InputDataPage* inputPage = new InputDataPage(this);
  //setPage(2, inputPage);

  QWizardPage* placeholderPage = new QWizardPage(this);
  setPage(1, placeholderPage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MDCTool::ChooseFile(QWidget* parent, const QString &caption, const QString &filter)
{
  QString filePath = QFileDialog::getOpenFileName(parent, caption, m_LastOpenDialogFilePath, filter);

  filePath = QDir::toNativeSeparators(filePath);

  // Store the last used directory into the private instance variable
  m_LastOpenDialogFilePath = filePath;

  return filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MDCTool::ChooseDirectory(QWidget* parent, const QString &caption)
{
  QString filePath = QFileDialog::getExistingDirectory(parent, caption, m_LastOpenDialogFilePath);

  filePath = QDir::toNativeSeparators(filePath);

  QFileInfo fi(filePath);

  // Store the last used directory into the private instance variable
  QString path = fi.path();
  path = QDir::toNativeSeparators(path);
  m_LastOpenDialogFilePath = path;

  return filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MDCTool::TokenizeConfiguration(MDCConfiguration* config, char token)
{
  QVector<QSharedPointer<MDCCondition> > conditions = config->getConditions();
  QString configText;

  for (int i = 0; i < conditions.size(); i++)
  {
    MDCCondition* condition = conditions[i].data();
    configText.append(condition->conditionName + " - " + QString::number(condition->conditionValue));

    if (i < conditions.size() - 1)
    {
      configText.append(token);
      configText.append(" ");
    }
  }

  return configText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject MDCTool::ReadJsonFile(const QString &filePath, int &errorCode)
{
  QJsonObject obj;

  QFile inputFile(filePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QJsonParseError parseError;
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
      errorCode = -1;
      return obj;
    }

    obj = doc.object();
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QSharedPointer<MDCConfiguration> > MDCTool::readJsonConfigurationFile(const QString &filePath)
{
  m_Configurations.clear();

  int error = 0;
  QJsonObject root = ReadJsonFile(filePath, error);

  if (error < 0)
  {
    QString s = "The configuration file that was chosen does not conform to universal JSON standards.\n\n";
    s.append("Please choose a new JSON file that conforms to the universal JSON standard.");
    QMessageBox::critical(NULL, "MDCTool Error", s, QMessageBox::Ok);
    return QVector<QSharedPointer<MDCConfiguration> >();
  }

  for (int i = 0; i < root.count(); i++)
  {
    QJsonObject configObj = root.value(QString::number(i)).toObject();
    if (configObj.isEmpty())
    {
      QString s = "The configuration file that was chosen contains at least one unrecognizable configuration name.\n\n";
      s.append("Please choose a new JSON file that conforms to the MDCTool standard for configuration files.");
      QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
      return QVector<QSharedPointer<MDCConfiguration> >();
    }

    QSharedPointer<MDCConfiguration> config = QSharedPointer<MDCConfiguration>(new MDCConfiguration());
    for (QJsonObject::iterator iter = configObj.begin(); iter != configObj.end(); iter++)
    {
      config->addCondition(iter.key(), iter.value().toDouble());
    }

    m_Configurations.push_back(config);
  }

  if (m_Configurations.size() <= 0)
  {
    QString s = "The configuration file that was chosen does not contain any configuration objects.\n\n";
    s.append("Please choose a new JSON file that contains at least one configuration object.");
    QMessageBox::critical(this, "MDCTool Error", s, QMessageBox::Ok);
    return m_Configurations;
  }

  return m_Configurations;
}


