#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream> 

#include <curl/curl.h> 

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "j2_library/network/ftp/ftp_client.hpp"

using namespace j2::network::ftp::client;

// Custom Progress Listener 1: Progress Bar
class progress_bar_listener : public i_progress_listener {
public:
    void on_progress(curl_off_t dlnow, curl_off_t dltotal,
        curl_off_t ulnow, curl_off_t ultotal) override {

        // Download progress
        if (dltotal > 0) {
            double percentage = (static_cast<double>(dlnow) / dltotal) * 100.0;
            int bar_width = 50;
            int pos = static_cast<int>(bar_width * dlnow / dltotal);

            std::cout << "\r[Download] [";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% "
                << "(" << dlnow / 1024 << "KB / " << dltotal / 1024 << "KB)    " << std::flush;

            if (dlnow == dltotal && dltotal > 0) {
                std::cout << std::endl;
            }
        }

        // Upload progress
        if (ultotal > 0) {
            double percentage = (static_cast<double>(ulnow) / ultotal) * 100.0;
            int bar_width = 50;
            int pos = static_cast<int>(bar_width * ulnow / ultotal);

            std::cout << "\r[Upload  ] [";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% "
                << "(" << ulnow / 1024 << "KB / " << ultotal / 1024 << "KB)    " << std::flush;

            if (ulnow == ultotal && ultotal > 0) {
                std::cout << std::endl;
            }
        }
    }
};

// Custom Progress Listener 2: Advanced (Speed + ETA)
class advanced_progress_listener : public i_progress_listener {
private:
    std::chrono::steady_clock::time_point start_time;
    curl_off_t last_bytes;
    std::chrono::steady_clock::time_point last_time;

public:
    advanced_progress_listener() : last_bytes(0) {
        start_time = std::chrono::steady_clock::now();
        last_time = start_time;
    }

    void on_progress(curl_off_t dlnow, curl_off_t dltotal,
        curl_off_t ulnow, curl_off_t ultotal) override {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();

        // Download progress
        if (dltotal > 0 && elapsed >= 100) { // Update every 100ms
            double percentage = (static_cast<double>(dlnow) / dltotal) * 100.0;

            // Speed calculation         (KB/s)
            curl_off_t byte_diff = dlnow - last_bytes;
            double speed = (byte_diff / 1024.0) / (elapsed / 1000.0);

            // Remaining time estimation        
            double remaining_bytes = dltotal - dlnow;
            double eta = (speed > 0) ? (remaining_bytes / 1024.0) / speed : 0;

            int bar_width = 40;
            int pos = static_cast<int>(bar_width * dlnow / dltotal);

            std::cout << "\r[Download] [";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) std::cout << "#";
                else std::cout << "-";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% | "
                << dlnow / (1024 * 1024) << "MB / " << dltotal / (1024 * 1024) << "MB | "
                << std::setprecision(2) << speed << " KB/s | "
                << "ETA: " << static_cast<int>(eta) << "s     " << std::flush;

            last_bytes = dlnow;
            last_time = now;

            if (dlnow == dltotal && dltotal > 0) {
                auto total_time = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
                std::cout << "\nDownload complete! (Total time: " << total_time << " seconds)" << std::endl;
            }
        }

        // Upload progress
        if (ultotal > 0 && elapsed >= 100) {
            double percentage = (static_cast<double>(ulnow) / ultotal) * 100.0;

            curl_off_t byte_diff = ulnow - last_bytes;
            double speed = (byte_diff / 1024.0) / (elapsed / 1000.0);

            double remaining_bytes = ultotal - ulnow;
            double eta = (speed > 0) ? (remaining_bytes / 1024.0) / speed : 0;

            int bar_width = 40;
            int pos = static_cast<int>(bar_width * ulnow / ultotal);

            std::cout << "\r[Upload] [";
            for (int i = 0; i < bar_width; ++i) {
                if (i < pos) std::cout << "#";
                else std::cout << "-";
            }
            std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% | "
                << ulnow / (1024 * 1024) << "MB / " << ultotal / (1024 * 1024) << "MB | "
                << std::setprecision(2) << speed << " KB/s | "
                << "ETA: " << static_cast<int>(eta) << "s     " << std::flush;

            last_bytes = ulnow;
            last_time = now;

            if (ulnow == ultotal && ultotal > 0) {
                auto total_time = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
                std::cout << "\nUpload complete! (Total time: " << total_time << " seconds)" << std::endl;
            }
        }
    }
};

