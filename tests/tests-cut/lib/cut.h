#ifndef CUT_CORE_H
#define CUT_CORE_H

/* XXX: make it work on MacOS, broken otherwise due to some intereferences which prevent to define u_XXX types required in sysctl.h
Should not harm elsewhere.
 */

#include <sys/types.h>

#if !defined(CUT) && !defined(DEBUG) && !defined(CUT_MAIN)

# define ASSERT(e) (void)0
# define ASSERT_FILE(f, content) (void)0
# define ASSERT_BINARY_FILE(f, array) (void)0
# define CHECK(e) (void)0
# define CHECK_FILE(f, content) (void)0
# define CHECK_BINARY_FILE(f, array) (void)0
# define INPUT_STRING(str) (void)0
# define INPUT_BYTES(array) (void)0
# define TEST(name) static void unitTest_ ## name()
# define GLOBAL_TEAR_UP() static void cut_GlobalTearUpInstance()
# define GLOBAL_TEAR_DOWN() static void cut_GlobalTearDownInstance()
# define SUBTEST(name) if (0)
# define REPEATED_SUBTEST(name, count) if (0)
# define SUBTEST_NO 0
# define DEBUG_MSG(...) (void)0

#else

# define CUT_PRIVATE static

# if !defined(CUT_TIMEOUT)
#  define CUT_TIMEOUT 3
# endif

# if !defined(CUT_NO_FORK)
#  define CUT_NO_FORK cut_IsDebugger()
# else
#  undef CUT_NO_FORK
#  define CUT_NO_FORK 1
# endif

# if !defined(CUT_NO_COLOR)
#  define CUT_NO_COLOR !cut_IsTerminalOutput()
# else
#  undef CUT_NO_COLOR
#  define CUT_NO_COLOR 1
# endif


