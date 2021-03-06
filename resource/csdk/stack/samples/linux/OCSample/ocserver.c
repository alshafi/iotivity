//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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


#include "iotivity_config.h"
#include "ocstack.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <signal.h>
#include <stdbool.h>

/// This example is using experimental API, so there is no guarantee of support for future release,
/// nor any there any guarantee that breaking changes will not occur across releases.
#include "experimental/logger.h"

#define TAG ("ocserver")

int gQuitFlag = 0;
OCStackResult createLightResource(void);

typedef struct LIGHTRESOURCE{
    OCResourceHandle handle;
    bool power;
} LightResource;

static LightResource Light;

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

int main(void) {
    OIC_LOG_V(INFO, TAG, "Starting ocserver");
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /*
     * Declare and create the example resource: Light
     */
    if(createLightResource() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack cannot create resource...");
    }

    // Break from loop with Ctrl-C
    OIC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(1);
    }

    OIC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}

OCStackResult createLightResource(void) {
    Light.power = false;
    OCStackResult res = OCCreateResource(&Light.handle,
                    "core.light",
                    "core.rw",
                    "/a/light",
                    0,
                    NULL,
                    OC_DISCOVERABLE|OC_OBSERVABLE);
    return res;
}

