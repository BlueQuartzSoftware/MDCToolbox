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

#ifndef _D3DProcessor_H_
#define _D3DProcessor_H_

#include <QtCore/QObject>

class D3DProcessor : public QObject
{
    Q_OBJECT

  public:
    D3DProcessor(const QString &pipelineFilePath, const QString &pipelineRunnerFilePath, const QString &outputDir, QObject* parent = NULL);
    virtual ~D3DProcessor();

    void setStop(bool val);

  protected slots:
    void run();
    void stopProcess();

  signals:
    void processFinished();
    void processGeneratedMessage(const QString &msg);
    void processGeneratedProgressValue(double value);

  private:
    struct ErrorPackage
    {
      int errorCode;
      QString errorMessage;
    };

    D3DProcessor::ErrorPackage executePipeline(const QString &pipelineFilePath);

    QString                                   m_PipelineFilePath;
    QString                                   m_PipelineRunnerFilePath;
    QString                                   m_OutputDir;

    bool                                      m_Stop;

    D3DProcessor(const D3DProcessor&);    // Copy Constructor Not Implemented
    void operator=(const D3DProcessor&);  // Operator '=' Not Implemented
};

#endif /* _D3DProcessor_H_ */
