#include <map>
#include <az_core.h>

std::map<int, const char *> az_result_codes {
    // === Core: Success results ====
    /// Success.
    {_az_RESULT_MAKE_SUCCESS(_az_FACILITY_CORE, 0), "AZ_OK"},

    // === Core: Error results ===
    /// A context was canceled, and a function had to return before result was ready.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 0), "AZ_ERROR_CANCELED"},

    /// Input argument does not comply with the expected range of values.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 1), "AZ_ERROR_ARG"},

    /// The destination size is too small for the operation.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 2), "AZ_ERROR_NOT_ENOUGH_SPACE"},

    /// Requested functionality is not implemented.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 3), "AZ_ERROR_NOT_IMPLEMENTED"},

    /// Requested item was not found.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 4), "AZ_ERROR_ITEM_NOT_FOUND"},

    /// Input can't be successfully parsed.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 5), "AZ_ERROR_UNEXPECTED_CHAR"},

    /// Unexpected end of the input data.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 6), "AZ_ERROR_UNEXPECTED_END"},

    /// Not supported.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 7), "AZ_ERROR_NOT_SUPPORTED"},

    /// An external dependency required to perform the operation was not provided. The operation needs
    /// an implementation of the platform layer or an HTTP transport adapter.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE, 8), "AZ_ERROR_DEPENDENCY_NOT_PROVIDED"},

    // === Platform ===
    /// Dynamic memory allocation request was not successful.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_PLATFORM, 1), "AZ_ERROR_OUT_OF_MEMORY"},

    // === JSON error codes ===
    /// The kind of the token being read is not compatible with the expected type of the value.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_JSON, 1), "AZ_ERROR_JSON_INVALID_STATE"},

    /// The JSON depth is too large.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_JSON, 2), "AZ_ERROR_JSON_NESTING_OVERFLOW"},

    /// No more JSON text left to process.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_JSON, 3), "AZ_ERROR_JSON_READER_DONE"},

    // === HTTP error codes ===
    /// The #az_http_response instance is in an invalid state.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 1), "AZ_ERROR_HTTP_INVALID_STATE"},

    /// HTTP pipeline is malformed.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 2), "AZ_ERROR_HTTP_PIPELINE_INVALID_POLICY"},

    /// Unknown HTTP method verb.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 3), "AZ_ERROR_HTTP_INVALID_METHOD_VERB"},

    /// Authentication failed.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 4), "AZ_ERROR_HTTP_AUTHENTICATION_FAILED"},

    /// HTTP response overflow.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 5), "AZ_ERROR_HTTP_RESPONSE_OVERFLOW"},

    /// Couldn't resolve host.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 6), "AZ_ERROR_HTTP_RESPONSE_COULDNT_RESOLVE_HOST"},

    /// Error while parsing HTTP response header.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 7), "AZ_ERROR_HTTP_CORRUPT_RESPONSE_HEADER"},

    /// There are no more headers within the HTTP response payload.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 8), "AZ_ERROR_HTTP_END_OF_HEADERS"},

    // === HTTP Adapter error codes ===
    /// Generic error in the HTTP transport adapter implementation.
    {_az_RESULT_MAKE_ERROR(_az_FACILITY_CORE_HTTP, 9), "AZ_ERROR_HTTP_ADAPTER"},
};
