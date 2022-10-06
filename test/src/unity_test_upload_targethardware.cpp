#include <gtest/gtest.h>

#include "UploadTargetHardwareARINC615A.h"
#include "UploadDataLoaderARINC615A.h"

#include "InitializationFileARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"
#include "LoadUploadRequestFileARINC615A.h"

#define LOCALHOST "127.0.0.1"

#define BUSY_WAIT_DELAY 500 // ms
#define MAX_RETRIES 10

#define NUM_LOADS 3

#define TARGET_HARDWARE_ID "HNPFMS"
#define TARGET_HARDWARE_POSITION "L"
#define TARGET_HARDWARE_IP LOCALHOST

#define TFTP_TARGETHARDWARE_SERVER_PORT 28132
#define TFTP_DATALOADER_SERVER_PORT 45426

class ARINC615AUploadTargetHardwareTest : public ::testing::Test
{
protected:
    ARINC615AUploadTargetHardwareTest()
    {
        uploadTargetHardwareARINC615A =
            new UploadTargetHardwareARINC615A(LOCALHOST,
                                              TFTP_DATALOADER_SERVER_PORT);
        tftpDataLoaderServerThread = nullptr;
        tftpDataLoaderServer = new TFTPServer();

        baseFileName = std::string(TARGET_HARDWARE_ID) +
                       std::string("_") +
                       std::string(TARGET_HARDWARE_POSITION);

        initializationFileName = baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION;
        initializationFile = new InitializationFileARINC615A(
            initializationFileName);

        loadUploadRequestFileName = baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION;
        loadUploadRequestFile = new LoadUploadRequestFileARINC615A(
            loadUploadRequestFileName);

        loadUploadStatusFileName = baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        loadUploadStatusFile = new LoadUploadStatusFileARINC615A(
            loadUploadStatusFileName);
    }

    ~ARINC615AUploadTargetHardwareTest() override
    {
        delete initializationFile;
        delete loadUploadRequestFile;
        delete loadUploadStatusFile;
        delete uploadTargetHardwareARINC615A;
        delete tftpDataLoaderServer;

        if (tftpDataLoaderServerThread != nullptr)
        {
            delete tftpDataLoaderServerThread;
            tftpDataLoaderServerThread = nullptr;
        }
    }
    void SetUp() override
    {
        for (int i = 1; i <= NUM_LOADS; ++i)
        {
            LoadUploadRequestHeaderFileARINC615A headerFile;
            headerFile.setHeaderFileName("images/load" + std::to_string(i) + ".bin");
            headerFile.setLoadPartNumberName("0000000" + std::to_string(i));
            loadUploadRequestFile->addHeaderFile(headerFile);
        }

        ASSERT_EQ(tftpDataLoaderServer->setPort(
                      TFTP_DATALOADER_SERVER_PORT),
                  TftpServerOperationResult::TFTP_SERVER_OK);

        ASSERT_EQ(tftpDataLoaderServer->registerOpenFileCallback(
                      OpenFileCbk,
                      this),
                  TftpServerOperationResult::TFTP_SERVER_OK);

        ASSERT_EQ(tftpDataLoaderServer->registerSectionFinishedCallback(
                      SectionFinishedCbk,
                      this),
                  TftpServerOperationResult::TFTP_SERVER_OK);

        numAcceptedStatusReceived = 0;
        numInProgresStatusReceived = 0;
        completedRatioOk = false;
        numCompletedStatusReceived = 0;
        numWaitsReceived = 0;
        numAbortedByDataLoaderStatusReceived = 0;
        numAbortedByOperatorStatusReceived = 0;
        numAbortedByTargetHardwareStatusReceived = 0;

        toggleAbortMsg = false;
        abortDataLoaderTest = false;
        abortOperatorTest = false;
        waitTest = false;
    }

    void TearDown() override
    {
        if (tftpDataLoaderServerThread != nullptr)
        {
            tftpDataLoaderServer->stopListening();
            tftpDataLoaderServerThread->join();
        }
    }

    void abortDataLoaderTestFlag()
    {
        abortDataLoaderTest = true;
        toggleAbortMsg = true;
    }

    void abortOperatorTestFlag()
    {
        abortOperatorTest = true;
        toggleAbortMsg = true;
    }

    void setWaitTestFlag()
    {
        waitTest = true;
        waitCounter = 0;
    }

