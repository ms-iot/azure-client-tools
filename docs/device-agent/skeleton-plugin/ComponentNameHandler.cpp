#include "stdafx.h"
#include "PluginJsonConstants.h"
#include "..\..\AzureDeviceManagementCommon\DMConstants.h"
#include "..\..\AzureDeviceManagementCommon\Plugins\PluginConstants.h"
#include "ComponentHandler.h"

using namespace DMCommon;
using namespace DMUtils;
using namespace std;

namespace Microsoft { namespace Windows { namespace Azure { namespace DeviceManagement { namespace ComponentNamePlugin {

    ComponentNameHandler::ComponentNameHandler() :
        BaseHandler(JsonPluginId, ReportedSchema(JsonDeviceSchemasTypeRaw, JsonDeviceSchemasTagDM, 1, 1))
    {
    }

    int ComponentNameHandler::Start(
        const Json::Value& handlerConfig)
    {
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

        // Do initialization here.

        // Start any monitoring threads here.

        return PLUGIN_ERR_SUCCESS;
    }

    InvokeResult ComponentNameHandler::Invoke(
        const Json::Value& desiredConfig)
    {
        (void)desiredConfig;
        TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
        Json::Value reportedObject(Json::objectValue);

        // Process...
        std::shared_ptr errorList = make_shared();

        // Merge
        // Parse
        // Validate
        // Execute
        // Building Report

        FinalizeAndReport(reportedObject, errorList);

    }

}}}}}
  