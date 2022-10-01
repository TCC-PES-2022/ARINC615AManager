//
// Created by kollins on 15/09/2022.
//

#include <stdio.h>
#include <algorithm>

#include "UploadTargetHardwareARINC615A.h"
#include "InitializationFileARINC615A.h"

#define STATUS_UPLOAD_PERIOD 1000 // ms

UploadTargetHardwareARINC615A::UploadTargetHardwareARINC615A(
    std::string dataLoaderIp, int dataLoaderPort)
{
    this->dataLoaderIp = dataLoaderIp;
    this->dataLoaderPort = dataLoaderPort;

    _checkFilesCallback = nullptr;
    _checkFilesContext = nullptr;

    uploadStatusDescription.clear();

    loadUploadInitializationFileBuffer = std::make_shared<std::vector<uint8_t>>();
    loadUploadRequestFileBuffer = std::make_shared<std::vector<uint8_t>>();
    statusHeaderFiles = std::make_shared<std::vector<LoadUploadStatusHeaderFileARINC615A>>();

    uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    currentState = UploadTargetHardwareARINC615AState::CREATED;
    nextState = UploadTargetHardwareARINC615AState::CREATED;

    runMainThread = true;
    runStatusThread = false;
    runUploadThread = false;
    _mainThread = new std::thread(&UploadTargetHardwareARINC615A::mainThread, this);

    loadListRatio = 0;
    uploadAborted = false;
}

UploadTargetHardwareARINC615A::~UploadTargetHardwareARINC615A()
{
    runMainThread = false;
    nextState = UploadTargetHardwareARINC615AState::FINISHED;
    if (_mainThread->joinable())
    {
        _mainThreadCV.notify_one();
        _mainThread->join();
        delete _mainThread;
    }

    statusHeaderFiles->clear();
    statusHeaderFiles.reset();

    loadUploadInitializationFileBuffer.reset();
    loadUploadInitializationFileBuffer = nullptr;

    loadUploadRequestFileBuffer.reset();
    loadUploadRequestFileBuffer = nullptr;
}

