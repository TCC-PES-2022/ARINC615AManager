//
// Created by kollins on 15/09/2022.
//

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - SERIALIZABLE_OK:                    Operation was successful.
 * - SERIALIZABLE_ERROR:                 Generic error.
 */
enum class SerializableOperationResult {
    SERIALIZABLE_OK = 0,
    SERIALIZABLE_ERROR
};

class ISerializable {
public:
    /**
     * @brief Serialize object to binary data.
     *
     * @param[out] data serialized object.
     *
     * @return SERIALIZABLE_OK if success.
     * @return SERIALIZABLE_ERROR otherwise.
     */
    virtual SerializableOperationResult serialize(
            std::shared_ptr<std::vector<uint8_t>> &data) = 0;

    /**
     * @brief Serialize object to JSON string.
     *
     * @param[out] data serialized object.
     *
     * @return SERIALIZABLE_OK if success.
     * @return SERIALIZABLE_ERROR otherwise.
     */
        virtual SerializableOperationResult serializeJSON(
                std::string &data) = 0;

    /**
     * @brief Deserialize object to binary data.
     *
     * @param[in] data serialized object.
     *
     * @return SERIALIZABLE_OK if success.
     * @return SERIALIZABLE_ERROR otherwise.
     */
    virtual SerializableOperationResult deserialize(
            std::shared_ptr<std::vector<uint8_t>> &data) = 0;

    /**
     * @brief Deserialize object from JSON string.
     *
     * @param[in] data serialized object.
     *
     * @return SERIALIZABLE_OK if success.
     * @return SERIALIZABLE_ERROR otherwise.
     */
    virtual SerializableOperationResult deserializeJSON(
            std::string &data) = 0;
};

#endif //SERIALIZABLE_H
