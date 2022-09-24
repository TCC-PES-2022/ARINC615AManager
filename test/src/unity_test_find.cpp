#include <gtest/gtest.h>

#include "FindARINC615A.h"
#include <fstream>
#include <mutex>
#include <condition_variable>

#define WAIT_DELAY 3 // seconds
#define FINDSTUB_FILE "findstub.json"
#define FIND_STUB_CONTENT   "{\n"                                                   \
                            "  \"devices\": [\n"                                    \
                            "    {\n"                                               \
                            "      \"mac\": \"9A-DA-C1-D7-51-D7\",\n"               \
                            "      \"ip\": \"127.0.0.1\",\n"                        \
                            "      \"hardware\": {\n"                               \
                            "           \"targetHardwareIdentifier\": \"HNPFMS\",\n"\
                            "           \"targetTypeName\": \"FMS\",\n"             \
                            "           \"targetPosition\": \"L\",\n"               \
                            "           \"literalName\": \"FMS LEFT\",\n"           \
                            "           \"manufacturerCode\": \"HNP\"\n"            \
                            "       }\n"                                            \
                            "    }\n"                                               \
                            "  ]\n"                                                 \
                            "}"

typedef struct {
    bool findStartedCalled;
    std::string deviceInfo;
    bool findFinishedCalled;
    std::mutex contextMutex;
    std::condition_variable contextCV;
} FindARINC615AContext;

FindOperationResult findStartedStopped_startedCallback(std::shared_ptr<void> context) {
    if (context != nullptr) {
        std::shared_ptr<FindARINC615AContext> findARINC615AContext =
                std::static_pointer_cast<FindARINC615AContext>(context);
        findARINC615AContext->findStartedCalled = true;
    }
    return FindOperationResult::FIND_OPERATION_OK;
}

FindOperationResult findStartedStopped_finishedCallback(std::shared_ptr<void> context) {
    if (context != nullptr) {
        std::shared_ptr<FindARINC615AContext> findARINC615AContext =
                std::static_pointer_cast<FindARINC615AContext>(context);
        findARINC615AContext->findFinishedCalled = true;
        findARINC615AContext->contextCV.notify_one();
    }
    return FindOperationResult::FIND_OPERATION_OK;
}

TEST(ARINC615AFindTest, FindStartedStopped)
{
    FindARINC615A findObj;
//    FindARINC615AContext context;
//    std::shared_ptr<FindARINC615AContext> findARINC615AContext =
//            std::make_shared<FindARINC615AContext>(std::move(context));
    std::shared_ptr<FindARINC615AContext> context = std::make_shared<FindARINC615AContext>();
    context->findStartedCalled = false;
    context->findFinishedCalled = false;

    findObj.registerFindStartedCallback(
            findStartedStopped_startedCallback, context);
    findObj.registerFindFinishedCallback(
            findStartedStopped_finishedCallback, context);
    findObj.find();

    {
        std::unique_lock<std::mutex> lk(context->contextMutex);
        while (!context->findFinishedCalled) {
            context->contextCV.wait_for(lk, std::chrono::seconds(WAIT_DELAY));
        }
    }

    ASSERT_TRUE(context->findStartedCalled);
    ASSERT_TRUE(context->findFinishedCalled);
}

FindOperationResult newDeviceInfo_newDeviceCallback(std::string deviceInfo, std::shared_ptr<void> context) {
    if (context != nullptr) {
        std::shared_ptr<FindARINC615AContext> findARINC615AContext =
                std::static_pointer_cast<FindARINC615AContext>(context);
        findARINC615AContext->deviceInfo = deviceInfo;
    }
    return FindOperationResult::FIND_OPERATION_OK;
}

void createFindStub() {
    std::ofstream findstub(FINDSTUB_FILE);
    if (findstub.is_open()) {
        findstub << FIND_STUB_CONTENT;
        findstub.close();
    }
}

TEST(ARINC615AFindTest, NewDeviceInfo)
{
    createFindStub();
    FindARINC615A findObj;
    std::shared_ptr<FindARINC615AContext> context = std::make_shared<FindARINC615AContext>();
    context->findStartedCalled = false;
    context->findFinishedCalled = false;

    findObj.registerFindFinishedCallback(
            findStartedStopped_finishedCallback, context);
    findObj.registerFindNewDeviceCallback(
            newDeviceInfo_newDeviceCallback, context);

    findObj.find();

    {
        std::unique_lock<std::mutex> lk(context->contextMutex);
        while (!context->findFinishedCalled) {
            context->contextCV.wait_for(lk, std::chrono::seconds(WAIT_DELAY));
        }
    }

    ASSERT_STREQ(context->deviceInfo.c_str(), FIND_STUB_CONTENT);
}