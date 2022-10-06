//
// Created by kollins on 12/09/2022.
//

#ifndef INITIALIZATIONFILE_H
#define INITIALIZATIONFILE_H

#include "BaseFileARINC615A.h"

#define MAX_STATUS_DESCRIPTION_SIZE             static_cast<size_t>(255) // bytes

/*
 * TODO: Remove UPLOAD from variable names because these codes are used 
 * for every initialization operation
 */

/*
 * The operation is accepted.
 */
#define INITIALIZATION_UPLOAD_IS_ACCEPTED            static_cast<uint16_t>(0x0001)

/*
 * The operation is denied. The reason is described in the status description
 * field.
 */
#define INITIALIZATION_UPLOAD_IS_DENIED              static_cast<uint16_t>(0x1000)

/*
 * The operation is not supported by the Target
 */
#define INITIALIZATION_UPLOAD_IS_NOT_SUPPORTED       static_cast<uint16_t>(0x1002)

class InitializationFileARINC615A : public BaseFileARINC615A {
public:
    InitializationFileARINC615A(std::string fileName = std::string(""),
                                std::string protocolVersion =
                                         std::string(SUPPLEMENT3_ARINC_REPORT_615A_VERSION));
    virtual ~InitializationFileARINC615A();

    /**
     * @brief Set Operation Operation Acceptance Status Code
     *
     * @param[in] operationAcceptanceStatusCode Operation Acceptance Status Code.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setOperationAcceptanceStatusCode (
            uint16_t operationAcceptanceStatusCode);

    /**
     * @brief Get Operation Operation Acceptance Status Code
     *
     * @param[out] operationAcceptanceStatusCode Operation Acceptance Status Code.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getOperationAcceptanceStatusCode (
            uint16_t &operationAcceptanceStatusCode);

    /**
     * @brief Get Status Description Length
     *
     * @param[out] setStatusDescriptionLength Status Description Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getStatusDescriptionLength (
            uint8_t &statusDescriptionLength);

    /**
     * @brief Set Status Description
     *
     * @param[in] statusDescription Status Description.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setStatusDescription (
            std::string statusDescription);

    /**
     * @brief Get Status Description
     *
     * @param[out] statusDescription Status Description Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getStatusDescription (
            std::string &statusDescription);

    FileOperationResult getFileSize(size_t &fileSize) override;

    SerializableOperationResult serialize(
            std::shared_ptr<std::vector<uint8_t>> &data) override;

    SerializableOperationResult deserialize(
            std::shared_ptr<std::vector<uint8_t>> &data) override;

    SerializableOperationResult serializeJSON(
            std::string &data) override;

    SerializableOperationResult deserializeJSON(
            std::string &data) override;

private:
    uint16_t        operationAcceptanceStatusCode;
    uint8_t         statusDescriptionLength;
    char            statusDescription[MAX_STATUS_DESCRIPTION_SIZE];
};


#endif //INITIALIZATIONFILE_H
