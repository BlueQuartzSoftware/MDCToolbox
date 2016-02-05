/*
 * Your License should go here
 */
#ifndef _MDCToolboxConstants_H_
#define _MDCToolboxConstants_H_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace MDCToolboxConstants
{
  const QString MDCToolboxPluginFile("MDCToolboxPlugin");
  const QString MDCToolboxPluginDisplayName("MDCToolboxPlugin");
  const QString MDCToolboxBaseName("MDCToolboxPlugin");

  namespace FilterGroups
  {
  	const QString MDCToolboxFilters("MDCToolbox");
  }
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
#endif
