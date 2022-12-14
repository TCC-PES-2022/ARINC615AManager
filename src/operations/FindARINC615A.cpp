//
// Created by kollins on 11/09/2022.
//

#include "FindARINC615A.h"

#include <fstream>
#include <algorithm>
#include <cjson/cJSON.h>

FindARINC615A::FindARINC615A() {
    _findStartedCallback = nullptr;
    _findStartedContext = nullptr;
    _findFinishedCallback = nullptr;
    _findFinishedContext = nullptr;
    _findNewDeviceCallback = nullptr;
    _findNewDeviceContext = nullptr;
}

FindARINC615A::~FindARINC615A() {
    _findStartedCallback = nullptr;
    _findStartedContext = nullptr;
    _findFinishedCallback = nullptr;
    _findFinishedContext = nullptr;
    _findNewDeviceCallback = nullptr;
    _findNewDeviceContext = nullptr;
}

FindOperationResult FindARINC615A::registerFindStartedCallback(
        findStarted callback, void *context) {
    _findStartedCallback = callback;
    _findStartedContext = context;
    return FindOperationResult::FIND_OPERATION_OK;
}

FindOperationResult FindARINC615A::registerFindFinishedCallback(
        findFinished callback, void *context) {
    _findFinishedCallback = callback;
    _findFinishedContext = context;
    return FindOperationResult::FIND_OPERATION_OK;
}

FindOperationResult FindARINC615A::registerFindNewDeviceCallback(
        findNewDevice callback, void *context) {
    _findNewDeviceCallback = callback;
    _findNewDeviceContext = context;
    return FindOperationResult::FIND_OPERATION_OK;
}

FindOperationResult FindARINC615A::find() {
    if (_findStartedCallback != nullptr) {
        _findStartedCallback(_findStartedContext);
    }

    if (_findNewDeviceCallback != nullptr) {
        /********* FIND STUB **********/

        std::ifstream findstub("findstub.json");
        if (findstub.is_open())
        {
            std::string fileContent((std::istreambuf_iterator<char>(findstub)),
                                    std::istreambuf_iterator<char>());
            cJSON *findstubJson = cJSON_Parse(fileContent.c_str());
            if (findstubJson != NULL)
            {
                cJSON *devices = cJSON_GetObjectItem(findstubJson, "devices");
                if (devices != NULL)
                {
                    cJSON *device = devices->child;
                    while (device != NULL)
                    {
                        std::string deviceInfo = cJSON_PrintUnformatted(device);
                        _findNewDeviceCallback(deviceInfo, _findNewDeviceContext);
                        device = device->next;
                    }
                }
            }
            findstub.close();
        }

        /******************************/
    }

    if (_findFinishedCallback != nullptr) {
        _findFinishedCallback(_findFinishedContext);
    }
    return FindOperationResult::FIND_OPERATION_OK;
}
