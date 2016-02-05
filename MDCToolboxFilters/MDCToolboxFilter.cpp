/*
 * Your License or Copyright can go here
 */

#include "MDCToolboxFilter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"



#include "MDCToolbox/MDCToolboxConstants.h"
#include "MDCToolbox/MDCToolboxVersion.h"

// Include the MOC generated file for this class
#include "moc_MDCToolboxFilter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MDCToolboxFilter::MDCToolboxFilter() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MDCToolboxFilter::~MDCToolboxFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCToolboxFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCToolboxFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MDCToolboxFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCToolboxFilter::dataCheck()
{
  setErrorCondition(0);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCToolboxFilter::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MDCToolboxFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MDCToolboxFilter::newFilterInstance(bool copyFilterParameters)
{
  MDCToolboxFilter::Pointer filter = MDCToolboxFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getCompiledLibraryName()
{ return MDCToolboxConstants::MDCToolboxBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getBrandingString()
{
  return "MDCToolbox";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  MDCToolbox::Version::Major() << "." << MDCToolbox::Version::Minor() << "." << MDCToolbox::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getGroupName()
{ return SIMPL::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getSubGroupName()
{ return "MDCToolbox"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MDCToolboxFilter::getHumanLabel()
{ return "MDCToolboxFilter"; }