# if defined(__linux__)
// 1hc substitution of /builds/pb071/cut/src/linux-define.h
#ifndef CUT_LINUX_DEFINE_H
#define CUT_LINUX_DEFINE_H

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_DESTRUCTOR(name) __attribute__((destructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif

#endif // CUT_LINUX_DEFINE_H
# elif defined(__APPLE__) || defined(__unix)
// 1hc substitution of /builds/pb071/cut/src/unix-define.h
#ifndef CUT_UNIX_DEFINE_H
#define CUT_UNIX_DEFINE_H

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_DESTRUCTOR(name) __attribute__((destructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif


#endif // CUT_UNIX_DEFINE_H
# elif defined(_WIN32)
// 1hc substitution of /builds/pb071/cut/src/windows-define.h
#ifndef CUT_WINDOWS_DEFINE_H
#define CUT_WINDOWS_DEFINE_H

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_DESTRUCTOR(name) __attribute__((destructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#elif defined(_MSC_VER)

# define CUT_NORETURN __declspec(noreturn)
# pragma section(".CRT$XCU",read)
# define CUT_CONSTRUCTOR(name)                                          \
    static void name( void );                                           \
    __declspec(allocate(".CRT$XCU")) void (* name ## _)(void) = name;   \
    static void name( void )


# define CUT_UNUSED(name) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif


#endif // CUT_WINDOWS_DEFINE_H
# else
#  error "unsupported platform"
# endif

# define _POSIX_C_SOURCE 199309L
# define _XOPEN_SOURCE 500
# include <stdio.h>

# define ASSERT(e) do { if (!(e)) {                                             \
        cut_Stop(#e, __FILE__, __LINE__);                                       \
    } } while(0)

# define ASSERT_FILE(f, content) do {                                           \
    if (!cut_File(f, content)) {                                                \
        cut_Stop("content of file is not equal", __FILE__, __LINE__);           \
    } } while(0)

# define ASSERT_BINARY_FILE(f, bytes) do {                                      \
    if (!cut_BinaryFile(f, sizeof(bytes), bytes)) {                             \
        cut_Stop("content of file is not equal", __FILE__, __LINE__);           \
    } } while(0)

# define CHECK(e) do { if (!(e)) {                                              \
        cut_Check(#e, __FILE__, __LINE__);                                      \
    } } while(0)

# define CHECK_FILE(f, content) do {                                            \
    if (!cut_File(f, content)) {                                                \
        cut_Check("content of file is not equal", __FILE__, __LINE__);          \
    } } while(0)

# define CHECK_BINARY_FILE(f, bytes) do {                                            \
    if (!cut_BinaryFile(f, sizeof(bytes), bytes)) {                                                \
        cut_Check("content of file is not equal", __FILE__, __LINE__);          \
    } } while(0)

# define INPUT_FILE(name)                                                       \
        (cut_InputFile(name))

# define INPUT_STRING(str)                                                      \
        (cut_Input(strlen(str), str))

# define INPUT_BYTES(array)                                                     \
        (cut_InputBytes(sizeof(array), array))

# define TEST(name)                                                             \
    void cut_instance_ ## name(int *, int, int);                                \
    CUT_CONSTRUCTOR(cut_Register ## name) {                                     \
        cut_Register(cut_instance_ ## name, #name, __FILE__, __LINE__);         \
    }                                                                           \
    void cut_instance_ ## name(CUT_UNUSED(int *cut_subtest), CUT_UNUSED(int cut_test_id), CUT_UNUSED(int cut_current))

# define GLOBAL_TEAR_UP()                                                       \
    void cut_GlobalTearUpInstance();                                            \
    CUT_CONSTRUCTOR(cut_RegisterTearUp) {                                       \
        cut_RegisterGlobalTearUp(cut_GlobalTearUpInstance);                     \
    }                                                                           \
    void cut_GlobalTearUpInstance()

# define GLOBAL_TEAR_DOWN()                                                     \
    void cut_GlobalTearDownInstance();                                          \
    CUT_CONSTRUCTOR(cut_RegisterTearDown) {                                     \
        cut_RegisterGlobalTearDown(cut_GlobalTearDownInstance);                 \
    }                                                                           \
    void cut_GlobalTearDownInstance()

# define SUBTEST(name)                                                          \
    if (++*cut_subtest == cut_current)                                          \
        cut_Subtest(0, #name);                                                  \
    if (*cut_subtest == cut_current)                                            \
        if (cut_SkipSubtest(cut_test_id, #name)) {                              \
            cut_SendSkipSubtest(cut_current);                                   \
        } else /* subtest block goes here */

# define REPEATED_SUBTEST(name, count)                                          \
    *cut_subtest = (count);                                                     \
    if (cut_current && count)                                                   \
        cut_Subtest(cut_current, #name);                                        \
    if (cut_current && count)

# define SUBTEST_NO cut_current

# define DEBUG_MSG(...) cut_DebugMessage(__FILE__, __LINE__, __VA_ARGS__)

#  include <string.h>

# ifdef __cplusplus
extern "C" {
# endif

typedef void(*cut_Instance)(int *, int, int);
typedef void(*cut_GlobalTear)();
void cut_Register(cut_Instance instance, const char *name, const char *file, size_t line);
void cut_RegisterGlobalTearUp(cut_GlobalTear instance);
void cut_RegisterGlobalTearDown(cut_GlobalTear instance);
int cut_File(FILE *file, const char *content);
int cut_BinaryFile(FILE *file, size_t size, const unsigned char *bytes);
void cut_InputFile(const char *name);
void cut_InputBytes(size_t size, const unsigned char *bytes);
void cut_Input(size_t size, const char *bytes);
CUT_NORETURN void cut_Stop(const char *text, const char *file, size_t line);
void cut_Check(const char *text, const char *file, size_t line);
void cut_Subtest(int number, const char *name);
void cut_SendSkipSubtest(int counter);
int cut_SkipSubtest(int testId, const char *name);
void cut_DebugMessage(const char *file, size_t line, const char *fmt, ...);

# if defined(CUT_MAIN)

#  include <stdlib.h>
#  include <setjmp.h>
#  include <stdarg.h>


struct cut_Info {
    char *message;
    char *file;
    int line;
    struct cut_Info *next;
};

enum cut_MessageType {
    cut_NO_TYPE = 0,
    cut_MESSAGE_SUBTEST,
    cut_MESSAGE_DEBUG,
    cut_MESSAGE_OK,
    cut_MESSAGE_SKIPPED,
    cut_MESSAGE_FAIL,
    cut_MESSAGE_EXCEPTION,
    cut_MESSAGE_TIMEOUT,
    cut_MESSAGE_CHECK
};

struct cut_UnitResult {
    char *name;
    int number;
    int subtests;
    int completed;
    int failed;
    int skipped;
    char *file;
    int line;
    char *statement;
    char *exceptionType;
    char *exceptionMessage;
    int returnCode;
    int signal;
    int timeouted;
    struct cut_Info *debug;
    struct cut_Info *check;
};

struct cut_UnitTest {
    cut_Instance instance;
    const char *name;
    const char *file;
    size_t line;
};

struct cut_UnitTestArray {
    int size;
    int capacity;
    struct cut_UnitTest *tests;
};

struct cut_Arguments {
    int help;
    int list_tests;
    unsigned timeout;
    int noFork;
    int noColor;
    char *output;
    int exactMatch;
    int testId;
    int subtestId;
    int matchSize;
    char **match;
    const char *selfName;
    int shortPath;
};

enum cut_ReturnCodes {
    cut_NORMAL_EXIT = 0,
    cut_ERROR_EXIT = 254,
    cut_FATAL_EXIT = 255
};

enum cut_Colors {
    cut_NO_COLOR = 0,
    cut_YELLOW_COLOR,
    cut_GREEN_COLOR,
    cut_RED_COLOR
};

// 1hc substitution of /builds/pb071/cut/src/globals.h
#ifndef CUT_GLOBALS_H
#define CUT_GLOBALS_H

#ifndef CUT_MAIN
#error "cannot be standalone"
#endif

#define CUT_MAX_LOCAL_MESSAGE_LENGTH 4096

CUT_PRIVATE struct cut_Arguments cut_arguments;
CUT_PRIVATE struct cut_UnitTestArray cut_unitTests = {0, 0, NULL};
CUT_PRIVATE FILE *cut_output = NULL;
CUT_PRIVATE int cut_outputsRedirected = 0;
CUT_PRIVATE int cut_pipeWrite = 0;
CUT_PRIVATE int cut_pipeRead = 0;
CUT_PRIVATE int cut_originalStdIn = 0;
CUT_PRIVATE int cut_originalStdOut = 0;
CUT_PRIVATE int cut_originalStdErr = 0;
CUT_PRIVATE FILE *cut_stdout = NULL;
CUT_PRIVATE FILE *cut_stderr = NULL;
CUT_PRIVATE jmp_buf cut_executionPoint;
CUT_PRIVATE const char *cut_emergencyLog = "cut.log";
CUT_PRIVATE int cut_localMessageSize = 0;
CUT_PRIVATE char cut_localMessage[CUT_MAX_LOCAL_MESSAGE_LENGTH];
CUT_PRIVATE char *cut_localMessageCursor = NULL;
CUT_PRIVATE cut_GlobalTear cut_globalTearUp = NULL;
CUT_PRIVATE cut_GlobalTear cut_globalTearDown = NULL;

#if defined(_WIN32)
CUT_PRIVATE int cut_tmpInput = -1;
#elif defined(__linux__) || defined(__unix) || defined(__APPLE__)
CUT_PRIVATE int cut_inputFeedPID = 0;
#endif

#endif // CUT_GLOBALS_H
// 1hc substitution of /builds/pb071/cut/src/fragments.h
#ifndef CUT_FRAGMENTS_H
#define CUT_FRAGMENTS_H

#ifndef CUT_MAIN
#error "cannot be standalone"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#define CUT_MAX_SLICE_COUNT 255
#define CUT_MAX_SERIALIZED_LENGTH (256*256-1)
#define CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH 512

struct cut_FragmentSlice {
    char *data;
    uint16_t length;
    struct cut_FragmentSlice *next;
};

struct cut_Fragment {
    uint32_t serializedLength;
    uint8_t id;
    uint8_t sliceCount;
    char *serialized;
    struct cut_FragmentSlice *slices;
    struct cut_FragmentSlice *lastSlice;
};

struct cut_FragmentHeader {
    uint32_t length;
    uint8_t id;
    uint8_t sliceCount;
};

typedef union {
    struct {
        uint32_t length;
        uint32_t processed;
    } structured;
    uint64_t raw;
} cut_FragmentReceiveStatus;
#define CUT_FRAGMENT_RECEIVE_STATUS {0}

CUT_PRIVATE void cut_FragmentInit(struct cut_Fragment *fragments, int id) {
    fragments->id = id;
    fragments->sliceCount = 0;
    fragments->serializedLength = 0;
    fragments->serialized = NULL;
    fragments->slices = NULL;
    fragments->lastSlice = NULL;
}

CUT_PRIVATE void cut_FragmentClean(struct cut_Fragment *fragments) {
    if (!fragments)
        return;
    if (fragments->serialized)
        free(fragments->serialized);
    while (fragments->slices) {
        struct cut_FragmentSlice *current = fragments->slices;
        fragments->slices = fragments->slices->next;
        free(current->data);
        free(current);
    }
}

CUT_PRIVATE void *cut_FragmentReserve(struct cut_Fragment *fragments, size_t length, int *sliceId) {
    if (!fragments)
        return NULL;
    if (fragments->sliceCount == CUT_MAX_SLICE_COUNT)
        return NULL;
    struct cut_FragmentSlice *slice = (struct cut_FragmentSlice *)malloc(sizeof(struct cut_FragmentSlice));
    if (!slice)
        return NULL;
    slice->data = (char *)malloc(length);
    if (!slice->data) {
        free(slice);
        return NULL;
    }
    slice->length = (uint16_t)length;
    slice->next = NULL;
    if (fragments->lastSlice)
        fragments->lastSlice->next = slice;
    else
        fragments->slices = slice;
    fragments->lastSlice = slice;
    if (sliceId)
        *sliceId = fragments->sliceCount;
    ++fragments->sliceCount;
    return slice->data;
}

CUT_PRIVATE void *cut_FragmentAddString(struct cut_Fragment *fragments, const char *str) {
    if (!fragments)
        return NULL;
    size_t length = strlen(str) + 1;
    void *data = cut_FragmentReserve(fragments, length, NULL);
    if (!data)
        return NULL;
    memcpy(data, str, length);
    return data;
}

CUT_PRIVATE char *cut_FragmentGet(struct cut_Fragment *fragments, int sliceId, size_t *length) {
    if (!fragments)
        return NULL;
    if (sliceId >= fragments->sliceCount)
        return NULL;
    struct cut_FragmentSlice *current = fragments->slices;
    for (int id = 0; id < sliceId; ++id) {
        current = current->next;
    }
    if (length)
        *length = current->length;
    return current->data;
}

CUT_PRIVATE int cut_FragmentSerialize(struct cut_Fragment *fragments) {
    if (!fragments)
        return 0;
    if (fragments->serialized)
        free(fragments->serialized);
    uint32_t length = sizeof(struct cut_FragmentHeader);
    length += fragments->sliceCount * sizeof(uint16_t);
    uint32_t contentOffset = length;
    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        length += current->length;
    }
    if (length > CUT_MAX_SERIALIZED_LENGTH)
        return 0;
    fragments->serialized = (char *)malloc(length);
    if (!fragments->serialized)
        return 0;
    fragments->serializedLength = length;
    memset(fragments->serialized, 0, length);
    struct cut_FragmentHeader *header = (struct cut_FragmentHeader *)fragments->serialized;
    header->length = length;
    header->id = fragments->id;
    header->sliceCount = fragments->sliceCount;
    uint16_t *sliceLength = (uint16_t *)(header + 1);
    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        *sliceLength = current->length;
        ++sliceLength;
        memcpy(fragments->serialized + contentOffset, current->data, current->length);
        contentOffset += current->length;
    }
    return 1;
}

CUT_PRIVATE int cut_FragmentSignalSafeSerialize(struct cut_Fragment *fragments) {
    static char buffer[CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH];
    if (!fragments)
        return 0;
    if (fragments->serialized)
        return 0;
    uint32_t length = sizeof(struct cut_FragmentHeader);
    length += fragments->sliceCount * sizeof(uint16_t);
    uint32_t contentOffset = length;
    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        length += current->length;
    }
    if (length > CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH)
        return 0;
    fragments->serialized = buffer;
    fragments->serializedLength = length;
    memset(fragments->serialized, 0, length);
    struct cut_FragmentHeader *header = (struct cut_FragmentHeader *)fragments->serialized;
    header->length = length;
    header->id = fragments->id;
    header->sliceCount = fragments->sliceCount;
    uint16_t *sliceLength = (uint16_t *)(header + 1);
    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        *sliceLength = current->length;
        ++sliceLength;
        memcpy(fragments->serialized + contentOffset, current->data, current->length);
        contentOffset += current->length;
    }
    return 1;
}

CUT_PRIVATE int cut_FragmentDeserialize(struct cut_Fragment *fragments) {
    if (!fragments)
        return 0;
    if (!fragments->serialized)
        return 0;
    struct cut_FragmentHeader *header = (struct cut_FragmentHeader *)fragments->serialized;
    fragments->serializedLength = header->length;
    fragments->id = header->id;
    fragments->sliceCount = 0;
    uint16_t *sliceLength = (uint16_t *)(header + 1);
    uint32_t contentOffset = sizeof(struct cut_FragmentHeader)
                           + header->sliceCount * sizeof(uint16_t);
    for (uint16_t slice = 0; slice < header->sliceCount; ++slice, ++sliceLength) {
        void *data = cut_FragmentReserve(fragments, *sliceLength, NULL);
        if (!data)
            return 0;
        memcpy(data, fragments->serialized + contentOffset, *sliceLength);
        contentOffset += *sliceLength;
    }
    return 1;
}

CUT_PRIVATE int64_t cut_FragmentReceiveContinue(cut_FragmentReceiveStatus *status, void *data, int64_t length) {
    if (!status)
        return 0;
    if (!data) {
        status->raw = 0;
        return sizeof(struct cut_FragmentHeader);
    }
    if (length <= 0)
        return length;
    if (!status->structured.length) {
        if (length != sizeof(struct cut_FragmentHeader))
            return 0;
        status->structured.length = ((struct cut_FragmentHeader*)data)->length;
    }
    status->structured.processed += (uint32_t)length;
    return status->structured.length - status->structured.processed;
}

CUT_PRIVATE size_t cut_FragmentReceiveProcessed(cut_FragmentReceiveStatus *status) {
    if (!status)
        return 0;
    return status->structured.processed;
}

#endif
// 1hc substitution of /builds/pb071/cut/src/declarations.h
#ifndef CUT_DECLARATIONS_H
#define CUT_DECLARATIONS_H

#ifndef CUT_MAIN
#error "cannot be standalone"
#endif

// common functions
CUT_NORETURN int cut_FatalExit(const char *reason);
CUT_NORETURN int cut_ErrorExit(const char *reason, ...);
void cut_Register(cut_Instance instance, const char *name, const char *file, size_t line);
void cut_RegisterGlobalTearUp(cut_GlobalTear instance);
void cut_RegisterGlobalTearDown(cut_GlobalTear instance);
CUT_PRIVATE int cut_Help();
CUT_PRIVATE int cut_SendMessage(const struct cut_Fragment *message);
CUT_PRIVATE int cut_ReadMessage(struct cut_Fragment *message);
CUT_PRIVATE void cut_ResetLocalMessage();
CUT_PRIVATE int cut_SendLocalMessage(struct cut_Fragment *message);
CUT_PRIVATE int cut_ReadLocalMessage(struct cut_Fragment *message);
CUT_PRIVATE void cut_SendOK(int counter);
void cut_SendSkipSubtest(int counter);
void cut_DebugMessage(const char *file, size_t line, const char *fmt, ...);
CUT_NORETURN void cut_Stop(const char *text, const char *file, size_t line);
void cut_Check(const char *text, const char *file, size_t line);
#  ifdef __cplusplus
CUT_PRIVATE void cut_StopException(const char *type, const char *text);
#  endif
CUT_PRIVATE void cut_ExceptionBypass(int testId, int subtest);
CUT_PRIVATE void cut_Timeouted();
void cut_Subtest(int number, const char *name);
CUT_PRIVATE void *cut_PipeReader(struct cut_UnitResult *result);
CUT_PRIVATE int cut_SetSubtestName(struct cut_UnitResult *result, int number, const char *name);
CUT_PRIVATE int cut_AddInfo(struct cut_Info **info,
    size_t line, const char *file, const char *text);
CUT_PRIVATE int cut_SetFailResult(struct cut_UnitResult *result,
    size_t line, const char *file, const char *text);
CUT_PRIVATE int cut_SetExceptionResult(struct cut_UnitResult *result,
    const char *type, const char *text);
CUT_PRIVATE void cut_ParseArguments(int argc, char **argv);
CUT_PRIVATE void cut_ChangePath(const char *exe_path);
CUT_PRIVATE int cut_SkipUnit(int testId);
int cut_SkipSubtest(int testId, const char *name);
CUT_PRIVATE const char *cut_GetStatus(const struct cut_UnitResult *result, enum cut_Colors *color);
CUT_PRIVATE const char *cut_ShortPath(const char *path);
CUT_PRIVATE void cut_PrintResult(int base, int subtest, int subtests, const struct cut_UnitResult *result);
CUT_PRIVATE void cut_CleanInfo(struct cut_Info *info);
CUT_PRIVATE void cut_CleanMemory(struct cut_UnitResult *result);
CUT_PRIVATE int cut_TestComparator(const void *lhs, const void *rhs);
CUT_PRIVATE int cut_Runner(int argc, char **argv);
CUT_PRIVATE void cut_RunUnitForkless(int testId, int subtest, struct cut_UnitResult *result);

// platform specific functions
CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes);
CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes);
CUT_PRIVATE void cut_RedirectIO();
CUT_PRIVATE void cut_ResumeIO();
CUT_PRIVATE int cut_PreRun();
CUT_PRIVATE void cut_RunUnit(int testId, int subtest, struct cut_UnitResult *result);
int cut_File(FILE *file, const char *content);
int cut_BinaryFile(FILE *file, size_t size, const unsigned char *bytes);
void cut_InputFile(const char *name);
void cut_InputBytes(size_t size, const unsigned char *bytes);
void cut_Input(size_t size, const char *bytes);
CUT_PRIVATE void cut_ReapInputFeed();
CUT_PRIVATE int cut_IsDebugger();
CUT_PRIVATE int cut_IsTerminalOutput();
CUT_PRIVATE int cut_PrintColorized(enum cut_Colors color, const char *text);