    static TftpServerOperationResult OpenFileCbk(
        ITFTPSection *sectionHandler,
        FILE **fp,
        char *filename,
        char *mode,
        size_t *bufferSize,
        void *context)
    {
        if (context == nullptr)
        {
            return TftpServerOperationResult::TFTP_SERVER_ERROR;
        }

        ARINC615AUploadTargetHardwareTest *ctx =
            (ARINC615AUploadTargetHardwareTest *)context;

        std::string cleanFileName(filename);
        cleanFileName = cleanFileName.substr(cleanFileName.find_last_of('/') + 1);

        if (cleanFileName == ctx->loadUploadStatusFileName)
        {
            if (ctx->abortDataLoaderTest &&
                ctx->numInProgresStatusReceived > 0 &&
                ctx->toggleAbortMsg)
            {
                ctx->toggleAbortMsg = !ctx->toggleAbortMsg;
                std::stringstream errorMessageStream;
                errorMessageStream << ARINC_ABORT_MSG_PREFIX;
                errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
                errorMessageStream << std::hex << UPLOAD_ABORT_SOURCE_DATALOADER;
                std::string errorMessage = errorMessageStream.str();
                sectionHandler->setErrorMessage(errorMessage);
                (*fp) = NULL;
                return TftpServerOperationResult::TFTP_SERVER_ERROR;
            }
            else if (ctx->abortOperatorTest &&
                     ctx->numInProgresStatusReceived > 0 &&
                     ctx->toggleAbortMsg)
            {
                ctx->toggleAbortMsg = !ctx->toggleAbortMsg;
                std::stringstream errorMessageStream;
                errorMessageStream << ARINC_ABORT_MSG_PREFIX;
                errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
                errorMessageStream << std::hex << UPLOAD_ABORT_SOURCE_OPERATOR;
                std::string errorMessage = errorMessageStream.str();
                sectionHandler->setErrorMessage(errorMessage);
                (*fp) = NULL;
                return TftpServerOperationResult::TFTP_SERVER_ERROR;
            }
            else
            {
                TftpSectionId id;
                sectionHandler->getSectionId(&id);
                {
                    std::lock_guard<std::mutex> lock(ctx->statusFileMutex);
                    ctx->statusFileBuffer[id] = std::make_shared<std::vector<uint8_t>>();
                    ctx->statusFileBuffer[id]->resize(MAX_FILE_BUFFER_SIZE);
                    *fp = fmemopen(ctx->statusFileBuffer[id]->data(), ctx->statusFileBuffer[id]->size(), mode);
                    if (bufferSize != NULL)
                    {
                        *bufferSize = MAX_FILE_BUFFER_SIZE;
                    }
                }
            }
        }
        else
        {
            if (ctx->waitTest &&
                ctx->numInProgresStatusReceived > 0)
            {
                if (ctx->waitCounter++ > 3)
                {
                    std::stringstream errorMessageStream;
                    errorMessageStream << ARINC_ABORT_MSG_PREFIX;
                    errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
                    errorMessageStream << std::hex << UPLOAD_ABORT_SOURCE_DATALOADER;
                    std::string errorMessage = errorMessageStream.str();
                    sectionHandler->setErrorMessage(errorMessage);
                }
                else
                {
                    std::stringstream errorMessageStream;
                    errorMessageStream << ARINC_WAIT_MSG_PREFIX;
                    errorMessageStream << ARINC_ERROR_MSG_DELIMITER;
                    errorMessageStream << std::hex << 3 * DEFAULT_ARINC615A_WAIT_TIME;
                    std::string errorMessage = errorMessageStream.str();
                    sectionHandler->setErrorMessage(errorMessage);
                }
                (*fp) = NULL;
                return TftpServerOperationResult::TFTP_SERVER_ERROR;
            }
            else
            {
                *fp = fopen(filename, mode);
            }
        }

        return TftpServerOperationResult::TFTP_SERVER_OK;
    }

