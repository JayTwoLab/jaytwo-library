#include "j2_library/system/system.hpp"

#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <string>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <dbghelp.h>
    // DbgHelp.lib 링크 필요 (CMake에서 설정 권장)
    #pragma comment(lib, "dbghelp.lib")
#else
    #include <csignal>
    #include <execinfo.h> // Linux/macOS 백트레이스 표준
    #include <dlfcn.h>   // dladdr 함수용
    #include <cxxabi.h>  // C++ 함수명 데멀링용
#endif

namespace j2 {
    namespace system {

        CrashCallback CrashHandler::s_callback = nullptr;

        void CrashHandler::initialize(CrashCallback callback) {
            s_callback = callback;

            // 1. C++ 미처리 예외 핸들러 등록
            std::set_terminate(cxx_terminate_handler);

#ifdef _WIN32
            // 2. Windows SEH 핸들러 등록
            SetUnhandledExceptionFilter(windows_exception_handler);

            // Windows에서 DBGHELP 심볼 초기화
            SymInitialize(GetCurrentProcess(), NULL, TRUE);
#else
            // 2. POSIX 시그널 핸들러 등록
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
#endif

        void CrashHandler::cxx_terminate_handler() {
            handle_crash("C++ Terminate", "Unhandled C++ Exception");
            std::abort();
        }

        std::vector<std::string> CrashHandler::get_stack_trace() {
            std::vector<std::string> stack_frames;

#ifdef _WIN32
            // Windows StackWalk64 구현
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

                    // --- 라인 번호 추출 로직 추가 ---
                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD lineDisplacement = 0;

                    if (SymGetLineFromAddr64(process, (DWORD64)stack[i], &lineDisplacement, &line)) {
                        ss << " at " << line.FileName << ":" << std::dec << line.LineNumber;
                    }
                    // -------------------------------

                    stack_frames.push_back(ss.str());
                }
                else {
                    stack_frames.push_back("#" + std::to_string(i) + " [Unknown Address]");
                }
            }
#else
            // Linux/macOS backtrace 구현
            void* callstack[64];
            int frames = backtrace(callstack, 64);
            char** strs = backtrace_symbols(callstack, frames);

            for (int i = 0; i < frames; ++i) {
                // backtrace_symbols 결과는 포맷이 OS마다 다르므로 raw하게 넣거나,
                // 필요시 dladdr+demangle로 가공 가능 (여기서는 raw하게 유지)
                stack_frames.push_back(std::string(strs[i]));
            }
            free(strs);
#endif
            return stack_frames;
        }

        void CrashHandler::handle_crash(const std::string& type, const std::string& reason) {
            // 1. 현재 시간 가져오기
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);

            // 2. 로그 파일 내용 작성
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

            // 3. 콜백 실행 또는 표준 에러 출력
            if (s_callback) {
                s_callback(full_log);
            }
            else {
                std::cerr << full_log << std::endl;
            }
        }

    } // namespace system
} // namespace j2
