//
// Created by kollins on 11/09/2022.
//

#ifndef FINDARINC615A_H
#define FINDARINC615A_H

#include <string>
#include <memory>
#include <vector>

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - FIND_OPERATION_OK:                    Operation was successful.
 * - FIND_OPERATION_ERROR:                 Generic error.
 */
enum class FindOperationResult {
    FIND_OPERATION_OK = 0,
    FIND_OPERATION_ERROR
};

/**
 * @brief Callback for find operation started. This callback is called right
 * before the find operation is started.
 *
 * @param[in] context the user context.
 *
 * @return FIND_OPERATION_OK if success.
 * @return FIND_OPERATION_ERROR otherwise.
 */
typedef FindOperationResult (*findStarted) (
        std::shared_ptr<void>
);

/**
 * @brief Callback for find operation finished
 *
 * @param[in] context the user context.
 *
 * @return FIND_OPERATION_OK if success.
 * @return FIND_OPERATION_ERROR otherwise.
 */
typedef FindOperationResult (*findFinished) (
        std::shared_ptr<void>
);

/**
 * @brief Callback for new devices found. The device information is
 * returned in a JSON format.
 *
 * TODO: document the JSON format.
 *
 * @param[in] device JSON with device information.
 * @param[in] context the user context.
 *
 * @return FIND_OPERATION_OK if success.
 * @return FIND_OPERATION_ERROR otherwise.
 */
typedef FindOperationResult (*findNewDevice) (
        std::string device,
        std::shared_ptr<void>
);

class FindARINC615A {
public:
    FindARINC615A();
    ~FindARINC615A();

    /**
     * Register a callback for find operation started.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return FIND_OPERATION_OK if success.
     * @return FIND_OPERATION_ERROR otherwise.
     */
    FindOperationResult registerFindStartedCallback(
        findStarted callback, std::shared_ptr<void> context);

    /**
     * Register a callback for find operation finished.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return FIND_OPERATION_OK if success.
     * @return FIND_OPERATION_ERROR otherwise.
     */
    FindOperationResult registerFindFinishedCallback(
        findFinished callback, std::shared_ptr<void> context);

    /**
     * Register a callback for new devices found.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return FIND_OPERATION_OK if success.
     * @return FIND_OPERATION_ERROR otherwise.
     */
    FindOperationResult registerFindNewDeviceCallback(
        findNewDevice callback, std::shared_ptr<void> context);
    /**
    * Start find operation. This is a non-blocking function.
    * Devices found will be notified through the callback.
    *
    * @return FIND_OPERATION_OK if success.
    * @return FIND_OPERATION_ERROR otherwise.
    */
    FindOperationResult find();

private:
    findStarted _findStartedCallback;
    std::shared_ptr<void> _findStartedContext;

    findFinished _findFinishedCallback;
    std::shared_ptr<void> _findFinishedContext;

    findNewDevice _findNewDeviceCallback;
    std::shared_ptr<void> _findNewDeviceContext;
};


#endif //FINDARINC615A_H
