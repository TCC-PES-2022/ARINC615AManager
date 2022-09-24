//
// Created by kollins on 12/09/2022.
//

#include "InitializationFileARINC615A.h"
#include <algorithm>
#include <cstring>

InitializationFileARINC615A::InitializationFileARINC615A(
        std::string fileName, std::string protocolVersion) :
        BaseFileARINC615A(fileName, protocolVersion)
{
    operationAcceptanceStatusCode = 0;
    statusDescriptionLength = 0;
    std::memset(statusDescription, 0, MAX_STATUS_DESCRIPTION_SIZE);

    fileLength += sizeof(operationAcceptanceStatusCode) +
                  sizeof(statusDescriptionLength);
}

InitializationFileARINC615A::~InitializationFileARINC615A()
{
}

FileOperationResult InitializationFileARINC615A::setOperationAcceptanceStatusCode(
        uint16_t operationAcceptanceStatusCode)
{
    this->operationAcceptanceStatusCode = operationAcceptanceStatusCode;
    return FileOperationResult::FILE_OPERATION_OK;
}

FileOperationResult InitializationFileARINC615A::getOperationAcceptanceStatusCode(
        uint16_t &operationAcceptanceStatusCode)
{
    operationAcceptanceStatusCode = this->operationAcceptanceStatusCode;
    return FileOperationResult::FILE_OPERATION_OK;
}

FileOperationResult InitializationFileARINC615A::getStatusDescriptionLength(
        uint8_t &statusDescriptionLength)
{
    statusDescriptionLength = this->statusDescriptionLength;
    return FileOperationResult::FILE_OPERATION_OK;
}

FileOperationResult InitializationFileARINC615A::setStatusDescription(
        std::string statusDescription)
{
    fileLength -= statusDescriptionLength;
    statusDescriptionLength = std::min(statusDescription.length()+1,
                                       MAX_STATUS_DESCRIPTION_SIZE);
    std::memcpy(this->statusDescription, statusDescription.c_str(),
                statusDescriptionLength);
    if (statusDescriptionLength == MAX_STATUS_DESCRIPTION_SIZE) {
        // If the string is too long, we need to add a null terminator to the end
        this->statusDescription[statusDescriptionLength - 1] = '\0';
    }
    fileLength += statusDescriptionLength;
    return FileOperationResult::FILE_OPERATION_OK;
}

FileOperationResult InitializationFileARINC615A::getStatusDescription(
        std::string &statusDescription)
{
    statusDescription = std::string(this->statusDescription);
    return FileOperationResult::FILE_OPERATION_OK;
}

SerializableOperationResult InitializationFileARINC615A::serialize(
        std::shared_ptr<std::vector<uint8_t>> &data)
{
    SerializableOperationResult result = BaseFileARINC615A::serialize(data);
    if (result != SerializableOperationResult::SERIALIZABLE_OK) {
        return result;
    }

    data->push_back((operationAcceptanceStatusCode >> 8) & 0xFF);
    data->push_back(operationAcceptanceStatusCode & 0xFF);
    data->push_back(statusDescriptionLength);
    data->insert(data->end(), statusDescription,
                 statusDescription + statusDescriptionLength);

    return SerializableOperationResult::SERIALIZABLE_OK;
}