#endif // CUT_DECLARATIONS_H
// 1hc substitution of /builds/pb071/cut/src/messages.h
#ifndef CUT_MESSAGES_H
#define CUT_MESSAGES_H

#ifndef CUT_MAIN
#error "cannot be standalone"
#endif

CUT_PRIVATE int cut_SendMessage(const struct cut_Fragment *message) {
    size_t remaining = message->serializedLength;
    size_t position = 0;

    int64_t r;
    while (remaining && (r = cut_Write(cut_pipeWrite, message->serialized + position, remaining)) > 0) {
        position += (size_t)r;
        remaining -= (size_t)r;
    }
    return r != -1;
}

CUT_PRIVATE int cut_ReadMessage(struct cut_Fragment *message) {
    cut_FragmentReceiveStatus status = CUT_FRAGMENT_RECEIVE_STATUS;

    message->serialized = NULL;
    message->serializedLength = 0;
    int64_t r = 0;
    int64_t toRead = 0;
    size_t processed = 0;
    while ((toRead = cut_FragmentReceiveContinue(&status, message->serialized, r)) > 0) {
        processed = cut_FragmentReceiveProcessed(&status);

        if (message->serializedLength < processed + toRead) {
            message->serializedLength = (uint32_t)(processed + toRead);
            message->serialized = (char *)realloc(message->serialized, message->serializedLength);
            if (!message->serialized)
                cut_FatalExit("cannot allocate memory for reading a message");
        }
        r = cut_Read(cut_pipeRead, message->serialized + processed, (size_t)toRead);
    }
    processed = cut_FragmentReceiveProcessed(&status);
    if (processed < message->serializedLength) {
        memset(message->serialized, 0, message->serializedLength);
    }
    return toRead != -1;
}

CUT_PRIVATE void cut_ResetLocalMessage() {
    cut_localMessageCursor = NULL;
    cut_localMessageSize = 0;
}

CUT_PRIVATE int cut_SendLocalMessage(struct cut_Fragment *message) {
    if (!cut_arguments.noFork)
        return cut_SendMessage(message);
    if (message->serializedLength + cut_localMessageSize > CUT_MAX_LOCAL_MESSAGE_LENGTH)
        return 0;
    memcpy(cut_localMessage + cut_localMessageSize, message->serialized, message->serializedLength);
    cut_localMessageSize += message->serializedLength;
    return 1;
}

CUT_PRIVATE void cut_SendStatus(enum cut_MessageType mt, int counter) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, mt);
    int *pCounter = (int *)cut_FragmentReserve(&message, sizeof(int), NULL);
    if (!pCounter)
        cut_FatalExit("cannot allocate memory STATUS:fragment:counter");
    *pCounter = counter;

    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize STATUS:fragment");
    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send STATUS:message");
    cut_FragmentClean(&message);
}

CUT_PRIVATE void cut_SendOK(int counter) {
    cut_SendStatus(cut_MESSAGE_OK, counter);
}

void cut_SendSkipSubtest(int counter) {
    cut_SendStatus(cut_MESSAGE_SKIPPED, counter);
}

void cut_DebugMessage(const char *file, size_t line, const char *fmt, ...) {
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    size_t length = 1 + vsnprintf(NULL, 0, fmt, args1);
    char *buffer;
    (buffer = (char *)malloc(length)) || cut_FatalExit("cannot allocate buffer");
    va_end(args1);
    vsnprintf(buffer, length, fmt, args2);
    va_end(args2);

    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_DEBUG);
    size_t *pLine = (size_t *)cut_FragmentReserve(&message, sizeof(size_t), NULL);
    if (!pLine)
        cut_FatalExit("cannot insert debug:fragment:line");
    *pLine = line;
    cut_FragmentAddString(&message, file) || cut_FatalExit("cannot insert debug:fragment:file");
    cut_FragmentAddString(&message, buffer) || cut_FatalExit("cannot insert debug:fragment:buffer");
    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize debug:fragment");

    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send debug:message");
    cut_FragmentClean(&message);
    free(buffer);
}

void cut_Stop(const char *text, const char *file, size_t line) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_FAIL);
    size_t *pLine = (size_t*)cut_FragmentReserve(&message, sizeof(size_t), NULL);
    if (!pLine)
        cut_FatalExit("cannot insert stop:fragment:line");
    *pLine = line;
    cut_FragmentAddString(&message, file) || cut_FatalExit("cannot insert stop:fragment:file");
    cut_FragmentAddString(&message, text) || cut_FatalExit("cannot insert stop:fragment:text");
    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize stop:fragment");

    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send stop:message");
    cut_FragmentClean(&message);
    longjmp(cut_executionPoint, 1);
}

void cut_Check(const char *text, const char *file, size_t line) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_CHECK);
    size_t *pLine = (size_t*)cut_FragmentReserve(&message, sizeof(size_t), NULL);
    if (!pLine)
        cut_FatalExit("cannot insert check:fragment:line");
    *pLine = line;
    cut_FragmentAddString(&message, file) || cut_FatalExit("cannot insert check:fragment:file");
    cut_FragmentAddString(&message, text) || cut_FatalExit("cannot insert check:fragment:text");
    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize check:fragment");

    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send check:message");
    cut_FragmentClean(&message);
}

CUT_PRIVATE void cut_StopException(const char *type, const char *text) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_EXCEPTION);
    cut_FragmentAddString(&message, type) || cut_FatalExit("cannot insert exception:fragment:type");
    cut_FragmentAddString(&message, text) || cut_FatalExit("cannot insert exception:fragment:text");
    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize exception:fragment");

    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send exception:message");
    cut_FragmentClean(&message);
}

CUT_PRIVATE void cut_Timeouted() {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_TIMEOUT);
    cut_FragmentSignalSafeSerialize(&message) || cut_FatalExit("cannot serialize timeout:fragment");
    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send timeout:message");
}

void cut_Subtest(int number, const char *name) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_SUBTEST);
    int * pNumber = (int *)cut_FragmentReserve(&message, sizeof(int), NULL);
    if (!pNumber)
        cut_FatalExit("cannot insert subtest:fragment:number");
    *pNumber = number;
    cut_FragmentAddString(&message, name) || cut_FatalExit("cannot insert subtest:fragment:name");
    cut_FragmentSerialize(&message) || cut_FatalExit("cannot serialize subtest:fragment");

    cut_SendLocalMessage(&message) || cut_FatalExit("cannot send subtest:message");
    cut_FragmentClean(&message);
}

CUT_PRIVATE int cut_ReadLocalMessage(struct cut_Fragment *message) {
    if (!cut_localMessageSize)
        return cut_ReadMessage(message);
    // first read
    if (!cut_localMessageCursor)
        cut_localMessageCursor = cut_localMessage;

    // nothing to read
    if (cut_localMessageCursor >= cut_localMessage + cut_localMessageSize)
        return 0;

    cut_FragmentReceiveStatus status = CUT_FRAGMENT_RECEIVE_STATUS;
    message->serialized = NULL;
    message->serializedLength = 0;

    int64_t r = 0;
    int64_t toRead = 0;
    while ((toRead = cut_FragmentReceiveContinue(&status, message->serialized, r)) > 0) {
        size_t processed = cut_FragmentReceiveProcessed(&status);

        if (message->serializedLength < processed + toRead) {
            message->serializedLength = (uint32_t)(processed + toRead);
            message->serialized = (char *)realloc(message->serialized, message->serializedLength);
            if (!message->serialized)
                cut_FatalExit("cannot allocate memory for reading a message");
        }
        memcpy(message->serialized + processed, cut_localMessageCursor, (size_t)toRead);
        cut_localMessageCursor += toRead;
        r = toRead;
    }
    return toRead != -1;
}

