//
// Created by Deakin Simpson on 21/11/2026.
//

#pragma once
#include <string>
#include <unordered_map>
#include <variant>

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
        Method method;          /* http request method */
        std::string target;     /* path to URI */
        std::string version;    /* clients http version */
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

    struct StatusLine /* for server responses */
    {
        std::string version;

    };

    struct Message
    {
        std::variant<RequestLine, StatusLine> start_line;
    };


}
