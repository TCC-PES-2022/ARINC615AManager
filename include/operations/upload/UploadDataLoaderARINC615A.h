//
// Created by kollins on 12/09/2022.
//

#ifndef UPLOADARINC615A_H
#define UPLOADARINC615A_H

#include <string>
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <tuple>

#include "TFTPServer.h"
#include "UploadBaseARINC615A.h"

#define DEFAULT_WAIT_TIME 1 // second

/**
 * @brief This data type will be used to store a single load. The stored
 *        format must be <FileName, PartNumber>
 */
#define LOAD_FILE_NAME_IDX 0
#define LOAD_PART_NUMBER_IDX 1
typedef std::tuple<std::string, std::string> ArincLoad;

class LoadUploadRequestFileARINC615A;

/**
 * @brief Callback for upload initialization operation response. This callback
 * is called when the DataLoader receives [Uploading_Initialization_Response]
 *
 * //TODO: document the JSON format.
 *
 * @param[in] uploadInitializationResponseJson JSON with initialization response.
 * @param[in] context the user context.
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*uploadInitializationResponseCallback)(
    std::string uploadInitializationResponseJson,
    void *context);

/**
 * @brief Callback for upload progress report. This callback is called when the
 * DataLoader receives [Upload_Information_Status].
 *
 * //TODO: document the JSON format.
 *
 * @param[in] uploadInformationStatusJson JSON with upload information status.
 * @param[in] context the user context.
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*uploadInformationStatusCallback)(
    std::string uploadInformationStatusJson,
    void *context);

/**
 * @brief Callback for file not available. This callback is called when the
 * DataLoader receives [File_Not_Available].
 *
 * //TODO: document the JSON format.
 *
 * @param[in] fileName the file name.
 * @param[out] waitTimeS time to wait in seconds before next call.
 * @param[in] context the user context.
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*fileNotAvailableCallback)(
    std::string fileName,
    uint16_t *waitTimeS,
    void *context);

/**
 * @brief Class to handle ARINC-615A upload operation on the DataLoader side.
 */
class UploadDataLoaderARINC615A : public UploadBaseARINC615A
{
public:
    UploadDataLoaderARINC615A(std::string targetHardwareId = "",
                              std::string targetHardwarePosition = "",
                              std::string targetHardwareIp = "");
    virtual ~UploadDataLoaderARINC615A();

    /**
     * @brief Set TargetHardware ID. This is the ID of the TargetHardware where the
     *        load will be uploaded. You can get the TargetHardware's ID from the
     *        find operation.
     *
     * @param[in] targetHardwareId the TargetHardware ID.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setTargetHardwareId(std::string &targetHardwareId);

    /**
     * @brief Set TargetHardware position. This is the position of the TargetHardware
     *        where the load will be uploaded. You can get the TargetHardware's
     *        position from the find operation.
     *
     * @param[in] targetHardwarePosition the TargetHardware position.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setTargetHardwarePosition(
        std::string &targetHardwarePosition);

    /**
     * @brief Set TargetHardware IP. This is the IP of the TargetHardware
     *        where the load will be uploaded. You can get the TargetHardware's
     *        IP from the find operation.
     *
     * @param[in] targetHardwareIp the TargetHardware IP.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setTargetHardwareIp(std::string &targetHardwareIp);

    /**
     * @brief Set DataLoader server PORT. This is the port where the DataLoader
     *       will listen for the upload operation.
     *
     * @param[in] port the port number.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setTftpDataLoaderServerPort(uint16_t port);

    /**
     * @brief Set TargetHardware server PORT. This is the port to connect to the
     *       TargetHardware.
     *
     * @param[in] port the port number.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setTftpTargetHardwareServerPort(uint16_t port);

    /**
     * @brief Set load list.
     *
     * @param[in] loadList the load list.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult setLoadList(std::vector<ArincLoad> loadList);

    /**
     * @brief Start upload operation. This method must called by the dataloader
     * to start the upload to the target hardware.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult upload();

    /**
     * Register a callback for upload initialization response.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerUploadInitializationResponseCallback(
        uploadInitializationResponseCallback callback,
        void *context);

    /**
     * Register a callback for upload information status.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerUploadInformationStatusCallback(
        uploadInformationStatusCallback callback,
        void *context);

    /**
     * Register a callback for file not available.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerFileNotAvailableCallback(
        fileNotAvailableCallback callback,
        void *context);

    UploadOperationResult abort(uint16_t abortSource) override;

private:
    class TargetClient
    {
    public:
        TargetClient(TftpSectionId clientId);
        ~TargetClient();

        UploadOperationResult getClientId(TftpSectionId &clientId);
        UploadOperationResult getClientFileBufferReference(FILE **fp);
        UploadOperationResult getClientBufferReference(char **buffer);
        UploadOperationResult setFileName(std::string fileName);
        UploadOperationResult getFileName(std::string &fileName);
        UploadOperationResult setSectionFinished();
        UploadOperationResult isSectionFinished(bool &sectionFinished);
        UploadOperationResult hasDataToProcess(bool &hasDataToProcess);

    private:
        TftpSectionId clientId;
        std::string fileName;
        char *clientFileBuffer;
        bool sectionFinished;
    };

    UploadOperationResult initTFTP();
    UploadOperationResult initUploadFiles(
        LoadUploadRequestFileARINC615A &loadUploadRequestFileARINC615A);

    static TftpServerOperationResult targetHardwareSectionStarted(
        ITFTPSection *sectionHandler, void *context);
    static TftpServerOperationResult targetHardwareSectionFinished(
        ITFTPSection *sectionHandler, void *context);
    static TftpServerOperationResult targetHardwareOpenFileRequest(
        ITFTPSection *sectionHandler, FILE **fp, char *filename,
        char *mode, size_t *bufferSize, void *context);
    static TftpServerOperationResult targetHardwareCloseFileRequest(
        ITFTPSection *sectionHandler, FILE *fp, void *context);

    std::mutex targetClientsMutex;
    std::unordered_map<TftpSectionId, std::shared_ptr<TargetClient>> targetClients;
    std::condition_variable clientProcessorCV;
    std::mutex clientProcessorMutex;
    UploadOperationResult clientProcessor();
    UploadOperationResult processFile(std::string fileName, char *buffer);
    UploadOperationResult processLoadUploadStatusFile(char *buffer);

    bool toggleAbortSend;
    UploadOperationResult abortTargetRequest(uint16_t abortSource,
                                           ITFTPSection *sectionHandler,
                                           char *filename, char *mode);
    uint16_t abortSource;

    std::string targetHardwareId;
    std::string targetHardwarePosition;
    std::string targetHardwareIp;
    std::vector<ArincLoad> loadList;

    void *_uploadInitializationResponseContext;
    uploadInitializationResponseCallback _uploadInitializationResponseCallback;

    void *_uploadInformationStatusContext;
    uploadInformationStatusCallback _uploadInformationStatusCallback;

    void *_fileNotAvailableContext;
    fileNotAvailableCallback _fileNotAvailableCallback;

    uint16_t tftpTargetHardwareServerPort;
    uint16_t tftpDataLoaderServerPort;
    std::unique_ptr<ITFTPServer> tftpServer;

    std::condition_variable filesProcessedCV;
    std::mutex filesProcessedMutex;

    std::condition_variable endUploadCV;
    std::mutex endUploadMutex;

    bool uploadInitializationAccepted;
    bool uploadCompleted;
    bool endUpload;
};

#endif // UPLOADARINC615A_H