CUT_PRIVATE void *cut_PipeReader(struct cut_UnitResult *result) {
    int repeat;
    do {
        repeat = 0;
        struct cut_Fragment message;
        cut_FragmentInit(&message, cut_NO_TYPE);
        cut_ReadLocalMessage(&message) || cut_FatalExit("cannot read message");
        cut_FragmentDeserialize(&message) || cut_FatalExit("cannot deserialize message");

        switch (message.id) {
        case cut_MESSAGE_SUBTEST:
            message.sliceCount == 2 || cut_FatalExit("invalid debug:message format");
            cut_SetSubtestName(
                result,
                *(int *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL)
            ) || cut_FatalExit("cannot set subtest name");
            repeat = 1;
            break;
        case cut_MESSAGE_DEBUG:
            message.sliceCount == 3 || cut_FatalExit("invalid debug:message format");
            cut_AddInfo(
                &result->debug,
                *(size_t *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || cut_FatalExit("cannot add debug");
            repeat = 1;
            break;
        case cut_MESSAGE_OK:
            message.sliceCount == 1 || cut_FatalExit("invalid ok:message format");
            result->completed = 1;
            result->subtests = *(int *)cut_FragmentGet(&message, 0, NULL);
            break;
        case cut_MESSAGE_SKIPPED:
            message.sliceCount == 1 || cut_FatalExit("invalid skipped:message format");
            result->skipped = 1;
            break;
        case cut_MESSAGE_FAIL:
            message.sliceCount == 3 || cut_FatalExit("invalid fail:message format");
            cut_SetFailResult(
                result,
                *(size_t *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || cut_FatalExit("cannot set fail result");
            break;
        case cut_MESSAGE_EXCEPTION:
            message.sliceCount == 2 || cut_FatalExit("invalid exception:message format");
            cut_SetExceptionResult(
                result,
                cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL)
            ) || cut_FatalExit("cannot set exception result");
            break;
        case cut_MESSAGE_TIMEOUT:
            result->timeouted = 1;
            result->failed = 1;
            break;
        case cut_MESSAGE_CHECK:
            message.sliceCount == 3 || cut_FatalExit("invalid check:message format");
            cut_AddInfo(
                &result->check,
                *(size_t *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || cut_FatalExit("cannot add check");
            result->failed = 1;
            repeat = 1;
            break;
        }
        cut_FragmentClean(&message);
    } while (repeat);
    return NULL;
}

CUT_PRIVATE int cut_SetSubtestName(struct cut_UnitResult *result, int number, const char *name) {
    result->name = (char *)malloc(strlen(name) + 1);
    if (!result->name)
        return 0;
    result->number = number;
    strcpy(result->name, name);
    return 1;
}

CUT_PRIVATE int cut_AddInfo(struct cut_Info **info,
                             size_t line, const char *file, const char *text) {
    struct cut_Info *item = (struct cut_Info *)malloc(sizeof(struct cut_Info));
    if (!item)
        return 0;
    item->file = (char *)malloc(strlen(file) + 1);
    if (!item->file) {
        free(item);
        return 0;
    }
    item->message = (char *)malloc(strlen(text) + 1);
    if (!item->message) {
        free(item->file);
        free(item);
        return 0;
    }
    strcpy(item->file, file);
    strcpy(item->message, text);
    item->line = line;
    item->next = NULL;
    while (*info) {
        info = &(*info)->next;
    }
    *info = item;
    return 1;
}

CUT_PRIVATE int cut_SetFailResult(struct cut_UnitResult *result,
                                  size_t line, const char *file, const char *text) {
    result->file = (char *)malloc(strlen(file) + 1);
    if (!result->file)
        return 0;
    result->statement = (char *)malloc(strlen(text) + 1);
    if (!result->statement) {
        free(result->file);
        return 0;
    }
    result->line = line;
    strcpy(result->file, file);
    strcpy(result->statement, text);
    result->failed = 1;
    return 1;
}

CUT_PRIVATE int cut_SetExceptionResult(struct cut_UnitResult *result,
                                       const char *type, const char *text) {
    result->exceptionType = (char *)malloc(strlen(type) + 1);
    if (!result->exceptionType)
        return 0;
    result->exceptionMessage = (char *)malloc(strlen(text) + 1);
    if (!result->exceptionMessage) {
        free(result->exceptionType);
        return 0;
    }
    strcpy(result->exceptionType, type);
    strcpy(result->exceptionMessage, text);
    result->failed = 1;
    return 1;
}

#endif // CUT_MESSAGES_H
// 1hc substitution of /builds/pb071/cut/src/execution.h
#ifndef CUT_EXECUTION_H
#define CUT_EXECUTION_H

#ifndef CUT_MAIN
#error "cannot be standalone"
#endif

# ifdef __cplusplus
} // extern C

#  include <stdexcept>
#  include <typeinfo>
#  include <string>

CUT_PRIVATE void cut_ExceptionBypass(int testId, int subtest) {
    cut_RedirectIO();
    if (setjmp(cut_executionPoint))
        goto cleanup;
    if (cut_globalTearUp)
        cut_globalTearUp();
    try {
        int counter = 0;
        cut_unitTests.tests[testId].instance(&counter, testId, subtest);
        cut_SendOK(counter);
    } catch (const std::exception &e) {
        std::string name = typeid(e).name();
        cut_StopException(name.c_str(), e.what() ? e.what() : "(no reason)");
    } catch (...) {
        cut_StopException("unknown type", "(empty message)");
    }
cleanup:
    if (cut_globalTearDown)
        cut_globalTearDown();
    cut_ResumeIO();
    cut_ReapInputFeed();
}

extern "C" {
# else
CUT_PRIVATE void cut_ExceptionBypass(int testId, int subtest) {
    cut_RedirectIO();
    if (setjmp(cut_executionPoint))
        goto cleanup;
    if (cut_globalTearUp)
        cut_globalTearUp();
    int counter = 0;
    cut_unitTests.tests[testId].instance(&counter, testId, subtest);
    cut_SendOK(counter);
cleanup:
    if (cut_globalTearDown)
        cut_globalTearDown();
    cut_ResumeIO();
}
# endif

CUT_PRIVATE int cut_MatchArgument(const char *name, const char *pattern) {
    return cut_arguments.exactMatch
        ? strcmp(name, pattern) == 0
        : strstr(name, pattern) != NULL;
}

CUT_PRIVATE int cut_MatchTestName(const char *name, char *pattern) {
    char *delimiter = strchr(pattern, '/');

    // If there is no '/' character in the pattern, then any match will do.
    if (delimiter == NULL)
        return strstr(name, pattern) != NULL;

    // Otherwise make sure that we only match the part before '/'.
    *delimiter = '\0';
    int matched = cut_MatchArgument(name, pattern);
    *delimiter = '/';
    return matched;
}

CUT_PRIVATE int cut_MatchSubtestName(const char *name, const char *pattern) {
    const char *delimiter = strchr(pattern, '/');
    const char *matched = strstr(name, pattern);

    // If there is no '/' character in the pattern, allow the test to run.
    if (delimiter == NULL)
        return 1;

    // Otherwise match in the pattern after the delimiter.
    return cut_MatchArgument(name, delimiter + 1);
}

CUT_PRIVATE int cut_SkipUnit(int testId) {
    if (cut_arguments.testId >= 0)
        return testId != cut_arguments.testId;
    if (!cut_arguments.matchSize)
        return 0;
    const char *name = cut_unitTests.tests[testId].name;
    for (int i = 0; i < cut_arguments.matchSize; ++i) {
        if (cut_MatchTestName(name, cut_arguments.match[i]))
            return 0;
    }
    return 1;
}

int cut_SkipSubtest(int testId, const char *subtest_name) {
    if (!cut_arguments.matchSize)
        return 0;

    const char *test_name = cut_unitTests.tests[testId].name;
    for (int i = 0; i < cut_arguments.matchSize; ++i) {
        if (cut_MatchTestName(test_name, cut_arguments.match[i])
                && cut_MatchSubtestName(subtest_name, cut_arguments.match[i])) {
            return 0;
        }
    }

    return 1;
}

CUT_PRIVATE int cut_CanSkipAllSubtests(int testId) {
    int backup = cut_arguments.exactMatch;
    cut_arguments.exactMatch = 1;
    int result = !cut_SkipSubtest(testId, ".");
    cut_arguments.exactMatch = backup;
    return result;
}

CUT_PRIVATE const char *cut_GetStatus(const struct cut_UnitResult *result, enum cut_Colors *color) {
    static const char *ok = "OK";
    static const char *skipped = "SKIPPED";
    static const char *fail = "FAIL";
    static const char *incomplete = "INCOMPLETE";
    static const char *internalFail = "INTERNAL ERROR";

    if (result->returnCode == cut_FATAL_EXIT) {
        *color = cut_YELLOW_COLOR;
        return internalFail;
    }
    if (result->failed) {
        *color = cut_RED_COLOR;
        return fail;
    }
    if (result->skipped) {
        *color = cut_YELLOW_COLOR;
        return skipped;
    }
    if (!result->completed) {
        *color = cut_RED_COLOR;
        return incomplete;
    }
    *color = cut_GREEN_COLOR;
    return ok;
}

CUT_PRIVATE const char *cut_ShortPath(const char *path) {
    enum { MAX_PATH = 80 };
    static char shortenedPath[MAX_PATH + 1];
    char *cursor = shortenedPath;
    const char *dots = "...";
    const size_t dotsLength = strlen(dots);
    int pathLength = strlen(path);
    if (cut_arguments.shortPath < 0 || pathLength <= cut_arguments.shortPath)
        return path;
    if (cut_arguments.shortPath > MAX_PATH)
        cut_arguments.shortPath = MAX_PATH;

    int fullName = 0;
    const char *end = path + strlen(path);
    const char *name = end;
    for (; end - name < cut_arguments.shortPath && path < name; --name) {
        if (*name == '/' || *name == '\\') {
            fullName = 1;
            break;
        }
    }
    int consumed = (end - name) + dotsLength;
    if (consumed < cut_arguments.shortPath) {
        size_t remaining = cut_arguments.shortPath - consumed;
        size_t firstPart = remaining - remaining / 2;
        strncpy(cursor, path, firstPart);
        cursor += firstPart;
        strcpy(cursor, dots);
        cursor += dotsLength;
        remaining -= firstPart;
        name -= remaining;
    }
    strcpy(cursor, name);
    return shortenedPath;
}

CUT_PRIVATE const char *cut_Signal(int signal) {
    static char number[16];
    const char *names[] = {
        "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT",
        "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV", "SIGSYS",
        "SIGPIPE", "SIGALRM", "SIGTERM", "SIGUSR1", "SIGUSR2"
    };
    if (0 < signal <= sizeof(names) / sizeof(*names))
        sprintf(number, "%s (%d)", names[signal - 1], signal);
    else
        sprintf(number, "%d", signal);
    return number;
}

CUT_PRIVATE const char *cut_ReturnCode(int returnCode) {
    static char number[16];
    switch (returnCode) {
    case cut_ERROR_EXIT:
        return "ERROR EXIT";
    case cut_FATAL_EXIT:
        return "FATAL EXIT";
    default:
        sprintf(number, "%d", returnCode);
        return number;
    }
}

CUT_PRIVATE void cut_PrintResult(int base, int subtest, int subtests, const struct cut_UnitResult *result) {
    static const char *shortIndent = "    ";
    static const char *longIndent = "        ";
    enum cut_Colors color;
    const char *status = cut_GetStatus(result, &color);
    int lastPosition = 80 - 1 - strlen(status);
    int extended = 0;

    const char *indent = shortIndent;
    if (result->name && subtest) {
        if (result->number <= 1)
            lastPosition -= fprintf(cut_output, "%s%s", indent, result->name);
        else {
            lastPosition -= fprintf(cut_output, "%s", indent);
            int length = strlen(result->name);
            for (int i = 0; i < length; ++i)
                putc(' ', cut_output);
            lastPosition -= length;
        }
        if (result->number)
            lastPosition -= fprintf(cut_output, " #%d", result->number);
        indent = longIndent;
    } else {
        lastPosition -= base;
    }
    if (subtests < 0)
        extended = 1;

    if (!subtest && subtests > 0) {
        fprintf(cut_output, ": %d subtests", subtests);
    } else {
        for (int i = 0; i < lastPosition; ++i) {
            putc('.', cut_output);
        }
        if (cut_arguments.noColor)
            fprintf(cut_output, status);
        else
            cut_PrintColorized(color, status);
    }
    putc('\n', cut_output);
    if (result->failed) {
        for (const struct cut_Info *current = result->check; current; current = current->next) {
            fprintf(cut_output, "%scheck '%s' (%s:%d)\n", indent, current->message,
                    cut_ShortPath(current->file), current->line);
        }
        if (result->timeouted)
            fprintf(cut_output, "%stimeouted (%d s)\n", indent, cut_arguments.timeout);
        else if (result->signal)
            fprintf(cut_output, "%ssignal: %s\n", indent, cut_Signal(result->signal));
        else if (result->returnCode)
            fprintf(cut_output, "%sreturn code: %s\n", indent, cut_ReturnCode(result->returnCode));
        else if (result->statement && result->file && result->line)
            fprintf(cut_output, "%sassert '%s' (%s:%d)\n", indent,
                    result->statement, cut_ShortPath(result->file), result->line);
        else if (result->exceptionType && result->exceptionMessage)
            fprintf(cut_output, "%sexception %s: %s\n", indent,
                    result->exceptionType, result->exceptionMessage);
        extended = 1;
    } else if (!result->completed) {
        fprintf(cut_output, "%stested code called exit() directly", indent);
        extended = 1;
    }
    if (result->debug) {
        fprintf(cut_output, "%sdebug messages:\n", indent);
        extended = 1;
    }
    for (const struct cut_Info *current = result->debug; current; current = current->next) {
        fprintf(cut_output, "%s  %s (%s:%d)\n", indent, current->message,
                cut_ShortPath(current->file), current->line);
    }
    if (extended)
        fprintf(cut_output, "\n");
    fflush(cut_output);
}

CUT_PRIVATE void cut_RunUnitForkless(int testId, int subtest, struct cut_UnitResult *result) {
    cut_ExceptionBypass(testId, subtest);
    cut_PipeReader(result);
    cut_ResetLocalMessage();
    cut_ReapInputFeed();
    result->returnCode = 0;
    result->signal = 0;
}

CUT_PRIVATE int cut_TestComparator(const void *_lhs, const void *_rhs) {
    struct cut_UnitTest *lhs = (struct cut_UnitTest *)_lhs;
    struct cut_UnitTest *rhs = (struct cut_UnitTest *)_rhs;

    int result = strcmp(lhs->file, rhs->file);
    if (!result)
        result = lhs->line <= rhs->line ? -1 : 1;
    return result;
}

CUT_PRIVATE int cut_ListTests() {
    for (int i = 0; i < cut_unitTests.size; ++i) {
        const char *test_name = cut_unitTests.tests[i].name;
        fprintf(cut_output, "%s\n", test_name);
    }
}

CUT_PRIVATE int cut_Runner(int argc, char **argv) {
    cut_output = stdout;
    cut_ParseArguments(argc, argv);
    cut_ChangePath(argv[0]);

    void (*unitRunner)(int, int, struct cut_UnitResult *) =
        cut_arguments.noFork ? cut_RunUnitForkless : cut_RunUnit;

    int helpLikeOption = 0;
    int failed = 0;
    int executed = 0;

    qsort(cut_unitTests.tests, cut_unitTests.size, sizeof(struct cut_UnitTest), cut_TestComparator);

    if (cut_PreRun())
        goto cleanup;

    if (cut_arguments.output) {
        cut_output = fopen(cut_arguments.output, "w");
        if (!cut_output)
            cut_ErrorExit("cannot open file %s for writing", cut_arguments.output);
    }

    if (cut_arguments.help) {
        cut_Help();
        helpLikeOption = 1;
        goto cleanup;
    }

    if(cut_arguments.list_tests) {
        cut_ListTests();
        helpLikeOption = 1;
        goto cleanup;
    }

    for (int i = 0; i < cut_unitTests.size; ++i) {
        if (cut_SkipUnit(i))
            continue;
        ++executed;
        int base = fprintf(cut_output, "[%3i] %s", executed, cut_unitTests.tests[i].name);
        fflush(cut_output);
        int subtests = 0;
        if (cut_arguments.subtestId > 0)
            subtests = cut_arguments.subtestId;
        int subtestFailure = 0;
        int executedSubtests = 0;
        for (int subtest = 0; subtest <= subtests; ++subtest) {
            if (cut_arguments.subtestId >= 0 && cut_arguments.subtestId != subtest)
                continue;
            struct cut_UnitResult result;
            memset(&result, 0, sizeof(result));
            unitRunner(i, subtest, &result);
            if (result.failed || (i > 0 && !result.completed && !result.skipped))
                ++subtestFailure;
            FILE *emergencyLog = fopen(cut_emergencyLog, "r");
            if (emergencyLog) {
                char buffer[512] = {0,};
                fread(buffer, 512, 1, emergencyLog);
                if (*buffer) {
                    result.statement = (char *)malloc(strlen(buffer) + 1);
                    strcpy(result.statement, buffer);
                }
                fclose(emergencyLog);
                remove(cut_emergencyLog);
            }
            if (result.subtests > subtests)
                subtests = result.subtests;
            if (!result.skipped) {
                cut_PrintResult(base, subtest, subtests, &result);
                ++executedSubtests;
            }
            cut_CleanMemory(&result);
        }
        // Skip this test if no subtests were run due to limiting arguments
        // being too strict or erroneous, unless one of the exceptions is true:
        //   - The wrapper test failed, which cannot be simply ignored.
        //   - The user requested this situation with 'TEST/.' argument.
        if (executedSubtests <= 1 && subtests > 0 && cut_arguments.matchSize > 0
                && !subtestFailure && !cut_CanSkipAllSubtests(i)) {
            --executed;
        }
        if (subtests > 1) {
            base = fprintf(cut_output, "[%3i] %s (overall)", executed, cut_unitTests.tests[i].name);
            struct cut_UnitResult result;
            memset(&result, 0, sizeof(result));
            result.completed = 1;
            result.failed = subtestFailure;
            result.skipped = executedSubtests == 0;
            cut_PrintResult(base, 0, -1, &result);
        }
        if (subtestFailure)
            ++failed;
    }
    if (executed > 0) {
        fprintf(cut_output,
                "\nSummary:\n"
                "  tests:     %3i\n"
                "  succeeded: %3i\n"
                "  skipped:   %3i\n"
                "  failed:    %3i\n",
                cut_unitTests.size,
                executed - failed,
                cut_unitTests.size - executed,
                failed);
    } else {
        fprintf(cut_output, "No tests were run!\n");
    }
    if (cut_arguments.output)
        fclose(cut_output);
cleanup:
    free(cut_arguments.match);

    // Consider it an error if tests were supposed to run (no '--help' or
    // similar option was given), but none did.
    if (executed == 0 && !helpLikeOption) {
        return 255;
    }

    return failed;
}

#endif // CUT_EXECUTION_H
// 1hc substitution of /builds/pb071/cut/src/common.h
#ifndef CUT_COMMON_H
#define CUT_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cut_File(FILE *f, const char *content) {
    return cut_BinaryFile(f, strlen(content), (const unsigned char *)content);
}

void cut_InputBytes(size_t size, const unsigned char *bytes) {
    cut_Input(size, (const char *) bytes);
}

#endif // CUT_COMMON_H

#  if defined(__linux__)
// 1hc substitution of /builds/pb071/cut/src/linux.h
#ifndef CUT_LINUX_H
#define CUT_LINUX_H

# include <errno.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/prctl.h>
# include <fcntl.h>
# include <signal.h>

// 1hc substitution of /builds/pb071/cut/src/unix-common.h
#if !defined(CUT_UNIX_H) && !defined(CUT_LINUX_H)
#   warning "this header should be included from unix.h or linux."
#endif

#ifndef CUT_UNIX_COMMON_H
#define CUT_UNIX_COMMON_H

# include <sys/types.h>
# include <errno.h>
# include <signal.h>

CUT_PRIVATE void cut_RedirectIO() {
    cut_outputsRedirected = 1;
    cut_stdout = tmpfile();
    cut_stderr = tmpfile();
    cut_originalStdIn = dup(STDIN_FILENO);
    cut_originalStdOut = dup(STDOUT_FILENO);
    cut_originalStdErr = dup(STDERR_FILENO);

    dup2(fileno(cut_stdout), STDOUT_FILENO);
    dup2(fileno(cut_stderr), STDERR_FILENO);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdout) != -1 || cut_FatalExit("cannot close file");
    fclose(cut_stderr) != -1 || cut_FatalExit("cannot close file");
    close(1) != -1 || cut_FatalExit("cannot close file");
    close(2) != -1 || cut_FatalExit("cannot close file");
    if (cut_inputFeedPID != 0)
        dup2(cut_originalStdIn, STDIN_FILENO);
    dup2(cut_originalStdOut, STDOUT_FILENO);
    dup2(cut_originalStdErr, STDERR_FILENO);

    // close resources to make Valgrind happy
    close(cut_originalStdErr);
    close(cut_originalStdOut);
    close(cut_originalStdIn);

    cut_outputsRedirected = 0;
}

CUT_PRIVATE void cut_InputProcess(size_t size, const char *bytes) {
    // silence SIGPIPE, we will detect it in write()
    signal(SIGPIPE, SIG_IGN);

    size_t cursor = 0;
    while (cursor < size) {
        ssize_t written = cut_Write(STDOUT_FILENO, bytes + cursor, size - cursor);

        if (written < 0) {
            switch (errno) {
                case EPIPE:
                    return;
                default:
                    cut_FatalExit("error while pumping input data");
            }
        }

        cursor += written;
    }
}

CUT_PRIVATE void cut_ReapInputFeed(void) {
    if (cut_inputFeedPID == 0)
        return;

    if (kill(cut_inputFeedPID, SIGTERM) == -1) {
        DEBUG_MSG("cannot kill input feed %d: %s", cut_inputFeedPID,
                strerror(errno));
        goto reset_pid;
    }

    int wstatus;
    if (waitpid(cut_inputFeedPID, &wstatus, 0) != cut_inputFeedPID) {
        DEBUG_MSG("cannot wait for input feed %d: %s", cut_inputFeedPID,
                strerror(errno));
        goto reset_pid;
    }

    if (!WIFEXITED(wstatus) && WTERMSIG(wstatus) != SIGTERM) {
        DEBUG_MSG("input feed %d signaled: %d", cut_inputFeedPID, WTERMSIG(wstatus));
    } else if (WEXITSTATUS(wstatus) != 0) {
        DEBUG_MSG("input feed %d failed: %d", cut_inputFeedPID, WEXITSTATUS(wstatus));
    }

reset_pid:
    cut_inputFeedPID = 0;
}

#endif // CUT_UNIX_COMMON_H

CUT_PRIVATE int cut_IsTerminalOutput() {
    return isatty(fileno(stdout));
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return write(fd, source, bytes);
}

CUT_PRIVATE int cut_PreRun() {
    return 0;
}

CUT_PRIVATE void cut_SigAlrm(CUT_UNUSED(int signum)) {
    cut_Timeouted();
    _exit(cut_NORMAL_EXIT);
}

CUT_PRIVATE void cut_SetPDeathSignal(int ppid) {
    if (prctl(PR_SET_PDEATHSIG, SIGTERM) == -1)
        cut_FatalExit("cannot set child death signal");
    if (getppid() != ppid)
        exit(cut_ERROR_EXIT);
}

CUT_PRIVATE void cut_RunUnit(int testId, int subtest, struct cut_UnitResult *result) {
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    if (r == -1)
        cut_FatalExit("cannot establish communication pipe");

    cut_pipeRead = pipefd[0];
    cut_pipeWrite = pipefd[1];

    int parentPid = getpid();
    int pid = fork();
    if (pid == -1)
        cut_FatalExit("cannot fork");
    if (!pid) {
        cut_SetPDeathSignal(parentPid);
        close(cut_pipeRead) != -1 || cut_FatalExit("cannot close file");

        if (cut_arguments.timeout) {
            signal(SIGALRM, cut_SigAlrm);
            alarm(cut_arguments.timeout);
        }
        cut_ExceptionBypass(testId, subtest);

        close(cut_pipeWrite) != -1 || cut_FatalExit("cannot close file");
        free(cut_arguments.match);
        if (cut_arguments.output)
            fclose(cut_output);
        exit(cut_NORMAL_EXIT);
    }
    // parent process only
    int status = 0;
    close(cut_pipeWrite) != -1 || cut_FatalExit("cannot close file");
    cut_PipeReader(result);
    waitpid(pid, &status, 0) != -1 || cut_FatalExit("cannot wait for unit");
    result->returnCode = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
    result->signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    result->failed |= result->returnCode ||  result->signal;
    close(cut_pipeRead) != -1 || cut_FatalExit("cannot close file");
}

CUT_PRIVATE int cut_ReadWholeFile(int fd, char **buffer, size_t *length) {
    int result = 0;
    size_t capacity = 16;
    *length = 0;
    *buffer = (char *) malloc(capacity);
    if (!*buffer)
        return 0;

    long offset = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);

    int rv;
    while ((rv = read(fd, *buffer + *length, capacity - *length - 1)) > 0) {
        *length += rv;
        if (*length + 1 == capacity) {
            capacity *= 2;
            char *newBuffer = (char *)realloc(*buffer, capacity);
            if (!newBuffer)
                break;
            *buffer = newBuffer;
        }
    }

    if (!rv)
        result = 1;
    (*buffer)[*length] = '\0';
cleanup:
    lseek(fd, offset, SEEK_SET);
    return result;
}

int cut_BinaryFile(FILE *f, size_t length, const unsigned char *content) {
    int result = 0;
    size_t fileLength;
    int fd = fileno(f);
    fflush(f);
    char *buf = NULL;

    if (!cut_ReadWholeFile(fd, &buf, &fileLength))
        cut_FatalExit("cannot read whole file");

    result = length == fileLength && memcmp(content, buf, length) == 0;
cleanup:
    free(buf);
    return result;
}


void cut_InputFile(const char *name) {
    if (freopen(name, "r", stdin) == NULL)
        cut_FatalExit("cannot replace standard input");

    clearerr(stdin);
}

void cut_Input(size_t size, const char *bytes) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        cut_FatalExit("cannot create input pipe");

    int parentPid = getpid();
    int pid = fork();
    if (pid == -1)
        cut_FatalExit("cannot create child process");

    if (pid == 0) {
        // child process

        // replace standard output, so that Valgrind will less likely complain
        // about open file descriptor
        if (!dup2(pipefd[1], STDOUT_FILENO))
            cut_FatalExit("cannot replace standard output");
        close(pipefd[1]);

        // commit seppuku when the parent dies
        cut_SetPDeathSignal(parentPid);

        // mark current descriptor maximum
        int max_fd = dup(0);

        // here we only need to keep pipefd[1] and close other descriptors;
        // first close FILE* pointers
        fclose(cut_stderr);
        fclose(cut_stdout);

        // now we don't know which descriptors we still have, but we have
        // a maximum hint in max_fd, so we close all up to that descriptor
        // NOTE: this is a nasty hack and everyone who uses it in production
        //       code deserves to be spanked unconscious
        for (int fd = 3; fd <= max_fd; ++fd)
            close(fd);

        // signal the parent that we are ready
        write(STDOUT_FILENO, "OK", 2);

        cut_InputProcess(size, bytes);
        exit(cut_NORMAL_EXIT);
    }

    // parent process
    if (cut_inputFeedPID != 0)
        cut_FatalExit("input feed PID already in use");

    // wait for the child to initialize itself
    char child_ok[2];
    if (read(pipefd[0], child_ok, sizeof(child_ok)) != sizeof(child_ok)
                || memcmp(child_ok, "OK", 2) != 0) {
        kill(pid, SIGTERM);
        cut_FatalExit("the child failed to initialize itself properly");
    }

    cut_inputFeedPID = pid;
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
        cut_FatalExit("cannot replace standard input");

    // std* symbols cannot be (portably) assigned to, freopen() must be
    // used here
    cut_InputFile("/proc/self/fd/0");
    close(pipefd[0]);
    close(pipefd[1]);
}

CUT_PRIVATE int cut_IsDebugger() {
    const char *desired = "TracerPid:";
    const char *found = NULL;
    int tracerPid;
    int result = 0;
    int status = open("/proc/self/status", O_RDONLY);
    if (status < 0)
        return 0;

    char *buffer;
    size_t length;

    if (!cut_ReadWholeFile(status, &buffer, &length))
        goto cleanup;

    found = strstr(buffer, desired);
    if (!found)
        goto cleanup;

    if (!sscanf(found + strlen(desired), "%i", &tracerPid))
        goto cleanup;

    if (tracerPid)
        result = 1;
cleanup:
    free(buffer);
    close(status);
    return result;
}

CUT_PRIVATE int cut_PrintColorized(enum cut_Colors color, const char *text) {
    const char *prefix;
    const char *suffix = "\x1B[0m";
    switch (color) {
    case cut_YELLOW_COLOR:
        prefix = "\x1B[1;33m";
        break;
    case cut_RED_COLOR:
        prefix = "\x1B[1;31m";
        break;
    case cut_GREEN_COLOR:
        prefix = "\x1B[1;32m";
        break;
    default:
        prefix = "";
        suffix = "";
        break;
    }
    return fprintf(cut_output, "%s%s%s", prefix, text, suffix);
}
#endif // CUT_LINUX_H
#  elif defined(__APPLE__) || defined(__unix)
// 1hc substitution of /builds/pb071/cut/src/unix.h
#ifndef CUT_UNIX_H
#define CUT_UNIX_H

# include <sys/types.h>
# include <sys/sysctl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <assert.h>

// 1hc substitution of /builds/pb071/cut/src/unix-common.h
#if !defined(CUT_UNIX_H) && !defined(CUT_LINUX_H)
#   warning "this header should be included from unix.h or linux."
#endif

#ifndef CUT_UNIX_COMMON_H
#define CUT_UNIX_COMMON_H

# include <sys/types.h>
# include <errno.h>
# include <signal.h>

CUT_PRIVATE void cut_RedirectIO() {
    cut_outputsRedirected = 1;
    cut_stdout = tmpfile();
    cut_stderr = tmpfile();
    cut_originalStdIn = dup(STDIN_FILENO);
    cut_originalStdOut = dup(STDOUT_FILENO);
    cut_originalStdErr = dup(STDERR_FILENO);

    dup2(fileno(cut_stdout), STDOUT_FILENO);
    dup2(fileno(cut_stderr), STDERR_FILENO);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdout) != -1 || cut_FatalExit("cannot close file");
    fclose(cut_stderr) != -1 || cut_FatalExit("cannot close file");
    close(1) != -1 || cut_FatalExit("cannot close file");
    close(2) != -1 || cut_FatalExit("cannot close file");
    if (cut_inputFeedPID != 0)
        dup2(cut_originalStdIn, STDIN_FILENO);
    dup2(cut_originalStdOut, STDOUT_FILENO);
    dup2(cut_originalStdErr, STDERR_FILENO);

    // close resources to make Valgrind happy
    close(cut_originalStdErr);
    close(cut_originalStdOut);
    close(cut_originalStdIn);

    cut_outputsRedirected = 0;
}

