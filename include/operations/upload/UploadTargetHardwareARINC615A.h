//
// Created by kollins on 15/09/2022.
//

#ifndef UPLOADTARGETHARDWAREARINC615A_H
#define UPLOADTARGETHARDWAREARINC615A_H

#include "UploadBaseARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"
#include "INotifier.h"

#include <thread>
#include <mutex>
#include <condition_variable>

// TODO: Create an interface for generic FSM
enum class UploadTargetHardwareARINC615AState
{
    CREATED,
    ACCEPTED,
    DENIED,
    IN_PROGRESS,
    IN_PROGRESS_WITH_DESCRIPTION,
    ABORTED_BY_TARGET,
    ABORTED_BY_DATALOADER,
    ABORTED_BY_OPERATOR,
    COMPLETED,
    ERROR,
    FINISHED,
};

/*
 * @brief Callback to check received files
 *
 * @param[in] files list of received files
 * @param[in] checkReport report description of operation
 * @param[in] context user context
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*checkFilesCallback)(
    std::vector<std::string> files,
    std::string &checkDescription,
    void *context);

/*
 * @brief Callback to check upload operation. This callback is called when all
 *        files have been received, so the TargetHardware can perform one
 *       last check before sending OK to DataLoader.
 *
 * @param[in] checkReport report description of operation
 * @param[in] context user context
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*transmissionCheckCallback)(
    std::string &checkDescription,
    void *context);

/**
 * @brief Class to handle ARINC-615A upload operation on the TargetHardware
 * side.
 */
class UploadTargetHardwareARINC615A : public UploadBaseARINC615A, public INotifier
{
public:
    UploadTargetHardwareARINC615A(std::string dataLoaderIp,
                                  int dataLoaderPort =
                                      DEFAULT_ARINC615A_TFTP_PORT);
    virtual ~UploadTargetHardwareARINC615A();

    /**
     * @brief Register a callback to check if the files are valid.
     *
     * @param[in] callback the callback to check if the files are valid.
     * @param[in] context the context to be passed to the callback.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerCheckFilesCallback(
        checkFilesCallback callback,
        void *context);

    /**
     * @brief Register a callback for a final transmission check.
     *
     * @param[in] callback the callback to check if the files are valid.
     * @param[in] context the context to be passed to the callback.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerTransmissionCheckCallback(
        transmissionCheckCallback callback,
        void *context);

    /**
     * @brief Upload request from the dataloader.
     *
     * @param[out] fp file descriptor to the <THW_ID_POS>.LUI for read
     * ([Uploading_Initialization_Response])
     * @param[out] bufferSize size of the buffer containing the file.
     * @param[in] fileName name of the LUI file.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult loadUploadInitialization(
        FILE **fp, size_t *bufferSize, std::string &fileName);

    /**
     * @brief Load list write request from the dataloader.
     *
     * @param[out] fp file descriptor to the <THW_ID_POS>.LUR for write
     * ([Load_List])
     * @param[out] bufferSize size of the buffer containing the file.
     * @param[in] fileName name of the LUR file.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult loadUploadRequest(
        FILE **fp, size_t *bufferSize, std::string &fileName);

    /**
     * @brief Get current state of the upload operation.
     *
     * @param[out] state current state of the upload operation.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult getState(UploadTargetHardwareARINC615AState &state);

    NotifierOperationResult notify(NotifierEventType event) override;

    UploadOperationResult abort(uint16_t abortSource) override;

private:
    std::string dataLoaderIp;
    int dataLoaderPort;

    uint16_t uploadOperationStatusCode;
    uint32_t loadListRatio;

    bool uploadAborted;
    std::mutex abortedMutex;

    checkFilesCallback _checkFilesCallback;
    void *_checkFilesContext;
    transmissionCheckCallback _transmissionCheckCallback;
    void *_transmissionCheckContext;

    std::string baseFileName;
    std::shared_ptr<std::vector<uint8_t>> loadUploadInitializationFileBuffer;
    std::shared_ptr<std::vector<uint8_t>> loadUploadRequestFileBuffer;

    std::shared_ptr<std::vector<LoadUploadStatusHeaderFileARINC615A>> statusHeaderFiles;

    UploadOperationResult checkUploadConditions();

    // Current state holds the last state successfully sent to the dataloader
    UploadTargetHardwareARINC615AState currentState;

    // Next state holds the next state to be sent to the dataloader (except for
    // created, error and finished, those are internal states)
    UploadTargetHardwareARINC615AState nextState;

    bool runMainThread;
    bool runStatusThread;
    bool runUploadThread;
    UploadOperationResult mainThread();
    std::thread *_mainThread;
    std::mutex _mainThreadMutex;
    std::condition_variable _mainThreadCV;

    std::string uploadStatusDescription;
    UploadOperationResult statusThread();

    static TftpClientOperationResult tftpUploadErrorCbk(short error_code,
                                                        std::string &error_message,
                                                        void *context);
    UploadOperationResult uploadThread();

#ifdef PARALLEL_UPLOAD
    std::mutex uploadHeaderMutex;
    std::vector<LoadUploadStatusHeaderFileARINC615A>::iterator uploadHeaderIt;
    typedef struct
    {
        std::thread::id uploadHeaderThreadId;
        UploadTargetHardwareARINC615A *thiz;
    } UploadThreadContext;
    UploadOperationResult uploadHeaderThread();

    std::mutex uploadWaitTimeMutex;
    std::unordered_map<std::thread::id, uint16_t> uploadWaitTime;
#else
    uint16_t uploadWaitTime;
#endif
};

#endif // UPLOADTARGETHARDWAREARINC615A_H
