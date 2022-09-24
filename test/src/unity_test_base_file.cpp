#include <gtest/gtest.h>

#include "BaseFileARINC615A.h"

TEST(ARINC615AFilesTest, BaseFileSerialization)
{
    BaseFileARINC615A baseFile("TEST_FILE.TEST","A4");
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(baseFile.serialize(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 6);
    ASSERT_EQ(data->at(0), 0);
    ASSERT_EQ(data->at(1), 0);
    ASSERT_EQ(data->at(2), 0);
    ASSERT_EQ(data->at(3), 6);
    ASSERT_EQ(data->at(4), 'A');
    ASSERT_EQ(data->at(5), '4');
}

TEST(ARINC615AFilesTest, BaseFileDeserialization)
{
    BaseFileARINC615A baseFile;
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    data->push_back(0);
    data->push_back(0);
    data->push_back(0);
    data->push_back(6);
    data->push_back('A');
    data->push_back('4');

    ASSERT_EQ(baseFile.deserialize(data), SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength = 0;
    ASSERT_EQ(baseFile.getFileLength(fileLength), FileOperationResult::FILE_OPERATION_OK);

    std::string protocolVersion = "";
    ASSERT_EQ(baseFile.getProtocolVersion(protocolVersion), FileOperationResult::FILE_OPERATION_OK);

    ASSERT_EQ(fileLength, 6);
    ASSERT_EQ(protocolVersion, std::string("A4"));
}

TEST(ARINC615AFilesTest, BaseFileSerializeJSON)
{
    BaseFileARINC615A baseFile("TEST_FILE.TEST","A4");
    std::string data("");
    ASSERT_EQ(baseFile.serializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data, "{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":6,"
                    "\"protocolVersion\":\"A4\""
                    "}");
}

TEST(ARINC615AFilesTest, BaseFileDeserializeJSON)
{
    BaseFileARINC615A baseFile;
    std::string data("{"
                     "\"fileName\":\"TEST_FILE.TEST\","
                     "\"fileLength\":6,"
                     "\"protocolVersion\":\"A4\""
                     "}");
    ASSERT_EQ(baseFile.deserializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);

    std::string fileName = "";
    ASSERT_EQ(baseFile.getFileName(fileName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileName, "TEST_FILE.TEST");

    uint32_t fileLength = 0;
    ASSERT_EQ(baseFile.getFileLength(fileLength), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileLength, 6);

    std::string protocolVersion = "";
    ASSERT_EQ(baseFile.getProtocolVersion(protocolVersion), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(protocolVersion, "A4");
}

TEST(ARINC615AFilesTest, BaseFileFileName)
{
    BaseFileARINC615A baseFile("TEST_FILE.TEST");
    std::string fileName = "";
    ASSERT_EQ(baseFile.getFileName(fileName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileName, "TEST_FILE.TEST");
}