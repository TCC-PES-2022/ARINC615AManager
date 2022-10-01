//
// Created by kollins on 15/09/2022.
//

#ifndef UPLOADBASEARINC615A_H
#define UPLOADBASEARINC615A_H

#include <vector>
#include <memory>

#include "TFTPClient.h"

// TODO: Maybe this may be moved to the ARINC615A file classes
#define UPLOAD_INITIALIZATION_FILE_EXTENSION std::string(".LUI")
#define UPLOAD_LOAD_UPLOAD_REQUEST_FILE_EXTENSION std::string(".LUR")
#define UPLOAD_LOAD_UPLOAD_STATUS_FILE_EXTENSION std::string(".LUS")

#define DEFAULT_ARINC615A_TFTP_PORT             59
#define DEFAULT_ARINC615A_TFTP_TIMEOUT          2   // seconds
#define DEFAULT_ARINC615A_DLP_TIMEOUT           13  // seconds
#define DEFAULT_ARINC615A_WAIT_TIME             1   // second
#define MAX_DLP_TRIES 2

#define MAX_NUM_FILES 10

// LUS file is the biggest one. This is the worst case scenario for this file.
#define MAX_FILE_BUFFER_SIZE (275 + (MAX_NUM_FILES * 773))

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

#define UPLOAD_ABORT_SOURCE_NONE            0
#define UPLOAD_ABORT_SOURCE_TARGETHARDWARE  0x1003
#define UPLOAD_ABORT_SOURCE_DATALOADER      0x1004
#define UPLOAD_ABORT_SOURCE_OPERATOR        0x1005

#define ARINC_ABORT_MSG_PREFIX              "ABORT"
#define ARINC_WAIT_MSG_PREFIX               "WAIT"
#define ARINC_ERROR_MSG_DELIMITER           ":"

/**
 * @brief Base class for ARINC-615A upload operations.
 */
class UploadBaseARINC615A {
public:
    virtual ~UploadBaseARINC615A() = default;

    /**
     * @brief Abort upload operation.
     *
     * @param[in] abortSource the abort source.
     * 
     * @return UPLOAD_OPERATION_OK if success.
     * @return UPLOAD_OPERATION_ERROR otherwise.
     */
    virtual UploadOperationResult abort(uint16_t abortSource) = 0;

protected:
    std::unique_ptr<ITFTPClient> tftpClient;
};

#endif //ARINC615AMANAGER_UPLOADBASEARINC615A_H
