#include <gtest/gtest.h>

#include "InitializationFileARINC615A.h"

TEST(ARINC615AFilesTest, InitializationFileSerialization)
{
    InitializationFileARINC615A initializationFile("TEST_FILE.TEST", "A4");
    initializationFile.setOperationAcceptanceStatusCode(0x0001);
    initializationFile.setStatusDescription("Test file");

    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(initializationFile.serialize(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 19);
    ASSERT_EQ(data->at(0), 0);
    ASSERT_EQ(data->at(1), 0);
    ASSERT_EQ(data->at(2), 0);
    ASSERT_EQ(data->at(3), 19);
    ASSERT_EQ(data->at(4), 'A');
    ASSERT_EQ(data->at(5), '4');
    ASSERT_EQ(data->at(6), 0);
    ASSERT_EQ(data->at(7), 1);
    ASSERT_EQ(data->at(8), 0x0A);
    ASSERT_EQ(data->at(9), 'T');
    ASSERT_EQ(data->at(10), 'e');
    ASSERT_EQ(data->at(11), 's');
    ASSERT_EQ(data->at(12), 't');
    ASSERT_EQ(data->at(13), ' ');
    ASSERT_EQ(data->at(14), 'f');
    ASSERT_EQ(data->at(15), 'i');
    ASSERT_EQ(data->at(16), 'l');
    ASSERT_EQ(data->at(17), 'e');
    ASSERT_EQ(data->at(18), '\0');
}

TEST(ARINC615AFilesTest, InitializationFileSerializationDescriptionOverflow)
{
    InitializationFileARINC615A initializationFile("TEST_FILE.TEST", "A4");
    initializationFile.setOperationAcceptanceStatusCode(0x0001);

    //Max description length is 255 bytes
    std::string description(1024, 'a');
    initializationFile.setStatusDescription(description);

    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(initializationFile.serialize(data), SerializableOperationResult::SERIALIZABLE_OK);
    EXPECT_EQ(data->size(), 264);
    EXPECT_EQ(data->at(0), 0);
    EXPECT_EQ(data->at(1), 0);
    EXPECT_EQ(data->at(2), 0x01);
    EXPECT_EQ(data->at(3), 0x08);
    EXPECT_EQ(data->at(4), 'A');
    EXPECT_EQ(data->at(5), '4');
    EXPECT_EQ(data->at(6), 0);
    EXPECT_EQ(data->at(7), 1);
    EXPECT_EQ(data->at(8), 0xFF);
    for (int i = 9; i < 262; i++)
    {
        EXPECT_EQ(data->at(i), 'a');
    }
    EXPECT_EQ(data->at(263), '\0');
}

TEST(ARINC615AFilesTest, InitializationFileDeserialization)
{
    InitializationFileARINC615A initializationFile;
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    data->push_back(0);
    data->push_back(0);
    data->push_back(0);
    data->push_back(19);
    data->push_back('A');
    data->push_back('4');
    data->push_back(0);
    data->push_back(1);
    data->push_back(0x0A);
    data->push_back('T');
    data->push_back('e');
    data->push_back('s');
    data->push_back('t');
    data->push_back(' ');
    data->push_back('f');
    data->push_back('i');
    data->push_back('l');
    data->push_back('e');
    data->push_back('\0');
    ASSERT_EQ(initializationFile.deserialize(data), SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength = 0;
    ASSERT_EQ(initializationFile.getFileLength(fileLength), FileOperationResult::FILE_OPERATION_OK);

    std::string protocolVersion = "";
    ASSERT_EQ(initializationFile.getProtocolVersion(protocolVersion), FileOperationResult::FILE_OPERATION_OK);

    uint16_t operationAcceptanceStatusCode = 0;
    ASSERT_EQ(initializationFile.getOperationAcceptanceStatusCode(operationAcceptanceStatusCode), FileOperationResult::FILE_OPERATION_OK);

    uint8_t statusDescriptionLength = 0;
    ASSERT_EQ(initializationFile.getStatusDescriptionLength(statusDescriptionLength), FileOperationResult::FILE_OPERATION_OK);

    std::string statusDescription = "";
    ASSERT_EQ(initializationFile.getStatusDescription(statusDescription), FileOperationResult::FILE_OPERATION_OK);

    ASSERT_EQ(fileLength, 19);
    ASSERT_EQ(protocolVersion, std::string("A4"));
    ASSERT_EQ(operationAcceptanceStatusCode, 0x0001);
    ASSERT_EQ(statusDescriptionLength, 0x0A);
    ASSERT_EQ(statusDescription, std::string("Test file"));
}

TEST(ARINC615AFilesTest, InitializationFileSerializationJSON)
{
    InitializationFileARINC615A initializationFile("TEST_FILE.TEST", "A4");
    initializationFile.setOperationAcceptanceStatusCode(0x0001);
    initializationFile.setStatusDescription("Test file");

    std::string data("");
    ASSERT_EQ(initializationFile.serializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data,"{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":19,"
                    "\"protocolVersion\":\"A4\","
                    "\"operationAcceptanceStatusCode\":1,"
                    "\"statusDescriptionLength\":10,"
                    "\"statusDescription\":\"Test file\""
                    "}");
}

TEST(ARINC615AFilesTest, InitializationFileDeserializationJSON)
{
    InitializationFileARINC615A initializationFile;
    std::string data("{"
                   "\"fileName\":\"TEST_FILE.TEST\","
                   "\"fileLength\":19,"
                    "\"protocolVersion\":\"A4\","
                    "\"operationAcceptanceStatusCode\":1,"
                    "\"statusDescriptionLength\":10,"
                    "\"statusDescription\":\"Test file\""
                    "}");
    ASSERT_EQ(initializationFile.deserializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength = 0;
    ASSERT_EQ(initializationFile.getFileLength(fileLength), FileOperationResult::FILE_OPERATION_OK);

    std::string protocolVersion = "";
    ASSERT_EQ(initializationFile.getProtocolVersion(protocolVersion), FileOperationResult::FILE_OPERATION_OK);

    uint16_t operationAcceptanceStatusCode = 0;
    ASSERT_EQ(initializationFile.getOperationAcceptanceStatusCode(operationAcceptanceStatusCode), FileOperationResult::FILE_OPERATION_OK);

    uint8_t statusDescriptionLength = 0;
    ASSERT_EQ(initializationFile.getStatusDescriptionLength(statusDescriptionLength), FileOperationResult::FILE_OPERATION_OK);

    std::string statusDescription = "";
    ASSERT_EQ(initializationFile.getStatusDescription(statusDescription), FileOperationResult::FILE_OPERATION_OK);

    ASSERT_EQ(fileLength, 19);
    ASSERT_EQ(protocolVersion, std::string("A4"));
    ASSERT_EQ(operationAcceptanceStatusCode, 0x0001);
    ASSERT_EQ(statusDescriptionLength, 0x0A);
    ASSERT_EQ(statusDescription, std::string("Test file"));
}

TEST(ARINC615AFilesTest, InitializationFileFileName)
{
    BaseFileARINC615A baseFile("TEST_FILE.TEST");
    std::string fileName = "";
    ASSERT_EQ(baseFile.getFileName(fileName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileName, "TEST_FILE.TEST");
}