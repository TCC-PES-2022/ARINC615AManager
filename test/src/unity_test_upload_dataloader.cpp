#include <gtest/gtest.h>

#include "UploadDataLoaderARINC615A.h"
#include "InitializationFileARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"
#include "TFTPServer.h"
#include "TFTPClient.h"
#include "ISerializable.h"

#include <thread>
#include <list>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>

#define LOCALHOST "127.0.0.1"

#define DELTA_TIME 2 // seconds

#define TARGET_HARDWARE_ID "HNPFMS"
#define TARGET_HARDWARE_POSITION "L"
#define TARGET_HARDWARE_IP LOCALHOST

#define TFTP_TARGETHARDWARE_SERVER_PORT 28132
#define TFTP_DATALOADER_SERVER_PORT 45426

class ARINC615AUploadDataLoaderTest : public ::testing::Test
{
protected:
    ARINC615AUploadDataLoaderTest()
    {
        uploadDataLoaderARINC615A =
            new UploadDataLoaderARINC615A(TARGET_HARDWARE_ID,
                                          TARGET_HARDWARE_POSITION,
                                          TARGET_HARDWARE_IP);
        tftpTargetHardwareServer = new TFTPServer();
        tftpTargetHardwareStatusClient = new TFTPClient();
    }

    ~ARINC615AUploadDataLoaderTest() override
    {
        delete uploadDataLoaderARINC615A;
        delete tftpTargetHardwareServer;
        delete tftpTargetHardwareStatusClient;
    }
    void SetUp() override
    {

        loadList.push_back(std::make_tuple("images/load1.bin", "00000001"));
        loadList.push_back(std::make_tuple("images/load2.bin", "00000002"));
        loadList.push_back(std::make_tuple("images/load3.bin", "00000003"));

        ASSERT_EQ(uploadDataLoaderARINC615A->setLoadList(loadList),
                  UploadOperationResult::UPLOAD_OPERATION_OK);

        ASSERT_EQ(uploadDataLoaderARINC615A->setTftpTargetHardwareServerPort(
                      TFTP_TARGETHARDWARE_SERVER_PORT),
                  UploadOperationResult::UPLOAD_OPERATION_OK);

        ASSERT_EQ(uploadDataLoaderARINC615A->setTftpDataLoaderServerPort(
                      TFTP_DATALOADER_SERVER_PORT),
                  UploadOperationResult::UPLOAD_OPERATION_OK);

        ASSERT_EQ(tftpTargetHardwareServer->setPort(
                      TFTP_TARGETHARDWARE_SERVER_PORT),
                  TftpServerOperationResult::TFTP_SERVER_OK);

        ASSERT_EQ(tftpTargetHardwareServer->setTimeout(
                      TFTP_TARGETHARDWARE_SERVER_PORT),
                  TftpServerOperationResult::TFTP_SERVER_OK);

        ASSERT_EQ(tftpTargetHardwareStatusClient->setConnection(LOCALHOST,
                                                                TFTP_DATALOADER_SERVER_PORT),
                  TftpClientOperationResult::TFTP_CLIENT_OK);

        baseFileName = std::string(TARGET_HARDWARE_ID) +
                       std::string("_") +
                       std::string(TARGET_HARDWARE_POSITION);
    }

    void TearDown() override
    {
    }

    UploadDataLoaderARINC615A *uploadDataLoaderARINC615A;
    TFTPServer *tftpTargetHardwareServer;
    TFTPClient *tftpTargetHardwareStatusClient;
    std::string baseFileName;
    std::vector<ArincLoad> loadList;
};

class TargetServerClienContext
{
public:
    TargetServerClienContext()
    {
        fileBuffer = std::make_shared<std::vector<uint8_t>>();
        loadListReceiveStarted = false;
        waitsReceived = 0;
    }
    ~TargetServerClienContext()
    {
        fileBuffer.reset();
    }
    std::shared_ptr<std::vector<uint8_t>> fileBuffer;
    InitializationFileARINC615A arincFileLUI;
    LoadUploadRequestFileARINC615A arincFileLUR;
    bool loadListReceiveStarted;
    TFTPServer *tftpServer;
    uint16_t uploadOperationStatusCode;
    uint8_t waitsReceived;
};

