//
// Created by kollins on 12/09/2022.
//

#include "UploadDataLoaderARINC615A.h"
#include "InitializationFileARINC615A.h"
#include "LoadUploadRequestFileARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"

#include <thread>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <sstream>

#define MAX_DLP_TRIES 2

UploadDataLoaderARINC615A::UploadDataLoaderARINC615A(std::string targetHardwareId,
                                                     std::string targetHardwarePosition,
                                                     std::string targetHardwareIp)
{
    this->targetHardwareId = targetHardwareId;
    this->targetHardwarePosition = targetHardwarePosition;
    this->targetHardwareIp = targetHardwareIp;
    this->loadList.clear();

    tftpClient = nullptr;
    tftpServer = nullptr;

    tftpDataLoaderServerPort = DEFAULT_ARINC615A_TFTP_PORT;
    tftpTargetHardwareServerPort = DEFAULT_ARINC615A_TFTP_PORT;
}

UploadDataLoaderARINC615A::~UploadDataLoaderARINC615A()
{
    loadList.clear();

    if (tftpClient != nullptr)
    {
        tftpClient.reset();
        tftpClient = nullptr;
    }

    if (tftpServer != nullptr)
    {
        tftpServer.reset();
        tftpServer = nullptr;
    }
}

UploadOperationResult
UploadDataLoaderARINC615A::registerUploadInitializationResponseCallback(
    uploadInitializationResponseCallback callback,
    std::shared_ptr<void> context)
{
    _uploadInitializationResponseCallback = callback;
    _uploadInitializationResponseContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerUploadInformationStatusCallback(
    uploadInformationStatusCallback callback,
    std::shared_ptr<void> context)
{
    _uploadInformationStatusCallback = callback;
    _uploadInformationStatusContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerFileNotAvailableCallback(
    fileNotAvailableCallback callback,
    std::shared_ptr<void> context)
{
    _fileNotAvailableCallback = callback;
    _fileNotAvailableContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult
UploadDataLoaderARINC615A::registerAbortRequestCallback(
    abortRequestCallback callback,
    std::shared_ptr<void> context)
{
    _abortRequestCallback = callback;
    _abortRequestContext = context;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::setLoadList(
    std::vector<ArincLoad> loadList)
{
    if (loadList.size() == 0)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    if (loadList.size() > MAX_NUM_FILES)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    this->loadList.clear();
    this->loadList = loadList;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::setTftpTargetHardwareServerPort(
    uint16_t port)
{
    tftpTargetHardwareServerPort = port;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::setTftpDataLoaderServerPort(
    uint16_t port)
{
    tftpDataLoaderServerPort = port;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::abort(
    uint16_t abortSource)
{
    this->abortSource = abortSource;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::initTFTP()
{
    tftpClient = std::unique_ptr<TFTPClient>(new TFTPClient());
    if (tftpClient == nullptr)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    tftpServer = std::unique_ptr<TFTPServer>(new TFTPServer());
    if (tftpServer == nullptr)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    TftpClientOperationResult resultTftpClientOperation;
    resultTftpClientOperation = tftpClient->setConnection(
        targetHardwareIp.c_str(), tftpTargetHardwareServerPort);
    if (resultTftpClientOperation != TftpClientOperationResult::TFTP_CLIENT_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    TftpServerOperationResult resultTftpServerOperation;
    resultTftpServerOperation = tftpServer->setPort(
        tftpDataLoaderServerPort);
    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    //    resultTftpServerOperation = tftpServer->setTimeout(
    //            DEFAULT_ARINC615A_TFTP_TIMEOUT);
    //    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    //    {
    //        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    //    }

    resultTftpServerOperation = tftpServer->registerOpenFileCallback(
        UploadDataLoaderARINC615A::targetHardwareOpenFileRequest, this);
    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    resultTftpServerOperation = tftpServer->registerCloseFileCallback(
        UploadDataLoaderARINC615A::targetHardwareCloseFileRequest, this);
    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    resultTftpServerOperation = tftpServer->registerSectionStartedCallback(
        UploadDataLoaderARINC615A::targetHardwareSectionStarted, this);
    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    resultTftpServerOperation = tftpServer->registerSectionFinishedCallback(
        UploadDataLoaderARINC615A::targetHardwareSectionFinished, this);
    if (resultTftpServerOperation != TftpServerOperationResult::TFTP_SERVER_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::initUploadFiles(
    LoadUploadRequestFileARINC615A &loadUploadRequestFile)
{
    for (ArincLoad load : loadList)
    {
        std::string headerName = std::get<LOAD_FILE_NAME_IDX>(load);
        std::string loadPartNumberName = std::get<LOAD_PART_NUMBER_IDX>(load);

        LoadUploadRequestHeaderFileARINC615A headerFile;
        headerFile.setHeaderFileName(headerName);
        headerFile.setLoadPartNumberName(loadPartNumberName);
        loadUploadRequestFile.addHeaderFile(headerFile);
    }
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::upload()
{
    abortSource = UPLOAD_ABORT_SOURCE_NONE;
    endUpload = false;
    uploadInitializationAccepted = false;
    uploadCompleted = false;

    if (loadList.size() == 0)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    std::shared_ptr<std::vector<uint8_t>> fileBuffer = std::make_shared<
        std::vector<uint8_t>>();
    if (fileBuffer == nullptr)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    /***************************************************************************
                                     START TFTP
    ***************************************************************************/
    if (initTFTP() != UploadOperationResult::UPLOAD_OPERATION_OK)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }
    std::thread serverThread = std::thread([this]
                                           { tftpServer->startListening(); });

    /***************************************************************************
                                INITIALIZATION
    ***************************************************************************/

    std::string baseFileName = targetHardwareId + std::string("_") + targetHardwarePosition;

    /********************* [TH_Uploading_Initialization] *********************/
    std::string initializationFileName = baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION;

    fileBuffer->clear();
    fileBuffer->resize(MAX_FILE_BUFFER_SIZE);
    unsigned char *initializationFileBuffer = fileBuffer->data();
    memset(initializationFileBuffer, 0, fileBuffer->size());
    FILE *fpInitializationFile = fmemopen(initializationFileBuffer,
                                          fileBuffer->size(), "w");
    if (fpInitializationFile == NULL)
    {
        endUpload = true;
        tftpServer->stopListening();
        serverThread.join();
        fileBuffer.reset();
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    TftpClientOperationResult resultTftpClientOperation;
    int numTries = 0;
    do
    {
        fseek(fpInitializationFile, 0, SEEK_SET);
        resultTftpClientOperation = tftpClient->fetchFile(
            initializationFileName.c_str(), fpInitializationFile);
        numTries++;
    } while ((resultTftpClientOperation != TftpClientOperationResult::TFTP_CLIENT_OK) && (numTries < MAX_DLP_TRIES));
    fclose(fpInitializationFile);

    if (resultTftpClientOperation != TftpClientOperationResult::TFTP_CLIENT_OK)
    {
        endUpload = true;
        tftpServer->stopListening();
        serverThread.join();
        fileBuffer.reset();
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    InitializationFileARINC615A initializationFile;
    initializationFile.deserialize(fileBuffer);

    uint16_t operationAcceptanceStatusCode;
    initializationFile.getOperationAcceptanceStatusCode(
        operationAcceptanceStatusCode);

    if (_uploadInitializationResponseContext != nullptr)
    {
        std::string jsonResponse("");
        initializationFile.serializeJSON(jsonResponse);
        _uploadInitializationResponseCallback(
            jsonResponse,
            _uploadInitializationResponseContext);
    }

    if (operationAcceptanceStatusCode !=
        INITIALIZATION_UPLOAD_IS_ACCEPTED)
    {
        tftpServer->stopListening();
        serverThread.join();
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    std::thread clientProcessorThread = std::thread([this]
                                                    { this->clientProcessor(); });

    /*********** Wait for status file with operation accepted code ***********/

    {
        std::unique_lock<std::mutex> lock(endUploadMutex);
        filesProcessedCV.wait(lock, [this]
                              { return uploadInitializationAccepted || endUpload; });
    }

    if (endUpload)
    {
        tftpServer->stopListening();
        serverThread.join();
        clientProcessorThread.join();
        fileBuffer.reset();
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    /****************************** [Load_List] ******************************/
    std::string loadListFileName = baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION;

    LoadUploadRequestFileARINC615A loadUploadRequestFile(loadListFileName);
    initUploadFiles(loadUploadRequestFile);

    fileBuffer->clear();
    fileBuffer->resize(0);
    loadUploadRequestFile.serialize(fileBuffer);

    unsigned char *loadListFileBuffer = fileBuffer->data();
    FILE *fpLoadListFile = fmemopen(loadListFileBuffer,
                                    fileBuffer->size(), "r");
    numTries = 0;
    do
    {
        fseek(fpLoadListFile, 0, SEEK_SET);
        resultTftpClientOperation = tftpClient->sendFile(
            loadListFileName.c_str(), fpLoadListFile);
        numTries++;
    } while ((resultTftpClientOperation != TftpClientOperationResult::TFTP_CLIENT_OK) && (numTries < MAX_DLP_TRIES));
    fclose(fpLoadListFile);

    if (resultTftpClientOperation != TftpClientOperationResult::TFTP_CLIENT_OK)
    {
        endUpload = true;
        clientProcessorCV.notify_one();
        tftpServer->stopListening();
        serverThread.join();
        clientProcessorThread.join();
        fileBuffer.reset();
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    /***************************************************************************
                                   WAITING UPLOAD
    ***************************************************************************/
    {
        std::unique_lock<std::mutex> lock(endUploadMutex);
        endUploadCV.wait(lock, [this]
                         { return endUpload; });
    }

    tftpServer->stopListening();
    serverThread.join();
    clientProcessorThread.join();
    fileBuffer.reset();

    return uploadCompleted ? UploadOperationResult::UPLOAD_OPERATION_OK
                           : UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

TftpServerOperationResult UploadDataLoaderARINC615A::targetHardwareSectionStarted(
    ITFTPSection *sectionHandler, void *context)
{
    if (context != nullptr)
    {
        UploadDataLoaderARINC615A *thiz =
            static_cast<UploadDataLoaderARINC615A *>(context);
        TftpSectionId id;
        sectionHandler->getSectionId(&id);
        std::shared_ptr<TargetClient> targetClient = std::make_shared<TargetClient>(id);
        {
            std::lock_guard<std::mutex> lock(thiz->targetClientsMutex);
            thiz->targetClients[id] = targetClient;
        }
    }

    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult UploadDataLoaderARINC615A::targetHardwareSectionFinished(
    ITFTPSection *sectionHandler, void *context)
{
    if (context != nullptr)
    {
        UploadDataLoaderARINC615A *thiz =
            static_cast<UploadDataLoaderARINC615A *>(context);
        TftpSectionId id;
        sectionHandler->getSectionId(&id);
        {
            std::lock_guard<std::mutex> lock(thiz->targetClientsMutex);
            thiz->targetClients[id]->setSectionFinished();
        }
        thiz->clientProcessorCV.notify_one();
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

/*
 * If upload operation is aborted, all subsequent TFTP transfers will
 * receive an abort message, but we still need to wait for the abort
 * confirmation from the target hardware, so LUS files will still be accepted.
 */
UploadOperationResult UploadDataLoaderARINC615A::sendAbortMessage(
    uint16_t abortSource, ITFTPSection *sectionHandler, char *filename,
    char *mode)
{
    std::string lusExtension =
        std::string(UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION);

    if (std::strcmp(mode, "w") == 0 &&
        std::strstr(filename, lusExtension.c_str()) != nullptr)
    {
        // Status file will still be accepted.
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    if (abortSource != UPLOAD_ABORT_SOURCE_NONE)
    {
        std::stringstream errorMessageStream;
        errorMessageStream << ARINC_ABORT_MSG_PREFIX;
        errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
        errorMessageStream << std::hex << abortSource;
        std::string errorMessage = errorMessageStream.str();
        sectionHandler->setErrorMessage(errorMessage);
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }

    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

TftpServerOperationResult UploadDataLoaderARINC615A::targetHardwareOpenFileRequest(
    ITFTPSection *sectionHandler, FILE **fp, char *filename, char *mode,
    size_t *bufferSize, void *context)
{
    UploadDataLoaderARINC615A *thiz;
    if (context != nullptr)
    {
        thiz = static_cast<UploadDataLoaderARINC615A *>(context);

        if (thiz->sendAbortMessage(thiz->abortSource, sectionHandler,
                                   filename, mode) == UploadOperationResult::UPLOAD_OPERATION_OK)
        {
            (*fp) = NULL;
            return TftpServerOperationResult::TFTP_SERVER_ERROR;
        }

        if (std::strcmp(mode, "r") == 0)
        {
            *fp = fopen(filename, mode);
            if (bufferSize != nullptr)
            {
                *bufferSize = 0;
            }
            if (*fp == NULL)
            {
                uint16_t waitTime = DEFAULT_WAIT_TIME;
                if (thiz->_fileNotAvailableCallback != nullptr)
                {
                    thiz->_fileNotAvailableCallback(&waitTime,
                                                    thiz->_fileNotAvailableContext);
                }

                std::stringstream errorMessageStream;
                errorMessageStream << ARINC_WAIT_MSG_PREFIX;
                errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
                errorMessageStream << waitTime;
                std::string errorMessage = errorMessageStream.str();
                sectionHandler->setErrorMessage(errorMessage);

                return TftpServerOperationResult::TFTP_SERVER_ERROR;
            }
        }
        else
        {
            TftpSectionId id;
            sectionHandler->getSectionId(&id);
            {
                std::lock_guard<std::mutex> lock(thiz->targetClientsMutex);
                if (thiz->targetClients[id]->getClientFileBufferReference(fp) ==
                    UploadOperationResult::UPLOAD_OPERATION_OK)
                {
                    thiz->targetClients[id]->setFileName(std::string(filename));
                }
            }
        }

        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    else
    {
        (*fp) = NULL;
    }

    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult UploadDataLoaderARINC615A::targetHardwareCloseFileRequest(
    ITFTPSection *sectionHandler, FILE *fp, void *context)
{
    if (fp != NULL)
    {
        fclose(fp);
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::clientProcessor()
{
    while (!endUpload)
    {
        /*********************** Wait for client event ***********************/
        {
            std::unique_lock<std::mutex> lock(clientProcessorMutex);
            clientProcessorCV.wait_for(lock,
                                       std::chrono::seconds(
                                           DEFAULT_ARINC615A_DLP_TIMEOUT));
        }

        /************************ Process client event ***********************/
        endUpload = (targetClients.size() == 0);
        {
            std::lock_guard<std::mutex> lock(targetClientsMutex);
            for (std::unordered_map<TftpSectionId, std::shared_ptr<TargetClient>>::iterator it = targetClients.begin();
                 (it != targetClients.end()) && (!endUpload);)
            {
                bool sectionFinished, hasDataToProcess;
                (*it).second->isSectionFinished(sectionFinished);
                (*it).second->hasDataToProcess(hasDataToProcess);
                if (sectionFinished)
                {
                    if (hasDataToProcess)
                    {
                        std::string fileName;
                        char *buffer;
                        (*it).second->getFileName(fileName);
                        (*it).second->getClientBufferReference(&buffer);
                        if (buffer != NULL)
                        {
                            processFile(fileName, buffer);
                        }
                    }
                    it = targetClients.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        filesProcessedCV.notify_one();
    }

    this->targetClients.clear();
    endUploadCV.notify_one();

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::processFile(
    std::string fileName, char *buffer)
{
    if (fileName.find(UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION) != std::string::npos)
    {
        return processLoadUploadStatusFile(buffer);
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

UploadOperationResult UploadDataLoaderARINC615A::processLoadUploadStatusFile(
    char *buffer)
{
    std::shared_ptr<std::vector<uint8_t>> data =
        std::make_shared<std::vector<uint8_t>>(buffer,
                                               buffer + MAX_FILE_BUFFER_SIZE);
    LoadUploadStatusFileARINC615A loadUploadStatusFile;
    loadUploadStatusFile.deserialize(data);

    if (_uploadInformationStatusCallback != nullptr)
    {
        std::string jsonResponse("");
        loadUploadStatusFile.serializeJSON(jsonResponse);
        _uploadInformationStatusCallback(
            jsonResponse,
            _uploadInformationStatusContext);
    }

    uint16_t uploadOperationStatusCode;
    loadUploadStatusFile.getUploadOperationStatusCode(uploadOperationStatusCode);
    switch (uploadOperationStatusCode)
    {
    case STATUS_UPLOAD_ACCEPTED:
        uploadInitializationAccepted = true;
        break;
    case STATUS_UPLOAD_COMPLETED:
        uploadCompleted = true;
        endUpload = true;
        endUploadCV.notify_one();
        break;
    case STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE:
    case STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST:
    case STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST:
        endUpload = true;
        endUploadCV.notify_one();
        break;
    default:
        break;
    }

    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadDataLoaderARINC615A::TargetClient::TargetClient(SectionId clientId)
{
    this->clientId = clientId;
    clientFileBuffer = NULL;
    sectionFinished = false;
    fileName = "";
}

UploadDataLoaderARINC615A::TargetClient::~TargetClient()
{
    if (clientFileBuffer != NULL)
    {
        free(clientFileBuffer);
        clientFileBuffer = NULL;
    }
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::getClientId(
    SectionId &clientId)
{
    clientId = this->clientId;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::getClientFileBufferReference(
    FILE **fp)
{
    if (clientFileBuffer == NULL)
    {
        clientFileBuffer = (char *)malloc(MAX_FILE_BUFFER_SIZE);
    }

    *fp = fmemopen(clientFileBuffer, MAX_FILE_BUFFER_SIZE, "w");
    if (*fp == NULL)
    {
        return UploadOperationResult::UPLOAD_OPERATION_ERROR;
    }

    this->fileName = fileName;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::getClientBufferReference(
    char **buffer)
{
    *buffer = clientFileBuffer;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::setFileName(
    std::string fileName)
{
    this->fileName = fileName;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::getFileName(
    std::string &fileName)
{
    fileName = this->fileName;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::setSectionFinished()
{
    this->sectionFinished = true;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::isSectionFinished(
    bool &sectionFinished)
{
    sectionFinished = this->sectionFinished;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

UploadOperationResult UploadDataLoaderARINC615A::TargetClient::hasDataToProcess(
    bool &hasDataToProcess)
{
    hasDataToProcess = (clientFileBuffer != NULL);
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}