SerializableOperationResult InitializationFileARINC615A::deserialize(
        std::shared_ptr<std::vector<uint8_t>> &data)
{
    SerializableOperationResult result = BaseFileARINC615A::deserialize(data);
    if (result != SerializableOperationResult::SERIALIZABLE_OK) {
        return result;
    }

    size_t parentSize = 0;
    BaseFileARINC615A::getFileSize(parentSize);
    size_t offset = parentSize;

    if (data->size() < offset + sizeof(operationAcceptanceStatusCode)) {
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    operationAcceptanceStatusCode = (data->at(offset) << 8) | data->at(offset+1);
    offset += sizeof(operationAcceptanceStatusCode);

    if (data->size() < offset + sizeof(statusDescriptionLength)) {
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    statusDescriptionLength = data->at(offset);
    offset += sizeof(statusDescriptionLength);

    if (data->size() < offset + statusDescriptionLength) {
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    std::memcpy(statusDescription, data->data() + offset, statusDescriptionLength);

    return SerializableOperationResult::SERIALIZABLE_OK;
}

SerializableOperationResult InitializationFileARINC615A::serializeJSON(
        std::string &data)
{
    SerializableOperationResult result = BaseFileARINC615A::serializeJSON(data);
    if (result != SerializableOperationResult::SERIALIZABLE_OK) {
        return result;
    }

    cJSON *root = cJSON_Parse(data.c_str());
    if (root == nullptr) {
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }

    cJSON *operationAcceptanceStatusCodeJSON = cJSON_CreateNumber(
            operationAcceptanceStatusCode);
    if (operationAcceptanceStatusCodeJSON == NULL) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    cJSON_AddItemToObject(root, "operationAcceptanceStatusCode",
                          operationAcceptanceStatusCodeJSON);

    cJSON *statusDescriptionLengthJSON = cJSON_CreateNumber(
            statusDescriptionLength);
    if (statusDescriptionLengthJSON == NULL) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    cJSON_AddItemToObject(root, "statusDescriptionLength",
                          statusDescriptionLengthJSON);

    cJSON *statusDescriptionJSON = cJSON_CreateString(statusDescription);
    if (statusDescriptionJSON == NULL) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    cJSON_AddItemToObject(root, "statusDescription", statusDescriptionJSON);

    char *serializedJSON = cJSON_PrintUnformatted(root);
    if (serializedJSON == NULL) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    data = std::string(serializedJSON);
    free(serializedJSON);
    cJSON_Delete(root);

    return SerializableOperationResult::SERIALIZABLE_OK;
}

SerializableOperationResult InitializationFileARINC615A::deserializeJSON(
        std::string &data)
{
    SerializableOperationResult result = BaseFileARINC615A::deserializeJSON(data);
    if (result != SerializableOperationResult::SERIALIZABLE_OK) {
        return result;
    }

    cJSON *root = cJSON_Parse(data.c_str());
    if (root == nullptr) {
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }

    cJSON *operationAcceptanceStatusCodeJSON = cJSON_GetObjectItem(root,
            "operationAcceptanceStatusCode");
    if (operationAcceptanceStatusCodeJSON == nullptr) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    operationAcceptanceStatusCode = operationAcceptanceStatusCodeJSON->valueint;

    cJSON *statusDescriptionLengthJSON = cJSON_GetObjectItem(root,
            "statusDescriptionLength");
    if (statusDescriptionLengthJSON == nullptr) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    statusDescriptionLength = statusDescriptionLengthJSON->valueint;

    cJSON *statusDescriptionJSON = cJSON_GetObjectItem(root,
            "statusDescription");
    if (statusDescriptionJSON == nullptr) {
        cJSON_Delete(root);
        return SerializableOperationResult::SERIALIZABLE_ERROR;
    }
    std::string statusDescription = statusDescriptionJSON->valuestring;
    std::memcpy(this->statusDescription, statusDescription.c_str(),
                statusDescriptionLength);
    cJSON_Delete(root);

    return SerializableOperationResult::SERIALIZABLE_OK;
}

FileOperationResult InitializationFileARINC615A::getFileSize(size_t &fileSize)
{
    FileOperationResult result = BaseFileARINC615A::getFileSize(fileSize);
    if (result != FileOperationResult::FILE_OPERATION_OK) {
        return result;
    }

    fileSize += sizeof(operationAcceptanceStatusCode);
    fileSize += sizeof(statusDescriptionLength);
    fileSize += statusDescriptionLength;

    return FileOperationResult::FILE_OPERATION_OK;
}