TftpServerOperationResult targetHardwareOpenFileCallback(
    ITFTPSection *sectionHandler,
    FILE **fp,
    char *filename,
    char *mode,
    size_t *bufferSize,
    void *context)
{
    if (context != nullptr)
    {
        TargetServerClienContext *targetServerClienContext =
            static_cast<TargetServerClienContext *>(context);

        targetServerClienContext->fileBuffer->clear();
        ISerializable *arincFile;
        if (strcmp(mode, "r") == 0)
        {
            // SEND LUI FILE
            arincFile = dynamic_cast<ISerializable *>(&targetServerClienContext->arincFileLUI);
            targetServerClienContext->fileBuffer->resize(0);
            arincFile->serialize(targetServerClienContext->fileBuffer);

            //            for (auto &it : *targetServerClienContext->fileBuffer) {
            //                printf("%02X (%c) ", it, it);
            //            }
            //            printf("\n");
        }
        else if (strcmp(mode, "w") == 0)
        {
            // RECEIVE LUR FILE
            arincFile = dynamic_cast<ISerializable *>(&targetServerClienContext->arincFileLUR);
            targetServerClienContext->loadListReceiveStarted = true;
            targetServerClienContext->fileBuffer->resize(MAX_FILE_BUFFER_SIZE);
        }
        else
        {
            printf("ERROR: Unknown mode %s\n", mode);
            return TftpServerOperationResult::TFTP_SERVER_ERROR;
        }

        (*fp) = fmemopen(targetServerClienContext->fileBuffer->data(),
                         targetServerClienContext->fileBuffer->size(),
                         mode);
        if (bufferSize != NULL)
        {
            *bufferSize = targetServerClienContext->fileBuffer->size();
        }
        if ((*fp) == NULL)
        {
            return TftpServerOperationResult::TFTP_SERVER_ERROR;
        }

        return TftpServerOperationResult::TFTP_SERVER_OK;
    }

    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderRegisterUploadInitializationResponseCallback)
{
    ASSERT_EQ(uploadDataLoaderARINC615A->registerUploadInitializationResponseCallback(nullptr, nullptr),
              UploadOperationResult::UPLOAD_OPERATION_OK);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderRegisterUploadInformationStatusCallback)
{
    ASSERT_EQ(uploadDataLoaderARINC615A->registerUploadInformationStatusCallback(nullptr, nullptr),
              UploadOperationResult::UPLOAD_OPERATION_OK);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderRegisterFileNotAvailableCallback)
{
    ASSERT_EQ(uploadDataLoaderARINC615A->registerFileNotAvailableCallback(nullptr, nullptr),
              UploadOperationResult::UPLOAD_OPERATION_OK);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderLoadEmptyLoadList)
{
    std::vector<ArincLoad> loadList;
    loadList.clear();

    ASSERT_EQ(uploadDataLoaderARINC615A->setLoadList(loadList),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderOverloadLoadList)
{
    std::vector<ArincLoad> loadList;
    loadList.clear();

    for (int i = 0; i < MAX_NUM_FILES + 1; i++)
    {
        loadList.push_back(std::make_tuple(std::string("images/file") + std::to_string(i),
                                           std::to_string(i)));
    }

    ASSERT_EQ(uploadDataLoaderARINC615A->setLoadList(loadList),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderTHUploadingInitializationTimeout)
{
    int upload_operation_time = 0;
    time_t start = time(NULL);
    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
    time_t end = time(NULL);
    upload_operation_time = difftime(end, start);

    /*
     * TFTP_TIMEOUT = 2s
     * TFTP_TRIES = 2
     * DLP_TRIES = 2
     * EXPECTED_UPLOAD_OPERATION_TIME = 2*(2*2s) = 8s
     */
    int expected_upload_operation_time = 8;

    // Times may vary depending on the machine, that's why we're using EXPECT
    // instead of ASSERT
    // EXPECT_LE(upload_operation_time, expected_upload_operation_time + DELTA_TIME);
    // EXPECT_GE(upload_operation_time, expected_upload_operation_time - DELTA_TIME);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderTHUploadingInitializationRefused)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to refuse connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_DENIED);

    targetServerClienContext.arincFileLUI.setStatusDescription(
        "Operation refused by the target hardware");

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    tftpTargetHardwareServer->stopListening();
    serverThread.join();
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderLoadUploadStatusTimeout)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    fprintf(stdout,
            "This test will take about 13 seconds to complete, please wait...\n");

    int upload_operation_time = 0;
    time_t start = time(NULL);
    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
    time_t end = time(NULL);
    upload_operation_time = difftime(end, start);

    // Times may vary depending on the machine, that's why we're using EXPECT
    // instead of ASSERT
    // EXPECT_LE(upload_operation_time, DEFAULT_ARINC615A_DLP_TIMEOUT + DELTA_TIME);
    // EXPECT_GE(upload_operation_time, DEFAULT_ARINC615A_DLP_TIMEOUT - DELTA_TIME);

    tftpTargetHardwareServer->stopListening();
    serverThread.join();
}

TftpServerOperationResult
UploadDataLoaderLoadUploadRequestTimeout_TargetHardwareSectionFinished(
    ITFTPSection *sectionHandler, void *context)
{
    if (context != nullptr)
    {
        TargetServerClienContext *targetServerClienContext =
            static_cast<TargetServerClienContext *>(context);
        targetServerClienContext->tftpServer->stopListening();
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderLoadUploadRequestTimeout)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Save TargetHardware server instance to stop listening as soon as
    // the first section is finished
    targetServerClienContext.tftpServer = tftpTargetHardwareServer;

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderLoadUploadRequestTimeout_TargetHardwareSectionFinished,
        &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    // Start TargetHardware client to send status message (heartbeat)
    bool sendHeartBeat = true;
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        // Prepare LUS file with upload accepted status
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                STATUS_UPLOAD_ACCEPTED);

        // Serialize message
        std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                std::make_shared<std::vector<uint8_t>>();
        loadUploadStatusFileARINC615A.serialize(fileBuffer);

        // Send heartbeat
        while (sendHeartBeat) {
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                tftpTargetHardwareStatusClient->sendFile(
                        targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }
            sleep(1);
        } });

    int upload_operation_time = 0;
    time_t start = time(NULL);
    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
    time_t end = time(NULL);
    upload_operation_time = difftime(end, start);

    sendHeartBeat = false;
    targetHardwareStatusThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();

    /*
     * TFTP_TIMEOUT = 2s
     * TFTP_TRIES = 2
     * DLP_TRIES = 2
     * EXPECTED_UPLOAD_OPERATION_TIME = 2*(2*2s) = 8s
     */
    int expected_upload_operation_time = 8;

    // Times may vary depending on the machine, that's why we're using EXPECT
    // instead of ASSERT
    // EXPECT_LE(upload_operation_time, expected_upload_operation_time + DELTA_TIME);
    // EXPECT_GE(upload_operation_time, expected_upload_operation_time - DELTA_TIME);
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderLoadUploadStatusTimeoutAfterLUR)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    // Start TargetHardware client to send status message (heartbeat)
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        // Prepare LUS file with upload accepted status
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                STATUS_UPLOAD_ACCEPTED);

        // Serialize message
        std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                std::make_shared<std::vector<uint8_t>>();
        loadUploadStatusFileARINC615A.serialize(fileBuffer);

        // Send 1 heartbeat and stop client as soon as the first message is sent
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_ERROR;
        while (result != TftpClientOperationResult::TFTP_CLIENT_OK) {
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                result = tftpTargetHardwareStatusClient->sendFile(
                        targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }
            sleep(1);
        } });

    fprintf(stdout,
            "This test will take about 13 seconds to complete, please wait...\n");

    int upload_operation_time = 0;
    time_t start = time(NULL);
    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);
    time_t end = time(NULL);
    upload_operation_time = difftime(end, start);

    targetHardwareStatusThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();

    // Times may vary depending on the machine, that's why we're using EXPECT
    // instead of ASSERT
    // EXPECT_LE(upload_operation_time, DEFAULT_ARINC615A_DLP_TIMEOUT + DELTA_TIME);
    // EXPECT_GE(upload_operation_time, DEFAULT_ARINC615A_DLP_TIMEOUT - DELTA_TIME);
}

