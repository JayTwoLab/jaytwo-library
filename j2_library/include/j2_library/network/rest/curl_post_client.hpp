#if 0 // disabled for now 

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <mutex>

#include <curl/curl.h>

#include "j2_library/export.hpp"

namespace j2::network::rest {

class J2LIB_API curl_post_client
{
public:
    enum class http_status
    {
        unknown = 0,
        ok = 200,
        created = 201,
        no_content = 204,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        bad_gateway = 502,
        service_unavailable = 503
    };

    enum class result_code
    {
        ok,
        curl_timeout,
        curl_ssl_error,
        curl_network_error,
        curl_other_error,

        http_client_error_4xx,
        http_not_found,
        http_server_error_5xx,
        http_redirect_3xx,
        http_other_error,

        unknown_error
    };

    struct response
    {
        http_status  status = http_status::unknown;
        long         raw_status_code = 0;
        int          curl_error_code = 0;
        std::string  body;
        std::string  error;
        std::vector<std::string> headers;
        std::string content_type;

        bool is_success() const
        {
            if (!error.empty()) return false;
            return (raw_status_code >= 200 && raw_status_code < 300);
        }
    };

    using query_params = std::vector<std::pair<std::string, std::string>>;
    using headers = std::vector<std::pair<std::string, std::string>>;

    curl_post_client();
    ~curl_post_client();

    void set_server(const std::string& scheme,
        const std::string& host,
        long port,
        const std::string& path);

    void set_headers(const headers& headers);
    void set_timeout_ms(long timeout_ms);

    // Set whether to ignore SSL certificate errors
    void set_ignore_ssl_errors(bool ignore);
 
    // ------------------------------
    // POST: Exception version / noexcept version
    //  - The body string is sent as is (JSON, XML, text, etc.)
    //  - The Content-Type header is set via set_headers()
    // ------------------------------
    response post(const std::string& body);
    result_code post(const std::string& body, response& out_resp) noexcept;

    static result_code classify(const response& resp);

private:
    CURL* curl_ = nullptr;

    std::string scheme_ = "http";
    std::string host_;
    long        port_ = 0;
    std::string path_;

    headers headers_;
    long timeout_ms_ = 30000;   // 30 seconds

    bool ignore_ssl_errors_ = false;

    static void global_init();
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
    static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata);

    std::string build_url(const query_params& query_params);
    static std::vector<std::string> build_header_lines(const headers& headers);
    static http_status to_http_status(long code);
};


} // namespace j2::network::rest

 
#endif
