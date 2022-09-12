//
// Created by kollins on 12/09/2022.
//

#ifndef UPLOADARINC615A_H
#define UPLOADARINC615A_H

#include <vector>
#include <memory>
#include <string>

#define DEFAULT_PORT        59
#define DEFAULT_DELAY_TIME  1 // second

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - UPLOAD_OPERATION_OK:                    Operation was successful.
 * - UPLOAD_OPERATION_ERROR:                 Generic error.
 */
enum class UploadOperationResult {
    UPLOAD_OPERATION_OK = 0,
    UPLOAD_OPERATION_ERROR
};

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
typedef UploadOperationResult (*uploadInitializationResponseCallback) (
        std::string uploadInitializationResponseJson,
        std::shared_ptr<void> context
);

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
typedef UploadOperationResult (*uploadInformationStatusCallback) (
        std::string uploadInformationStatusJson,
        std::shared_ptr<void> context
);

/**
 * @brief Callback for file not available. This callback is called when the
 * DataLoader receives [File_Not_Available].
 *
 * //TODO: document the JSON format.
 *
 * @param[out] waitTimeS time to wait in seconds before next call.
 * @param[in] context the user context.
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*fileNotAvailableCallback) (
        int *waitTimeS,
        std::shared_ptr<void> context
);

/**
 * @brief Callback for abort request. This callback is called when the
 * DataLoader or the TargetHardware receives [Abort_Request].
 *
 * //TODO: document the JSON format.
 *
 * @param[in] context the user context.
 *
 * @return UPLOAD_OPERATION_OK if success.
 * @return UPLOAD_OPERATION_ERROR otherwise.
 */
typedef UploadOperationResult (*abortRequestCallback)(
        std::shared_ptr<void> context
);

/**
 * @brief Interface class for ARINC-615A upload operations.
 */
class IUploadARINC615A {
public:
    virtual ~IUploadARINC615A();

    /**
     * @brief Abort upload operation.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    virtual UploadOperationResult abort() = 0;

    virtual UploadOperationResult registerAbortRequestCallback(
            abortRequestCallback callback,
            std::shared_ptr<void> context) = 0;

protected:
    abortRequestCallback _abortRequestCallback;
    std::shared_ptr<void> _abortRequestContext;
};

/**
 * @brief Class to handle ARINC-615A upload operation on the DataLoader side.
 */
class UploadDataLoaderARINC615A : public IUploadARINC615A {
public:
    UploadDataLoaderARINC615A(std::string targetIp,
                              std::vector<std::string> loadList);
    virtual ~UploadDataLoaderARINC615A();

    /**
     * @brief Start upload operation. This method must called by the dataloader
     * to start the upload to the target hardware.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult upload();

    UploadOperationResult abort() override;

    /**
     * Register a callback for upload initialization response.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return FIND_OPERATION_OK if success.
     * @return FIND_OPERATION_ERROR otherwise.
     */
    UploadOperationResult registerUploadInitializationResponseCallback(
            uploadInitializationResponseCallback callback,
            std::shared_ptr<void> context);

    /**
    * Register a callback for upload information status.
    *
    * @param[in] callback the callback.
    * @param[in] context the user context.
    *
    * @return FIND_OPERATION_OK if success.
    * @return FIND_OPERATION_ERROR otherwise.
    */
    UploadOperationResult registerUploadInformationStatusCallback(
            uploadInformationStatusCallback callback,
            std::shared_ptr<void> context);

    /**
    * Register a callback for file not available.
    *
    * @param[in] callback the callback.
    * @param[in] context the user context.
    *
    * @return FIND_OPERATION_OK if success.
    * @return FIND_OPERATION_ERROR otherwise.
    */
    UploadOperationResult registerFileNotAvailableCallback(
            fileNotAvailableCallback callback,
            std::shared_ptr<void> context);

    UploadOperationResult registerAbortRequestCallback(
            abortRequestCallback callback,
            std::shared_ptr<void> context) override;

private:
    std::string targetIp;
    std::vector<std::string> loadList;

    std::shared_ptr<void> _uploadInitializationResponseContext;
    uploadInitializationResponseCallback _uploadInitializationResponseCallback;

    std::shared_ptr<void> _uploadInformationStatusContext;
    uploadInformationStatusCallback _uploadInformationStatusCallback;

    std::shared_ptr<void> _fileNotAvailableContext;
    fileNotAvailableCallback _fileNotAvailableCallback;
};

/**
 * @brief Class to handle ARINC-615A upload operation on the TargetHardware
 * side.
 */
class UploadTargetHardwareARINC615A : public IUploadARINC615A {
public:
    UploadTargetHardwareARINC615A(std::string dataLoaderIp,
                                  int dataLoaderPort=DEFAULT_PORT);
    virtual ~UploadTargetHardwareARINC615A();

    /**
     * @brief Upload request. This method must be called by the target hardware
     * when the [TH_Uploading_Initialization] is received.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult uploadRequest();

    UploadOperationResult abort() override;

    UploadOperationResult registerAbortRequestCallback(
            abortRequestCallback callback,
            std::shared_ptr<void> context) override;

private:
    std::string dataLoaderIp;
    int dataLoaderPort;
    std::vector<std::string> loadList;
};


#endif //UPLOADARINC615A_H
