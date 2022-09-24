//
// Created by kollins on 15/09/2022.
//

#include "UploadTargetHardwareARINC615A.h"

UploadTargetHardwareARINC615A::UploadTargetHardwareARINC615A(
        std::string dataLoaderIp, int dataLoaderPort)
{
    dataLoaderIp = dataLoaderIp;
    dataLoaderPort = dataLoaderPort;
    loadList.clear();
}

UploadTargetHardwareARINC615A::~UploadTargetHardwareARINC615A()
{
    loadList.clear();
}

UploadOperationResult
UploadTargetHardwareARINC615A::registerAbortRequestCallback (
        abortRequestCallback callback,
        std::shared_ptr<void> context)
{
    _abortRequestCallback = callback;
    _abortRequestContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::uploadRequest(
        FILE **initializationFile)
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::uploadLoadListRequest(
        FILE **fd)
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::uploadLoadListReceived()
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::waitUploadOperation()
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::abort(uint16_t abortSource)
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}
