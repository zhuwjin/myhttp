#ifndef MYHTTP_HTTPRESPONSE_H
#define MYHTTP_HTTPRESPONSE_H

#include "base/copyable.h"
#include <map>
#include <string>
class Buffer;

class HttpResponse : public copyable {
public:
    enum HttpStatusCode {
        Unknown,
        Ok = 200,
        MovedPermanently = 301,
        BadRequest = 400,
        NotFound = 404,
    };

    explicit HttpResponse(bool close)
        : status_code_(Unknown),
          close_connection_(close) {}

    void setStatusCode(HttpStatusCode code) {
        status_code_ = code;
    }

    void setStatusMessage(const std::string &message) {
        status_message_ = message;
    }

    void setCloseConnection(bool on) {
        close_connection_ = on;
    }

    bool closeConnection() const {
        return close_connection_;
    }

    void setContentType(const std::string &content_type) {
        addHeader("Content-Type", content_type);
    }

    void addHeader(const std::string &key, const std::string &value) {
        headers_[key] = value;
    }

    void setBody(const std::string &body) {
        body_ = body;
    }

    void appendToBuffer(Buffer *output) const;

private:
    std::map<std::string, std::string> headers_;
    HttpStatusCode status_code_;
    std::string status_message_;
    bool close_connection_;
    std::string body_;
};

#endif//MYHTTP_HTTPRESPONSE_H
