//
// Created by Deakin Simpson on 21/11/2026.
//

#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace http_server::http
{
    enum class Method /* request methods */
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
    };

    struct RequestLine /* for client requests */
    {
        Method              method;     /* http request method */
        std::string_view    target;     /* path to URI */
        std::string_view    version;    /* clients http version */
    };

    /**
     * Not all of these (in fact very few) will be implemented,
     * however they are all here for fast, future implementation.
     */
    enum class StatusCode
    {
        Continue,               /* 100 */
        Switching_Protocols,    /* 101 */
        Ok,                     /* 200 */
        Created,                /* 201 */
        Accepted,               /* 202 */
        Non_Authoritative_Info, /* 203 */
        No_Content,             /* 204 */
        Reset_Connection,       /* 205 */
        Partial_Content,        /* 206 */
        Multiple_Choices,       /* 300 */
        Moved_Permanently,      /* 301 */
        Found,                  /* 302 */
        See_Other,              /* 303 */
        Not_Modified,           /* 304 */
        Use_Proxy,              /* 305 */
        Unused_306,             /* 306 */
        Temporary_Redirection,  /* 307 */
        Permanent_Redirection,  /* 308 */
        Bad_Request,            /* 400 */
        Unauthorized,           /* 401 */
        Payment_Required,       /* 402 */
        Forbidden,              /* 403 */
        Not_Found,              /* 404 */
        Method_Not_Allowed,     /* 405 */
        Not_Acceptable,         /* 406 */
        Proxy_Auth_Required,    /* 407 */
        Request_Timeout,        /* 408 */
        Conflict,               /* 409 */
        Gone,                   /* 410 */
        Length_Required,        /* 411 */
        Precondition_Failed,    /* 412 */
        Content_Too_Large,      /* 413 */
        URI_Too_Long,           /* 414 */
        Unsupported_Media_Type, /* 415 */
        Range_Not_Satisfiable,  /* 416 */
        Expectation_Failed,     /* 417 */
        Unused_418,             /* 418 */
        Misdirected_Request,    /* 421 */
        Unprocessable_Content,  /* 422 */
        Upgrade_Required,       /* 423 */
        Internal_Server_Error,  /* 500 */
        Not_Implemented,        /* 501 */
        Bad_Gateway,            /* 502 */
        Service_Unavailable,    /* 503 */
        Gateway_Timeout,        /* 504 */
        HTTP_Version_Not_Supp,  /* 505 */
    };

    const std::unordered_map<int, std::string> HTTP_STATUS_CODES = {
        /* 1xx - Informational */
        {100, "Continue"},
        {101, "Switching Protocols"},
        {102, "Processing"},
        {103, "Early Hints"},
        {104, "Upload Resumption Supported"},

        /* 2xx - Success */
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"},
        {208, "Already Reported"},
        {226, "IM Used"},

        /* 3xx - Redirection */
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {305, "Use Proxy"},
        {306, "Unused"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},

        /* 4xx - Client Error */
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Content Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "Unused"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Content"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {425, "Too Early"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},

        /* 5xx - Server Error */
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
    };

    static std::pair<int, std::string> get_status_code(const StatusCode status_code)
    {
        switch (status_code)
        {
            /* 1xx - Informational */
            case StatusCode::Continue:               return {100, "Continue"};
            case StatusCode::Switching_Protocols:    return {101, "Switching Protocols"};

            /* 2xx - Success */
            case StatusCode::Ok:                     return {200, "OK"};
            case StatusCode::Created:                return {201, "Created"};
            case StatusCode::Accepted:               return {202, "Accepted"};
            case StatusCode::Non_Authoritative_Info: return {203, "Non-Authoritative Information"};
            case StatusCode::No_Content:             return {204, "No Content"};
            case StatusCode::Reset_Connection:       return {205, "Reset Content"};
            case StatusCode::Partial_Content:        return {206, "Partial Content"};

            /* 3xx - Redirection */
            case StatusCode::Multiple_Choices:       return {300, "Multiple Choices"};
            case StatusCode::Moved_Permanently:      return {301, "Moved Permanently"};
            case StatusCode::Found:                  return {302, "Found"};
            case StatusCode::See_Other:              return {303, "See Other"};
            case StatusCode::Not_Modified:           return {304, "Not Modified"};
            case StatusCode::Use_Proxy:              return {305, "Use Proxy"};
            case StatusCode::Unused_306:             return {306, "Unused"};
            case StatusCode::Temporary_Redirection:  return {307, "Temporary Redirect"};
            case StatusCode::Permanent_Redirection:  return {308, "Permanent Redirect"};

            /* 4xx - Client Error */
            case StatusCode::Bad_Request:            return {400, "Bad Request"};
            case StatusCode::Unauthorized:           return {401, "Unauthorized"};
            case StatusCode::Payment_Required:       return {402, "Payment Required"};
            case StatusCode::Forbidden:              return {403, "Forbidden"};
            case StatusCode::Not_Found:              return {404, "Not Found"};
            case StatusCode::Method_Not_Allowed:     return {405, "Method Not Allowed"};
            case StatusCode::Not_Acceptable:         return {406, "Not Acceptable"};
            case StatusCode::Proxy_Auth_Required:    return {407, "Proxy Authentication Required"};
            case StatusCode::Request_Timeout:        return {408, "Request Timeout"};
            case StatusCode::Conflict:               return {409, "Conflict"};
            case StatusCode::Gone:                   return {410, "Gone"};
            case StatusCode::Length_Required:        return {411, "Length Required"};
            case StatusCode::Precondition_Failed:    return {412, "Precondition Failed"};
            case StatusCode::Content_Too_Large:      return {413, "Content Too Large"};
            case StatusCode::URI_Too_Long:           return {414, "URI Too Long"};
            case StatusCode::Unsupported_Media_Type: return {415, "Unsupported Media Type"};
            case StatusCode::Range_Not_Satisfiable:  return {416, "Range Not Satisfiable"};
            case StatusCode::Expectation_Failed:     return {417, "Expectation Failed"};
            case StatusCode::Unused_418:             return {418, "Unused"};
            case StatusCode::Misdirected_Request:    return {421, "Misdirected Request"};
            case StatusCode::Unprocessable_Content:  return {422, "Unprocessable Content"};
            case StatusCode::Upgrade_Required:       return {426, "Upgrade Required"};

            /* 5xx - Server Error */
            case StatusCode::Internal_Server_Error:  return {500, "Internal Server Error"};
            case StatusCode::Not_Implemented:        return {501, "Not Implemented"};
            case StatusCode::Bad_Gateway:            return {502, "Bad Gateway"};
            case StatusCode::Service_Unavailable:    return {503, "Service Unavailable"};
            case StatusCode::Gateway_Timeout:        return {504, "Gateway Timeout"};
            case StatusCode::HTTP_Version_Not_Supp:  return {505, "HTTP Version Not Supported"};

            default:                                 return {500, "Unknown Status Code"};
        }
    }

    struct StatusLine /* for server responses */
    {
        std::string version;

    };

    struct Message
    {
        std::variant<RequestLine, StatusLine> start_line;
        /* allows for no copying when reading the headers */
        std::vector<std::pair<std::string, std::string>> field_lines;
        std::vector<std::byte> body;
    };

}
