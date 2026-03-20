#include "j2_library/system/system.hpp"

#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32 
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <dbghelp.h>
    #pragma comment(lib, "dbghelp.lib")
#else
    #include <csignal>
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <cxxabi.h>
    #include <unistd.h>
    #include <memory>
    #include <array>
#endif

namespace j2 {
    namespace system {

        CrashCallback CrashHandler::s_callback = nullptr;

        void CrashHandler::initialize(CrashCallback callback) {
            s_callback = callback;

            std::set_terminate(cxx_terminate_handler);

#ifdef _WIN32
            SetUnhandledExceptionFilter(windows_exception_handler);
            SymInitialize(GetCurrentProcess(), NULL, TRUE);
            // 라인 정보를 가져오기 위해 심볼 로딩 옵션 설정
            SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS);
#else
            const int signals[] = { SIGSEGV, SIGFPE, SIGILL, SIGABRT, SIGBUS };
            for (int sig : signals) {
                std::signal(sig, posix_signal_handler);
            }
#endif
        }

#ifdef _WIN32
        long __stdcall CrashHandler::windows_exception_handler(struct _EXCEPTION_POINTERS* info) {
            std::string reason = "Exception Code: 0x" + std::to_string(info->ExceptionRecord->ExceptionCode);
            handle_crash("Windows SEH Exception", reason);
            return EXCEPTION_EXECUTE_HANDLER;
        }
#else
        void CrashHandler::posix_signal_handler(int sig) {
            std::string reason = "Signal: " + std::to_string(sig);
            handle_crash("POSIX Signal", reason);
            std::exit(sig);
        }

        std::string CrashHandler::addr_to_line(void* addr) {
            char path[1024];
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
            if (len == -1) return "";
            path[len] = '\0';

            std::stringstream ss_addr;
            ss_addr << std::hex << addr;

            // addr2line 명령어를 통해 파일명과 라인 추출
            std::string cmd = "addr2line -e " + std::string(path) + " " + ss_addr.str() + " -f -C";

            std::array<char, 128> buffer;
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);

            if (!pipe) return "";

            while (fgets(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            if (!result.empty() && result.find("??") == std::string::npos) {
                std::replace(result.begin(), result.end(), '\n', ' ');
                return " at " + result;
            }
            return "";
        }
#endif

        void CrashHandler::cxx_terminate_handler() {
            handle_crash("C++ Terminate", "Unhandled C++ Exception");
            std::abort();
        }

        std::vector<std::string> CrashHandler::get_stack_trace() {
            std::vector<std::string> stack_frames;

#ifdef _WIN32
            void* stack[64];
            unsigned short frames = CaptureStackBackTrace(0, 64, stack, NULL);
            HANDLE process = GetCurrentProcess();

            for (unsigned short i = 0; i < frames; ++i) {
                char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = MAX_SYM_NAME;

                DWORD64 displacement = 0;
                std::stringstream ss;

                if (SymFromAddr(process, (DWORD64)stack[i], &displacement, symbol)) {
                    ss << "#" << i << " 0x" << std::hex << symbol->Address << " in " << symbol->Name;

                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD lineDisplacement = 0;

                    if (SymGetLineFromAddr64(process, (DWORD64)stack[i], &lineDisplacement, &line)) {
                        ss << " at " << line.FileName << ":" << std::dec << line.LineNumber;
                    }
                    stack_frames.push_back(ss.str());
                }
                else {
                    stack_frames.push_back("#" + std::to_string(i) + " [Unknown Address]");
                }
            }
#else
            void* callstack[64];
            int frames = backtrace(callstack, 64);
            char** strs = backtrace_symbols(callstack, frames);

            for (int i = 0; i < frames; ++i) {
                std::string line_info = addr_to_line(callstack[i]);
                if (!line_info.empty()) {
                    stack_frames.push_back("#" + std::to_string(i) + " " + line_info);
                }
                else {
                    stack_frames.push_back(std::string(strs[i]));
                }
            }
            free(strs);
#endif
            return stack_frames;
        }

        void CrashHandler::handle_crash(const std::string& type, const std::string& reason) {
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);

            std::stringstream ss;
            ss << "=====================================================\n";
            ss << "[CRASH REPORT] " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
            ss << "=====================================================\n";
            ss << "- Type: " << type << "\n";
            ss << "- Reason: " << reason << "\n";
            ss << "-----------------------------------------------------\n";
            ss << "[CALL STACK]\n";

            std::vector<std::string> trace = get_stack_trace();
            for (const auto& frame : trace) {
                ss << frame << "\n";
            }
            ss << "=====================================================\n";

            std::string full_log = ss.str();

            if (s_callback) {
                s_callback(full_log);
            }
            else {
                std::cerr << full_log << std::endl;
            }
        }

    } // namespace system
} // namespace j2

