//
// Created by kollins on 16/09/2022.
//

#ifndef LOADUPLOADSTATUSFILEARINC615A_H
#define LOADUPLOADSTATUSFILEARINC615A_H

#include "UploadBaseARINC615A.h"
#include "LoadUploadRequestFileARINC615A.h"

#define MAX_UPLOAD_STATUS_DESCRIPTION_SIZE              static_cast<size_t>(255) // bytes
#define MAX_LOAD_STATUS_DESCRIPTION_SIZE                static_cast<size_t>(255) // bytes

#define MAX_HEADER_FILE_NAME_SIZE                       static_cast<size_t>(255)  // bytes
#define MAX_LOAD_PART_NUMBER_NAME_SIZE                  static_cast<size_t>(255)  // bytes

/*
 * The Target accepts the operation (not yet started).
 */
#define STATUS_UPLOAD_ACCEPTED                          static_cast<uint16_t>(0x0001)

/*
 * The operation is in progress.
 */
#define STATUS_UPLOAD_IN_PROGRESS                       static_cast<uint16_t>(0x0002)

/*
 * The operation is completed without error.
 */
#define STATUS_UPLOAD_COMPLETED                         static_cast<uint16_t>(0x0003)

/*
 * The operation is in progress, details provided in status description.
 */
#define STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION      static_cast<uint16_t>(0x0004)

/*
 * The operation is aborted by the target hardware. Target hardware text is
 * required in the status description field to identify the reason for this
 * interruption.
 */
#define STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE    static_cast<uint16_t>(0x1003)

/*
 * The operation is aborted in the target hardware due to the receipt of an
 * abort error message sent by the data loader protocol.
 */
#define STATUS_UPLOAD_ABORTED_IN_THE_TARGET_DL_REQUEST  static_cast<uint16_t>(0x1004)

/*
 * The operation is aborted in the target hardware due to the receipt of an
 * abort error message initiated by an operator action.
 */

/*
 * The load of this Header File has failed. Text is required in the "Status
 * Description" field to explain the failure.
 */
#define STATUS_UPLOAD_HEAD_FILE_FAILED static_cast<uint16_t>(0x1007)