    static TftpServerOperationResult SectionFinishedCbk(
        ITFTPSection *sectionHandler,
        void *context)
    {
        if (context == nullptr)
        {
            return TftpServerOperationResult::TFTP_SERVER_ERROR;
        }

        ARINC615AUploadTargetHardwareTest *ctx = (ARINC615AUploadTargetHardwareTest *)context;

        TftpSectionId id;
        sectionHandler->getSectionId(&id);
        {
            std::lock_guard<std::mutex> lock(ctx->statusFileMutex);
            if (ctx->statusFileBuffer.find(id) == ctx->statusFileBuffer.end())
            {
                return TftpServerOperationResult::TFTP_SERVER_OK;
            }

            LoadUploadStatusFileARINC615A *loadUploadStatusFile = new LoadUploadStatusFileARINC615A();
            if (loadUploadStatusFile->deserialize(ctx->statusFileBuffer[id]) ==
                SerializableOperationResult::SERIALIZABLE_OK)
            {
                uint16_t uploadOperationStatusCode;
                loadUploadStatusFile->getUploadOperationStatusCode(
                    uploadOperationStatusCode);

                switch (uploadOperationStatusCode)
                {
                case STATUS_UPLOAD_ACCEPTED:
                    ctx->numAcceptedStatusReceived++;
                    break;
                case STATUS_UPLOAD_IN_PROGRESS:
                    ctx->numInProgresStatusReceived++;
                    break;
                case STATUS_UPLOAD_COMPLETED:
                {
                    ctx->numCompletedStatusReceived++;
                    ctx->completedRatioOk = true;
                    std::shared_ptr<std::vector<LoadUploadStatusHeaderFileARINC615A>> headerFiles;
                    headerFiles = std::make_shared<std::vector<LoadUploadStatusHeaderFileARINC615A>>();
                    loadUploadStatusFile->getHeaderFiles(headerFiles);
                    for (auto headerFile : *headerFiles)
                    {
                        std::string headerFileName;
                        headerFile.getHeaderFileName(headerFileName);
                        uint32_t loadRatio;
                        headerFile.getLoadRatio(loadRatio);
                        ctx->completedRatioOk &= (loadRatio == 100);
                    }
                    uint32_t loadListRatio;
                    loadUploadStatusFile->getLoadListRatio(loadListRatio);
                    ctx->completedRatioOk &= (loadListRatio == 100);
                    break;
                }
                case STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION:
                {
                    std::string loadUploadDescription;
                    loadUploadStatusFile->getUploadStatusDescription(
                        loadUploadDescription);
                    if (loadUploadDescription.find("to be available...") != std::string::npos)
                    {
                        ctx->numWaitsReceived++;
                    }
                    break;
                }
                case STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE:
                    ctx->numAbortedByTargetHardwareStatusReceived++;
                    break;
                case STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST:
                    ctx->numAbortedByDataLoaderStatusReceived++;
                    break;
                case STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST:
                    ctx->numAbortedByOperatorStatusReceived++;
                    break;
                default:
                    break;
                }
            }
            delete loadUploadStatusFile;
            ctx->statusFileBuffer.erase(id);
        }
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }

    void startDataLoaderServer()
    {
        if (tftpDataLoaderServerThread == nullptr)
        {
            tftpDataLoaderServerThread = new std::thread([this]()
                                                         { tftpDataLoaderServer->startListening(); });
        }
    }

    UploadTargetHardwareARINC615A *uploadTargetHardwareARINC615A;
    std::thread *tftpDataLoaderServerThread;
    TFTPServer *tftpDataLoaderServer;
    std::string baseFileName;
    std::string initializationFileName;
    std::string loadUploadRequestFileName;
    std::string loadUploadStatusFileName;
    InitializationFileARINC615A *initializationFile;
    LoadUploadRequestFileARINC615A *loadUploadRequestFile;
    LoadUploadStatusFileARINC615A *loadUploadStatusFile;

    std::mutex statusFileMutex;
    std::unordered_map<TftpSectionId, std::shared_ptr<std::vector<uint8_t>>> statusFileBuffer;
    uint32_t numAcceptedStatusReceived;
    uint32_t numInProgresStatusReceived;
    bool completedRatioOk;
    uint32_t numCompletedStatusReceived;
    uint32_t numAbortedByTargetHardwareStatusReceived;
    uint32_t numAbortedByDataLoaderStatusReceived;
    uint32_t numAbortedByOperatorStatusReceived;
    uint32_t numWaitsReceived;

