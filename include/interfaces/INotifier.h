#ifndef INOTIFIER_H
#define INOTIFIER_H

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - NOTIFIER_OK:                    Operation was successful.
 * - NOTIFIER_ERROR:                 Generic error.
 */
enum class NotifierOperationResult
{
    NOTIFIER_OK = 0,
    NOTIFIER_ERROR
};

/**
 * @brief Enum with possible TFTP notification events.
 * Possible events are:
 * - NOTIFIER_EVENT_TFTP_SECTION_CLOSED:    TFTP section closed.
 * - NOTIFIER_EVENT_UNKNOWN:                Unknown event.
 */
enum class NotifierEventType
{
    NOTIFIER_EVENT_TFTP_SECTION_CLOSED,
    NOTIFIER_EVENT_UNKNOWN
};

/**
 * @brief Interface for classes that need to be notified when a
 *        TFTP section has changed.
 */
class INotifier
{
public:
    /**
     * @brief Notify class that a TFTP section has changed.
     *
     * @param[in] event type of TFTP event.
     *
     * @return NOTIFIER_OK if success.
     * @return NOTIFIER_ERROR otherwise.
     */
    virtual NotifierOperationResult notify(NotifierEventType event) = 0;
};

#endif // INOTIFIER_H