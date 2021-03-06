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


#ifndef _D3DInfoPage_H_
#define _D3DInfoPage_H_

#include <QtWidgets/QWizardPage>

#include "ui_D3DInfoPage.h"

class GatherDataPage;
class MDCConfiguration;

class D3DInfoPage : public QWizardPage, private Ui::D3DInfoPage
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    D3DInfoPage(QWidget* parent = NULL);

    virtual ~D3DInfoPage();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    /**
    * @brief Determines whether the Next button should be enabled or disabled
    */
    bool isComplete() const;

    /**
    * @brief Slot that is called after the user presses Next but before the next page shows
    */
    bool validatePage();

  protected:
    void showEvent(QShowEvent* event);

  protected slots:
    void on_pipelineRunnerSelectBtn_pressed();
    void on_pipelineSelectBtn_pressed();
    void on_configSelectBtn_pressed();
    void on_pipelineRunnerLineEdit_textChanged(const QString &text);
    void on_pipelineLineEdit_textChanged(const QString &text);

  private:
    int                                             m_NumLines;
    QString                                         m_LastOpenDialogFilePath;

    D3DInfoPage(const D3DInfoPage&); // Copy Constructor Not Implemented
    void operator=(const D3DInfoPage&); // Operator '=' Not Implemented
};

#endif /* D3DInfoPage_H_ */