TftpServerOperationResult
UploadDataLoaderUploadSuccess_TargetHardwareSectionFinished(
    ITFTPSection *sectionHandler, void *context)
{
    if (context != nullptr)
    {
        TargetServerClienContext *targetServerClienContext =
            static_cast<TargetServerClienContext *>(context);
        if (targetServerClienContext->loadListReceiveStarted)
        {
            targetServerClienContext->uploadOperationStatusCode =
                STATUS_UPLOAD_IN_PROGRESS;
        }
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

// Thanks to https://stackoverflow.com/a/15119347/4625435
template <typename InputIterator1, typename InputIterator2>
bool range_equal(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 != *first2)
            return false;
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 == last2);
}

bool compare_files(const std::string &filename1, const std::string &filename2)
{
    std::ifstream file1(filename1);
    std::ifstream file2(filename2);

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);

    std::istreambuf_iterator<char> end;

    return range_equal(begin1, end, begin2, end);
}
/////////////////////////////////////////////////////////

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderUploadSuccess)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Init LUR file
    targetServerClienContext.arincFileLUR = LoadUploadRequestFileARINC615A(
        baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderUploadSuccess_TargetHardwareSectionFinished,
        &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    // Start TargetHardware client to send status message (heartbeat)
    targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    bool sendHeartBeat = true;
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        // Prepare LUS file
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        while (sendHeartBeat) {
            // Set status code
            loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                    targetServerClienContext.uploadOperationStatusCode);

            // Serialize message
            std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                    std::make_shared<std::vector<uint8_t>>();
            loadUploadStatusFileARINC615A.serialize(fileBuffer);

            // Send heartbeat
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                tftpTargetHardwareStatusClient->sendFile(
                        targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }
            sleep(1);
        } });

    // TargetHardware client to fetch file from DataLoader
    std::thread targetHardwareUploadThread = std::thread([&]
                                                         {

        // Setup connection
        TFTPClient tftpTargetHardwareUploadClient;
        tftpTargetHardwareUploadClient.setConnection(LOCALHOST,
                                                TFTP_DATALOADER_SERVER_PORT);

        // Wait for connection to be accepted and LUR file is received
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }

        // Deserialize LUR file
        LoadUploadRequestFileARINC615A loadUploadRequestFileArinc615A;
        loadUploadRequestFileArinc615A.deserialize(
                targetServerClienContext.fileBuffer);

        std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
        loadUploadRequestFileArinc615A.getHeaderFiles(headerFiles);

        // Fetch files
        std::vector<std::string> uploadFiles;
        for (std::vector<LoadUploadRequestHeaderFileARINC615A>::iterator
                it = headerFiles->begin(); it != headerFiles->end(); ++it)
        {
            std::string fileName;
            (*it).getHeaderFileName(fileName);

            FILE *fp = fopen((fileName+"_tw").c_str(), "w");
            if (fp != NULL) {
                ASSERT_EQ(tftpTargetHardwareUploadClient.fetchFile(fileName.c_str(), fp),
                          TftpClientOperationResult::TFTP_CLIENT_OK);
                fclose(fp);
            } else {
                FAIL() << "FAIL TO CREATE HEADER FILE";
            }
        }

        // Complete operation without errors
        targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_COMPLETED; });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_OK);

    sendHeartBeat = false;
    targetHardwareStatusThread.join();

    targetHardwareUploadThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();

    // Check if fetched files are the same as the ones sent by DataLoader
    for (std::vector<ArincLoad>::iterator it = loadList.begin();
         it != loadList.end(); ++it)
    {
        std::string fileName = std::get<LOAD_FILE_NAME_IDX>(*it);
        ASSERT_TRUE(compare_files(fileName, fileName + "_tw"));
    }
}

