//
// Created by kollins on 12/09/2022.
//

#ifndef BASEFILEARINC615A_H
#define BASEFILEARINC615A_H

#include <string>
#include <memory>
#include <vector>

#include "ISerializable.h"
#include "IFileARINC615A.h"
#include <cjson/cJSON.h>

#define PROTOCOL_VERSION_SIZE                   static_cast<size_t>(2)   // bytes

class BaseFileARINC615A :
        public ISerializable, public IFileARINC615A
{
public:
    BaseFileARINC615A(std::string fileName = std::string(""),
                      std::string protocolVersion =
                              std::string(SUPPLEMENT3_ARINC_REPORT_615A_VERSION));
    virtual ~BaseFileARINC615A();

    /**
     * @brief Get Protocol Version
     *
     * @param[out] protocolVersion Protocol Version.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getProtocolVersion(
            std::string &protocolVersion);

    /**
     * @brief Get File Length
     *
     * @param[out] fileLength File Length.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    FileOperationResult getFileLength(
            uint32_t &fileLength);

    /**
     * @brief Get File Name
     *
     * @param[out] fileName File Name.
     *
     * @return FILE_OPERATION_OK if success.
     * @return FILE_OPERATION_ERROR otherwise.
     */
    virtual FileOperationResult getFileName(
            std::string &fileName);

    FileOperationResult getFileSize(size_t &fileSize) override;

    SerializableOperationResult serialize(
            std::shared_ptr<std::vector<uint8_t>> &data) override;

    SerializableOperationResult deserialize(
            std::shared_ptr<std::vector<uint8_t>> &data) override;

    SerializableOperationResult serializeJSON(
            std::string &data) override;

    SerializableOperationResult deserializeJSON(
            std::string &data) override;

protected:
    uint32_t    fileLength;
    char        protocolVersion[PROTOCOL_VERSION_SIZE];

    std::string fileName;
};

#endif //BASEFILEARINC615A_H