UploadOperationResult UploadTargetHardwareARINC615A::registerCheckFilesCallback(
    checkFilesCallback callback, std::shared_ptr<void> context)
{
    _checkFilesCallback = callback;
    _checkFilesContext = context;

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::checkUploadConditions()
{
    // No conditions to check right now.
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::loadUploadInitialization(
    FILE **fp, size_t *bufferSize, std::string &fileName)
{
    if (currentState != UploadTargetHardwareARINC615AState::CREATED)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    InitializationFileARINC615A loadUploadInitializationResponse(fileName);
    if (checkUploadConditions() == UploadOperationResult::UPLOAD_OPERATION_OK)
    {
        loadUploadInitializationResponse.setOperationAcceptanceStatusCode(
            INITIALIZATION_UPLOAD_IS_ACCEPTED);
        nextState = UploadTargetHardwareARINC615AState::ACCEPTED;
    }
    else
    {
        loadUploadInitializationResponse.setOperationAcceptanceStatusCode(
            INITIALIZATION_UPLOAD_IS_DENIED);
        nextState = UploadTargetHardwareARINC615AState::DENIED;
    }

    baseFileName = fileName.substr(0, fileName.find_last_of('.'));

    loadUploadInitializationFileBuffer->clear();
    loadUploadInitializationFileBuffer->resize(0);
    loadUploadInitializationResponse.serialize(loadUploadInitializationFileBuffer);

    (*fp) = fmemopen(loadUploadInitializationFileBuffer->data(),
                     loadUploadInitializationFileBuffer->size(), "r");
    if ((*fp) == NULL)
    {
        (*bufferSize) = 0;
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    (*bufferSize) = loadUploadInitializationFileBuffer->size();

    _mainThreadCV.notify_one();

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::loadUploadRequest(
    FILE **fp, size_t *bufferSize, std::string &fileName)
{
    if (currentState != UploadTargetHardwareARINC615AState::ACCEPTED &&
        // In case the accepted status was not received yet.
        nextState != UploadTargetHardwareARINC615AState::ACCEPTED)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    loadUploadInitializationFileBuffer->clear();
    loadUploadInitializationFileBuffer->resize(MAX_FILE_BUFFER_SIZE);

    (*fp) = fmemopen(loadUploadInitializationFileBuffer->data(),
                     loadUploadInitializationFileBuffer->size(), "w");
    if ((*fp) == NULL)
    {
        (*bufferSize) = 0;
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    (*bufferSize) = loadUploadInitializationFileBuffer->size();

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

NotifierOperationResult UploadTargetHardwareARINC615A::notify(
    NotifierEventType event)
{
    switch (event)
    {
    case NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED:
    {

        if (currentState != UploadTargetHardwareARINC615AState::ACCEPTED &&
            // In case the accepted status was not received yet.
            nextState != UploadTargetHardwareARINC615AState::ACCEPTED)
        {
            return NotifierOperationResult::NOTIFIER_ERROR;
        }

        std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
        headerFiles = std::make_shared<std::vector<LoadUploadRequestHeaderFileARINC615A>>();
        LoadUploadRequestFileARINC615A loadUploadRequestFile;
        if (loadUploadRequestFile.deserialize(loadUploadInitializationFileBuffer) == SerializableOperationResult::SERIALIZABLE_OK)
        {
            loadUploadRequestFile.getHeaderFiles(headerFiles);

            for (std::vector<LoadUploadRequestHeaderFileARINC615A>::iterator it =
                     headerFiles->begin();
                 it != headerFiles->end(); ++it)
            {
                LoadUploadStatusHeaderFileARINC615A statusHeaderFile;

                std::string headerFileName;
                (*it).getHeaderFileName(headerFileName);
                statusHeaderFile.setHeaderFileName(headerFileName);

                std::string loadPartNumberName;
                (*it).getLoadPartNumberName(loadPartNumberName);
                statusHeaderFile.setLoadPartNumberName(loadPartNumberName);

                statusHeaderFile.setLoadRatio(0);
                statusHeaderFile.setLoadStatus(STATUS_UPLOAD_ACCEPTED);
                statusHeaderFiles->push_back(statusHeaderFile);
            }

            nextState = UploadTargetHardwareARINC615AState::IN_PROGRESS;
            _mainThreadCV.notify_one();
        }
        else
        {
            return NotifierOperationResult::NOTIFIER_ERROR;
        }
        break;
    }
    default:
        return NotifierOperationResult::NOTIFIER_ERROR;
        break;
    }
    return NotifierOperationResult::NOTIFIER_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::abort(uint16_t abortSource)
{
    {
        std::lock_guard<std::mutex> lock(abortedMutex);
        if (uploadAborted == true)
        {
            return UploadOperationResult::UPLOAD_OPERATION_OK;
        }
        uploadAborted = true;
    }

    switch (abortSource)
    {
    case UPLOAD_ABORT_SOURCE_TARGETHARDWARE:
        uploadStatusDescription = "Upload aborted by the target hardware.";
        nextState = UploadTargetHardwareARINC615AState::ABORTED_BY_TARGET;
        break;
    case UPLOAD_ABORT_SOURCE_DATALOADER:
        uploadStatusDescription = "Upload aborted by the data loader.";
        nextState = UploadTargetHardwareARINC615AState::ABORTED_BY_DATALOADER;
        break;
    case UPLOAD_ABORT_SOURCE_OPERATOR:
        uploadStatusDescription = "Upload aborted by the operator.";
        nextState = UploadTargetHardwareARINC615AState::ABORTED_BY_OPERATOR;
        break;
    default:
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
        break;
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::getState(
    UploadTargetHardwareARINC615AState &state)
{
    state = currentState;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::mainThread()
{
    bool statusThreadStarted = false;
    std::thread *_statusThread;

    bool uploadThreadStarted = false;
    std::thread *_uploadThread;

    runStatusThread = false;
    runUploadThread = false;

    while (runMainThread)
    {
        {
            std::unique_lock<std::mutex> lock(_mainThreadMutex);
            _mainThreadCV.wait(lock, [this]
                               { return currentState != nextState; });
        }

        switch (nextState)
        {
        case UploadTargetHardwareARINC615AState::ACCEPTED:
            runStatusThread = true;
            if (!statusThreadStarted)
            {
                _statusThread = new std::thread(&UploadTargetHardwareARINC615A::statusThread, this);
                statusThreadStarted = true;
            }
            break;
        case UploadTargetHardwareARINC615AState::IN_PROGRESS:
        case UploadTargetHardwareARINC615AState::IN_PROGRESS_WITH_DESCRIPTION:
            runUploadThread = true;
            if (!uploadThreadStarted)
            {
                _uploadThread = new std::thread(&UploadTargetHardwareARINC615A::uploadThread, this);
                uploadThreadStarted = true;
            }
            break;
        case UploadTargetHardwareARINC615AState::COMPLETED:
        case UploadTargetHardwareARINC615AState::ABORTED_BY_TARGET:
        case UploadTargetHardwareARINC615AState::ABORTED_BY_DATALOADER:
        case UploadTargetHardwareARINC615AState::ABORTED_BY_OPERATOR:
            runUploadThread = false;
            break;
        default:
            runMainThread = false;
            break;
        }
    }

    runStatusThread = false;
    if (statusThreadStarted && _statusThread->joinable())
    {
        _statusThread->join();
        delete _statusThread;
    }

    runUploadThread = false;
    if (uploadThreadStarted && _uploadThread->joinable())
    {
        _uploadThread->join();
        delete _uploadThread;
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

TftpClientOperationResult UploadTargetHardwareARINC615A::tftpUploadErrorCbk(
    short error_code, std::string &error_message, void *context)
{
    if (context != NULL && error_code == 0)
    {
        UploadTargetHardwareARINC615A *uploadTargetHardwareARINC615A =
            (UploadTargetHardwareARINC615A *)context;

        std::string abortPrefix = std::string(ARINC_ABORT_MSG_PREFIX) +
                                  std::string(ARINC_ERROR_MSG_DELIMITER);
        size_t pos = error_message.find(abortPrefix);
        if (pos != std::string::npos)
        {
            pos = error_message.find(ARINC_ERROR_MSG_DELIMITER);
            std::string abortCode = error_message.substr(
                pos + 1, error_message.length());

            uploadTargetHardwareARINC615A->abort(std::stoul(abortCode, nullptr, 16));
            return TftpClientOperationResult::TFTP_CLIENT_OK;
        }

        std::string waitPrefix = std::string(ARINC_WAIT_MSG_PREFIX) +
                                 std::string(ARINC_ERROR_MSG_DELIMITER);
        pos = error_message.find(waitPrefix);
        if (pos != std::string::npos)
        {
            pos = error_message.find(ARINC_ERROR_MSG_DELIMITER);
            std::string waitSeconds = error_message.substr(
                pos + 1, error_message.length());

            uploadTargetHardwareARINC615A->uploadWaitTime = std::stoi(waitSeconds);

            return TftpClientOperationResult::TFTP_CLIENT_OK;
        }
    }
    return TftpClientOperationResult::TFTP_CLIENT_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::statusThread()
{
    TFTPClient uploadClient;
    uploadClient.setConnection(dataLoaderIp.c_str(), dataLoaderPort);
    uploadClient.registerTftpErrorCallback(UploadTargetHardwareARINC615A::tftpUploadErrorCbk, this);

    std::string statusFileName = baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
    

    std::shared_ptr<std::vector<uint8_t>> loadUploadStatusFileBuffer = std::make_shared<std::vector<uint8_t>>();

    uint8_t sendRetry = MAX_DLP_TRIES;

    bool sendOnce = false;

    while (runStatusThread && !sendOnce && sendRetry > 0)
    {
        switch (nextState)
        {
        case UploadTargetHardwareARINC615AState::ACCEPTED:
            uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
            break;
        case UploadTargetHardwareARINC615AState::IN_PROGRESS:
            uploadOperationStatusCode = STATUS_UPLOAD_IN_PROGRESS;
            break;
        case UploadTargetHardwareARINC615AState::IN_PROGRESS_WITH_DESCRIPTION:
            uploadOperationStatusCode = STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION;
            break;
        case UploadTargetHardwareARINC615AState::COMPLETED:
            uploadOperationStatusCode = STATUS_UPLOAD_COMPLETED;
            sendOnce = true;
            break;
        case UploadTargetHardwareARINC615AState::ABORTED_BY_TARGET:
            uploadOperationStatusCode = STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE;
            sendOnce = true;
            break;
        case UploadTargetHardwareARINC615AState::ABORTED_BY_DATALOADER:
            uploadOperationStatusCode = STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST;
            sendOnce = true;
            break;
        case UploadTargetHardwareARINC615AState::ABORTED_BY_OPERATOR:
            uploadOperationStatusCode = STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST;
            sendOnce = true;
            break;
        default:
            uploadOperationStatusCode = STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE;
            break;
        }

        // Prepare the status file
        LoadUploadStatusFileARINC615A loadUploadStatusFile(statusFileName);

        loadUploadStatusFile.setUploadOperationStatusCode(uploadOperationStatusCode);
        if (uploadOperationStatusCode == STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION ||
            uploadOperationStatusCode == STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE)
        {
            loadUploadStatusFile.setUploadStatusDescription(uploadStatusDescription);
        }

        uint16_t counter;
        loadUploadStatusFile.getCounter(counter);
        loadUploadStatusFile.setCounter(++counter);
        loadUploadStatusFile.setExceptionTimer(0);

        if (uploadOperationStatusCode == STATUS_UPLOAD_IN_PROGRESS ||
            uploadOperationStatusCode == STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION)
        {
            loadUploadStatusFile.setEstimatedTime(0xFFFF);
        }
        else
        {
            loadUploadStatusFile.setEstimatedTime(0);
        }

        loadUploadStatusFile.setLoadListRatio(loadListRatio);
        for (std::vector<LoadUploadStatusHeaderFileARINC615A>::iterator it =
                 statusHeaderFiles->begin();
             it != statusHeaderFiles->end(); ++it)
        {
            loadUploadStatusFile.addHeaderFile(*it);
        }

        loadUploadStatusFileBuffer->clear();
        loadUploadStatusFileBuffer->resize(0);
        loadUploadStatusFile.serialize(loadUploadStatusFileBuffer);
        FILE *fp = fmemopen(loadUploadStatusFileBuffer->data(),
                            loadUploadStatusFileBuffer->size(), "r");

        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_ERROR;
        if (fp != NULL)
        {
            result = uploadClient.sendFile(statusFileName.c_str(), fp);
            fclose(fp);
        }

        if (result == TftpClientOperationResult::TFTP_CLIENT_OK)
        {
            sendRetry = MAX_DLP_TRIES;
            currentState = nextState;
        }
        else
        {
            sendRetry--;
            sendOnce = false; // Will try again if we have retries left.
        }
#ifdef PARALLEL_UPLOAD
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<uint16_t>(STATUS_UPLOAD_PERIOD));
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(
            std::max(uploadWaitTime, static_cast<uint16_t>(STATUS_UPLOAD_PERIOD))));
#endif
    }

    if (sendRetry == 0)
    {
        nextState = UploadTargetHardwareARINC615AState::ERROR;
    }
    else
    {
        nextState = UploadTargetHardwareARINC615AState::FINISHED;
    }
    _mainThreadCV.notify_one();

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

/*
 * TODO:
 * To perform parallel upload, the fetch file needs to be non-blocking, so we
 * can cancel the upload if aborted.
 */
#ifdef PARALLEL_UPLOAD
TftpClientOperationResult UploadTargetHardwareARINC615A::tftpUploadErrorCbk(
    short error_code, std::string &error_message, void *context)
{
    if (context != NULL && error_code == 0)
    {
        UploadHeaderThreadContext *ctx = (UploadHeaderThreadContext *)context;

        std::string abortPrefix = std::string(ARINC_ABORT_MSG_PREFIX) +
                                  std::string(ARINC_ERROR_MSG_DELIMITER);
        size_t pos = error_message.find(abortPrefix);
        if (pos != std::string::npos)
        {
            pos = error_message.find(ARINC_ERROR_MSG_DELIMITER);
            std::string abortCode = error_message.substr(
                pos + 1, error_message.length());

            ctx->thiz->abort(std::stoul(abortCode, nullptr, 16));
            return TftpClientOperationResult::TFTP_CLIENT_OK;
        }

        std::string waitPrefix = std::string(ARINC_WAIT_MSG_PREFIX) +
                                 std::string(ARINC_ERROR_MSG_DELIMITER);
        pos = error_message.find(waitPrefix);
        if (pos != std::string::npos)
        {
            pos = error_message.find(ARINC_ERROR_MSG_DELIMITER);
            std::string waitSeconds = error_message.substr(
                pos + 1, error_message.length());

            {
                std::lock_guard<std::mutex> lock(ctx->thiz->uploadWaitTimeMutex);
                ctx->uploadWaitTime[ctx->uploadHeaderThreadId] = std::stoi(waitSeconds);
            }

            return TftpClientOperationResult::TFTP_CLIENT_OK;
        }
    }
    return TftpClientOperationResult::TFTP_CLIENT_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::uploadHeaderThread()
{
    std::thread::id id = std::this_thread::get_id();

    TFTPClient uploadClient;
    uploadClient.setConnection(dataLoaderIp.c_str(), dataLoaderPort);

    UploadHeaderThreadContext context;
    context.thiz = this;
    context.uploadHeaderThreadId = id;
    uploadClient.registerTftpErrorCallback(UploadTargetHardwareARINC615A::tftpUploadErrorCbk, &context);

    uint8_t fetchRetry = MAX_DLP_TRIES;

    LoadUploadStatusHeaderFileARINC615A headerStatusFile;

    {
        std::lock_guard<std::mutex> lock(uploadHeaderMutex);
        if (uploadHeaderIt == statusHeaderFiles->end())
        {
            return UploadOperationResult::UPLOAD_OPERATION_ERROR;
        }
        headerStatusFile = (*uploadHeaderIt);
        uploadHeaderIt++;
    }

    // Restore retry counter if we receive a wait message.
    fetchRetry = (uploadWaitTime[id] > 0) ? MAX_DLP_TRIES : fetchRetry;

    std::this_thread::sleep_for(std::chrono::seconds(uploadWaitTime[id]));
    {
        std::lock_guard<std::mutex> lock(uploadWaitTimeMutex);
        uploadWaitTime[id] = 0;
    }

    headerStatusFile.setLoadStatus(STATUS_UPLOAD_IN_PROGRESS);
    headerStatusFile.setLoadRatio(0);

    std::string headerFileName;
    headerStatusFile.getHeaderFileName(headerFileName);

    // Remove file path if any
    std::string cleanHeaderFileName = headerFileName;
    size_t fileNamePosition = headerFileName.find_last_of("/\\");
    if (fileNamePosition != std::string::npos)
    {
        cleanHeaderFileName = headerFileName.substr(fileNamePosition + 1);
    }

    FILE *fp = fopen(cleanHeaderFileName.c_str(), "w");
    TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_ERROR;
    if (fp != NULL)
    {
        do
        {
            result = uploadClient.fetchFile(headerFileName.c_str(), fp);
        } while (result == TftpClientOperationResult::TFTP_CLIENT_ERROR &&
                 runUploadThread && fetchRetry-- > 0);
        fclose(fp);
    }
    if (result != TftpClientOperationResult::TFTP_CLIENT_OK)
    {
        headerStatusFile.setLoadStatus(STATUS_UPLOAD_HEAD_FILE_FAILED);
        headerStatusFile.setLoadStatusDescription("Failed to fetch header file");
    }
    else
    {
        headerStatusFile.setLoadStatus(STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION);
        headerStatusFile.setLoadRatio(50);
        headerStatusFile.setLoadStatusDescription("Checking received file...");

        if (_checkFilesCallback != nullptr)
        {
            std::string checkFileReport;
            std::vector<std::string> receivedFiles = {cleanHeaderFileName};
            if (_checkFilesCallback(receivedFiles, checkFileReport, _checkFilesContext) ==
                UploadOperationResult::UPLOAD_OPERATION_ERROR)
            {
                headerStatusFile.setLoadStatus(STATUS_UPLOAD_HEAD_FILE_FAILED);
                headerStatusFile.setLoadStatusDescription(checkFileReport);
            }
        }
        headerStatusFile.setLoadStatus(STATUS_UPLOAD_COMPLETED);
        headerStatusFile.setLoadRatio(100);
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadTargetHardwareARINC615A::uploadThread()
{

    uploadHeaderIt = statusHeaderFiles->begin();
    std::vector<std::thread> headerThreads;

    for (uint8_t i = 0; i < statusHeaderFiles->size; i++)
    {
        headerThreads.push_back(std::thread(&UploadTargetHardwareARINC615A::uploadHeaderThread, this));
    }

    for (uint8_t i = 0; i < statusHeaderFiles->size; i++)
    {
        loadListRatio = (i * 100) / statusHeaderFiles->size;
        headerThreads[i].join();
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}
#else
UploadOperationResult UploadTargetHardwareARINC615A::uploadThread()
{
    TFTPClient uploadClient;
    uploadClient.setConnection(dataLoaderIp.c_str(), dataLoaderPort);
    uploadClient.registerTftpErrorCallback(UploadTargetHardwareARINC615A::tftpUploadErrorCbk, this);

    uint8_t fetchRetry = MAX_DLP_TRIES;

    bool receiveError = false;

    uint32_t numOfSuccessfullUploads = 0;
    uint32_t numOfFilesToUpload = statusHeaderFiles->size();
    loadListRatio = 0;
    std::vector<LoadUploadStatusHeaderFileARINC615A>::iterator it;
    for (it = statusHeaderFiles->begin();
         (it != statusHeaderFiles->end()) && runUploadThread;
         it = (uploadWaitTime > 0) ? it : it + 1)
    {
        std::string headerFileName;
        (*it).getHeaderFileName(headerFileName);

        if (uploadWaitTime > 0)
        {
            (*it).setLoadStatus(STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION);
            (*it).setLoadRatio(0);
            (*it).setLoadStatusDescription("Waiting " + std::to_string(uploadWaitTime) +
                                           " seconds before upload...");

            nextState = UploadTargetHardwareARINC615AState::IN_PROGRESS_WITH_DESCRIPTION;
            uploadStatusDescription = "Waiting file " + headerFileName + " to be available...";
            _mainThreadCV.notify_one();
        }

        std::this_thread::sleep_for(std::chrono::seconds(uploadWaitTime));
        uploadWaitTime = 0;

        (*it).setLoadStatus(STATUS_UPLOAD_IN_PROGRESS);
        (*it).setLoadRatio(0);

        nextState = UploadTargetHardwareARINC615AState::IN_PROGRESS_WITH_DESCRIPTION;
        _mainThreadCV.notify_one();

        // Remove file path if any
        std::string cleanHeaderFileName = headerFileName;
        size_t fileNamePosition = headerFileName.find_last_of("/\\");
        if (fileNamePosition != std::string::npos)
        {
            cleanHeaderFileName = headerFileName.substr(fileNamePosition + 1);
        }

        FILE *fp = fopen(cleanHeaderFileName.c_str(), "w");
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_ERROR;
        if (fp != NULL)
        {
            do
            {
                result = uploadClient.fetchFile(headerFileName.c_str(), fp);
            } while (result == TftpClientOperationResult::TFTP_CLIENT_ERROR &&
                     runUploadThread && fetchRetry-- > 0 && uploadWaitTime == 0);
            fclose(fp);
        }
        if (uploadWaitTime > 0)
        {
            fetchRetry = MAX_DLP_TRIES;
            continue;
        }
        if (result != TftpClientOperationResult::TFTP_CLIENT_OK)
        {
            (*it).setLoadStatus(STATUS_UPLOAD_HEAD_FILE_FAILED);
            (*it).setLoadStatusDescription("Failed to fetch header file");
            receiveError = true;
            break;
        }
        else
        {
            (*it).setLoadStatus(STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION);
            (*it).setLoadRatio(50);
            (*it).setLoadStatusDescription("Checking received file...");

            if (_checkFilesCallback != nullptr)
            {
                std::string checkFilesReport;
                std::vector<std::string> receivedFiles = {cleanHeaderFileName};
                if (_checkFilesCallback(receivedFiles, checkFilesReport, _checkFilesContext) ==
                    UploadOperationResult::UPLOAD_OPERATION_ERROR)
                {
                    (*it).setLoadStatus(STATUS_UPLOAD_HEAD_FILE_FAILED);
                    (*it).setLoadStatusDescription(checkFilesReport);
                    receiveError = true;
                    break;
                }
            }
            (*it).setLoadStatus(STATUS_UPLOAD_COMPLETED);
            (*it).setLoadRatio(100);
            numOfSuccessfullUploads++;
            loadListRatio = (numOfSuccessfullUploads * 100) / numOfFilesToUpload;
        }
    }

    // In case of abort, define status of remaining files.
    while (it != statusHeaderFiles->end())
    {
        (*it).setLoadStatus(uploadOperationStatusCode);
        (*it).setLoadStatusDescription(uploadStatusDescription);
        it++;
    }

    if (receiveError)
    {
        abort(UPLOAD_ABORT_SOURCE_TARGETHARDWARE);
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    nextState = UploadTargetHardwareARINC615AState::COMPLETED;
    _mainThreadCV.notify_one();

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}
#endif