TftpServerOperationResult
UploadDataLoaderTargetHardwareAbort_TargetHardwareSectionFinished(
    ITFTPSection *sectionHandler, void *context)
{
    if (context != nullptr)
    {
        TargetServerClienContext *targetServerClienContext =
            static_cast<TargetServerClienContext *>(context);

        // Abort operation as soon as the LUR file is received.
        if (targetServerClienContext->loadListReceiveStarted)
        {
            targetServerClienContext->uploadOperationStatusCode =
                STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE;
        }
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderTargetHardwareAbort)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Init LUR file
    targetServerClienContext.arincFileLUR = LoadUploadRequestFileARINC615A(
        baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderTargetHardwareAbort_TargetHardwareSectionFinished,
        &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    // Start TargetHardware client to send status message (heartbeat)
    targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        // Prepare LUS file
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        // Set status description. This will be relevant only when the
        // operation is aborted.
        loadUploadStatusFileARINC615A.setUploadStatusDescription(
                "Upload aborted by the TargetHardware");

        // Send heartbeat until the operation is aborted
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_OK;
        bool abortSent = false;
        while (!abortSent) {
            // Set status code
            loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                    targetServerClienContext.uploadOperationStatusCode);

            // Get status code to confirm if we'll send abort
            uint16_t statusCode;
            loadUploadStatusFileARINC615A.getUploadOperationStatusCode(statusCode);

            // Serialize message
            std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                    std::make_shared<std::vector<uint8_t>>();
            loadUploadStatusFileARINC615A.serialize(fileBuffer);

            // Send heartbeat
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                result = tftpTargetHardwareStatusClient->sendFile(
                            targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }
            sleep(1);

            if ((statusCode == STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE)
                && (result == TftpClientOperationResult::TFTP_CLIENT_OK)) {
                abortSent = true;
            }
        } });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    targetHardwareStatusThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();
}