CUT_PRIVATE void cut_InputProcess(size_t size, const char *bytes) {
    // silence SIGPIPE, we will detect it in write()
    signal(SIGPIPE, SIG_IGN);

    size_t cursor = 0;
    while (cursor < size) {
        ssize_t written = cut_Write(STDOUT_FILENO, bytes + cursor, size - cursor);

        if (written < 0) {
            switch (errno) {
                case EPIPE:
                    return;
                default:
                    cut_FatalExit("error while pumping input data");
            }
        }

        cursor += written;
    }
}

CUT_PRIVATE void cut_ReapInputFeed(void) {
    if (cut_inputFeedPID == 0)
        return;

    if (kill(cut_inputFeedPID, SIGTERM) == -1) {
        DEBUG_MSG("cannot kill input feed %d: %s", cut_inputFeedPID,
                strerror(errno));
        goto reset_pid;
    }

    int wstatus;
    if (waitpid(cut_inputFeedPID, &wstatus, 0) != cut_inputFeedPID) {
        DEBUG_MSG("cannot wait for input feed %d: %s", cut_inputFeedPID,
                strerror(errno));
        goto reset_pid;
    }

    if (!WIFEXITED(wstatus) && WTERMSIG(wstatus) != SIGTERM) {
        DEBUG_MSG("input feed %d signaled: %d", cut_inputFeedPID, WTERMSIG(wstatus));
    } else if (WEXITSTATUS(wstatus) != 0) {
        DEBUG_MSG("input feed %d failed: %d", cut_inputFeedPID, WEXITSTATUS(wstatus));
    }

reset_pid:
    cut_inputFeedPID = 0;
}

