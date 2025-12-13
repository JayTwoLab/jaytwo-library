
#include <iostream>

#include "j2_library/j2_library.hpp"

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
    downloader.set_progress_callback(&progress);
    downloader.set_delete_partial_file_on_fail(true);

    std::vector<std::string> files;
    std::string error;

    std::string url = "http://127.0.0.1:18080";
    std::string output_dir = ".";

    if (!downloader.download_multipart(url, output_dir, files, &error))
    {
        std::cout << "\n Failed: " << error << std::endl;
        return 1;
    }

    std::cout << "\n Success" << std::endl;
    return 0;
}
