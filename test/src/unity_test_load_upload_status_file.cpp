#include <gtest/gtest.h>

#include "LoadUploadStatusFileARINC615A.h"

TEST(ARINC615AFilesTest, LoadUploadStatusFileAddHeaderFile)
{
    LoadUploadStatusFileARINC615A loadUploadStatusFile("TEST_FILE.TEST", "A4");

    LoadUploadStatusHeaderFileARINC615A headerFile1;
    ASSERT_EQ(headerFile1.setHeaderFileName("TEST_FILE1.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadRatio(42),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatus(0x4242),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatusDescription("TEST_STATUS_DESCRIPTION1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile1),
              FileOperationResult::FILE_OPERATION_OK);

    LoadUploadStatusHeaderFileARINC615A headerFile2;
    ASSERT_EQ(headerFile2.setHeaderFileName("TEST_FILE2.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadRatio(43),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatus(0x4343),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatusDescription("TEST_STATUS_DESCRIPTION2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile2),
              FileOperationResult::FILE_OPERATION_OK);

    std::shared_ptr<std::vector<LoadUploadStatusHeaderFileARINC615A>> headerFiles;
    ASSERT_EQ(loadUploadStatusFile.getHeaderFiles(headerFiles),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFiles->size(), 2);

    std::string headerFileName1 = "";
    ASSERT_EQ(headerFiles->at(0).getHeaderFileName(headerFileName1),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName1, "TEST_FILE1.TEST");

    std::string loadPartNumberName1 = "";
    ASSERT_EQ(headerFiles->at(0).getLoadPartNumberName(loadPartNumberName1),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName1, "TEST_PART_NUMBER1");

    uint32_t loadRatio1 = 0;
    ASSERT_EQ(headerFiles->at(0).getLoadRatio(loadRatio1),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadRatio1, 42);

    uint16_t loadStatus1 = 0;
    ASSERT_EQ(headerFiles->at(0).getLoadStatus(loadStatus1),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatus1, 0x4242);

    std::string loadStatusDescription1 = "";
    ASSERT_EQ(headerFiles->at(0).getLoadStatusDescription(loadStatusDescription1),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatusDescription1, "TEST_STATUS_DESCRIPTION1");

    std::string headerFileName2 = "";
    ASSERT_EQ(headerFiles->at(1).getHeaderFileName(headerFileName2),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName2, "TEST_FILE2.TEST");

    std::string loadPartNumberName2 = "";
    ASSERT_EQ(headerFiles->at(1).getLoadPartNumberName(loadPartNumberName2),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName2, "TEST_PART_NUMBER2");

    uint32_t loadRatio2 = 0;
    ASSERT_EQ(headerFiles->at(1).getLoadRatio(loadRatio2),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadRatio2, 43);

    uint16_t loadStatus2 = 0;
    ASSERT_EQ(headerFiles->at(1).getLoadStatus(loadStatus2),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatus2, 0x4343);

    std::string loadStatusDescription2 = "";
    ASSERT_EQ(headerFiles->at(1).getLoadStatusDescription(loadStatusDescription2),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatusDescription2, "TEST_STATUS_DESCRIPTION2");

}

