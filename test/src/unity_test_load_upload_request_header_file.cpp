#include <gtest/gtest.h>

#include "LoadUploadRequestFileARINC615A.h"

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileSetFileName)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    ASSERT_EQ(loadUploadRequestHeaderFile.setHeaderFileName("TEST_FILE1.TEST"),
              FileOperationResult::FILE_OPERATION_OK);
    std::string headerFileName = "";
    uint8_t headerFileNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileName(headerFileName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileNameLength(headerFileNameLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE1.TEST");
    ASSERT_EQ(headerFileNameLength, 16);
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileSetLoadPartNumber)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    ASSERT_EQ(loadUploadRequestHeaderFile.setLoadPartNumberName("TEST_PART_NUMBER1"),
              FileOperationResult::FILE_OPERATION_OK);
    std::string loadPartNumberName = "";
    uint8_t loadPartNumberNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberName(loadPartNumberName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberNameLength(loadPartNumberNameLength),
                FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER1");
    ASSERT_EQ(loadPartNumberNameLength, 18);
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileSerialize)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    ASSERT_EQ(loadUploadRequestHeaderFile.setHeaderFileName("TEST_FILE1.TEST"),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.setLoadPartNumberName("TEST_PART_NUMBER1"),
                FileOperationResult::FILE_OPERATION_OK);
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(loadUploadRequestHeaderFile.serialize(data),
              SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 36);
    ASSERT_EQ(data->at(0), 16);
    ASSERT_EQ(data->at(1), 'T');
    ASSERT_EQ(data->at(2), 'E');
    ASSERT_EQ(data->at(3), 'S');
    ASSERT_EQ(data->at(4), 'T');
    ASSERT_EQ(data->at(5), '_');
    ASSERT_EQ(data->at(6), 'F');
    ASSERT_EQ(data->at(7), 'I');
    ASSERT_EQ(data->at(8), 'L');
    ASSERT_EQ(data->at(9), 'E');
    ASSERT_EQ(data->at(10), '1');
    ASSERT_EQ(data->at(11), '.');
    ASSERT_EQ(data->at(12), 'T');
    ASSERT_EQ(data->at(13), 'E');
    ASSERT_EQ(data->at(14), 'S');
    ASSERT_EQ(data->at(15), 'T');
    ASSERT_EQ(data->at(16), '\0');
    ASSERT_EQ(data->at(17), 18);
    ASSERT_EQ(data->at(18), 'T');
    ASSERT_EQ(data->at(19), 'E');
    ASSERT_EQ(data->at(20), 'S');
    ASSERT_EQ(data->at(21), 'T');
    ASSERT_EQ(data->at(22), '_');
    ASSERT_EQ(data->at(23), 'P');
    ASSERT_EQ(data->at(24), 'A');
    ASSERT_EQ(data->at(25), 'R');
    ASSERT_EQ(data->at(26), 'T');
    ASSERT_EQ(data->at(27), '_');
    ASSERT_EQ(data->at(28), 'N');
    ASSERT_EQ(data->at(29), 'U');
    ASSERT_EQ(data->at(30), 'M');
    ASSERT_EQ(data->at(31), 'B');
    ASSERT_EQ(data->at(32), 'E');
    ASSERT_EQ(data->at(33), 'R');
    ASSERT_EQ(data->at(34), '1');
    ASSERT_EQ(data->at(35), '\0');
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileSerializeOverflow)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    std::string overflowStringA(1024, 'A');
    ASSERT_EQ(loadUploadRequestHeaderFile.setHeaderFileName(overflowStringA),
    FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.setLoadPartNumberName(overflowStringA),
    FileOperationResult::FILE_OPERATION_OK);
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(loadUploadRequestHeaderFile.serialize(data),
            SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 512);
    for (uint16_t i = 0; i < 2; i++)
    {
        ASSERT_EQ(data->at(i*256), 255);
        for (uint16_t j = 1; j < 255; j++)
        {
            ASSERT_EQ(data->at(i*256 + j), 'A');
        }
        ASSERT_EQ(data->at(i*256 + 255), '\0');
    }
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileDeserialize)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    data->push_back(16);
    data->push_back('T');
    data->push_back('E');
    data->push_back('S');
    data->push_back('T');
    data->push_back('_');
    data->push_back('F');
    data->push_back('I');
    data->push_back('L');
    data->push_back('E');
    data->push_back('1');
    data->push_back('.');
    data->push_back('T');
    data->push_back('E');
    data->push_back('S');
    data->push_back('T');
    data->push_back('\0');
    data->push_back(18);
    data->push_back('T');
    data->push_back('E');
    data->push_back('S');
    data->push_back('T');
    data->push_back('_');
    data->push_back('P');
    data->push_back('A');
    data->push_back('R');
    data->push_back('T');
    data->push_back('_');
    data->push_back('N');
    data->push_back('U');
    data->push_back('M');
    data->push_back('B');
    data->push_back('E');
    data->push_back('R');
    data->push_back('1');
    data->push_back('\0');
    ASSERT_EQ(loadUploadRequestHeaderFile.deserialize(data),
              SerializableOperationResult::SERIALIZABLE_OK);
    std::string headerFileName = "";
    uint8_t headerFileNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileName(headerFileName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileNameLength(headerFileNameLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE1.TEST");
    ASSERT_EQ(headerFileNameLength, 16);
    std::string loadPartNumberName = "";
    uint8_t loadPartNumberNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberName(loadPartNumberName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberNameLength(loadPartNumberNameLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER1");
    ASSERT_EQ(loadPartNumberNameLength, 18);
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileSerializeJSON)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    ASSERT_EQ(loadUploadRequestHeaderFile.setHeaderFileName("TEST_FILE1.TEST"),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.setLoadPartNumberName("TEST_PART_NUMBER1"),
              FileOperationResult::FILE_OPERATION_OK);
    std::string data("");
    ASSERT_EQ(loadUploadRequestHeaderFile.serializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data, "{"
                    "\"headerFileNameLength\":16,"
                    "\"headerFileName\":\"TEST_FILE1.TEST\","
                    "\"loadPartNumberNameLength\":18,"
                    "\"loadPartNumberName\":\"TEST_PART_NUMBER1\""
                    "}");
}

TEST(ARINC615AFilesTest, LoadUploadRequestHeaderFileDeserializeJSON)
{
    LoadUploadRequestHeaderFileARINC615A loadUploadRequestHeaderFile;
    std::string data("{"
                     "\"headerFileNameLength\":16,"
                     "\"headerFileName\":\"TEST_FILE1.TEST\","
                     "\"loadPartNumberNameLength\":18,"
                     "\"loadPartNumberName\":\"TEST_PART_NUMBER1\""
                     "}");
    ASSERT_EQ(loadUploadRequestHeaderFile.deserializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);
    std::string headerFileName = "";
    uint8_t headerFileNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileName(headerFileName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getHeaderFileNameLength(headerFileNameLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE1.TEST");
    ASSERT_EQ(headerFileNameLength, 16);
    std::string loadPartNumberName = "";
    uint8_t loadPartNumberNameLength = 0;
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberName(loadPartNumberName),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadRequestHeaderFile.getLoadPartNumberNameLength(loadPartNumberNameLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER1");
    ASSERT_EQ(loadPartNumberNameLength, 18);
}