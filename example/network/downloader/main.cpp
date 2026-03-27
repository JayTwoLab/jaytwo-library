
#include <iostream>

#include "j2_library/j2_library.hpp"

// 진행 상태를 콘솔에 출력하는 콜백 클래스
class console_progress : public j2::network::downloader::multipart_progress_callback
{
public:
    bool on_progress(long long total, long long now) override
    {
        if (total > 0)
        {
            long double pct = (static_cast<long double>(now) * 100.0L) / static_cast<long double>(total);
            std::cout << "\r" << std::fixed << std::setprecision(1) << pct << "%";
        }
        else
        {
            std::cout << "\r" << now << " bytes";
        }
        return true;
    }   
};

int main()
{

    j2::network::downloader::multipart_downloader downloader;

    console_progress progress;
    downloader.set_progress_callback(&progress); // 진행 상태 콜백 설정

    downloader.set_delete_partial_file_on_fail(true); // 다운로드 실패 시 임시 파일 삭제

    std::string url = "http://127.0.0.1:18080";
    std::string output_dir = "."; // 현재 디렉토리에 다운로드

    std::vector<std::string> files; // 다운로드된 파일 경로를 저장할 벡터
    std::string error; // 오류 메시지를 저장할 문자열

    // 멀티파트 다운로드 시도
    if (!downloader.download_multipart(url, output_dir, files, &error))
    {
        std::cout << "\n Failed: " << error << std::endl;
        return 1;
    }

    std::cout << "\n Success" << std::endl;
    return 0;
}
