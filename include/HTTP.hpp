//
// Created by Deakin Simpson on 21/11/2026.
//

#pragma once
#include <string>
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

    struct StatusLine /* for server responses */
    {
        std::string version;

    };

    struct Message
    {
        std::variant<RequestLine, StatusLine> start_line;
    };


}