// Custom Progress Listener 3: Log File
class log_progress_listener : public i_progress_listener {
private:
    std::ofstream log_file;
    std::chrono::steady_clock::time_point start_time;

public:
    explicit log_progress_listener(const std::string& filename) {
        log_file.open(filename, std::ios::app);
        start_time = std::chrono::steady_clock::now();

        if (log_file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            log_file << "\n=== Transfer started at " << std::ctime(&time);
        }
    }

    ~log_progress_listener() {
        if (log_file.is_open()) {
            log_file << "=== Transfer ended ===\n" << std::endl;
            log_file.close();
        }
    }

    void on_progress(curl_off_t dlnow, curl_off_t dltotal,
        curl_off_t ulnow, curl_off_t ultotal) override {
        if (!log_file.is_open()) return;

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::tm tm_now;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_now, &now_time_t);
#else
        localtime_r(&now_time_t, &tm_now);
#endif

        std::ostringstream oss;
        oss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << now_ms.count();

        // Log download progress
        if (dltotal > 0) {
            double percentage = (static_cast<double>(dlnow) / dltotal) * 100.0;
            log_file << "[" << oss.str() << "] Download: "
                << std::fixed << std::setprecision(2) << percentage << "% "
                << "(" << dlnow << "/" << dltotal << " bytes)" << std::endl;
        }

        // Log upload progress
        if (ultotal > 0) {
            double percentage = (static_cast<double>(ulnow) / ultotal) * 100.0;
            log_file << "[" << oss.str() << "] Upload: "
                << std::fixed << std::setprecision(2) << percentage << "% "
                << "(" << ulnow << "/" << ultotal << " bytes)" << std::endl;
        }
    }
};

// Custom Progress Listener 4: Multi-File Progress Tracking
class multi_file_progress_listener : public i_progress_listener {
private:
    std::string current_file;
    int file_index;
    int total_files;

public:
    explicit multi_file_progress_listener(int total)
        : file_index(0), total_files(total) {
    }

    void set_current_file(const std::string& filename, int index) {
        current_file = filename;
        file_index = index;
    }

    void on_progress(curl_off_t dlnow, curl_off_t dltotal,
        curl_off_t ulnow, curl_off_t ultotal) override {

        // Download progress
        if (dltotal > 0) {
            double percentage = (static_cast<double>(dlnow) / dltotal) * 100.0;
            std::cout << "\r[File " << file_index << "/" << total_files << "] "
                << current_file << " - "
                << std::fixed << std::setprecision(1) << percentage << "%     " << std::flush;

            if (dlnow == dltotal && dltotal > 0) {
                std::cout << std::endl;
            }
        }

        // Upload progress
        if (ultotal > 0) {
            double percentage = (static_cast<double>(ulnow) / ultotal) * 100.0;
            std::cout << "\r[File " << file_index << "/" << total_files << "] "
                << current_file << " - "
                << std::fixed << std::setprecision(1) << percentage << "%     " << std::flush;

            if (ulnow == ultotal && ultotal > 0) {
                std::cout << std::endl;
            }
        }
    }
};

class spdlog_progress_listener : public i_progress_listener {
private:
    std::shared_ptr<spdlog::logger> logger;
    std::string logger_name;

public:
    spdlog_progress_listener() = default;