TEST(ARINC615AFilesTest, LoadUploadStatusFileSerialization)
{
    LoadUploadStatusFileARINC615A loadUploadStatusFile("TEST_FILE.TEST", "A4");
    loadUploadStatusFile.setUploadOperationStatusCode(0x4141);
    loadUploadStatusFile.setUploadStatusDescription("TEST_STATUS_DESCRIPTION");
    loadUploadStatusFile.setCounter(72);
    loadUploadStatusFile.setExceptionTimer(73);
    loadUploadStatusFile.setEstimatedTime(74);
    loadUploadStatusFile.setLoadListRatio(75);

    LoadUploadStatusHeaderFileARINC615A headerFile1;
    ASSERT_EQ(headerFile1.setHeaderFileName("TEST_FILE1.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadRatio(42),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatus(0x4242),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatusDescription("TEST_STATUS_DESCRIPTION1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile1),
            FileOperationResult::FILE_OPERATION_OK);

    LoadUploadStatusHeaderFileARINC615A headerFile2;
    ASSERT_EQ(headerFile2.setHeaderFileName("TEST_FILE2.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadRatio(43),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatus(0x4243),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatusDescription("TEST_STATUS_DESCRIPTION2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile2),
        FileOperationResult::FILE_OPERATION_OK);

    LoadUploadStatusHeaderFileARINC615A headerFile3;
    ASSERT_EQ(headerFile3.setHeaderFileName("TEST_FILE3.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadPartNumberName("TEST_PART_NUMBER3"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadRatio(44),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadStatus(0x4244),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadStatusDescription("TEST_STATUS_DESCRIPTION3"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile3),
        FileOperationResult::FILE_OPERATION_OK);

    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    ASSERT_EQ(loadUploadStatusFile.serialize(data),
              SerializableOperationResult::SERIALIZABLE_OK);
    ASSERT_EQ(data->size(), 245);
    ASSERT_EQ(data->at(0), 0);
    ASSERT_EQ(data->at(1), 0);
    ASSERT_EQ(data->at(2), 0);
    ASSERT_EQ(data->at(3), 245);
    ASSERT_EQ(data->at(4), 'A');
    ASSERT_EQ(data->at(5), '4');
    ASSERT_EQ(data->at(6), 0x41);
    ASSERT_EQ(data->at(7), 0x41);
    ASSERT_EQ(data->at(8), 24);
    ASSERT_EQ(data->at(9), 'T');
    ASSERT_EQ(data->at(10), 'E');
    ASSERT_EQ(data->at(11), 'S');
    ASSERT_EQ(data->at(12), 'T');
    ASSERT_EQ(data->at(13), '_');
    ASSERT_EQ(data->at(14), 'S');
    ASSERT_EQ(data->at(15), 'T');
    ASSERT_EQ(data->at(16), 'A');
    ASSERT_EQ(data->at(17), 'T');
    ASSERT_EQ(data->at(18), 'U');
    ASSERT_EQ(data->at(19), 'S');
    ASSERT_EQ(data->at(20), '_');
    ASSERT_EQ(data->at(21), 'D');
    ASSERT_EQ(data->at(22), 'E');
    ASSERT_EQ(data->at(23), 'S');
    ASSERT_EQ(data->at(24), 'C');
    ASSERT_EQ(data->at(25), 'R');
    ASSERT_EQ(data->at(26), 'I');
    ASSERT_EQ(data->at(27), 'P');
    ASSERT_EQ(data->at(28), 'T');
    ASSERT_EQ(data->at(29), 'I');
    ASSERT_EQ(data->at(30), 'O');
    ASSERT_EQ(data->at(31), 'N');
    ASSERT_EQ(data->at(32), '\0');
    ASSERT_EQ(data->at(33), 0);
    ASSERT_EQ(data->at(34), 72);
    ASSERT_EQ(data->at(35), 0);
    ASSERT_EQ(data->at(36), 73);
    ASSERT_EQ(data->at(37), 0);
    ASSERT_EQ(data->at(38), 74);
    ASSERT_EQ(data->at(39), 0);
    ASSERT_EQ(data->at(40), 0);
    ASSERT_EQ(data->at(41), 75);
    ASSERT_EQ(data->at(42), 0);
    ASSERT_EQ(data->at(43), 3);

    for (int i = 0; i < 3; ++i)
    {
        size_t index = 44 + i * 67;
        ASSERT_EQ(data->at(index), 16);
        ASSERT_EQ(data->at(index+1), 'T');
        ASSERT_EQ(data->at(index+2), 'E');
        ASSERT_EQ(data->at(index+3), 'S');
        ASSERT_EQ(data->at(index+4), 'T');
        ASSERT_EQ(data->at(index+5), '_');
        ASSERT_EQ(data->at(index+6), 'F');
        ASSERT_EQ(data->at(index+7), 'I');
        ASSERT_EQ(data->at(index+8), 'L');
        ASSERT_EQ(data->at(index+9), 'E');
        ASSERT_EQ(data->at(index+10), std::to_string(i + 1).c_str()[0]);
        ASSERT_EQ(data->at(index+11), '.');
        ASSERT_EQ(data->at(index+12), 'T');
        ASSERT_EQ(data->at(index+13), 'E');
        ASSERT_EQ(data->at(index+14), 'S');
        ASSERT_EQ(data->at(index+15), 'T');
        ASSERT_EQ(data->at(index+16), '\0');
        ASSERT_EQ(data->at(index+17), 18);
        ASSERT_EQ(data->at(index+18), 'T');
        ASSERT_EQ(data->at(index+19), 'E');
        ASSERT_EQ(data->at(index+20), 'S');
        ASSERT_EQ(data->at(index+21), 'T');
        ASSERT_EQ(data->at(index+22), '_');
        ASSERT_EQ(data->at(index+23), 'P');
        ASSERT_EQ(data->at(index+24), 'A');
        ASSERT_EQ(data->at(index+25), 'R');
        ASSERT_EQ(data->at(index+26), 'T');
        ASSERT_EQ(data->at(index+27), '_');
        ASSERT_EQ(data->at(index+28), 'N');
        ASSERT_EQ(data->at(index+29), 'U');
        ASSERT_EQ(data->at(index+30), 'M');
        ASSERT_EQ(data->at(index+31), 'B');
        ASSERT_EQ(data->at(index+32), 'E');
        ASSERT_EQ(data->at(index+33), 'R');
        ASSERT_EQ(data->at(index+34), std::to_string(i + 1).c_str()[0]);
        ASSERT_EQ(data->at(index+35), '\0');
        ASSERT_EQ(data->at(index+36), 00);
        ASSERT_EQ(data->at(index+37), 00);
        ASSERT_EQ(data->at(index+38), 42 + i);
        ASSERT_EQ(data->at(index+39), 0x42);
        ASSERT_EQ(data->at(index+40), 0x42 + i);
        ASSERT_EQ(data->at(index+41), 25);
        ASSERT_EQ(data->at(index+42), 'T');
        ASSERT_EQ(data->at(index+43), 'E');
        ASSERT_EQ(data->at(index+44), 'S');
        ASSERT_EQ(data->at(index+45), 'T');
        ASSERT_EQ(data->at(index+46), '_');
        ASSERT_EQ(data->at(index+47), 'S');
        ASSERT_EQ(data->at(index+48), 'T');
        ASSERT_EQ(data->at(index+49), 'A');
        ASSERT_EQ(data->at(index+50), 'T');
        ASSERT_EQ(data->at(index+51), 'U');
        ASSERT_EQ(data->at(index+52), 'S');
        ASSERT_EQ(data->at(index+53), '_');
        ASSERT_EQ(data->at(index+54), 'D');
        ASSERT_EQ(data->at(index+55), 'E');
        ASSERT_EQ(data->at(index+56), 'S');
        ASSERT_EQ(data->at(index+57), 'C');
        ASSERT_EQ(data->at(index+58), 'R');
        ASSERT_EQ(data->at(index+59), 'I');
        ASSERT_EQ(data->at(index+60), 'P');
        ASSERT_EQ(data->at(index+61), 'T');
        ASSERT_EQ(data->at(index+62), 'I');
        ASSERT_EQ(data->at(index+63), 'O');
        ASSERT_EQ(data->at(index+64), 'N');
        ASSERT_EQ(data->at(index+65), std::to_string(i + 1).c_str()[0]);
        ASSERT_EQ(data->at(index+66), '\0');
    }
}

TEST(ARINC615AFilesTest, LoadUploadStatusFileDeserialization)
{
    std::shared_ptr<std::vector<uint8_t>> data = std::make_shared<std::vector<uint8_t>>();
    data->push_back(0);
    data->push_back(0);
    data->push_back(0);
    data->push_back(245);
    data->push_back('A');
    data->push_back('4');
    data->push_back(0x41);
    data->push_back(0x41);
    data->push_back(24);
    data->push_back('T');
    data->push_back('E');
    data->push_back('S');
    data->push_back('T');
    data->push_back('_');
    data->push_back('S');
    data->push_back('T');
    data->push_back('A');
    data->push_back('T');
    data->push_back('U');
    data->push_back('S');
    data->push_back('_');
    data->push_back('D');
    data->push_back('E');
    data->push_back('S');
    data->push_back('C');
    data->push_back('R');
    data->push_back('I');
    data->push_back('P');
    data->push_back('T');
    data->push_back('I');
    data->push_back('O');
    data->push_back('N');
    data->push_back('\0');
    data->push_back(0);
    data->push_back(72);
    data->push_back(0);
    data->push_back(73);
    data->push_back(0);
    data->push_back(74);
    data->push_back(0);
    data->push_back(0);
    data->push_back(75);
    data->push_back(0);
    data->push_back(3);

    for (int i = 0; i < 3; ++i)
    {
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
        data->push_back(00);
        data->push_back(00);
        data->push_back(42 + i);
        data->push_back(0x42);
        data->push_back(0x42 + i);
        data->push_back(25);
        data->push_back('T');
        data->push_back('E');
        data->push_back('S');
        data->push_back('T');
        data->push_back('_');
        data->push_back('S');
        data->push_back('T');
        data->push_back('A');
        data->push_back('T');
        data->push_back('U');
        data->push_back('S');
        data->push_back('_');
        data->push_back('D');
        data->push_back('E');
        data->push_back('S');
        data->push_back('C');
        data->push_back('R');
        data->push_back('I');
        data->push_back('P');
        data->push_back('T');
        data->push_back('I');
        data->push_back('O');
        data->push_back('N');
        data->push_back(std::to_string(i + 1).c_str()[0]);
        data->push_back('\0');
    }

    LoadUploadStatusFileARINC615A loadUploadStatusFile;
    ASSERT_EQ(loadUploadStatusFile.deserialize(data),
              SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength = 0;
    ASSERT_EQ(loadUploadStatusFile.getFileLength(fileLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileLength, 245);

    std::string protocolVersion;
    ASSERT_EQ(loadUploadStatusFile.getProtocolVersion(protocolVersion),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(protocolVersion, "A4");

    uint16_t uploadOperationStatusCode = 0;
    ASSERT_EQ(loadUploadStatusFile.getUploadOperationStatusCode(uploadOperationStatusCode),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadOperationStatusCode, 0x4141);

    uint8_t uploadStatusDescriptionLength = 0;
    ASSERT_EQ(loadUploadStatusFile.getUploadStatusDescriptionLength(uploadStatusDescriptionLength),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadStatusDescriptionLength, 24);

    std::string uploadStatusDescription;
    ASSERT_EQ(loadUploadStatusFile.getUploadStatusDescription(uploadStatusDescription),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadStatusDescription, "TEST_STATUS_DESCRIPTION");

    uint16_t counter = 0;
    ASSERT_EQ(loadUploadStatusFile.getCounter(counter),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(counter, 72);

    uint16_t exceptionTimer = 0;
    ASSERT_EQ(loadUploadStatusFile.getExceptionTimer(exceptionTimer),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(exceptionTimer, 73);

    uint16_t estimatedTime = 0;
    ASSERT_EQ(loadUploadStatusFile.getEstimatedTime(estimatedTime),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(estimatedTime, 74);

    uint32_t loadListRatio = 0;
    ASSERT_EQ(loadUploadStatusFile.getLoadListRatio(loadListRatio),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadListRatio, 75);

    std::shared_ptr <std::vector<LoadUploadStatusHeaderFileARINC615A>> headerFiles;
    ASSERT_EQ(loadUploadStatusFile.getHeaderFiles(headerFiles),
              FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFiles->size(), 3);

    for (int i = 0; i < 3; ++i)
    {
        uint8_t headerFileNameLength = 0;
        ASSERT_EQ(headerFiles->at(i).getHeaderFileNameLength(headerFileNameLength),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(headerFileNameLength, 16);

        std::string headerFileName;
        ASSERT_EQ(headerFiles->at(i).getHeaderFileName(headerFileName),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(headerFileName, "TEST_FILE" + std::to_string(i + 1) + ".TEST");

        uint8_t loadPartNumberNameLength = 0;
        ASSERT_EQ(headerFiles->at(i).getLoadPartNumberNameLength(loadPartNumberNameLength),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadPartNumberNameLength, 18);

        std::string loadPartNumberName;
        ASSERT_EQ(headerFiles->at(i).getLoadPartNumberName(loadPartNumberName),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER" + std::to_string(i + 1));

        uint32_t loadRatio = 0;
        ASSERT_EQ(headerFiles->at(i).getLoadRatio(loadRatio),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadRatio, 42 + i);

        uint16_t loadStatus = 0;
        ASSERT_EQ(headerFiles->at(i).getLoadStatus(loadStatus),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadStatus, 0x4242 + i);

        uint8_t loadStatusDescriptionLength = 0;
        ASSERT_EQ(headerFiles->at(i).getLoadStatusDescriptionLength(loadStatusDescriptionLength),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadStatusDescriptionLength, 25);

        std::string loadStatusDescription;
        ASSERT_EQ(headerFiles->at(i).getLoadStatusDescription(loadStatusDescription),
                  FileOperationResult::FILE_OPERATION_OK);
        ASSERT_EQ(loadStatusDescription, "TEST_STATUS_DESCRIPTION" + std::to_string(i + 1));
    }
}

TEST(ARINC615AFilesTest, LoadUploadStatusFileSerializationJSON)
{
    LoadUploadStatusFileARINC615A loadUploadStatusFile("TEST_FILE.TEST", "A4");
    loadUploadStatusFile.setUploadOperationStatusCode(0x4141);
    loadUploadStatusFile.setUploadStatusDescription("TEST_STATUS_DESCRIPTION");
    loadUploadStatusFile.setCounter(72);
    loadUploadStatusFile.setExceptionTimer(73);
    loadUploadStatusFile.setEstimatedTime(74);
    loadUploadStatusFile.setLoadListRatio(75);

    LoadUploadStatusHeaderFileARINC615A headerFile1;
    ASSERT_EQ(headerFile1.setHeaderFileName("TEST_FILE1.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadPartNumberName("TEST_PART_NUMBER1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadRatio(42),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatus(0x4242),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile1.setLoadStatusDescription("TEST_STATUS_DESCRIPTION1"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile1),
        FileOperationResult::FILE_OPERATION_OK);

    LoadUploadStatusHeaderFileARINC615A headerFile2;
    ASSERT_EQ(headerFile2.setHeaderFileName("TEST_FILE2.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadPartNumberName("TEST_PART_NUMBER2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadRatio(43),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatus(0x4243),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile2.setLoadStatusDescription("TEST_STATUS_DESCRIPTION2"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile2),
        FileOperationResult::FILE_OPERATION_OK);

    LoadUploadStatusHeaderFileARINC615A headerFile3;
    ASSERT_EQ(headerFile3.setHeaderFileName("TEST_FILE3.TEST"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadPartNumberName("TEST_PART_NUMBER3"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadRatio(44),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadStatus(0x4244),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFile3.setLoadStatusDescription("TEST_STATUS_DESCRIPTION3"),
        FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadUploadStatusFile.addHeaderFile(headerFile3),
        FileOperationResult::FILE_OPERATION_OK);

    std::string data;
    ASSERT_EQ(loadUploadStatusFile.serializeJSON(data),
            SerializableOperationResult::SERIALIZABLE_OK);

    ASSERT_EQ(data, "{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":245,"
                    "\"protocolVersion\":\"A4\","
                    "\"uploadOperationStatusCode\":16705,"
                    "\"uploadStatusDescriptionLength\":24,"
                    "\"uploadStatusDescription\":\"TEST_STATUS_DESCRIPTION\","
                    "\"counter\":72,"
                    "\"exceptionTimer\":73,"
                    "\"estimatedTime\":74,"
                    "\"loadListRatio\":75,"
                    "\"numberOfHeaderFiles\":3,"
                    "\"headerFiles\":["
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE1.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER1\","
                        "\"loadRatio\":42,"
                        "\"loadStatus\":16962,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION1\""
                    "},"
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE2.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER2\","
                        "\"loadRatio\":43,"
                        "\"loadStatus\":16963,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION2\""
                    "},"
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE3.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER3\","
                        "\"loadRatio\":44,"
                        "\"loadStatus\":16964,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION3\""
                    "}]}");
}

TEST(ARINC615AFilesTest, LoadUploadStatusFileDeserializationJSON)
{
    std::string data = "{"
                    "\"fileName\":\"TEST_FILE.TEST\","
                    "\"fileLength\":245,"
                    "\"protocolVersion\":\"A4\","
                    "\"uploadOperationStatusCode\":16705,"
                    "\"uploadStatusDescriptionLength\":24,"
                    "\"uploadStatusDescription\":\"TEST_STATUS_DESCRIPTION\","
                    "\"counter\":72,"
                    "\"exceptionTimer\":73,"
                    "\"estimatedTime\":74,"
                    "\"loadListRatio\":75,"
                    "\"numberOfHeaderFiles\":3,"
                    "\"headerFiles\":["
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE1.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER1\","
                        "\"loadRatio\":42,"
                        "\"loadStatus\":16962,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION1\""
                    "},"
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE2.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER2\","
                        "\"loadRatio\":43,"
                        "\"loadStatus\":16963,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION2\""
                    "},"
                    "{"
                        "\"headerFileNameLength\":16,"
                        "\"headerFileName\":\"TEST_FILE3.TEST\","
                        "\"loadPartNumberNameLength\":18,"
                        "\"loadPartNumberName\":\"TEST_PART_NUMBER3\","
                        "\"loadRatio\":44,"
                        "\"loadStatus\":16964,"
                        "\"loadStatusDescriptionLength\":25,"
                        "\"loadStatusDescription\":\"TEST_STATUS_DESCRIPTION3\""
                    "}]}";

    LoadUploadStatusFileARINC615A loadUploadStatusFile;
    ASSERT_EQ(loadUploadStatusFile.deserializeJSON(data),
            SerializableOperationResult::SERIALIZABLE_OK);

    uint32_t fileLength = 0;
    ASSERT_EQ(loadUploadStatusFile.getFileLength(fileLength),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(fileLength, 245);

    std::string protocolVersion;
    ASSERT_EQ(loadUploadStatusFile.getProtocolVersion(protocolVersion),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(protocolVersion, "A4");

    uint16_t uploadOperationStatusCode = 0;
    ASSERT_EQ(loadUploadStatusFile.getUploadOperationStatusCode(uploadOperationStatusCode),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadOperationStatusCode, 0x4141);

    uint8_t uploadStatusDescriptionLength = 0;
    ASSERT_EQ(loadUploadStatusFile.getUploadStatusDescriptionLength(uploadStatusDescriptionLength),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadStatusDescriptionLength, 24);

    std::string uploadStatusDescription;
    ASSERT_EQ(loadUploadStatusFile.getUploadStatusDescription(uploadStatusDescription),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(uploadStatusDescription, "TEST_STATUS_DESCRIPTION");

    uint16_t counter = 0;
    ASSERT_EQ(loadUploadStatusFile.getCounter(counter),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(counter, 72);

    uint16_t exceptionTimer = 0;
    ASSERT_EQ(loadUploadStatusFile.getExceptionTimer(exceptionTimer),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(exceptionTimer, 73);

    uint16_t estimatedTime = 0;
    ASSERT_EQ(loadUploadStatusFile.getEstimatedTime(estimatedTime),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(estimatedTime, 74);

    uint32_t loadListRatio = 0;
    ASSERT_EQ(loadUploadStatusFile.getLoadListRatio(loadListRatio),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadListRatio, 75);

    std::shared_ptr <std::vector<LoadUploadStatusHeaderFileARINC615A>> headerFiles;
    ASSERT_EQ(loadUploadStatusFile.getHeaderFiles(headerFiles),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFiles->size(), 3);

    for (int i = 0; i < 3; ++i)
    {
    uint8_t headerFileNameLength = 0;
    ASSERT_EQ(headerFiles->at(i).getHeaderFileNameLength(headerFileNameLength),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileNameLength, 16);

    std::string headerFileName;
    ASSERT_EQ(headerFiles->at(i).getHeaderFileName(headerFileName),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(headerFileName, "TEST_FILE" + std::to_string(i + 1) + ".TEST");

    uint8_t loadPartNumberNameLength = 0;
    ASSERT_EQ(headerFiles->at(i).getLoadPartNumberNameLength(loadPartNumberNameLength),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberNameLength, 18);

    std::string loadPartNumberName;
    ASSERT_EQ(headerFiles->at(i).getLoadPartNumberName(loadPartNumberName),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadPartNumberName, "TEST_PART_NUMBER" + std::to_string(i + 1));

    uint32_t loadRatio = 0;
    ASSERT_EQ(headerFiles->at(i).getLoadRatio(loadRatio),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadRatio, 42 + i);

    uint16_t loadStatus = 0;
    ASSERT_EQ(headerFiles->at(i).getLoadStatus(loadStatus),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatus, 0x4242 + i);

    uint8_t loadStatusDescriptionLength = 0;
    ASSERT_EQ(headerFiles->at(i).getLoadStatusDescriptionLength(loadStatusDescriptionLength),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatusDescriptionLength, 25);

    std::string loadStatusDescription;
    ASSERT_EQ(headerFiles->at(i).getLoadStatusDescription(loadStatusDescription),
            FileOperationResult::FILE_OPERATION_OK);
    ASSERT_EQ(loadStatusDescription, "TEST_STATUS_DESCRIPTION" + std::to_string(i + 1));
    }
}