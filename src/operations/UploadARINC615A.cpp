//
// Created by kollins on 12/09/2022.
//

#include "UploadARINC615A.h"


/******************************** DATA LOADER ********************************/

UploadDataLoaderARINC615A::UploadDataLoaderARINC615A(
        std::string targetIp, std::vector<std::string> loadList)
{
    targetIp = targetIp;
    loadList = loadList;
    loadList.clear();
}

UploadDataLoaderARINC615A::~UploadDataLoaderARINC615A()
{
    loadList.clear();
}

UploadOperationResult
UploadDataLoaderARINC615A::registerUploadInitializationResponseCallback (
        uploadInitializationResponseCallback callback,
        std::shared_ptr<void> context)
{
    _uploadInitializationResponseCallback = callback;
    _uploadInitializationResponseContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerUploadInformationStatusCallback (
        uploadInformationStatusCallback callback,
        std::shared_ptr<void> context)
{
    _uploadInformationStatusCallback = callback;
    _uploadInformationStatusContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerFileNotAvailableCallback (
        fileNotAvailableCallback callback,
        std::shared_ptr<void> context)
{
    _fileNotAvailableCallback = callback;
    _fileNotAvailableContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerAbortRequestCallback (
        abortRequestCallback callback,
        std::shared_ptr<void> context)
{
    _abortRequestCallback = callback;
    _abortRequestContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::upload()
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

/****************************** TARGET HARDWARE ******************************/

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

UploadOperationResult UploadTargetHardwareARINC615A::uploadRequest()
{
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}
