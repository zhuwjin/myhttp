#ifndef MYHTTP_HTTPCONTEXT_H
#define MYHTTP_HTTPCONTEXT_H
#include "HttpRequest.h"
#include "base/copyable.h"

class Buffer;

class HttpContext : public copyable {
public:
    enum HttpRequestParseState {
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        GotAll,
    };

    HttpContext() : state_(ExpectRequestLine) {}

    bool parseRequest(Buffer *buf, Timestamp receive_time);

    bool gotAll() const {
        return state_ == GotAll;
    }

    void reset() {
        state_ = ExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest &request() const {
        return request_;
    }

    HttpRequest &request() {
        return request_;
    }

private:
    bool processRequestLine(const char *begin, const char *end);

    HttpRequestParseState state_;
    HttpRequest request_;
};


#endif//MYHTTP_HTTPCONTEXT_H
