#pragma once

#include <string>
#include <vector>

#include "j2_library/export.hpp"

namespace j2::network::downloader
{

    // 진행 현황 콜백 인터페이스
    class J2LIB_API multipart_progress_callback
    {
    public:
        virtual ~multipart_progress_callback() = default;

        // total_bytes: 전체 크기 (알 수 없으면 0)
        // now_bytes  : 현재까지 받은 바이트
        // false 반환 시 다운로드 중단
        virtual bool on_progress(long long total_bytes, long long now_bytes) = 0;
    };

    // multipart/form-data 응답을 다운로드하는 클래스
    class J2LIB_API multipart_downloader
    {
    public:
        multipart_downloader();
        ~multipart_downloader();

        // 진행 콜백 등록 (nullptr 이면 비활성)
        void set_progress_callback(multipart_progress_callback* cb);

        // 다운로드 실패 시 임시 파일(.part) 삭제 여부 설정
        void set_delete_partial_file_on_fail(bool enable);

        // multipart/form-data 응답 다운로드
        bool download_multipart(const std::string& url,
            const std::string& output_dir,
            std::vector<std::string>& saved_files,
            std::string* error_message = nullptr);

    public:
        multipart_progress_callback* progress_cb_{ nullptr };

    private:
        void ensure_global_init();

        bool delete_partial_on_fail_{ true };

        multipart_downloader(const multipart_downloader&) = delete;
        multipart_downloader& operator=(const multipart_downloader&) = delete;
    };

} // namespace j2::network::downloader