TftpClientOperationResult TftpErrorCbk(
    short error_code,
    std::string &error_message,
    void *context)
{
    if (context != nullptr)
    {
        TargetServerClienContext *ctx =
            (TargetServerClienContext *)context;

        if (error_code == 0)
        {
            std::string abortPrefix = std::string(ARINC_ABORT_MSG_PREFIX) +
                                      std::string(ARINC_ERROR_MSG_DELIMITER);
            size_t pos = error_message.find(abortPrefix);
            if (pos != std::string::npos)
            {
                pos = error_message.find(ARINC_ERROR_MSG_DELIMITER);
                std::string abortCode = error_message.substr(
                    pos + 1, error_message.length());

                ctx->uploadOperationStatusCode = std::stoul(abortCode,
                                                            nullptr, 16);

                // std::stringstream ss;
                // ss << std::hex << abortCode;
                // ss >> ctx->uploadOperationStatusCode;
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

                ctx->waitsReceived += std::stoi(waitSeconds);

                return TftpClientOperationResult::TFTP_CLIENT_OK;
            }
        }
    }
    return TftpClientOperationResult::TFTP_CLIENT_ERROR;
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderDataLoaderAbort)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Init LUR file
    targetServerClienContext.arincFileLUR = LoadUploadRequestFileARINC615A(
        baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderUploadSuccess_TargetHardwareSectionFinished,
        &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    bool uploadAborted = false;

    // TargetHardware client to fetch file from DataLoader
    targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    std::thread targetHardwareUploadThread = std::thread([&]
                                                         {

        // Setup connection
        TFTPClient tftpTargetHardwareUploadClient;
        tftpTargetHardwareUploadClient.setConnection(LOCALHOST,
                                                TFTP_DATALOADER_SERVER_PORT);
        // tftpTargetHardwareUploadClient.registerTftpErrorCallback(
        //                                     TftpErrorCbk,
        //                                     &targetServerClienContext);

        // Wait for connection to be accepted and LUR file is received
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }

        // Deserialize LUR file
        LoadUploadRequestFileARINC615A loadUploadRequestFileArinc615A;
        loadUploadRequestFileArinc615A.deserialize(
                targetServerClienContext.fileBuffer);

        std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
        loadUploadRequestFileArinc615A.getHeaderFiles(headerFiles);

        // Fetch files
        std::vector<std::string> uploadFiles;
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_OK;
        for (std::vector<LoadUploadRequestHeaderFileARINC615A>::iterator
                it = headerFiles->begin(); it != headerFiles->end(); ++it)
        {
            // Make fetch process slow so we can abort the process
            sleep(5);

            std::string fileName;
            (*it).getHeaderFileName(fileName);

            if (uploadAborted) 
            {
                break;
            }

            FILE *fp = fopen((fileName+"_tw").c_str(), "w");
            if (fp != NULL) {
                result = tftpTargetHardwareUploadClient.fetchFile(fileName.c_str(), fp);
                fclose(fp);
            } else {
                FAIL() << "FAIL TO CREATE HEADER FILE";
            }
        }

        if (!uploadAborted && 
            result == TftpClientOperationResult::TFTP_CLIENT_OK) 
        {
            targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_COMPLETED;
        } });

    // Start TargetHardware client to send status message (heartbeat)
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        tftpTargetHardwareStatusClient->registerTftpErrorCallback(
                                            TftpErrorCbk,
                                            &targetServerClienContext);

        // Prepare LUS file
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        // Send heartbeat until the operation is aborted
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_OK;
        bool abortSent = false;
        while (!abortSent) {
            // Set status code
            loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                    targetServerClienContext.uploadOperationStatusCode);

            // Get status code to confirm if we'll send abort
            uint16_t statusCode;
            loadUploadStatusFileARINC615A.getUploadOperationStatusCode(
                                                    statusCode);

            if (statusCode == STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST) 
            {
                uploadAborted = true;
                if (targetHardwareUploadThread.joinable()) {
                    targetHardwareUploadThread.join();
                }                
            }

            // Serialize message
            std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                    std::make_shared<std::vector<uint8_t>>();
            loadUploadStatusFileARINC615A.serialize(fileBuffer);

            // Send heartbeat
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                result = tftpTargetHardwareStatusClient->sendFile(
                            targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }

            sleep(1);

            if ((statusCode == STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST)
                && (result == TftpClientOperationResult::TFTP_CLIENT_OK)) {
                abortSent = true;
            }
        } });

    // Upload is a blocking call, so we need to abord from another thread
    std::thread uploadAbortThread = std::thread([&]
                                                {
        // Wait for upload to start
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }
        ASSERT_EQ(uploadDataLoaderARINC615A->abort(
                    UPLOAD_ABORT_SOURCE_DATALOADER), 
                  UploadOperationResult::UPLOAD_OPERATION_OK); });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    ASSERT_EQ(targetServerClienContext.uploadOperationStatusCode,
              STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST);

    uploadAbortThread.join();
    targetHardwareStatusThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderOperatorAbort)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Init LUR file
    targetServerClienContext.arincFileLUR = LoadUploadRequestFileARINC615A(
        baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderUploadSuccess_TargetHardwareSectionFinished,
        &targetServerClienContext);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    bool uploadAborted = false;

    // TargetHardware client to fetch file from DataLoader
    targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    std::thread targetHardwareUploadThread = std::thread([&]
                                                         {

        // Setup connection
        TFTPClient tftpTargetHardwareUploadClient;
        tftpTargetHardwareUploadClient.setConnection(LOCALHOST,
                                                TFTP_DATALOADER_SERVER_PORT);
        // tftpTargetHardwareUploadClient.registerTftpErrorCallback(
        //                                     TftpErrorCbk,
        //                                     &targetServerClienContext);

        // Wait for connection to be accepted and LUR file is received
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }

        // Deserialize LUR file
        LoadUploadRequestFileARINC615A loadUploadRequestFileArinc615A;
        loadUploadRequestFileArinc615A.deserialize(
                targetServerClienContext.fileBuffer);

        std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
        loadUploadRequestFileArinc615A.getHeaderFiles(headerFiles);

        // Fetch files
        std::vector<std::string> uploadFiles;
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_OK;
        for (std::vector<LoadUploadRequestHeaderFileARINC615A>::iterator
                it = headerFiles->begin(); it != headerFiles->end(); ++it)
        {
            // Make fetch process slow so we can abort the process
            sleep(5);

            std::string fileName;
            (*it).getHeaderFileName(fileName);

            if (uploadAborted) 
            {
                break;
            }

            FILE *fp = fopen((fileName+"_tw").c_str(), "w");
            if (fp != NULL) {
                result = tftpTargetHardwareUploadClient.fetchFile(fileName.c_str(), fp);
                fclose(fp);
            } else {
                FAIL() << "FAIL TO CREATE HEADER FILE";
            }
        }

        if (!uploadAborted && 
            result == TftpClientOperationResult::TFTP_CLIENT_OK) 
        {
            targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_COMPLETED;
        } });

    // Start TargetHardware client to send status message (heartbeat)
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        tftpTargetHardwareStatusClient->registerTftpErrorCallback(
                                            TftpErrorCbk,
                                            &targetServerClienContext);
        // Prepare LUS file
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        // Send heartbeat until the operation is aborted
        TftpClientOperationResult result = TftpClientOperationResult::TFTP_CLIENT_OK;
        bool abortSent = false;
        while (!abortSent) {
            // Set status code
            loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                    targetServerClienContext.uploadOperationStatusCode);

            // Get status code to confirm if we'll send abort
            uint16_t statusCode;
            loadUploadStatusFileARINC615A.getUploadOperationStatusCode(
                                                    statusCode);

            if (statusCode == STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST) 
            {
                uploadAborted = true;
                if (targetHardwareUploadThread.joinable()) {
                    targetHardwareUploadThread.join();
                }                
            }

            // Serialize message
            std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                    std::make_shared<std::vector<uint8_t>>();
            loadUploadStatusFileARINC615A.serialize(fileBuffer);

            // Send heartbeat
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                result = tftpTargetHardwareStatusClient->sendFile(
                            targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }

            sleep(1);

            if ((statusCode == STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST)
                && (result == TftpClientOperationResult::TFTP_CLIENT_OK)) {
                abortSent = true;
            }
        } });

    // Upload is a blocking call, so we need to abord from another thread
    std::thread uploadAbortThread = std::thread([&]
                                                {
        // Wait for upload to start
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }
        ASSERT_EQ(uploadDataLoaderARINC615A->abort(
                    UPLOAD_ABORT_SOURCE_OPERATOR), 
                  UploadOperationResult::UPLOAD_OPERATION_OK); });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    ASSERT_EQ(targetServerClienContext.uploadOperationStatusCode,
              STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST);

    uploadAbortThread.join();
    targetHardwareStatusThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();
}

