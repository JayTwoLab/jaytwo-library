#include "j2_library/system/resource_monitor.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>

using namespace j2::system;

int main() {
    std::cout << "--- j2::system::ResourceMonitor Demo ---" << std::endl;
    std::cout << "Monitoring system resources... (Press Ctrl+C to stop)" << std::endl;

#ifdef _WIN32
    std::string root_path = "C:\\";
#else
    std::string root_path = "/";
#endif

    // 코어별 사용률 초기화(첫 호출은 0.0이므로 값을 채우기 위해 한 번 호출하고 대기)
    ResourceMonitor::get_cpu_usage_per_core();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true) {
        // 현재 시간 (스레드 안전)
        std::time_t now = std::time(nullptr);
        std::tm local_tm;
    #ifdef _WIN32
        localtime_s(&local_tm, &now);
    #else
        localtime_r(&now, &local_tm);
    #endif
        std::cout << "[" << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "] ";

        double cpu_usage = ResourceMonitor::get_cpu_usage();
        MemoryInfo mem_info = ResourceMonitor::get_memory_info();
        DiskInfo disk_info = ResourceMonitor::get_disk_info(root_path);

        std::cout << " [" << std::fixed << std::setprecision(1) << "System Status]" << std::endl;
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "CPU Usage:      " << cpu_usage << " %" << std::endl;

        auto per_core = ResourceMonitor::get_cpu_usage_per_core();
        if (!per_core.empty()) {
            std::cout << " Per-core CPU Usage:" << std::endl;
            for (size_t i = 0; i < per_core.size(); ++i) {
                std::cout << "  CPU " << std::setw(2) << i << ": " << std::fixed << std::setprecision(1)
                    << per_core[i] << " %" << std::endl;
            }
        }

        std::cout << "Memory Usage:   " << mem_info.usage_percent << " %" << std::endl;
        std::cout << "  (Used: " << (mem_info.total_phys_kb - mem_info.available_phys_kb) / 1024 << " MB / "
                  << "Total: " << mem_info.total_phys_kb / 1024 << " MB)" << std::endl;
        std::cout << "Disk (" << root_path << "):   " << disk_info.usage_percent << " %" << std::endl;
        std::cout << "  (Used: " << (disk_info.total_gb - disk_info.free_gb) << " GB / "
                  << "Total: " << disk_info.total_gb << " GB)" << std::endl;
        std::cout << "==========================================" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
