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

#ifndef _FilePathGenerator_H_
#define _FilePathGenerator_H_

#include <QtCore/QString>
#include <QtCore/QVector>

class FilePathGenerator
{
  public:

    virtual ~FilePathGenerator();

  struct FilePackage
  {
    QString completeFilePath;
    int index;
    QString inputPath;
    QString filePrefix;
    QString fileSuffix;
    QString fileExtension;
    int paddingDigits;
  };

    /**
     * @brief generateFileList This method will generate a file list in the correct order of the files that should
     * be imported into an h5ebsd file
     * @param start Z Slice Start
     * @param end S Slice End
     * @param hasMissingFiles Are any files missing
     * @param stackLowToHigh Is the Slice order low to high
     * @param filename Example File Name
     * @return
     */
    static QVector<FilePackage> GenerateFileList(int start, int end, bool& hasMissingFiles,
                                             const QString& inputPath, const QString& filePrefix,
                                             const QString& fileSuffix, const QString& fileExtension,
                                             int paddingDigits);
    /**
     * @brief generateFileList This method will generate a file list in the correct order of the files that should
     * be imported into an h5ebsd file
     * @param start Z Slice Start
     * @param end S Slice End
     * @param hasMissingFiles Are any files missing
     * @param stackLowToHigh Is the Slice order low to high
     * @param filename Example File Name
     * @return
     */
    static QVector<QString> GenerateVectorFileList(int start, int end, int compStart, int compEnd, bool& hasMissingFiles,
                                                   const QString& inputPath, const QString& filePrefix, const QString& separator,
                                                   const QString& fileSuffix, const QString& fileExtension,
                                                   int paddingDigits);

  protected:
    FilePathGenerator();

  private:
    FilePathGenerator(const FilePathGenerator&); // Copy Constructor Not Implemented
    void operator=(const FilePathGenerator&); // Operator '=' Not Implemented
};

#endif /* _FilePathGenerator_H_ */