#endif // CUT_UNIX_COMMON_H

CUT_PRIVATE int cut_IsTerminalOutput() {
    return isatty(fileno(stdout));
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return write(fd, source, bytes);
}

CUT_PRIVATE int cut_PreRun() {
    return 0;
}

CUT_PRIVATE void cut_SigAlrm(CUT_UNUSED(int signum)) {
    cut_Timeouted();
    _exit(cut_NORMAL_EXIT);
}

CUT_PRIVATE void cut_RunUnit(int testId, int subtest, struct cut_UnitResult *result) {
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    if (r == -1)
        cut_FatalExit("cannot establish communication pipe");

    cut_pipeRead = pipefd[0];
    cut_pipeWrite = pipefd[1];

    int pid = getpid();
    int parentPid = getpid();
    pid = fork();
    if (pid == -1)
        cut_FatalExit("cannot fork");
    if (!pid) {
        /// TODO: missing feature - kill child when parent dies
        close(cut_pipeRead) != -1 || cut_FatalExit("cannot close file");

        if (cut_arguments.timeout) {
            signal(SIGALRM, cut_SigAlrm);
            alarm(cut_arguments.timeout);
        }
        cut_ExceptionBypass(testId, subtest);

        close(cut_pipeWrite) != -1 || cut_FatalExit("cannot close file");
        free(cut_unitTests.tests);
        free(cut_arguments.match);
        if (cut_arguments.output)
            fclose(cut_output);
        exit(cut_NORMAL_EXIT);
    }
    // parent process only
    int status = 0;
    close(cut_pipeWrite) != -1 || cut_FatalExit("cannot close file");
    cut_PipeReader(result);
    do {
        r = waitpid( pid, &status, 0 );
    } while (r == -1 && errno == EINTR);
    r != -1 || cut_FatalExit("cannot wait for unit");
    result->returnCode = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
    result->signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    result->failed |= result->returnCode ||  result->signal;
    close(cut_pipeRead) != -1 || cut_FatalExit("cannot close file");
}

CUT_PRIVATE int cut_ReadWholeFile(int fd, char **buffer, size_t *length) {
    int result = 0;
    size_t capacity = 16;
    *length = 0;
    *buffer = (char *) malloc(capacity);
    if (!*buffer)
        return 0;

    long offset = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);

    int rv;
    while ((rv = read(fd, *buffer + *length, capacity - *length - 1)) > 0) {
        *length += rv;
        if (*length + 1 == capacity) {
            capacity *= 2;
            char *newBuffer = (char *)realloc(*buffer, capacity);
            if (!newBuffer)
                break;
            *buffer = newBuffer;
        }
    }

    if (!rv)
        result = 1;
    (*buffer)[*length] = '\0';
cleanup:
    lseek(fd, offset, SEEK_SET);
    return result;
}

int cut_BinaryFile(FILE *f, size_t length, const unsigned char *content) {
    int result = 0;
    size_t fileLength;
    int fd = fileno(f);
    fflush(f);
    char *buf = NULL;

    if (!cut_ReadWholeFile(fd, &buf, &fileLength))
        cut_FatalExit("cannot read whole file");

    result = length == fileLength && memcmp(content, buf, length) == 0;
cleanup:
    free(buf);
    return result;
}

void cut_InputFile(const char *name) {
    if (freopen(name, "r", stdin) == NULL)
        cut_FatalExit("cannot replace standard input");

    clearerr(stdin);
}

