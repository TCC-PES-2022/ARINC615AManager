#include <gtest/gtest.h>

#include "LoadUploadRequestFileARINC615A.h"
#include "InitializationFileARINC615A.h"

TEST(ARINC615AFilesTest, LoadUploadRequestFileAddHeaderFile)
{
    LoadUploadRequestFileARINC615A loadUploadRequestFile("TEST_FILE.TEST", "A4");
    LoadUploadRequestHeaderFileARINC615A headerFile1;
    headerFile1.setHeaderFileName("TEST_FILE1.TEST");
    headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1");
    ASSERT_EQ(loadUploadRequestFile.addHeaderFile(headerFile1),
              FileOperationResult::FILE_OPERATION_OK);
    LoadUploadRequestHeaderFileARINC615A headerFile2;
    headerFile2.setHeaderFileName("TEST_FILE2.TEST");
    headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2");
    ASSERT_EQ(loadUploadRequestFile.addHeaderFile(headerFile2),
              FileOperationResult::FILE_OPERATION_OK);
    std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
    ASSERT_EQ(loadUploadRequestFile.getHeaderFiles(headerFiles), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFiles->size(), 2);

    std::string headerFileName = "";
    ASSERT_EQ(headerFiles->at(0).getHeaderFileName(headerFileName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE1.TEST");

    std::string loadPartNumberName = "";
    ASSERT_EQ(headerFiles->at(0).getLoadPartNumberName(loadPartNumberName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER1");

    headerFileName = "";
    ASSERT_EQ(headerFiles->at(1).getHeaderFileName(headerFileName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE2.TEST");

    loadPartNumberName = "";
    ASSERT_EQ(headerFiles->at(1).getLoadPartNumberName(loadPartNumberName), FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER2");
}

TEST(ARINC615AFilesTest, LoadUploadRequestFileSerialization)
{
    LoadUploadRequestFileARINC615A loadUploadRequestFile("TEST_FILE.TEST", "A4");
    LoadUploadRequestHeaderFileARINC615A headerFile1;
    headerFile1.setHeaderFileName("TEST_FILE1.TEST");
    headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1");
    ASSERT_EQ(loadUploadRequestFile.addHeaderFile(headerFile1),
              FileOperationResult::FILE_OPERATION_OK);
    LoadUploadRequestHeaderFileARINC615A headerFile2;
    headerFile2.setHeaderFileName("TEST_FILE2.TEST");
    headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2");
    ASSERT_EQ(loadUploadRequestFile.addHeaderFile(headerFile2),
              FileOperationResult::FILE_OPERATION_OK);
    LoadUploadRequestHeaderFileARINC615A headerFile3;
    headerFile3.setHeaderFileName("TEST_FILE3.TEST");
    headerFile3.setLoadPartNumberName("TEST_PART_NUMBER3");
    ASSERT_EQ(loadUploadRequestFile.addHeaderFile(headerFile3),
              FileOperationResult::FILE_OPERATION_OK);

    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(loadUploadRequestFile.serialize(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 116);
    ASSERT_EQ(data->at(0), 0);
    ASSERT_EQ(data->at(1), 0);
    ASSERT_EQ(data->at(2), 0);
    ASSERT_EQ(data->at(3), 116);
    ASSERT_EQ(data->at(4), 'A');
    ASSERT_EQ(data->at(5), '4');
    ASSERT_EQ(data->at(6), 0);
    ASSERT_EQ(data->at(7), 3);
    for (int i = 0; i < 3; ++i)
    {
        size_t index = 8 + i * 36;
        ASSERT_EQ(data->at(index), 16);
        ASSERT_EQ(data->at(index + 1), 'T');
        ASSERT_EQ(data->at(index + 2), 'E');
        ASSERT_EQ(data->at(index + 3), 'S');
        ASSERT_EQ(data->at(index + 4), 'T');
        ASSERT_EQ(data->at(index + 5), '_');
        ASSERT_EQ(data->at(index + 6), 'F');
        ASSERT_EQ(data->at(index + 7), 'I');
        ASSERT_EQ(data->at(index + 8), 'L');
        ASSERT_EQ(data->at(index + 9), 'E');
        ASSERT_EQ(data->at(index + 10), std::to_string(i + 1).c_str()[0]);
        ASSERT_EQ(data->at(index + 11), '.');
        ASSERT_EQ(data->at(index + 12), 'T');
        ASSERT_EQ(data->at(index + 13), 'E');
        ASSERT_EQ(data->at(index + 14), 'S');
        ASSERT_EQ(data->at(index + 15), 'T');
        ASSERT_EQ(data->at(index + 16), '\0');
        ASSERT_EQ(data->at(index + 17), 18);
        ASSERT_EQ(data->at(index + 18), 'T');
        ASSERT_EQ(data->at(index + 19), 'E');
        ASSERT_EQ(data->at(index + 20), 'S');
        ASSERT_EQ(data->at(index + 21), 'T');
        ASSERT_EQ(data->at(index + 22), '_');
        ASSERT_EQ(data->at(index + 23), 'P');
        ASSERT_EQ(data->at(index + 24), 'A');
        ASSERT_EQ(data->at(index + 25), 'R');
        ASSERT_EQ(data->at(index + 26), 'T');
        ASSERT_EQ(data->at(index + 27), '_');
        ASSERT_EQ(data->at(index + 28), 'N');
        ASSERT_EQ(data->at(index + 29), 'U');
        ASSERT_EQ(data->at(index + 30), 'M');
        ASSERT_EQ(data->at(index + 31), 'B');
        ASSERT_EQ(data->at(index + 32), 'E');
        ASSERT_EQ(data->at(index + 33), 'R');
        ASSERT_EQ(data->at(index + 34), std::to_string(i + 1).c_str()[0]);
        ASSERT_EQ(data->at(index + 35), '\0');
    }
}

TEST(ARINC615AFilesTest, LoadUploadRequestFileSerializationOverflow)
{
    LoadUploadRequestFileARINC615A loadUploadRequestFile("TEST_FILE.TEST", "A4");
    std::string overflowStringA(1024, 'A');
    LoadUploadRequestHeaderFileARINC615A headerFile1;
    headerFile1.setHeaderFileName(overflowStringA);
    headerFile1.setLoadPartNumberName(overflowStringA);
    loadUploadRequestFile.addHeaderFile(headerFile1);
    std::string overflowStringB(1024, 'B');
    LoadUploadRequestHeaderFileARINC615A headerFile2;
    headerFile2.setHeaderFileName(overflowStringB);
    headerFile2.setLoadPartNumberName(overflowStringB);
    loadUploadRequestFile.addHeaderFile(headerFile2);
    std::string overflowStringC(1024, 'C');
    LoadUploadRequestHeaderFileARINC615A headerFile3;
    headerFile3.setHeaderFileName(overflowStringC);
    headerFile3.setLoadPartNumberName(overflowStringC);
    loadUploadRequestFile.addHeaderFile(headerFile3);

    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(loadUploadRequestFile.serialize(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 1544);
    ASSERT_EQ(data->at(0), 0);
    ASSERT_EQ(data->at(1), 0);
    ASSERT_EQ(data->at(2), 0x06);
    ASSERT_EQ(data->at(3), 0x08);
    ASSERT_EQ(data->at(4), 'A');
    ASSERT_EQ(data->at(5), '4');
    ASSERT_EQ(data->at(6), 0);
    ASSERT_EQ(data->at(7), 3);
    for (int i = 0; i < 3; ++i)
    {
        char compareChar = 'A' + i;
        size_t baseIndex = 8 + i * 512;
        for (uint16_t j = 0; j < 2; j++)
        {
            ASSERT_EQ(data->at(baseIndex + j*256), 255);
            for (uint16_t k = 1; k < 255; k++)
            {
                ASSERT_EQ(data->at(baseIndex + j*256 + k), compareChar);
            }
            ASSERT_EQ(data->at(baseIndex + j*256 + 255), '\0');
        }
    }
}

TEST(ARINC615AFilesTest, LoadUploadRequestFileDeserialization)
{
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    data->push_back(0);
    data->push_back(0);
    data->push_back(0);
    data->push_back(116);
    data->push_back('A');
    data->push_back('4');
    data->push_back(0);
    data->push_back(3);
    for (int i = 0; i < 3; ++i)
    {
        size_t index = 8 + i * 36;
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
        data->push_back(std::to_string(i + 1).c_str()[0]);
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
        data->push_back(std::to_string(i + 1).c_str()[0]);
        data->push_back('\0');
    }

    LoadUploadRequestFileARINC615A loadUploadRequestFile;
    ASSERT_EQ(loadUploadRequestFile.deserialize(data), SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength;
    loadUploadRequestFile.getFileLength(fileLength);
    ASSERT_EQ(fileLength, 116);

    std::string protocolVersion;
    loadUploadRequestFile.getProtocolVersion(protocolVersion);
    ASSERT_EQ(protocolVersion, "A4");

    std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
    loadUploadRequestFile.getHeaderFiles(headerFiles);
    ASSERT_EQ(headerFiles->size(), 3);

    for (int i = 0; i < headerFiles->size(); ++i)
    {
        uint8_t headerFileNameLength;
        headerFiles->at(i).getHeaderFileNameLength(headerFileNameLength);
        ASSERT_EQ(headerFileNameLength, 16);

        std::string headerFileName;
        headerFiles->at(i).getHeaderFileName(headerFileName);
        ASSERT_EQ(headerFileName, "TEST_FILE" + std::to_string(i + 1) + ".TEST");

        uint8_t loadPartNumberNameLength;
        headerFiles->at(i).getLoadPartNumberNameLength(loadPartNumberNameLength);
        ASSERT_EQ(loadPartNumberNameLength, 18);

        std::string loadPartNumberName;
        headerFiles->at(i).getLoadPartNumberName(loadPartNumberName);
        ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER" + std::to_string(i + 1));
    }
}

TEST(ARINC615AFilesTest, LoadUploadRequestFileSerializationJSON)
{
    LoadUploadRequestFileARINC615A loadUploadRequestFile("TEST_FILE.TEST", "A4");
    LoadUploadRequestHeaderFileARINC615A headerFile1;
    headerFile1.setHeaderFileName("TEST_FILE1.TEST");
    headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1");
    loadUploadRequestFile.addHeaderFile(headerFile1);
    LoadUploadRequestHeaderFileARINC615A headerFile2;
    headerFile2.setHeaderFileName("TEST_FILE2.TEST");
    headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2");
    loadUploadRequestFile.addHeaderFile(headerFile2);
    LoadUploadRequestHeaderFileARINC615A headerFile3;
    headerFile3.setHeaderFileName("TEST_FILE3.TEST");
    headerFile3.setLoadPartNumberName("TEST_PART_NUMBER3");
    loadUploadRequestFile.addHeaderFile(headerFile3);

    std::string data("");
    ASSERT_EQ(loadUploadRequestFile.serializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data, "{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":116,"
                    "\"protocolVersion\":\"A4\","
                    "\"numberOfHeaderFiles\":3,"
                    "\"headerFiles\":["
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE1.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER1\""
                    "},{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE2.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER2\""
                    "},{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE3.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER3\""
                    "}]}");
}

TEST(ARINC615AFilesTest, LoadUploadRequestFileDeserializationJSON)
{
    std::string data("{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":116,"
                    "\"protocolVersion\":\"A4\","
                    "\"numberOfHeaderFiles\":3,"
                    "\"headerFiles\":["
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE1.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER1\""
                    "},{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE2.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER2\""
                    "},{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE3.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER3\""
                    "}]}");

    LoadUploadRequestFileARINC615A loadUploadRequestFile;
    ASSERT_EQ(loadUploadRequestFile.deserializeJSON(data), SerializableOperationResult::SERIALIZABLE_OK);

    std::string fileName;
    loadUploadRequestFile.getFileName(fileName);
    ASSERT_EQ(fileName, "TEST_FILE.TEST");

    uint32_t fileLength;
    loadUploadRequestFile.getFileLength(fileLength);
    ASSERT_EQ(fileLength, 116);

    std::string protocolVersion;
    loadUploadRequestFile.getProtocolVersion(protocolVersion);
    ASSERT_EQ(protocolVersion, "A4");

    std::shared_ptr<std::vector<LoadUploadRequestHeaderFileARINC615A>> headerFiles;
    loadUploadRequestFile.getHeaderFiles(headerFiles);
    ASSERT_EQ(headerFiles->size(), 3);

    for (int i = 0; i < headerFiles->size(); ++i)
    {
        uint8_t headerFileNameLength;
        headerFiles->at(i).getHeaderFileNameLength(headerFileNameLength);
        ASSERT_EQ(headerFileNameLength, 16);

        std::string headerFileName;
        headerFiles->at(i).getHeaderFileName(headerFileName);
        ASSERT_EQ(headerFileName, "TEST_FILE" + std::to_string(i + 1) + ".TEST");

        uint8_t loadPartNumberNameLength;
        headerFiles->at(i).getLoadPartNumberNameLength(loadPartNumberNameLength);
        ASSERT_EQ(loadPartNumberNameLength, 18);

        std::string loadPartNumberName;
        headerFiles->at(i).getLoadPartNumberName(loadPartNumberName);
        ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER" + std::to_string(i + 1));
    }
}