    bool toggleAbortMsg;
    bool abortDataLoaderTest;
    bool abortOperatorTest;
    bool waitTest;
    uint32_t waitCounter;
};

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareRegisterCheckFileCallback)
{
    ASSERT_EQ(uploadTargetHardwareARINC615A->registerCheckFilesCallback(
                  [](std::vector<std::string> files,
                     std::string &checkDescription,
                     void *context) -> UploadOperationResult
                  {
                      return UploadOperationResult::UPLOAD_OPERATION_OK;
                  },
                  nullptr),
              UploadOperationResult::UPLOAD_OPERATION_OK);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareRegisterTransmissionCheckCallback)
{
    ASSERT_EQ(uploadTargetHardwareARINC615A->registerTransmissionCheckCallback(
                  [](std::string &checkDescription,
                     void *context) -> UploadOperationResult
                  {
                      return UploadOperationResult::UPLOAD_OPERATION_OK;
                  },
                  nullptr),
              UploadOperationResult::UPLOAD_OPERATION_OK);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadInitialization)
{
    FILE *fp = NULL;
    size_t bufferSize = 0;
    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    ASSERT_NE(fp, nullptr);
    ASSERT_NE(bufferSize, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadInitializationAccepted)
{
    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);

    // Read to file pointer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(bufferSize);

    EXPECT_EQ(fread(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Check if initialization was accepted
    initializationFile->deserialize(fileBuffer);
    uint16_t operationAcceptanceStatusCode;
    EXPECT_EQ(initializationFile->getOperationAcceptanceStatusCode(
                  operationAcceptanceStatusCode),
              FileOperationResult::FILE_OPERATION_OK);

    ASSERT_EQ(operationAcceptanceStatusCode, INITIALIZATION_UPLOAD_IS_ACCEPTED);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadInitializationStatusAccepted)
{
    // Start data loader server for status thread
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);

    uint8_t maxRetries = MAX_RETRIES;
    while (numAcceptedStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numAcceptedStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareLoadUploadRequest)
{
    // Start data loader server for status thread
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    ASSERT_NE(fp, nullptr);
    ASSERT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareLoadUploadRequestReceived)
{
    // Start data loader server for status thread
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    ASSERT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    uint8_t maxRetries = MAX_RETRIES;
    while (numInProgresStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numInProgresStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadSuccess)
{
    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    ASSERT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    // Wait for upload to finish
    uint8_t maxTries = MAX_RETRIES;
    UploadTargetHardwareARINC615AState state;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        uploadTargetHardwareARINC615A->getState(state);
        maxTries--;
    } while (state != UploadTargetHardwareARINC615AState::COMPLETED && maxTries > 0);

    uint8_t maxRetries = MAX_RETRIES;
    while (numCompletedStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numCompletedStatusReceived, 0);
    ASSERT_TRUE(completedRatioOk);

    // Check if upload was completed
    if (state != UploadTargetHardwareARINC615AState::COMPLETED)
    {
        FAIL() << "UPLOAD DID NOT COMPLETE";
    }

    // Check files
    std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
    headerFiles = std::make_shared<std::vector<LoadUploadRequestHeaderFileARINC615A>>();
    loadUploadRequestFile->getHeaderFiles(headerFiles);

    for (auto headerFile : *headerFiles)
    {
        std::string dataLoaderFile;
        headerFile.getHeaderFileName(dataLoaderFile);
        FILE *fpDataLoader = fopen(dataLoaderFile.c_str(), "rb");

        std::string targetHardwareFile =
            dataLoaderFile.substr(dataLoaderFile.find_last_of("/\\") + 1);

        FILE *fpTargetHardware = fopen(targetHardwareFile.c_str(), "rb");

        ASSERT_NE(fpDataLoader, nullptr);
        ASSERT_NE(fpTargetHardware, nullptr);

        // Compare files
        fseek(fpDataLoader, 0L, SEEK_END);
        size_t dataLoaderFileSize = ftell(fpDataLoader);
        fseek(fpDataLoader, 0L, SEEK_SET);

        fseek(fpTargetHardware, 0L, SEEK_END);
        size_t targetHardwareFileSize = ftell(fpTargetHardware);
        fseek(fpTargetHardware, 0L, SEEK_SET);

        ASSERT_EQ(dataLoaderFileSize, targetHardwareFileSize);

        uint8_t *dataLoaderFileBuffer = new uint8_t[dataLoaderFileSize];
        uint8_t *targetHardwareFileBuffer = new uint8_t[targetHardwareFileSize];

        ASSERT_EQ(fread(dataLoaderFileBuffer, 1, dataLoaderFileSize, fpDataLoader),
                  dataLoaderFileSize);
        ASSERT_EQ(fread(targetHardwareFileBuffer, 1, targetHardwareFileSize, fpTargetHardware),
                  targetHardwareFileSize);

        ASSERT_EQ(memcmp(dataLoaderFileBuffer, targetHardwareFileBuffer, dataLoaderFileSize), 0);

        delete[] dataLoaderFileBuffer;
        delete[] targetHardwareFileBuffer;

        fclose(fpDataLoader);
        fclose(fpTargetHardware);
    }
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadAbortedTargetHardware)
{
    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    EXPECT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    // Abort upload
    ASSERT_EQ(uploadTargetHardwareARINC615A->abort(
                  STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE),
              UploadOperationResult::UPLOAD_OPERATION_OK);
    uint8_t maxTries = MAX_RETRIES;
    UploadTargetHardwareARINC615AState state;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        uploadTargetHardwareARINC615A->getState(state);
        maxTries--;
    } while (state != UploadTargetHardwareARINC615AState::ABORTED_BY_TARGET && maxTries > 0);

    uint8_t maxRetries = MAX_RETRIES;
    while (numAbortedByTargetHardwareStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numAbortedByTargetHardwareStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadAbortedDataLoader)
{
    abortDataLoaderTestFlag();

    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    EXPECT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    uint8_t maxRetries = MAX_RETRIES;
    while (numAbortedByDataLoaderStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numAbortedByDataLoaderStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareUploadAbortedOperator)
{
    abortOperatorTestFlag();

    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    EXPECT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    uint8_t maxRetries = MAX_RETRIES;
    while (numAbortedByOperatorStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numAbortedByOperatorStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareAutoAbortOnFileUploadFail)
{
    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Add inexistent file so the upload fails
    for (int i = NUM_LOADS; i <= 2 * NUM_LOADS; ++i)
    {
        LoadUploadRequestHeaderFileARINC615A headerFile;
        headerFile.setHeaderFileName("images/load" + std::to_string(i) + ".bin");
        headerFile.setLoadPartNumberName("0000000" + std::to_string(i));
        loadUploadRequestFile->addHeaderFile(headerFile);
    }

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    EXPECT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    uint8_t maxTries = MAX_RETRIES;
    UploadTargetHardwareARINC615AState state;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        uploadTargetHardwareARINC615A->getState(state);
        maxTries--;
    } while (state != UploadTargetHardwareARINC615AState::ABORTED_BY_TARGET && maxTries > 0);

    uint8_t maxRetries = MAX_RETRIES;
    while (numAbortedByTargetHardwareStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numAbortedByTargetHardwareStatusReceived, 0);
    ASSERT_EQ(numCompletedStatusReceived, 0);
}

TEST_F(ARINC615AUploadTargetHardwareTest, UploadTargetHardwareFileNotFoundWait)
{
    setWaitTestFlag();

    // Start data loader server for status thread and upload
    startDataLoaderServer();

    FILE *fp = NULL;
    size_t bufferSize = 0;

    // Request initialization file from target hardware
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadInitialization(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_NE(bufferSize, 0);
    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }

    // Request file to receive load list
    EXPECT_EQ(uploadTargetHardwareARINC615A->loadUploadRequest(
                  &fp, &bufferSize, initializationFileName),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    EXPECT_NE(fp, nullptr);
    EXPECT_EQ(bufferSize, MAX_FILE_BUFFER_SIZE);

    // Serialize load list file to buffer
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    fileBuffer = std::make_shared<std::vector<uint8_t>>();
    fileBuffer->resize(0);
    loadUploadRequestFile->serialize(fileBuffer);

    // Write to file pointer
    EXPECT_EQ(fwrite(fileBuffer->data(), 1, bufferSize, fp), bufferSize);
    fclose(fp);

    // Notify target hardware that load list was received
    EXPECT_EQ(uploadTargetHardwareARINC615A->notify(
                  NotifierEventType::NOTIFIER_EVENT_TFTP_SECTION_CLOSED),
              NotifierOperationResult::NOTIFIER_OK);

    uint8_t maxRetries = 3*MAX_RETRIES;
    while (numAbortedByDataLoaderStatusReceived == 0 && maxRetries > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_WAIT_DELAY));
        maxRetries--;
    }
    ASSERT_NE(numWaitsReceived, 0);
    ASSERT_NE(numAbortedByDataLoaderStatusReceived, 0);
    ASSERT_EQ(numCompletedStatusReceived, 0);
}