class LoadUploadStatusHeaderFileARINC615A :
        public ISerializable, public IFileARINC615A {
public:
    LoadUploadStatusHeaderFileARINC615A();

    ~LoadUploadStatusHeaderFileARINC615A();

    /**
     * @brief Set Header File Name
     *
     * @param[in] headerFileName Header File Name.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setHeaderFileName(
            std::string headerFileName);

    /**
     * @brief Get Header File Name
     *
     * @param[out] headerFileName Header File Name.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getHeaderFileName(
            std::string &headerFileName);

    /**
     * @brief Get Header File Name Length
     *
     * @param[out] headerFileNameLength Header File Name Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getHeaderFileNameLength(
            uint8_t &headerFileNameLength);

    /**
     * @brief Set Load Part Number Name
     *
     * @param[in] loadPartNumber Part Number.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setLoadPartNumberName(std::string loadPartNumber);

    /**
     * @brief Get Load Part Number Name
     *
     * @param[out] loadPartNumber Part Number.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadPartNumberName(std::string &loadPartNumber);

    /**
     * @brief Get Load Part Number Name Length
     *
     * @param[out] loadPartNumberLength Part Number Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadPartNumberNameLength(
            uint8_t &loadPartNumberLength);

    /**
     * @brief Set Load Ratio
     *
     * @param[in] loadRatio Load Ratio.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setLoadRatio(uint32_t loadRatio);

    /**
     * @brief Get Load Ratio
     *
     * @param[out] loadRatio Load Ratio.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadRatio(uint32_t &loadRatio);

    /**
     * @brief Set Load Status
     *
     * @param[in] loadStatus Load Status.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setLoadStatus(uint16_t loadStatus);

    /**
     * @brief Get Load Status
     *
     * @param[out] loadStatus Load Status.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadStatus(uint16_t &loadStatus);

    /**
     * @brief Set Load Status Description
     *
     * @param[in] loadStatusDescription Load Status Description.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult
    setLoadStatusDescription(std::string loadStatusDescription);

    /**
     * @brief Get Load Status Description
     *
     * @param[out] loadStatusDescription Load Status Description.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult
    getLoadStatusDescription(std::string &loadStatusDescription);

    /**
     * @brief Get Load Status Description Length
     *
     * @param[out] loadStatusDescriptionLength Load Status Description Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult
    getLoadStatusDescriptionLength(uint8_t &loadStatusDescriptionLength);

    FileOperationResult getFileSize(size_t &fileSize) override;

    SerializableOperationResult serialize(
            std::shared_ptr <std::vector<uint8_t>> &data) override;

    SerializableOperationResult deserialize(
            std::shared_ptr <std::vector<uint8_t>> &data) override;

    SerializableOperationResult serializeJSON(
            std::string &data) override;

    SerializableOperationResult deserializeJSON(
            std::string &data) override;

private:
    uint8_t headerFileNameLength;
    char headerFileName[MAX_HEADER_FILE_NAME_SIZE];
    uint8_t loadPartNumberNameLength;
    char loadPartNumberName[MAX_LOAD_PART_NUMBER_NAME_SIZE];
    uint32_t loadRatio; // This must be a 24-bit value
    uint16_t loadStatus;
    uint8_t loadStatusDescriptionLength;
    char loadStatusDescription[MAX_LOAD_STATUS_DESCRIPTION_SIZE];

};

class LoadUploadStatusFileARINC615A : public BaseFileARINC615A {
public:
    LoadUploadStatusFileARINC615A(std::string fileName = std::string(""),
                                  std::string protocolVersion =
                                  std::string(
                                          SUPPLEMENT3_ARINC_REPORT_615A_VERSION));

    virtual ~LoadUploadStatusFileARINC615A();

    /**
     * @brief Set Upload Operation Status Code
     *
     * @param[in] uploadOperationStatusCode Load Upload Operation Status Code.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setUploadOperationStatusCode(
            uint16_t uploadOperationStatusCode);

    /**
     * @brief Get Upload Operation Status Code
     *
     * @param[out] uploadOperationStatusCode Load Upload Operation Status Code.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getUploadOperationStatusCode(
            uint16_t &uploadOperationStatusCode);

    /**
     * @brief Set Upload Status Description
     *
     * @param[in] uploadStatusDescription Load Upload Status Description.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setUploadStatusDescription(
            std::string loadUploadStatusDescription);

    /**
     * @brief Get Upload Status Description
     *
     * @param[out] uploadStatusDescription Load Upload Status Description.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getUploadStatusDescription(
            std::string &loadUploadStatusDescription);

    /**
     * @brief Get Upload Status Description Length
     *
     * @param[out] uploadStatusDescriptionLength Load Upload Status Description Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getUploadStatusDescriptionLength(
            uint8_t &loadUploadStatusDescriptionLength);

    /**
     * @brief Set Counter
     *
     * @param[in] counter Status counter
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setCounter(uint16_t counter);

    /**
     * @brief Get Counter
     *
     * @param[out] counter Status counter
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getCounter(uint16_t &counter);

    /**
     * @brief Set Exception Timer
     *
     * @param[in] exceptionTimer Exception Timer
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setExceptionTimer(uint16_t exceptionTimer);

    /**
     * @brief Get Exception Timer
     *
     * @param[out] exceptionTimer Exception Timer
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getExceptionTimer(uint16_t &exceptionTimer);

    /**
     * @brief Set Estimated Time
     *
     * @param[in] estimatedTime Estimated Time
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setEstimatedTime(uint16_t estimatedTime);

    /**
     * @brief Get Estimated Time
     *
     * @param[out] estimatedTime Estimated Time
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getEstimatedTime(uint16_t &estimatedTime);

    /**
     * @brief Set Load Ratio
     *
     * @param[in] loadListRatio Load List Ratio
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setLoadListRatio(uint32_t loadListRatio);

    /**
     * @brief Get Load Ratio
     *
     * @param[out] loadListRatio Load List Ratio
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadListRatio(uint32_t &loadListRatio);

    /**
     * @brief Add header file to the list.
     *
     * @param[in] headerFile Header file.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult addHeaderFile(
            LoadUploadStatusHeaderFileARINC615A &headerFile);

    /**
     * @brief Get all header files.
     *
     * @param[out] headerFiles List of header files.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getHeaderFiles(
            std::shared_ptr <std::vector<LoadUploadStatusHeaderFileARINC615A>> &headerFiles);

    FileOperationResult getFileSize(size_t &fileSize) override;

    SerializableOperationResult serialize(
            std::shared_ptr <std::vector<uint8_t>> &data) override;

    SerializableOperationResult deserialize(
            std::shared_ptr <std::vector<uint8_t>> &data) override;

    SerializableOperationResult serializeJSON(
            std::string &data) override;

    SerializableOperationResult deserializeJSON(
            std::string &data) override;

private:
    uint16_t uploadOperationStatusCode;
    uint8_t uploadStatusDescriptionLength;
    char uploadStatusDescription[MAX_UPLOAD_STATUS_DESCRIPTION_SIZE];
    uint16_t counter;
    uint16_t exceptionTimer;
    uint16_t estimatedTime;
    uint32_t loadListRatio; // This must be a 24-bit value
    uint16_t numberOfHeaderFiles;
    std::shared_ptr <std::vector<LoadUploadStatusHeaderFileARINC615A>> headerFiles;
};


#endif //LOADUPLOADSTATUSFILEARINC615A_H