    // logger_name, console/file enable, file path, file options
    // max_file_size: bytes, max_files: backup count, daily: use daily file sink if true
    void setup_logger(const std::string& name,
        bool use_console,
        bool use_file,
        const std::string& file_path = "",
        size_t max_file_size = 10 * 1024 * 1024, // 10MB
        size_t max_files = 3,
        bool daily = false) {
        logger_name = name;
        std::vector<spdlog::sink_ptr> sinks;

        if (use_console) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(console_sink);
        }
        if (use_file && !file_path.empty()) {
            spdlog::sink_ptr file_sink;
            if (daily) {
                // Daily file sink: rotates at midnight, keeps all logs (no max_files)
                file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(file_path, 0, 0);
            }
            else {
                // Rotating file sink: rotates when file reaches max_file_size, keeps max_files backups
                file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_path, max_file_size, max_files);
            }
            sinks.push_back(file_sink);
        }
        if (!sinks.empty()) {
            logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %v");
            spdlog::register_logger(logger);
            logger->info("=== Transfer started ===");
        }
    }

    ~spdlog_progress_listener() {
        if (logger) {
            logger->info("=== Transfer ended ===\n");
            spdlog::drop(logger_name);
        }
    }

    void on_progress(curl_off_t dlnow, curl_off_t dltotal,
        curl_off_t ulnow, curl_off_t ultotal) override {
        // Download progress
        if (!logger) return;
        if (dltotal > 0) {
            double percentage = (static_cast<double>(dlnow) / dltotal) * 100.0;
            logger->info("Download: {:.2f}% ({} / {} bytes)", percentage, dlnow, dltotal);
        }
        // Upload progress
        if (ultotal > 0) {
            double percentage = (static_cast<double>(ulnow) / ultotal) * 100.0;
            logger->info("Upload: {:.2f}% ({} / {} bytes)", percentage, ulnow, ultotal);
        }
    }
};




