//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "IotivityandZigbeeServer.h"
#include <signal.h>
#include <ocstack.h>

#include "oic_string.h"
#include "oic_malloc.h"

#define TAG "IoTivityZigbeeServer"
#define defaultComPort "/dev/ttyUSB0"

/// This example is using experimental API, so there is no guarantee of support for future release,
/// nor any there any guarantee that breaking changes will not occur across releases.
#include "experimental/logger.h"


#define VERIFY_SUCCESS(op)                          \
{                                                   \
    if (op !=  OC_STACK_OK)                         \
    {                                               \
        OIC_LOG_V(FATAL, TAG, "%s failed!!", #op);  \
        goto exit;                                  \
    }                                               \
}

int main(void)
{
    OIC_LOG(INFO, TAG, "Initializing IoTivity...");
    OCStackResult result = OCInit(NULL, 0, OC_SERVER);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCInit Failed %d", result);
        return -1;
    }

    result = SetPlatformInfo();
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed %d", result);
        goto IotivityStop;
    }

    result  = SetDeviceInfo();
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed: %d", result);
        goto IotivityStop;
    }

    result  = OCStartPresence(0);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCStartPresence Failed: %d", result);
        goto IotivityStop;
    }

    // PIStartPlugin
    PIPluginPtr plugin = NULL;
    OIC_LOG(INFO, TAG, "IoTivity Initialized properly, Starting Zigbee Plugin...");
    result = PIStartPlugin(defaultComPort, PLUGIN_ZIGBEE, &plugin);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Zigbee Plugin Start Failed: %d", result);
        goto IotivityStop;
    }

    if (signal(SIGINT, processCancel) == SIG_ERR)
    {
        OIC_LOG(ERROR, TAG, "Unable to catch SIGINT, terminating...");
    }
    else
    {
        OIC_LOG(INFO, TAG, "Performing Zigbee discovery. This process takes 15 seconds.");

        result = PISetup(plugin);
        if (result != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Zigbee Plugin Discovery Failed: %d", result);
            goto IotivityStop;
        }

        OIC_LOG(INFO, TAG, "Zigbee Plugin started correctly, press Ctrl-C to terminate application");
        // Loop until sigint
        while (!processSignal(false) && result == OC_STACK_OK)
        {
            result = OCProcess();
            if (result != OC_STACK_OK)
            {
                OIC_LOG_V(ERROR, TAG, "OCProcess Failed: %d", result);
                break;
            }

            result = PIProcess(plugin);
            if (result != OC_STACK_OK)
            {
                OIC_LOG_V(ERROR, TAG, "PIProcess Failed: %d", result);
            }
        }
    }

    OIC_LOG(INFO, TAG, "Stopping Zigbee Plugin...");
    result = PIStopPlugin(plugin);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Zigbee Plugin Stop Failed: %d", result);
    }
    OIC_LOG(INFO, TAG, "Zigbee Plugin Stopped");
    // OCStop
IotivityStop:
    OIC_LOG(INFO, TAG, "Stopping IoTivity...");
    result = OCStop();
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCStop Failed: %d", result);
        return 0;
    }

    OIC_LOG(INFO, TAG, "Application Completed Successfully");
    return 0;
}

OCStackResult SetPlatformInfo(void)
{
    static const OCPlatformInfo platformInfo =
        {
            .platformID = "IoTivityZigbeeID",
            .manufacturerName = "IoTivity",
            .manufacturerUrl = "http://iotivity.org",
            .modelNumber = "T1000",
            .dateOfManufacture = "January 14th, 2015",
            .platformVersion = "0.9.2",
            .operatingSystemVersion = "7",
            .hardwareVersion = "0.5",
            .firmwareVersion = "0",
            .supportUrl = "http://iotivity.org",
            .systemTime = ""
        };

    return OCSetPlatformInfo(platformInfo);
}

OCStackResult SetDeviceInfo(void)
{
    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME,
                                      "IoTivity/Zigbee Server Sample"));

    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION,
                                      "IoTivity/Zigbee Device Spec Version"));

    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
                                      "IoTivity/Zigbee Data Model Version"));

    VERIFY_SUCCESS(OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
                                      "4ea65ac9-59a3-4eb8-8d77-76c3ee72c250"));

    OIC_LOG(INFO, TAG, "Device information initialized successfully.");
    return OC_STACK_OK;

exit:
    return OC_STACK_ERROR;

}

bool processSignal(bool set)
{
    static sig_atomic_t signal = 0;
    if (set)
    {
        signal = 1;
    }

    return signal == 1;
}

void processCancel(int signal)
{
    if(signal == SIGINT)
    {
        processSignal(true);
    }
}