void cut_Input(size_t size, const char *bytes) {
    int pipefd[2];
    if (pipe(pipefd) == -1)
        cut_FatalExit("cannot create input pipe");

    int parentPid = getpid();
    int pid = fork();
    if (pid == -1)
        cut_FatalExit("cannot create child process");

    if (pid == 0) {
        // child process

        // replace standard output, so that Valgrind will less likely complain
        // about open file descriptor
        if (!dup2(pipefd[1], STDOUT_FILENO))
            cut_FatalExit("cannot replace standard output");
        close(pipefd[1]);

        // mark current descriptor maximum
        int max_fd = dup(0);

        // here we only need to keep pipefd[1] and close other descriptors;
        // first close FILE* pointers
        fclose(cut_stderr);
        fclose(cut_stdout);

        // now we don't know which descriptors we still have, but we have
        // a maximum hint in max_fd, so we close all up to that descriptor
        // NOTE: this is a nasty hack and everyone who uses it in production
        //       code deserves to be spanked unconscious
        for (int fd = 3; fd <= max_fd; ++fd)
            close(fd);

        // signal the parent that we are ready
        write(STDOUT_FILENO, "OK", 2);

        cut_InputProcess(size, bytes);
        exit(cut_NORMAL_EXIT);
    }

    // parent process
    if (cut_inputFeedPID != 0)
        cut_FatalExit("input feed PID already in use");

    // wait for the child to initialize itself
    char child_ok[2];
    if (read(pipefd[0], child_ok, sizeof(child_ok)) != sizeof(child_ok)
                || memcmp(child_ok, "OK", 2) != 0) {
        kill(pid, SIGTERM);
        cut_FatalExit("the child failed to initialize itself properly");
    }

    cut_inputFeedPID = pid;
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
        cut_FatalExit("cannot replace standard input");

    // freopen() doesn't work here, but this does.
    // Unix does not necessarily have procfs, Mac has fpurge() though
    // that can be used here to revitalize stdin as much as possible
    fpurge(stdin);
    clearerr(stdin);
    close(pipefd[0]);
    close(pipefd[1]);
}

CUT_PRIVATE int cut_IsDebugger() {
    int                 junk;
    int                 mib[4];
    struct kinfo_proc   info;
    size_t              size;

    info.kp_proc.p_flag = 0;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    size = sizeof(info);
    junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    assert(junk == 0);


    return (info.kp_proc.p_flag & P_TRACED);
}

CUT_PRIVATE int cut_PrintColorized(enum cut_Colors color, const char *text) {
    const char *prefix;
    const char *suffix = "\x1B[0m";
    switch (color) {
    case cut_YELLOW_COLOR:
        prefix = "\x1B[1;33m";
        break;
    case cut_RED_COLOR:
        prefix = "\x1B[1;31m";
        break;
    case cut_GREEN_COLOR:
        prefix = "\x1B[1;32m";
        break;
    default:
        prefix = "";
        suffix = "";
        break;
    }
    return fprintf(cut_output, "%s%s%s", prefix, text, suffix);
}
#endif // CUT_UNIX_H
#  elif defined(_WIN32)
// 1hc substitution of /builds/pb071/cut/src/windows.h
#ifndef CUT_WINDOWS_H
#define CUT_WINDOWS_H

# include <Windows.h>
# include <io.h>
# include <fcntl.h>

CUT_PRIVATE HANDLE cut_jobGroup;

CUT_PRIVATE int cut_IsDebugger() {
    return IsDebuggerPresent();
}

CUT_PRIVATE int cut_IsTerminalOutput() {
    return _isatty(_fileno(stdout));
}

CUT_PRIVATE int cut_CreateTemporaryFile(FILE **file) {
    const char *name = tmpnam(NULL);
    while (*name == '/' || *name == '\\')
        ++name;
    *file = fopen(name, "w+TD");
    return !!*file;
}

CUT_PRIVATE void cut_RedirectIO() {
    cut_outputsRedirected = 1;
    cut_CreateTemporaryFile(&cut_stdout) || cut_FatalExit("cannot open temporary file");
    cut_CreateTemporaryFile(&cut_stderr) || cut_FatalExit("cannot open temporary file");
    cut_originalStdOut = _dup(1);
    cut_originalStdErr = _dup(2);
    _dup2(_fileno(cut_stdout), 1);
    _dup2(_fileno(cut_stderr), 2);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdout) != -1 || cut_FatalExit("cannot close file");
    fclose(cut_stderr) != -1 || cut_FatalExit("cannot close file");
    _close(1) != -1 || cut_FatalExit("cannot close file");
    _close(2) != -1 || cut_FatalExit("cannot close file");
    _dup2(cut_originalStdOut, 1);
    _dup2(cut_originalStdErr, 2);

    cut_outputsRedirected = 0;
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return _read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return _write(fd, source, bytes);
}

CUT_PRIVATE void NTAPI cut_TimerCallback(CUT_UNUSED(void *param), CUT_UNUSED(BOOLEAN timerEvent)) {
    cut_Timeouted();
    ExitProcess(cut_NORMAL_EXIT);
}

CUT_PRIVATE int cut_PreRun() {
    if (!cut_arguments.noFork && cut_arguments.testId < 0) {
        // create a group of processes to be able to kill unit when parent dies
		cut_jobGroup = CreateJobObject(NULL, NULL);
        if (!cut_jobGroup)
            cut_FatalExit("cannot create jobGroup object");
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(cut_jobGroup, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)) ||cut_FatalExit("cannot SetInformationJobObject");
    }
    if (cut_arguments.testId < 0)
        return 0;

    SetErrorMode(SEM_NOGPFAULTERRORBOX);

    HANDLE timer = NULL;
    if (cut_arguments.timeout) {
        CreateTimerQueueTimer(&timer, NULL, cut_TimerCallback, NULL,
                              cut_arguments.timeout * 1000, 0, WT_EXECUTEONLYONCE) || cut_FatalExit("cannot create timer");
    }

    cut_pipeWrite = _dup(1);
    _setmode(cut_pipeWrite, _O_BINARY);

    cut_ExceptionBypass(cut_arguments.testId, cut_arguments.subtestId);

    _close(cut_pipeWrite) != -1 || cut_FatalExit("cannot close file");

    if (timer) {
        DeleteTimerQueueTimer(NULL, timer, NULL) || cut_FatalExit("cannot delete timer");
    }

    return 1;
}