UploadOperationResult UploadDataLoaderFileNotFound_fileNotAvailableCbk(
    uint16_t *waitTimeS,
    void *context)
{
    *waitTimeS = DEFAULT_WAIT_TIME;
    return UploadOperationResult::UPLOAD_OPERATION_OK;
}

TEST_F(ARINC615AUploadDataLoaderTest, UploadDataLoaderFileNotFound)
{
    TargetServerClienContext targetServerClienContext;

    // Prepare LUI file to accept connection
    targetServerClienContext.arincFileLUI = InitializationFileARINC615A(
        baseFileName + UPLOAD_INITIALIZATION_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    targetServerClienContext.arincFileLUI.setOperationAcceptanceStatusCode(
        INITIALIZATION_UPLOAD_IS_ACCEPTED);

    // Init LUR file
    targetServerClienContext.arincFileLUR = LoadUploadRequestFileARINC615A(
        baseFileName + UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION,
        SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

    // Override load list
    std::vector<ArincLoad> inexistentLoadList;
    inexistentLoadList.push_back(
        std::make_tuple("images/inexistent_load1.bin", "00000001"));
    inexistentLoadList.push_back(
        std::make_tuple("images/inexistent_load2.bin", "00000002"));
    inexistentLoadList.push_back(
        std::make_tuple("images/inexistent_load3.bin", "00000003"));
    uploadDataLoaderARINC615A->setLoadList(inexistentLoadList);

    // Register server callbacks
    tftpTargetHardwareServer->registerOpenFileCallback(
        targetHardwareOpenFileCallback, &targetServerClienContext);

    tftpTargetHardwareServer->registerSectionFinishedCallback(
        UploadDataLoaderUploadSuccess_TargetHardwareSectionFinished,
        &targetServerClienContext);

    uploadDataLoaderARINC615A->registerFileNotAvailableCallback(
        UploadDataLoaderFileNotFound_fileNotAvailableCbk,
        nullptr);

    // Start TargetHardware server
    std::thread serverThread = std::thread([this]
                                           { tftpTargetHardwareServer->startListening(); });

    // Start TargetHardware client to send status message (heartbeat)
    targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ACCEPTED;
    bool sendHeartBeat = true;
    std::thread targetHardwareStatusThread = std::thread([&]
                                                         {

        // Prepare LUS file
        std::string targetHardwareStatusFileName =
                baseFileName + UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION;
        LoadUploadStatusFileARINC615A loadUploadStatusFileARINC615A(
                targetHardwareStatusFileName,
                SUPPLEMENT3_ARINC_REPORT_615A_VERSION);

        while (sendHeartBeat) {
            // Set status code
            loadUploadStatusFileARINC615A.setUploadOperationStatusCode(
                    targetServerClienContext.uploadOperationStatusCode);

            // Serialize message
            std::shared_ptr<std::vector<uint8_t>> fileBuffer =
                    std::make_shared<std::vector<uint8_t>>();
            loadUploadStatusFileARINC615A.serialize(fileBuffer);

            // Send heartbeat
            FILE *fp = fmemopen(fileBuffer->data(), fileBuffer->size(), "r");
            if (fp != NULL) {
                tftpTargetHardwareStatusClient->sendFile(
                        targetHardwareStatusFileName.c_str(), fp);
                fclose(fp);
            }
            sleep(1);
        } });

    // TargetHardware client to fetch file from DataLoader
    std::thread targetHardwareUploadThread = std::thread([&]
                                                         {

        // Setup connection
        TFTPClient tftpTargetHardwareUploadClient;
        tftpTargetHardwareUploadClient.setConnection(LOCALHOST,
                                                TFTP_DATALOADER_SERVER_PORT);
        tftpTargetHardwareUploadClient.registerTftpErrorCallback(
                                            TftpErrorCbk,
                                            &targetServerClienContext);

        // Wait for connection to be accepted and LUR file is received
        while (targetServerClienContext.uploadOperationStatusCode !=
                    STATUS_UPLOAD_IN_PROGRESS)
        {
            std::this_thread::yield();
        }

        // Deserialize LUR file
        LoadUploadRequestFileARINC615A loadUploadRequestFileArinc615A;
        loadUploadRequestFileArinc615A.deserialize(
                targetServerClienContext.fileBuffer);

        std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
        loadUploadRequestFileArinc615A.getHeaderFiles(headerFiles);

        // Fetch files
        targetServerClienContext.waitsReceived = 0;
        std::vector<std::string> uploadFiles;
        for (std::vector<LoadUploadRequestHeaderFileARINC615A>::iterator
                it = headerFiles->begin(); it != headerFiles->end(); ++it)
        {
            std::string fileName;
            (*it).getHeaderFileName(fileName);

            FILE *fp = fopen((fileName+"_tw").c_str(), "w");
            if (fp != NULL) {
                ASSERT_EQ(tftpTargetHardwareUploadClient.fetchFile(fileName.c_str(), fp),
                          TftpClientOperationResult::TFTP_CLIENT_ERROR);
                fclose(fp);
            } else {
                FAIL() << "FAIL TO CREATE HEADER FILE";
            }
        }

        // Abort operation
        targetServerClienContext.uploadOperationStatusCode = STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE; });

    ASSERT_EQ(uploadDataLoaderARINC615A->upload(),
              UploadOperationResult::UPLOAD_OPERATION_ERROR);

    sendHeartBeat = false;
    targetHardwareStatusThread.join();

    targetHardwareUploadThread.join();

    tftpTargetHardwareServer->stopListening();
    serverThread.join();

    ASSERT_EQ(targetServerClienContext.uploadOperationStatusCode,
              STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE);

    ASSERT_EQ(targetServerClienContext.waitsReceived,
              inexistentLoadList.size() * DEFAULT_ARINC615A_WAIT_TIME);
}