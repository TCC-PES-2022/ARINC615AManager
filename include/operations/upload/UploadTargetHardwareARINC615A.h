//
// Created by kollins on 15/09/2022.
//

#ifndef UPLOADTARGETHARDWAREARINC615A_H
#define UPLOADTARGETHARDWAREARINC615A_H

#include "UploadBaseARINC615A.h"

/**
 * @brief Class to handle ARINC-615A upload operation on the TargetHardware
 * side.
 */
class UploadTargetHardwareARINC615A : public UploadBaseARINC615A {
public:
    UploadTargetHardwareARINC615A(std::string dataLoaderIp,
                                  int dataLoaderPort=
                                  DEFAULT_ARINC615A_TFTP_PORT);
    virtual ~UploadTargetHardwareARINC615A();

    /**
     * @brief Upload request from the dataloader.
     *
     * @param[out] fd file descriptor to the <THW_ID_POS>.LUI for read
     * ([Uploading_Initialization_Response])
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult uploadRequest(FILE **fd);

    /**
     * @brief Load list write request from the dataloader.
     *
     * @param[out] fd file descriptor to the <THW_ID_POS>.LUR for write
     * ([Load_List])
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult uploadLoadListRequest(FILE **fd);

    /**
     * @brief Load list has been received from the dataloader. You can now read
     * from the buffer allocated on uploadLoadListRequest.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult uploadLoadListReceived();

    /**
     * @brief If an upload operation is in progress, this method can be called
     * wait until the operation is finished.
     *
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    UploadOperationResult waitUploadOperation();

    UploadOperationResult abort(uint16_t abortSource) override;

    UploadOperationResult registerAbortRequestCallback(
            abortRequestCallback callback,
            std::shared_ptr<void> context) override;

private:
    std::string dataLoaderIp;
    int dataLoaderPort;
    std::vector<std::string> loadList;
};

#endif //UPLOADTARGETHARDWAREARINC615A_H
