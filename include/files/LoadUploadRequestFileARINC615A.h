//
// Created by kollins on 15/09/2022.
//

#ifndef LOADUPLOADREQUESTFILEARINC615A_H
#define LOADUPLOADREQUESTFILEARINC615A_H

#include "BaseFileARINC615A.h"

#define MAX_HEADER_FILE_NAME_SIZE       static_cast<size_t>(255)  // bytes
#define MAX_LOAD_PART_NUMBER_NAME_SIZE  static_cast<size_t>(255)  // bytes

class LoadUploadRequestHeaderFileARINC615A :
        public ISerializable, public IFileARINC615A
{
public:
    LoadUploadRequestHeaderFileARINC615A();
    ~LoadUploadRequestHeaderFileARINC615A();

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
    FileOperationResult getHeaderFileName(std::string &headerFileName);

    /**
     * @brief Set Load Part Number Name
     *
     * @param[in] loadPartNumber Part Number Name
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult setLoadPartNumberName(std::string loadPartNumber);

    /**
     * @brief Get Load Part Number Name
     *
     * @param[out] loadPartNumberName Load Part Number Name.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadPartNumberName(std::string &loadPartNumberName);

    /**
     * @brief Get Header File Name Length
     *
     * @param[out] headerFileNameLength Header File Name Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getHeaderFileNameLength(uint8_t &headerFileNameLength);

    /**
     * @brief Get Load Part Number Name Length
     *
     * @param[out] loadPartNumberNameLength Load Part Number Name Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getLoadPartNumberNameLength(uint8_t &loadPartNumberNameLength);

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
    uint8_t headerFileNameLength;
    char    headerFileName[MAX_HEADER_FILE_NAME_SIZE];
    uint8_t loadPartNumberNameLength;
    char    loadPartNumberName[MAX_LOAD_PART_NUMBER_NAME_SIZE];
};

class LoadUploadRequestFileARINC615A : public BaseFileARINC615A {
public:
    LoadUploadRequestFileARINC615A(std::string fileName = std::string(""),
                                   std::string protocolVersion =
                                   std::string(SUPPLEMENT3_ARINC_REPORT_615A_VERSION));
    virtual ~LoadUploadRequestFileARINC615A();

    /**
     * @brief Add header file to the list.
     *
     * @param[in] headerFile Header file.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult addHeaderFile(
            LoadUploadRequestHeaderFileARINC615A &headerFile);
    /**
     * @brief Get all header files.
     *
     * @param[out] headerFiles List of header files.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getHeaderFiles(
            std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> &headerFiles);

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
    uint16_t numberOfHeaderFiles;
    std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
};

#endif //LOADUPLOADREQUESTFILEARINC615A_H