int main() {

    // --- Print supported protocols (for debugging SFTP support) ---
    std::cout << "Supported protocols: ";
    curl_version_info_data* data = curl_version_info(CURLVERSION_NOW);
    if (data && data->protocols) {
        for (const char* const* proto = data->protocols; *proto; ++proto)
            std::cout << *proto << " " << std::endl;
    }
    std::cout << std::endl;
    // --------------------------------------------------------------

    std::cout << "=== FTP/SFTP Object-Based Progress Listener Example ===" << std::endl << std::endl;


    // Example 1: Basic Progress Listener
    if (false) { // set true to enable test

        std::cout << "[ Example 1: Basic Progress Listener ]" << std::endl;
        {
            ftp_client ftp_client;
            default_progress_listener default_listener;

            // create filtered with default ctor, then set target and policy
            filtered_progress_listener filtered_default;
            filtered_default.set_target(&default_listener); // non-owning
            filtered_default.set_policy(1, 100); // 1% or 100ms
            // NOTE: If set_policy(1,100), the log is not output 
            // because it does not pass to the internal listener
            // unless the integer percentage increases by more than 1% 
            // (or 100 ms has elapsed).

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                // Register filtered progress listener
                ftp_client.set_progress_listener(&filtered_default);

                std::cout << std::endl << "Downloading file... /remote/file.zip" << std::endl;
                if (ftp_client.download("/remote/file.zip", "downloaded_file.zip")) {
                    std::cout << "Download complete! /remote/file.zip" << std::endl;
                }
                else {
                    std::cerr << "Download failed: " << ftp_client.get_last_error() << std::endl;
                }

                // disable progress for next transfer
                std::cout << std::endl << "Downloading file... /remote/other.bin" << std::endl;
                ftp_client.remove_progress_listener();
                if (ftp_client.download("/remote/other.bin", "other.bin")) {
                    std::cout << "Download complete! /remote/other.bin" << std::endl;
                }
                else {
                    std::cerr << "Download failed: " << ftp_client.get_last_error() << std::endl;
                }

                // Download a non-existing file to demonstrate error handling
                std::cout << std::endl << "Downloading file... /not/exist.file" << std::endl;
                ftp_client.remove_progress_listener();
                if (ftp_client.download("/not/exist.file", "exist.file")) {
                    std::cout << "Download complete! /not/exist.file" << std::endl;
                }
                else {
                    std::cerr << "Download failed: " << ftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
    }

    // Example 2: Progress Bar Listener
    if (false) { // set true to enable test 
        sftp_client sftp_client;
        progress_bar_listener progress_bar;

        // default construct filtered and set target/policy
        filtered_progress_listener filtered_bar;
        filtered_bar.set_target(&progress_bar);
        filtered_bar.set_policy(1, 100);

        // --- Auto-create large_file.iso if it does not exist ---
        const std::string local_iso = "large_file.iso";
        if (!std::filesystem::exists(local_iso)) {
            std::ofstream ofs(local_iso, std::ios::binary);
            constexpr size_t dummy_size = 1 * 1024 * 1024;
            std::vector<char> buffer(1024, 'A');
            for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                size_t remaining = dummy_size - written;
                size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
            }
            ofs.close();
            std::cout << "[Info] Created dummy file: " << local_iso << " (" << dummy_size << " bytes)" << std::endl;
        }
        // -------------------------------------------------------

        if (sftp_client.connect("127.0.0.1", 2222, "username", "password")) {
            std::cout << "SFTP server connection successful!" << std::endl;

            // Register progress listener
            sftp_client.set_progress_listener(&filtered_bar);

            std::cout << "Uploading file..." << std::endl;
            if (sftp_client.upload(local_iso, "/remote/large_file.iso")) {
                std::cout << "Upload complete!" << std::endl;
            }
            else {
                std::cerr << "Upload failed: " << sftp_client.get_last_error() << std::endl;
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
    }


    // Example 3: Advanced Progress Listener (Speed  + ETA)
    if (false) { // set true to enable test 

        std::cout << "[ Example 3: Advanced Progress Listener ]" << std::endl;
        {
            ftp_client ftp_client;
            advanced_progress_listener advanced_progress;

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                // Register advanced progress listener
                ftp_client.set_progress_listener(&advanced_progress);

                std::cout << "Downloading large file..." << std::endl;
                if (ftp_client.download("/remote/huge_file.tar.gz", "huge_file.tar.gz")) {
                    std::cout << "Operation complete!" << std::endl;
                }
                else {
                    std::cerr << "Operation failed: " << ftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;

    }

    // Example 4: Log File Progress Listener
    if (false) { // set true to enable test 

        std::cout << "[ Example 4: Log File Listener ]" << std::endl;
        {
            ftp_client ftp_client;
            log_progress_listener log_listener("transfer.log");

            // --- If document.pdf does not exist, create a dummy file ---
            const std::string local_doc = "document.pdf";
            if (!std::filesystem::exists(local_doc)) {
                std::ofstream ofs(local_doc, std::ios::binary);
                // 512KB dummy file (adjust size if needed)
                constexpr size_t dummy_size = 512 * 1024;
                std::vector<char> buffer(1024, 'B');
                for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                    size_t remaining = dummy_size - written;
                    size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                    ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
                }
                ofs.close();
                std::cout << "[Info] Created dummy file: " << local_doc << " (" << dummy_size << " bytes)" << std::endl;
            }
            // ------------------------------------------------

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                // Register log listener
                ftp_client.set_progress_listener(&log_listener);

                std::cout << "Uploading file (recorded in log file)..." << std::endl;
                if (ftp_client.upload(local_doc, "/remote/document.pdf")) {
                    std::cout << "Upload complete! (Check transfer.log file)" << std::endl;
                }
                else {
                    std::cerr << "Upload failed: " << ftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;

    }


    // Example 5: Multi-file Progress Tracking
    if (false) { // set true to enable test 

        std::cout << "[ Example 5: Multi-file Progress Listener ]" << std::endl;
        {
            ftp_client ftp_client;

            std::vector<std::string> files = {
                "file1.zip",
                "file2.tar.gz",
                "file3.iso"
            };

            // --- If file does not exist, create a dummy file ---
            for (const auto& fname : files) {
                if (!std::filesystem::exists(fname)) {
                    std::ofstream ofs(fname, std::ios::binary);
                    // Create file with different size by extension (example)
                    size_t dummy_size = 0;
                    if (fname.find(".zip") != std::string::npos) dummy_size = 256 * 1024;      // 256KB
                    else if (fname.find(".tar.gz") != std::string::npos) dummy_size = 512 * 1024; // 512KB
                    else if (fname.find(".iso") != std::string::npos) dummy_size = 1 * 1024 * 1024; // 1MB
                    else dummy_size = 128 * 1024; // default

                    std::vector<char> buffer(1024, 'C');
                    for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                        size_t remaining = dummy_size - written;
                        size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                        ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
                    }
                    ofs.close();
                    std::cout << "[Info] Created dummy file: " << fname << " (" << dummy_size << " bytes)" << std::endl;
                }
            }
            // -----------------------------------

            multi_file_progress_listener multi_progress(static_cast<int>(files.size()));

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                ftp_client.set_progress_listener(&multi_progress);

                for (size_t i = 0; i < files.size(); ++i) {
                    multi_progress.set_current_file(files[i], static_cast<int>(i) + 1);

                    if (ftp_client.upload(files[i], "/remote/" + files[i])) {
                        // Success
                        std::cout << "Upload complete: " << files[i] << std::endl;
                    }
                    else {
                        std::cerr << "Upload failed: " << files[i] << std::endl;
                    }
                }

                std::cout << "All files uploaded successfully!" << std::endl;
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;

    }


    // Example 6: Remove Progress Listener (No Progress Display)
    if (false) { // set true to enable test 

        std::cout << "[ Example 6: No Progress Display ]" << std::endl;
        {
            ftp_client ftp_client;

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                // No progress listener (or call remove_progress_listener())

                std::cout << "Downloading file (no progress display)..." << std::endl;
                if (ftp_client.download("/remote/file.txt", "file.txt")) {
                    std::cout << "Download complete!" << std::endl;
                }
                else {
                    std::cerr << "Download failed: " << ftp_client.get_last_error() << std::endl;
                }
            }
        }

    }

    // Example 7: spdlog Progress Listener
    if (false) { // set true to enable test

        std::cout << "[ Example 7: spdlog Progress Listener ]" << std::endl;
        {
            sftp_client sftp_client;
            spdlog_progress_listener spdlog_listener;

            // Setup logger: console + rotating file
            // Parameters: logger_name, use_console, use_file, file_path, max_file_size, max_files, daily
            spdlog_listener.setup_logger(
                "sftp_transfer",           // Logger name
                true,                       // Console output enabled
                true,                       // File output enabled
                "logs/transfer.log",        // Log file path
                5 * 1024 * 1024,           // Max file size: 5MB
                3,                          // Keep 3 backup files
                false                       // Use rotating file sink (not daily)
            );

            // --- Create dummy file if it doesn't exist ---
            const std::string local_video = "video.mp4";
            if (!std::filesystem::exists(local_video)) {
                std::ofstream ofs(local_video, std::ios::binary);
                // 2MB dummy file
                constexpr size_t dummy_size = 2 * 1024 * 1024;
                std::vector<char> buffer(4096, 'V');
                for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                    size_t remaining = dummy_size - written;
                    size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                    ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
                }
                ofs.close();
                std::cout << "[Info] Created dummy file: " << local_video
                    << " (" << dummy_size << " bytes)" << std::endl;
            }
            // ---------------------------------------------

            if (sftp_client.connect("127.0.0.1", 2222, "username", "password")) {
                std::cout << "SFTP server connection successful!" << std::endl;

                // Register spdlog listener
                sftp_client.set_progress_listener(&spdlog_listener);

                std::cout << "Uploading file (logged with spdlog)..." << std::endl;
                if (sftp_client.upload(local_video, "/remote/video.mp4")) {
                    std::cout << "Upload complete! (Check logs/transfer.log file)" << std::endl;
                }
                else {
                    std::cerr << "Upload failed: " << sftp_client.get_last_error() << std::endl;
                }

                std::cout << "\nDownloading file (logged with spdlog)..." << std::endl;
                if (sftp_client.download("/remote/video.mp4", "downloaded_video.mp4")) {
                    std::cout << "Download complete! (Check logs/transfer.log file)" << std::endl;
                }
                else {
                    std::cerr << "Download failed: " << sftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
    }

    // Example 7-2: spdlog with Daily File Sink
    if (false) { // set true to enable test

        std::cout << "[ Example 7-2: spdlog Daily File Sink ]" << std::endl;
        {
            ftp_client ftp_client;
            spdlog_progress_listener daily_logger;

            // Setup logger with daily file rotation (rotates at midnight)
            daily_logger.setup_logger(
                "ftp_daily_transfer",      // Logger name
                true,                       // Console output enabled
                true,                       // File output enabled
                "logs/daily_transfer.log",  // Log file path (will append date automatically)
                0,                          // Not used for daily sink
                0,                          // Not used for daily sink
                true                        // Use daily file sink
            );

            // --- Create dummy file if it doesn't exist ---
            const std::string local_backup = "backup.tar.gz";
            if (!std::filesystem::exists(local_backup)) {
                std::ofstream ofs(local_backup, std::ios::binary);
                // 1.5MB dummy file
                constexpr size_t dummy_size = 1536 * 1024;
                std::vector<char> buffer(4096, 'D');
                for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                    size_t remaining = dummy_size - written;
                    size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                    ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
                }
                ofs.close();
                std::cout << "[Info] Created dummy file: " << local_backup
                    << " (" << dummy_size << " bytes)" << std::endl;
            }
            // ---------------------------------------------

            if (ftp_client.connect("127.0.0.1", 2121, "username", "password")) {
                std::cout << "FTP server connection successful!" << std::endl;

                // Register daily logger
                ftp_client.set_progress_listener(&daily_logger);

                std::cout << "Uploading backup file (logged daily)..." << std::endl;
                if (ftp_client.upload(local_backup, "/remote/backup.tar.gz")) {
                    std::cout << "Upload complete! (Check logs/daily_transfer.log)" << std::endl;
                }
                else {
                    std::cerr << "Upload failed: " << ftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
    }

    // Example 7-3: spdlog with File Only (No Console)
    if (false) { // set true to enable test

        std::cout << "[ Example 7-3: spdlog File Only (Silent Mode) ]" << std::endl;
        {
            sftp_client sftp_client;
            spdlog_progress_listener silent_logger;

            // Setup logger with file only (no console output)
            silent_logger.setup_logger(
                "sftp_silent",              // Logger name
                false,                      // Console output disabled
                true,                       // File output enabled
                "logs/silent_transfer.log", // Log file path
                10 * 1024 * 1024,          // Max file size: 10MB
                5,                          // Keep 5 backup files
                false                       // Use rotating file sink
            );

            // --- Create dummy file if it doesn't exist ---
            const std::string local_data = "data.db";
            if (!std::filesystem::exists(local_data)) {
                std::ofstream ofs(local_data, std::ios::binary);
                // 3MB dummy file
                constexpr size_t dummy_size = 3 * 1024 * 1024;
                std::vector<char> buffer(4096, 'S');
                for (size_t written = 0; written < dummy_size; written += buffer.size()) {
                    size_t remaining = dummy_size - written;
                    size_t to_write = (buffer.size() < remaining) ? buffer.size() : remaining;
                    ofs.write(buffer.data(), static_cast<std::streamsize>(to_write));
                }
                ofs.close();
                std::cout << "[Info] Created dummy file: " << local_data
                    << " (" << dummy_size << " bytes)" << std::endl;
            }
            // ---------------------------------------------

            if (sftp_client.connect("127.0.0.1", 2222, "username", "password")) {
                std::cout << "SFTP server connection successful!" << std::endl;

                // Register silent logger (file only)
                sftp_client.set_progress_listener(&silent_logger);

                std::cout << "Uploading file in silent mode (no console output)..." << std::endl;
                if (sftp_client.upload(local_data, "/remote/data.db")) {
                    std::cout << "Upload complete! (Check logs/silent_transfer.log)" << std::endl;
                }
                else {
                    std::cerr << "Upload failed: " << sftp_client.get_last_error() << std::endl;
                }
            }
        }

        std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
    }

    // Example 8: List Directory Entries
    if (false) { // set true to enable test

        ftp_client client;
        if (client.connect("127.0.0.1", 2121, "username", "password")) {

            std::string root_path = "/";

            try {
                auto entries = client.list_directory(root_path);
                std::cout << "Entries in '" << root_path << "':\n";
                for (const auto& e : entries) {
                    std::cout << (e.is_directory ? "[DIR]  " : "[FILE] ")
                        << e.name << "    size=" << e.size << "\n";
                }

                // Entries in '/':
                // [DIR]  pub            size=0
                // [FILE] README.txt     size=1024
                // [FILE] image.png      size=204800
                // [DIR]  docs           size=0
                //  ... 
            }
            catch (const std::exception& ex) {
                std::cerr << "list_directory failed: " << ex.what() << "\n";
            }
        }
    }

    return 0;
}
