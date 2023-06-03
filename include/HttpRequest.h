#ifndef MYHTTP_HTTPREQUEST_H
#define MYHTTP_HTTPREQUEST_H

#include "base/Timestamp.h"
#include <algorithm>
#include <map>
#include <optional>
#include <string>

class HttpRequest : public copyable {
public:
    enum Method {
        Invalid,
        Get,
        Post,
        Head,
        Put,
        Delete
    };
    enum Version {
        Unknown,
        Http10,
        Http11
    };

    HttpRequest() : method_(Invalid), version_(Unknown) {}

    void setVersion(Version v) {
        version_ = v;
    }

    Version getVersion() const {
        return version_;
    }

    bool setMethod(const char *start, const char *end) {
        std::string m(start, end);
        if (m == "GET") {
            method_ = Get;
        } else if (m == "POST") {
            method_ = Post;
        } else if (m == "HEAD") {
            method_ = Head;
        } else if (m == "PUT") {
            method_ = Put;
        } else if (m == "DELETE") {
            method_ = Delete;
        } else {
            method_ = Invalid;
        }
        return method_ != Invalid;
    }

    Method method() const {
        return method_;
    }

    const char *methodString() const {
        const char *result = "UNKNOWN";
        switch (method_) {
            case Get:
                result = "GET";
                break;
            case Post:
                result = "POST";
                break;
            case Head:
                result = "HEAD";
                break;
            case Put:
                result = "PUT";
                break;
            case Delete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char *start, const char *end) {
        path_.assign(start, end);
    }

    void setPath(const std::string &str) {
        path_.assign(str);
    }

    const std::string &path() const {
        return path_;
    }

    void setQuery(const std::string &str) {
        auto query_start = str.cbegin();
        auto query_end = str.cend();
        for (auto qm = std::find(query_start, query_end, '?'); qm != query_end;) {
            auto qm_next = std::find(qm + 1, query_end, '?');
            auto eq = std::find(qm, qm_next, '=');
            std::string key(qm + 1, eq);
            std::string value(eq + 1, qm_next);
            queries_[key] = value;
            qm = qm_next;
        }
    }

    std::optional<std::string> getQuery(const std::string &key) const {
        auto _t = queries_.find(key);
        if (_t == queries_.end()) {
            return std::nullopt;
        } else {
            return _t->second;
        }
    }

    void setReceiveTime(Timestamp t) {
        receive_time_ = t;
    }

    void addHeader(const char *start, const char *colon, const char *end) {
        std::string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon)) {
            ++colon;//去除值前面的空格
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1])) {
            value.resize(value.size() - 1);//去除值后面的空格
        }
        headers_[field] = value;
    }

    std::string getHeader(const std::string &field) const {
        std::string result;
        auto it = headers_.find(field);
        if (it != headers_.end()) {
            result = it->second;
        }
        return result;
    }

    const std::map<std::string, std::string> &headers() const {
        return headers_;
    }

    void swap(HttpRequest &other) {
        std::swap(method_, other.method_);
        std::swap(version_, other.version_);
        path_.swap(other.path_);
        queries_.swap(other.queries_);
        body_.swap(other.body_);
        receive_time_.swap(other.receive_time_);
        headers_.swap(other.headers_);
    }

    const std::string &getBody() const {
        return body_;
    }

    void appendBody(const std::string &str) {
        body_.append(str);
    }

    size_t getBodyLen() const {
        return body_len_;
    }

    void setBodyLen(size_t len) {
        body_len_ = len;
    }

private:
    Method method_;
    Version version_;
    std::string path_;
    std::map<std::string, std::string> queries_;
    std::string body_;
    size_t body_len_;
    Timestamp receive_time_;
    std::map<std::string, std::string> headers_;
};

#endif//MYHTTP_HTTPREQUEST_H
