#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <curl/curl.h>

#include "j2_library/export.hpp"

namespace j2::network::ftp::client {

    // File information structure
    struct file_info {
        std::string name;
        long size;
        bool is_directory;
    };

    // Progress listener interface
    class i_progress_listener {
    public:
        virtual ~i_progress_listener() {}

        // Progress update callback
        // dlnow: bytes currently downloaded
        // dltotal: total bytes to download
        // ulnow: bytes currently uploaded
        // ultotal: total bytes to upload
        virtual void on_progress(curl_off_t dlnow, curl_off_t dltotal,
            curl_off_t ulnow, curl_off_t ultotal) = 0;
    };

    // Default progress listener
    class default_progress_listener : public i_progress_listener {
    public:
        void on_progress(curl_off_t dlnow, curl_off_t dltotal,
            curl_off_t ulnow, curl_off_t ultotal) override;
    };

    // Filtered progress listener (declaration)
    // - Wraps another i_progress_listener and forwards events to the inner listener
    //   only according to a forwarding policy (e.g. 1% change or a minimum time interval).
    class filtered_progress_listener : public i_progress_listener {
    private:
        // Prefer owning inner_shared; if absent, use inner_raw (non-owning).
        std::shared_ptr<i_progress_listener> inner_shared;
        i_progress_listener* inner_raw{ nullptr };

        int min_percent; // Minimum percent increase to forward (e.g. 1)
        int min_ms;      // Minimum time interval (ms) to forward even if percent hasn't changed

        int last_dl_percent;
        int last_ul_percent;
        std::chrono::steady_clock::time_point last_forward;

        i_progress_listener* inner() const noexcept {
            return inner_shared ? inner_shared.get() : inner_raw;
        }

    public:
        // default ctor (no target yet)
        filtered_progress_listener() noexcept;

        // non-owning constructor (raw pointer)
        explicit filtered_progress_listener(i_progress_listener* target,
            int min_percent_ = 1,
            int min_ms_ = 100) noexcept;

        // owning constructor (shared_ptr)
        explicit filtered_progress_listener(std::shared_ptr<i_progress_listener> target,
            int min_percent_ = 1,
            int min_ms_ = 100) noexcept;

        // set target later (non-owning)
        void set_target(i_progress_listener* target) noexcept;

        // set target later (owning)
        void set_target(std::shared_ptr<i_progress_listener> target) noexcept;

        // optionally allow changing policy after construction
        void set_policy(int min_percent_, int min_ms_) noexcept;

        // i_progress_listener override
        void on_progress(curl_off_t dlnow, curl_off_t dltotal,
            curl_off_t ulnow, curl_off_t ultotal) override;
    };

    // FTP/SFTP base class
    class ftp_client_base {
    protected:
        CURL* curl_handle;
        std::string host_name;
        int port;
        std::string user_name;
        std::string password;
        std::string last_error;
        i_progress_listener* progress_listener;

        static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);
        static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* stream);

        // CURL progress callback function
        static int progress_callback_internal(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
            curl_off_t ultotal, curl_off_t ulnow);

    public:
        ftp_client_base();
        virtual ~ftp_client_base();

        std::string get_last_error() const;

        // Register progress listener
        void set_progress_listener(i_progress_listener* listener);

        // Remove progress listener
        void remove_progress_listener();

        virtual bool connect(const std::string& host, int p,
            const std::string& user, const std::string& pass) = 0;
        virtual bool upload(const std::string& local_file, const std::string& remote_file) = 0;
        virtual bool download(const std::string& remote_file, const std::string& local_file) = 0;
        virtual bool delete_file(const std::string& remote_file) = 0;
        virtual std::vector<file_info> list_directory(const std::string& path) = 0;
        virtual bool create_directory(const std::string& path) = 0;
        virtual bool remove_directory(const std::string& path) = 0;
    };

    // FTP client class
    class ftp_client : public ftp_client_base {
    public:
        ftp_client();

        bool connect(const std::string& host, int p = 21,
            const std::string& user = "anonymous",
            const std::string& pass = "") override;

        bool upload(const std::string& local_file, const std::string& remote_file) override;
        bool download(const std::string& remote_file, const std::string& local_file) override;
        bool delete_file(const std::string& remote_file) override;
        std::vector<file_info> list_directory(const std::string& path) override;
        bool create_directory(const std::string& path) override;
        bool remove_directory(const std::string& path) override;
    };

    // SFTP client class
    class sftp_client : public ftp_client_base {
    public:
        sftp_client();

        bool connect(const std::string& host, int p = 22,
            const std::string& user = "",
            const std::string& pass = "") override;

        bool upload(const std::string& local_file, const std::string& remote_file) override;
        bool download(const std::string& remote_file, const std::string& local_file) override;
        bool delete_file(const std::string& remote_file) override;
        std::vector<file_info> list_directory(const std::string& path) override;
        bool create_directory(const std::string& path) override;
        bool remove_directory(const std::string& path) override;
    };


} // namespace j2::network::ftp::client