CUT_PRIVATE void cut_RunUnit(int testId, int subtest, struct cut_UnitResult *result) {

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(saAttr);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE parentStdInput = GetStdHandle(STD_INPUT_HANDLE);
    if (parentStdInput == INVALID_HANDLE_VALUE)
        cut_FatalExit("Cannot get standard input handle");

    HANDLE childOutWrite, childOutRead;

    CreatePipe(&childOutRead, &childOutWrite, &saAttr, 0);
    SetHandleInformation(childOutRead, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION procInfo;
    STARTUPINFOA startInfo;

    ZeroMemory(&procInfo, sizeof(procInfo));
    ZeroMemory(&startInfo, sizeof(startInfo));

    startInfo.cb = sizeof(startInfo);
    startInfo.dwFlags |= STARTF_USESTDHANDLES;
    startInfo.hStdOutput = childOutWrite;
    // we need to have open standard input for input-replacing macros.
    startInfo.hStdInput = parentStdInput;

    const char *fmtString = "\"%s\" --test %i --subtest %i --timeout %i";
    int length = snprintf(NULL, 0, fmtString, cut_arguments.selfName, testId, subtest,
                          cut_arguments.timeout);
    char *command = (char *)malloc(length + 1);
    sprintf(command, fmtString, cut_arguments.selfName, testId, subtest, cut_arguments.timeout);

    CreateProcessA(cut_arguments.selfName,
                   command,
                   NULL,
                   NULL,
                   TRUE,
                   CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED,
                   NULL,
                   NULL,
                   &startInfo,
                   &procInfo) || cut_FatalExit("cannot create process");
    free(command);

    AssignProcessToJobObject(cut_jobGroup, procInfo.hProcess) || cut_FatalExit("cannot assign process to job object");
    ResumeThread(procInfo.hThread) == 1 || cut_FatalExit("cannot resume thread");
	CloseHandle(childOutWrite) || cut_FatalExit("cannot close handle");

    cut_pipeRead = _open_osfhandle((intptr_t)childOutRead, 0);
    cut_PipeReader(result);

    WaitForSingleObject(procInfo.hProcess, INFINITE) == WAIT_OBJECT_0 || cut_FatalExit("cannot wait for single object");
    DWORD childResult;
    GetExitCodeProcess(procInfo.hProcess, &childResult) || cut_FatalExit("cannot get exit code");
    CloseHandle(procInfo.hProcess) || cut_FatalExit("cannot close handle");
    CloseHandle(procInfo.hThread) || cut_FatalExit("cannot close handle");

    result->returnCode = childResult;
    result->signal = 0;
    result->failed |= result->returnCode;

    _close(cut_pipeRead) != -1 || cut_FatalExit("cannot close file");
}

CUT_PRIVATE int cut_ReadWholeFile(int fd, char *buffer, size_t length) {
    while (length) {
        int64_t rv = _read(fd, buffer, length);
        if (rv < 0)
            return -1;
        buffer += rv;
        length -= (size_t)rv;
    }
    return 0;
}

int cut_BinaryFile(FILE *f, size_t length, const unsigned char *content) {
    int result = 0;
    _flushall();
    int fd = _fileno(f);
    char *buf = NULL;

    int prev_translation_mode = _setmode(fd, _O_BINARY);
    if (prev_translation_mode == -1)
        cut_FatalExit("cannot change translation mode");

    long offset = _lseek(fd, 0, SEEK_CUR);
    if ((size_t) _lseek(fd, 0, SEEK_END) != length)
        goto cleanup;

    _lseek(fd, 0, SEEK_SET);
    buf = (char*)malloc(length);
    if (!buf)
        cut_FatalExit("cannot allocate memory for file");
    if (cut_ReadWholeFile(fd, buf, length))
        cut_FatalExit("cannot read whole file");

    result = memcmp(content, buf, length) == 0;
cleanup:
    _lseek(fd, offset, SEEK_SET);
    if (_setmode(fd, prev_translation_mode) == -1)
        cut_FatalExit("cannot change translation mode");
    free(buf);
    return result;
}

void cut_InputFile(const char *name) {
    if (freopen(name, "r", stdin) == NULL)
        cut_FatalExit("cannot replace standard input");

    clearerr(stdin);
}

void cut_Input(size_t size, const char *bytes) {
    TCHAR tmpPath[MAX_PATH + 1];
    TCHAR tmpName[MAX_PATH + 1];

    if (GetTempPath(sizeof(tmpPath) - 1, tmpPath) == 0)
        cut_FatalExit("cannot determine Windows temporary directory path");

    if (GetTempFileName(tmpPath, "cut", 0, tmpName) == 0)
        cut_FatalExit("cannot get temporary file name");

    HANDLE tmpFile = CreateFile(tmpName, GENERIC_WRITE | GENERIC_READ, 0, NULL,
            OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);

    if (tmpFile == INVALID_HANDLE_VALUE)
        cut_FatalExit("cannot create temporary file");

    if (!WriteFile(tmpFile, bytes, size, NULL, NULL))
        cut_FatalExit("cannot write input to a temporary file");

    // we cannot open the temporary file held by tmpFile,
    // therefore we have to replace the input ourselfs.
    int fdTmpInput = _open_osfhandle((intptr_t)tmpFile, _O_RDONLY);
    if (fdTmpInput == -1)
        cut_FatalExit("cannot convert handle to file descriptor");
    cut_tmpInput = fdTmpInput;
   
    const int stdin_fd = _fileno(stdin);

    if (stdin_fd < 0)
        cut_FatalExit("cannot get file descriptor of standart input");

    if (_dup2(cut_tmpInput, stdin_fd) == -1)
        cut_FatalExit("cannot replace standart input with a temporary file");

    if (fseek(stdin, 0, SEEK_SET) != 0)
        cut_FatalExit("cannot set file position");
    
    clearerr(stdin);
}

CUT_PRIVATE void cut_ReapInputFeed(void) {
    if (cut_tmpInput >= 0)
        _close(cut_tmpInput);

    cut_tmpInput = -1;
}

CUT_PRIVATE int cut_PrintColorized(enum cut_Colors color, const char *text) {
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    WORD attributes = 0;

    GetConsoleScreenBufferInfo(stdOut, &info);
    switch (color) {
    case cut_YELLOW_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    case cut_RED_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
    case cut_GREEN_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        break;
    default:
        break;
    }
    if (attributes)
        SetConsoleTextAttribute(stdOut, attributes);
    int rv = fprintf(cut_output, "%s", text);
    if (attributes)
        SetConsoleTextAttribute(stdOut, info.wAttributes);
    return rv;
}

#endif // CUT_WINDOWS_H
#  else
#   error "unsupported platform"
#  endif

CUT_NORETURN int cut_FatalExit(const char *reason) {
    if (cut_outputsRedirected) {
        FILE *log = fopen(cut_emergencyLog, "w");
        if (log) {
            fprintf(log, "CUT internal error during unit test: %s\n", reason);
            fclose(log);
        }
    } else {
        fprintf(stderr, "CUT internal error: %s\n", reason);
    }
    exit(cut_FATAL_EXIT);
}

CUT_NORETURN int cut_ErrorExit(const char *reason, ...) {
    va_list args;
    va_start(args, reason);
    fprintf(stderr, "Error happened: ");
    vfprintf(stderr, reason, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(cut_ERROR_EXIT);
}


void cut_Register(cut_Instance instance, const char *name, const char *file, size_t line) {
    if (cut_unitTests.size == cut_unitTests.capacity) {
        cut_unitTests.capacity += 16;
        cut_unitTests.tests = (struct cut_UnitTest *)realloc(cut_unitTests.tests,
            sizeof(struct cut_UnitTest) * cut_unitTests.capacity);
        if (!cut_unitTests.tests)
            cut_FatalExit("cannot allocate memory for unit tests");
    }
    cut_unitTests.tests[cut_unitTests.size].instance = instance;
    cut_unitTests.tests[cut_unitTests.size].name = name;
    cut_unitTests.tests[cut_unitTests.size].file = file;
    cut_unitTests.tests[cut_unitTests.size].line = line;
    ++cut_unitTests.size;
}

void cut_RegisterGlobalTearUp(cut_GlobalTear instance) {
    if (cut_globalTearUp)
        cut_FatalExit("cannot overwrite tear up function");
    cut_globalTearUp = instance;
}

void cut_RegisterGlobalTearDown(cut_GlobalTear instance) {
    if (cut_globalTearDown)
        cut_FatalExit("cannot overwrite tear down function");
    cut_globalTearDown = instance;
}

CUT_DESTRUCTOR(destroy) {
    free(cut_unitTests.tests);
    cut_unitTests.tests = NULL;
}

CUT_PRIVATE void cut_ParseArguments(int argc, char **argv) {
    static const char *help = "--help";
    static const char *list_tests = "--list-tests";
    static const char *timeout = "--timeout";
    static const char *noFork = "--no-fork";
    static const char *doFork = "--fork";
    static const char *noColor = "--no-color";
    static const char *exactMatch = "--exact-match";
    static const char *output = "--output";
    static const char *subtest = "--subtest";
    static const char *exactTest = "--test";
    static const char *shortPath = "--short-path";
    cut_arguments.help = 0;
    cut_arguments.list_tests = 0;
    cut_arguments.timeout = CUT_TIMEOUT;
    cut_arguments.noFork = CUT_NO_FORK;
    cut_arguments.noColor = CUT_NO_COLOR;
    cut_arguments.exactMatch = 0;
    cut_arguments.output = NULL;
    cut_arguments.matchSize = 0;
    cut_arguments.match = NULL;
    cut_arguments.testId = -1;
    cut_arguments.subtestId = -1;
    cut_arguments.selfName = argv[0];
    cut_arguments.shortPath = -1;

    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--", 2)) {
            ++cut_arguments.matchSize;
            continue;
        }
        if (!strcmp(help, argv[i])) {
            cut_arguments.help = 1;
            continue;
        }
        if (!strcmp(list_tests, argv[i])) {
            cut_arguments.list_tests = 1;
            continue;
        }
        if (!strcmp(timeout, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%u", &cut_arguments.timeout))
                cut_ErrorExit("option %s requires numeric argument", timeout);
            continue;
        }
        if (!strcmp(noFork, argv[i])) {
            cut_arguments.noFork = 1;
            continue;
        }
        if (!strcmp(doFork, argv[i])) {
            cut_arguments.noFork = 0;
            continue;
        }
        if (!strcmp(noColor, argv[i])) {
            cut_arguments.noColor = 1;
            continue;
        }
        if (!strcmp(exactMatch, argv[i])) {
            cut_arguments.exactMatch = 1;
            continue;
        }
        if (!strcmp(output, argv[i])) {
            ++i;
            if (i < argc) {
                cut_arguments.output = argv[i];
                cut_arguments.noColor = 1;
            }
            else {
                cut_ErrorExit("option %s requires string argument", output);
            }
            continue;
        }
        if (!strcmp(exactTest, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &cut_arguments.testId))
                cut_ErrorExit("option %s requires numeric argument %d %d", exactTest, i, argc);
            continue;
        }
        if (!strcmp(subtest, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &cut_arguments.subtestId))
                cut_ErrorExit("option %s requires numeric argument", subtest);
            continue;
        }
        if (!strcmp(shortPath, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &cut_arguments.shortPath))
                cut_ErrorExit("option %s requires numeric argument", shortPath);
            continue;
        }
        cut_ErrorExit("option %s is not recognized", argv[i]);
    }
    if (!cut_arguments.matchSize)
        return;
    cut_arguments.match = (char **)malloc(cut_arguments.matchSize * sizeof(char *));
    if (!cut_arguments.match)
        cut_ErrorExit("cannot allocate memory for list of selected tests");
    int index = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "--", 2)) {
            if (!strcmp(timeout, argv[i]) || !strcmp(output, argv[i])
             || !strcmp(subtest, argv[i]) || !strcmp(exactTest, argv[i])
             || !strcmp(shortPath, argv[i]))
            {
                ++i;
            }
            continue;
        }
        cut_arguments.match[index++] = argv[i];
    }

    for (int i = 0; i < cut_arguments.matchSize; ++i) {
        const char *delimiter = strchr(cut_arguments.match[i], '/');
        if (delimiter == NULL)
            continue;
        if (delimiter == cut_arguments.match[i] || delimiter[1] == '\0')
            cut_ErrorExit("%s: invalid test filter", cut_arguments.match[i]);
    }
}

CUT_PRIVATE void cut_ChangePath(const char *exe_path) {
    char *path = (char *) malloc((strlen(exe_path) + 1) * sizeof(char));
    if (!path)
        cut_ErrorExit("cannot allocate memory for executable path");

    strcpy(path, exe_path);

    // We assume here that the path can only contain '/' or '\', but not both.
    char *delimiter = NULL;
    for (const char *d = "/\\"; delimiter == NULL && *d != '\0'; ++d)
        delimiter = strrchr(path, *d);

    // If there are no delimiters, the binary was called by its name,
    // do nothing.
    if (delimiter == NULL)
        goto release_path;

    *delimiter = '\0';
    if (strcmp(path, ".") == 0)
        goto release_path;

    if (chdir(path) == -1) {
        free(path);
        cut_ErrorExit("cannot change working directory to the executable path");
    }

release_path:
    free(path);
}

CUT_PRIVATE void cut_CleanInfo(struct cut_Info *info) {
    while (info) {
        struct cut_Info *current = info;
        info = info->next;
        free(current->message);
        free(current->file);
        free(current);
    }
}

CUT_PRIVATE void cut_CleanMemory(struct cut_UnitResult *result) {
    free(result->name);
    free(result->file);
    free(result->statement);
    free(result->exceptionType);
    free(result->exceptionMessage);
    cut_CleanInfo(result->debug);
    cut_CleanInfo(result->check);
}

CUT_PRIVATE int cut_Help() {
    const char *text = ""
    "Run as %s [OPTIONS] [TEST[/SUBTEST]]...\n"
    "\n"
    "Options:\n"
    "\t--help            Print out this help.\n"
    "\t--timeout <N>     Set timeout of each test in seconds. 0 for no timeout.\n"
    "\t--no-fork         Disable forking. Timeout is turned off.\n"
    "\t--fork            Force forking. Usefull during debugging with fork enabled.\n"
    "\t--no-color        Turn off colors.\n"
    "\t--exact-match     Match test and subtest names precisely.\n"
    "\t--output <file>   Redirect output to the file.\n"
    "\t--short-path <N>  Make filenames in the output shorter.\n"
    "\n"
    "Hidden options (for internal purposes only):\n"
    "\t--test <N>        Run test of index N.\n"
    "\t--subtest <N>     Run subtest of index N (for all tests).\n"
    "\t--list-tests      List all the available tests (without subtests).\n"
    "\n"
    "TEST - Any other parameter is accepted as a filter of test names.\n"
    "In case there is at least one filter parameter, a test is executed only if\n"
    "at least one of the filters is a substring of the test name.\n"
    "\n"
    "SUBTEST - If the test name is followed by a '/' character, then the\n"
    "rest of the string will be used to filter subtests of that test.\n"
    "Using '.' as a subtest name will allow to run the test without its subtests,\n"
    "which may be useful if the test contains some assertions outside of the subtests.\n"
    "Multiple filters can be combined, e.g. 'test/a test/b' will run test 'test'\n"
    "and all subtests matching either 'a' or 'b'.\n"
    "\n"
    "NOTE: Due to implementation technicalities, there are a few caveats\n"
    "involving subtest filters:\n"
    "  - If a TEST(T) has no SUBTEST() clauses, then 'T/...' argument\n"
    "    will be treated as if 'T' was provided instead.\n"
    "  - If a TEST(T) has at least one SUBTEST() clause and is limited with\n"
    "    an argument 'T/S', 'S' is not '.' and there is no SUBTEST(S) for this\n"
    "    test, the argument will be ignored.\n"
    "  - Do not combine subtest filters with '--test' or '--subtest' options.\n"
    "    They work on different levels and do not play nice together.\n"
    "In any case, uf no tests are run in the end, the test suite will fail\n"
    "with 'No tests were run!' and exit status of 255.\n"
    "\n";

    fprintf(cut_output, text, cut_arguments.selfName);
    return 0;
}

int main(int argc, char **argv) {
    int code = cut_Runner(argc, argv);
    // Make sure that if there are more than 255 failed tests, we cap the
    // exit status at 255.
    return code > 255 ? 255 : code;
}

#  undef CUT_PRIVATE

# endif // CUT_MAIN

# ifdef __cplusplus
} // extern "C"
# endif

#endif // CUT

#endif // CUT_CORE_H
