#include "HttpContext.h"
#include "net/Buffer.h"
#include <algorithm>
#include <stdexcept>

std::string str_convert(const char *begin, const char *end) {
    std::string result;
    for (auto b = begin; b != end;) {
        if (*b == '%') {
            std::string _t(b + 1, b + 3);
            char _c = static_cast<char>(std::stoul(_t, nullptr, 16));
            result.push_back(_c);
            b += 3;
        } else {
            result.push_back(*b);
            b += 1;
        }
    }
    return result;
}

/*
 * HTTP 请求行
 * 请求方法 + 空格 + URL + 空格 + 协议版本 + \r + \n
 * example: 'GET /test.html HTTP/1.1\r\n'
 * */

bool HttpContext::processRequestLine(const char *begin, const char *end) {
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end) {
            const char *question = std::find(start, space, '?');
            if (question != space) {
                request_.setPath(str_convert(start, question));
                request_.setQuery(str_convert(question, space));
            } else {
                request_.setPath(str_convert(start, space));
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
            if (succeed) {
                if (*(end - 1) == '1') {
                    request_.setVersion(HttpRequest::Http11);
                } else if (*(end - 1) == '0') {
                    request_.setVersion(HttpRequest::Http10);
                } else {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

/*
 * HTTP请求头部
 * 头部字段名 + : + 值 + \r + \n
 * example: 'User-Agent: Mozilla/4.0 \r\n'
 * */

bool HttpContext::parseRequest(Buffer *buf, Timestamp receive_time) {
    bool ok = true;
    bool has_more = true;
    while (has_more) {
        if (state_ == ExpectRequestLine) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok) {
                    request_.setReceiveTime(receive_time);
                    buf->retrieve(std::distance(buf->peek(), crlf + 2));
                    state_ = ExpectHeaders;
                } else {
                    has_more = false;
                }
            } else {
                has_more = false;
            }
        } else if (state_ == ExpectHeaders) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                const char *colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(buf->peek(), colon, crlf);
                } else {
                    state_ = ExpectBody;
                    std::string len = request_.getHeader("Content-Length");
                    if (len.empty()) {
                        request_.setBodyLen(0);
                    } else {
                        try {
                            request_.setBodyLen(std::stoul(len));
                        } catch (std::invalid_argument &e) {
                            ok = false;
                            has_more = false;
                        }
                    }
                }
                buf->retrieve(std::distance(buf->peek(), crlf + 2));
            } else {
                has_more = false;
            }
        } else if (state_ == ExpectBody) {
            size_t read_len = request_.getBodyLen() - request_.getBody().size();
            if (buf->readableBytes() >= read_len) {
                request_.appendBody(buf->retrieveAsString(read_len));
                buf->retrieveAll();
                state_ = GotAll;
            } else {
                request_.appendBody(buf->retrieveAllAsString());
            }
            has_more = false;
        }
    }
    return ok;
}