/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _EbsdImportWIDGET_H_
#define _EbsdImportWIDGET_H_


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include <QtWidgets/QLineEdit>

#include "ui_ImportFilesWidget.h"

/**
 * @class ImportFilesWidget ImportFilesWidget.h Plugins/EbsdImport/UI/ImportFilesWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class ImportFilesWidget : public QWidget, private Ui::ImportFilesWidget
{
    Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ImportFilesWidget(QWidget* parent = NULL);

    virtual ~ImportFilesWidget();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    QVector<QString> getFileList();

  protected slots:
    /* OIM Data Import Slots */
    void on_m_InputDirBtn_clicked();

    void on_m_FilePrefix_textChanged(const QString& string);
    void on_m_FileSuffix_textChanged(const QString& string);
    void on_m_FileExt_textChanged(const QString& string);
    void on_m_TotalDigits_valueChanged(int value);
    void on_m_ZStartIndex_valueChanged(int value);
    void on_m_ZEndIndex_valueChanged(int value);

    // slots to catch signals emitted by the various QLineEdit widgets
    void on_m_InputDir_textChanged(const QString& text);

  protected:
    static void setOpenDialogLastDirectory(QString val) { m_OpenDialogLastDirectory = val; }
    static QString getOpenDialogLastDirectory() { return m_OpenDialogLastDirectory; }

    /**
     * @brief checkIOFiles
     */
    virtual void checkIOFiles();

    /**
     * @brief verifyPathExists
     * @param outFilePath
     * @param lineEdit
     * @return
     */
    bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

    /**
     * @brief Method to attempt the extraction of the .ang max slice value and prefix
     */
    void findEbsdMaxSliceAndPrefix();

    /**
     * @brief m_generateExampleEbsdInputFile
     */
    void generateExampleEbsdInputFile();

  signals:
    void fileListChanged();

  private:
    QVector<QString>                    m_FileList;

    static QString                      m_OpenDialogLastDirectory;

    ImportFilesWidget(const ImportFilesWidget&); // Copy Constructor Not Implemented
    void operator=(const ImportFilesWidget&); // Operator '=' Not Implemented
};

#endif /* EbsdImportWIDGET_H_ */
