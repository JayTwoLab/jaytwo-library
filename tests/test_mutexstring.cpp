#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

using namespace std::chrono_literals;

// Google Test framework
// TEST(Test Suite Name, Test Name)

// 1. Constructor and basic state test
TEST(MutexString, ConstructorAndBasic) {
    jstr ms = "start";
    EXPECT_EQ(ms.str(), "start");
    EXPECT_EQ(ms.size(), 5);
    EXPECT_EQ(ms.length(), 5);
    EXPECT_FALSE(ms.empty());
}

// 2. Capacity-related function test
TEST(MutexString, CapacityFunctions) {
    jstr ms = "start";
    auto cap0 = ms.capacity();
    EXPECT_EQ(cap0, 15);
    ms.reserve(64);
#if defined(_MSC_VER)
    EXPECT_EQ(ms.capacity(), 79);
#elif defined(__GNUC__)
    EXPECT_EQ(ms.capacity(), 64);
#else
    EXPECT_GE(ms.capacity(), 64);
#endif
    ms.shrink_to_fit();
#if defined(_MSC_VER)
    EXPECT_EQ(ms.capacity(), 15);
#elif defined(__GNUC__)
    EXPECT_EQ(ms.capacity(), 15);
#else
    EXPECT_EQ(ms.capacity(), 15);
#endif
}

// 3. Append/Insert/Modify function test
TEST(MutexString, AppendInsertModify) {
    jstr ms = "start";
    ms.append(" plus");
    ms += ' ';
    ms += std::string("more");
    EXPECT_EQ(ms.str(), "start plus more");
    ms.insert(0, "[");
    ms.push_back(']');
    ms.insert(1, 3, '*');
    ms.insert(ms.size(), " tail");
    EXPECT_EQ(ms.str(), "[***start plus more] tail");
    ms.set(0, '{');
    ms.front('(');
    ms.back(')');
    EXPECT_EQ(ms.str(), "(***start plus more] tai)");
}

// 4. Search-related function test
TEST(MutexString, Find) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.find("plus"), 10u);
}

TEST(MutexString, RFind) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.rfind('l'), 24u);
}

TEST(MutexString, FindFirstOf) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.find_first_of("aeiou"), 6u);
}

TEST(MutexString, FindLastOf) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.find_last_of('o'), 16u);
}

TEST(MutexString, FindFirstNotOf) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.find_first_not_of("()*"), 0u);
}

TEST(MutexString, FindLastNotOf) {
    jstr ms = "[***start plus more] tail";
    EXPECT_EQ(ms.find_last_not_of(')'), 24u);
}

// 5. Substring and compare function test
TEST(MutexString, SubstrAndCompare) {
    jstr ms = "(***start plus more] tai)";
    std::size_t pos_plus = ms.find("plus");
    if (pos_plus != std::string::npos) {
        EXPECT_EQ(ms.substr(pos_plus, 4), "plus");
    }
    EXPECT_LT(ms.compare("(***{start plus more] tail)"), 0);
    EXPECT_LT(ms.compare(1, 5, std::string("***{")), 0);
}

// 6. Replace/Erase/Copy/Resize function test
TEST(MutexString, ReplaceEraseCopyResize) {
    jstr ms = "(***start plus more] tai)";
    if (ms.size() >= 5) {
        ms.replace(1, 5, "BEGIN");
    }
    ms.replace(0, 0, 2, '#');
    EXPECT_EQ(ms.str(), "##(BEGINart plus more] tai)");
    auto sp = ms.find(' ');
    if (sp != std::string::npos) ms.erase(sp, 1);
    EXPECT_EQ(ms.str(), "##(BEGINartplus more] tai)");
    char buf[128];
    std::size_t n = ms.copy(buf, ms.size(), 0);
    buf[n] = '\0';
    EXPECT_STREQ(buf, "##(BEGINartplus more] tai)");
    EXPECT_EQ(n, 26);
    ms.resize(ms.size() + 3, '!');
    ms.shrink_to_fit();
#if defined(_MSC_VER)
    EXPECT_EQ(ms.capacity(), 31);
#elif defined(__GNUC__)
    EXPECT_EQ(ms.capacity(), 29);
#else
    EXPECT_EQ(ms.capacity(), 29);
#endif
}

// 7. Swap/Clear/Empty function test
TEST(MutexString, SwapClearEmpty) {
    jstr ms = "##(BEGINartplus more] tai)!!!";
    std::string ext = "EXTERNAL";
    ms.swap(ext);
    EXPECT_EQ(ms.str(), "EXTERNAL");
    EXPECT_EQ(ext, "##(BEGINartplus more] tai)!!!");
    ms.swap(ext);
    jstr ms2 = "other";
    ms2.swap(ms);
    EXPECT_EQ(ms.str(), "other");
    EXPECT_EQ(ms2.str(), "##(BEGINartplus more] tai)!!!");
    ms2.swap(ms);
    jstr tmp = "temp";
    tmp.clear();
    EXPECT_TRUE(tmp.empty());
}

// 8. Guard/With function test
TEST(MutexString, GuardAndWith) {
    jstr ms = "##(BEGINartplus more] tai)!!!";
    {
        auto g = ms.guard();
        EXPECT_EQ(g->length(), 29);
        EXPECT_STREQ(g->c_str(), "##(BEGINartplus more] tai)!!!");
    }
    ms.with([](std::string& s) {
        s.append(" [WITH]");
        });
    EXPECT_EQ(ms.str(), "##(BEGINartplus more] tai)!!! [WITH]");
}

// 9. Operator and special function test
TEST(MutexString, OperatorsAndSpecial) {
    jstr ms = "##(BEGINartplus more] tai)!!! [WITH]";
    ms.insert(0, 3, '*');
    ms.replace(0, 3, 2, '#');
    ms.erase(0);
    EXPECT_EQ(ms.str(), "");
    EXPECT_EQ(ms.find_first_not_of(' '), std::string::npos);
    EXPECT_EQ(ms.str(), "");
    EXPECT_TRUE(ms == ms.str());
    EXPECT_TRUE(ms != "zzz");
}


TEST(MutexString, Jstr) {
    //---------------------------------------------------------------------------
    // Demonstration of various j2::MutexString features similar to std::string
    // testJstr() does not include multi-thread related tests.

    // std::cout << "\n===== testJstr: j2::MutexString basic functionality test =====\n";

    //-----------------------------------------------------------
// construction/copy initialization (implicit conversion allowed)
    // jstr is same as j2::MutexString
    jstr ms = "start"; // 5 characters
    EXPECT_EQ(ms.str(), "start");
    EXPECT_EQ(ms.size(), 5);
    EXPECT_EQ(ms.length(), 5);
    EXPECT_EQ(ms.empty(), false);

    //-----------------------------------------------------------
    // reserve/capacity/shrink_to_fit

    // capacity()는 j2::MutexString 클래스에서, 내부적으로 사용하는 std::string 객체가
    // 현재 얼마나 많은 문자를 추가적인 메모리 할당 없이 저장할 수 있는지를 반환하는 함수입니다.
    auto cap0 = ms.capacity();
    EXPECT_EQ(cap0, 15);

    ms.reserve(64); // reserve at least 64 characters
    // MSVC and GCC use different memory allocation strategies. Check accordingly.
#if defined(_MSC_VER)
    EXPECT_EQ(ms.capacity(), 79); // MSVC: capacity after reserve(64)
#elif defined(__GNUC__)
    EXPECT_EQ(ms.capacity(), 64); // GCC: capacity after reserve(64) (실제 값은 환경에 따라 다를 수 있음)
#else
    EXPECT_GE(ms.capacity(), 64); // 기타 컴파일러: 최소값만 체크
#endif

    //-----------------------------------------------------------
    // append / operator+=
    ms.append(" plus");
    ms += ' ';
    ms += std::string("more");
    // std::cout << "after append/+=: \"" << ms.str() << "\"\n";
    // after append/+=: "start plus more"
    EXPECT_EQ(ms.str(), "start plus more");

    //-----------------------------------------------------------
    // insert (string/repeated char/char*)
    ms.insert(0, "["); // insert at beginning
    ms.push_back(']'); // append at end
    ms.insert(1, 3, '*'); // insert 3 '*' at index 1
    ms.insert(ms.size(), " tail"); // append C-string " tail" at end
    // std::cout << "after insert/push_back: \"" << ms.str() << "\"\n";
    // after insert/push_back: "[***start plus more] tail"
    EXPECT_EQ(ms.str(), "[***start plus more] tail");

    //-----------------------------------------------------------
    // set/at/operator[]/front/back (getter/setter)
    char old0 = ms[0]; // read-only indexer
    ms.set(0, '{'); // modify character at index 0
    char f_before = ms.front();
    ms.front('(');
    char b_before = ms.back();
    ms.back(')');
    // std::cout << "indexing/front/back: old[0]='" << old0
    //    << "' front:" << f_before << "->" << ms.front()
    //    << " back:" << b_before << "->" << ms.back()
    //    << " result=\"" << ms.str() << "\"\n";
    // indexing/front/back: old[0]='[' front:{->( back:l->) result="(***start plus more] tai)"
    EXPECT_EQ(old0, '[');
    EXPECT_EQ(f_before, '{');
    EXPECT_EQ(ms.front(), '(');
    EXPECT_EQ(b_before, 'l');
    EXPECT_EQ(ms.back(), ')');
    EXPECT_EQ(ms.str(), "(***start plus more] tai)");

    //-----------------------------------------------------------
    // find / rfind / find_first_of / find_last_of / find_first_not_of / find_last_not_of
    std::size_t pos_plus = ms.find("plus");

    // std::cout << "find(\"plus\") = " << pos_plus << "\n"; // find("plus") = 10
    EXPECT_EQ(pos_plus, 10u);

    // std::cout << "rfind('l') = " << ms.rfind('l') << "\n"; // rfind('l') = 11
    EXPECT_EQ(ms.rfind('l'), 11u);

    // std::cout << "find_first_of(\"aeiou\") = " << ms.find_first_of("aeiou") << "\n"; // find_first_of("aeiou") = 6
    EXPECT_EQ(ms.find_first_of("aeiou"), 6u);

    // std::cout << "find_last_of('o') = " << ms.find_last_of('o') << "\n"; // find_last_of('o') = 16
    EXPECT_EQ(ms.find_last_of('o'), 16u);

    // std::cout << "find_first_not_of(\"()*\") = " << ms.find_first_not_of("()*") << "\n"; // find_first_not_of("()*") = 4
    EXPECT_EQ(ms.find_first_not_of("()*"), 4u);

    // std::cout << "find_last_not_of(')') = " << ms.find_last_not_of(')') << "\n"; // find_last_not_of(')') = 23
    EXPECT_EQ(ms.find_last_not_of(')'), 23u);

    //-----------------------------------------------------------
    // substr / compare
    if (pos_plus != std::string::npos) {
        auto sub = ms.substr(pos_plus, 4);
        // std::cout << "substr(pos_plus,4) = \"" << sub << "\"\n";
        // substr(pos_plus,4) = "plus"
        EXPECT_EQ(sub, "plus");
    }

    // std::cout
    //  << "compare(\"(***{start plus more] tail)\") = "
    //  << ms.compare("(***{start plus more] tail)") << "\n";
    // compare("(***{start plus more] tail)") = -1
#if defined(_MSC_VER)
    // MSVC returns -1 for this comparison
    EXPECT_EQ(ms.compare("(***{start plus more] tail)"), -1);
    EXPECT_EQ(ms.compare(1, 5, std::string("***{")), -1);
#elif defined(__GNUC__)
    // GCC returns -34 for this comparison (difference between '{' and 's')
    EXPECT_EQ(ms.compare("(***{start plus more] tail)"), int('{' - 's'));
    EXPECT_EQ(ms.compare(1, 5, std::string("***{")), int('{' - 's'));
#else
    // Fallback: just check it's negative
    EXPECT_LT(ms.compare("(***{start plus more] tail)"), 0);
    EXPECT_LT(ms.compare(1, 5, std::string("***{")), 0);
#endif

    //-----------------------------------------------------------
    // replace (string/char*/n, ch)
    if (ms.size() >= 5) {
        ms.replace(1, 5, "BEGIN");     // replace range 1~5 with "BEGIN"
    }
    ms.replace(0, 0, 2, '#');          // insert two '#' at beginning
    // std::cout << "after replace: \"" << ms.str() << "\"\n";
    // after replace: "##(BEGINart plus more] tai)"
    EXPECT_EQ(ms.str(), "##(BEGINart plus more] tai)");

    //-----------------------------------------------------------
    // erase (delete one space if exists)
    auto sp = ms.find(' ');
    if (sp != std::string::npos) ms.erase(sp, 1);
    // std::cout << "after erase(one space): \"" << ms.str() << "\"\n";
    // after erase(one space): "##(BEGINartplus more] tai)"
    EXPECT_EQ(ms.str(), "##(BEGINartplus more] tai)");

    //-----------------------------------------------------------
    // copy (must manually add null terminator)
    char buf[128];
    std::size_t n = ms.copy(buf, ms.size(), 0);
    buf[n] = '\0';
    // std::cout << "copy -> buf: \"" << buf << "\" (n=" << n << ")\n";
    // copy -> buf: "##(BEGINartplus more] tai)" (n=26)
    EXPECT_STREQ(buf, "##(BEGINartplus more] tai)");
    EXPECT_EQ(n, 26);

    //-----------------------------------------------------------
    // resize
    ms.resize(ms.size() + 3, '!');
    // std::cout << "after resize(+3,'!'): \"" << ms.str() << "\"\n";
    // after resize(+3,'!'): "##(BEGINartplus more] tai)!!!"
    ms.shrink_to_fit();
    // std::cout << "after shrink_to_fit() capacity=" << ms.capacity() << "\n";
    // after shrink_to_fit() capacity=29
#if defined(_MSC_VER)
    EXPECT_EQ(ms.capacity(), 31); // 31 on MSVC
#elif defined(__GNUC__)
    EXPECT_EQ(ms.capacity(), 29); // 29 on GCC
#else
    EXPECT_EQ(ms.capacity(), 29); 
#endif

    //-----------------------------------------------------------
    // swap(std::string&) — safe in single-thread example
    std::string ext = "EXTERNAL";
    ms.swap(ext);
    // std::cout << "after swap(std::string): ms=\"" << ms.str() << "\", ext=\"" << ext << "\"\n";
    // after swap(std::string): ms="EXTERNAL", ext="##(BEGINartplus more] tai)!!!"
    EXPECT_EQ(ms.str(), "EXTERNAL" );
    EXPECT_EQ(ext, "##(BEGINartplus more] tai)!!!");

    ms.swap(ext); // restore

    //-----------------------------------------------------------
    // swap(MutexString&)
    jstr ms2 = "other";
    ms2.swap(ms);
    // std::cout << "after swap(jstr): ms=\"" << ms.str()
    // << "\", ms2=\"" << ms2.str() << "\"\n";
    // after swap(jstr): ms="other", ms2="##(BEGINartplus more] tai)!!!"
    EXPECT_EQ(ms.str(), "other");
    EXPECT_EQ(ms2.str(), "##(BEGINartplus more] tai)!!!");

    ms2.swap(ms); // restore

    //-----------------------------------------------------------
    // clear / empty
    jstr tmp = "temp";
    tmp.clear();
    // std::cout << "after clear(): tmp.empty()=" << std::boolalpha << tmp.empty() << "\n";
    // after clear(): tmp.empty()=true
    EXPECT_TRUE(tmp.empty());

    //-----------------------------------------------------------
    // use guard(): safe usage of std::string pointer/full API within guard scope
    {
        auto g = ms.guard();
        // std::cout << "guard()->length()=" << g->length()
        //    << ", guard()->c_str()=\"" << g->c_str() << "\"\n";
        // guard()->length()=29, guard()->c_str()="##(BEGINartplus more] tai)!!!"
        EXPECT_EQ(g->length(), 29);
        EXPECT_STREQ(g->c_str(), "##(BEGINartplus more] tai)!!!");

        // ⚠ Do not store/return pointer outside guard scope.
    }

    //-----------------------------------------------------------
    // use with(): perform a batch modification within lock scope
    ms.with( [](std::string& s) {
            s.append(" [WITH]");
            // ⚠ Calling ms.* again here triggers assert (reentrancy) in debug mode
        }
    );
    // std::cout << "after with(): \"" << ms.str() << "\"\n";
    // after with(): "##(BEGINartplus more] tai)!!! [WITH]"
    EXPECT_EQ(ms.str(), "##(BEGINartplus more] tai)!!! [WITH]");

    //-----------------------------------------------------------
    // insert(repeated char) / replace(n, ch) / erase(all)
    ms.insert(0, 3, '*');          // insert 3 '*' at beginning
    ms.replace(0, 3, 2, '#');      // replace first 3 chars with 2 '#'
    ms.erase(0);                   // erase from beginning to end
    // std::cout << "after insert/replace/erase(0): \"" << ms.str() << "\"\n";
    // after insert/replace/erase(0): ""
    EXPECT_EQ(ms.str(), "");
   

    //-----------------------------------------------------------
    // find_first_not_of(char)
    // std::cout << "find_first_not_of(' ') = " << ms.find_first_not_of(' ') << "\n";
    // find_first_not_of(' ') = 18446744073709551615
    EXPECT_EQ(ms.find_first_not_of(' '), std::string::npos);

    //-----------------------------------------------------------
    // comparison operators
    // std::cout << "ms == \"" << ms.str() << "\" ? " << (ms == ms.str() ? "true" : "false") << "\n";
    // ms == "" ? true
    EXPECT_EQ(ms.str(), "");
    EXPECT_TRUE(ms == ms.str());

    // std::cout << "ms != \"zzz\" ? " << (ms != "zzz" ? "true" : "false") << "\n";
    EXPECT_TRUE(ms != "zzz");
    // ms != "zzz" ? true

    // std::cout << "===== end of testJstr =====\n";
}

// C API mimic(example)
#ifndef HAVE_C_API_SINK
#define HAVE_C_API_SINK
static void c_api_sink(const char* p) {
    (void)p; // demo: output omitted
}
#endif

TEST(MutexString, Atomicity) {
    //---------------------------------------------------------------------------
    // demo: two threads randomly read/write strings
    // - externally, since c_str()/guard_cstr() are protected, they cannot be called directly
    // - safe usage: (1) snapshot copy ms.str()
    //               (2) use std::string API within lock guard scope: auto g=ms.guard(); g->c_str();

    j2::MutexString ms("start"); // same as jstr ms("start");

    auto writer = [&] {
            std::mt19937 rng(std::random_device{}());
            for (int i = 0; i < 300; ++i) {
                if (rng() & 1) ms = "hello";
                else           ms = "world";
                std::this_thread::sleep_for(1ms);
            }
        };

    auto reader = [&] {
            std::mt19937 rng(std::random_device{}());
            int hello_hits = 0;
            for (int i = 0; i < 300; ++i) {

                // 1) Comparison itself is safe (inside lock)
                if (ms == "hello") {
                    ++hello_hits;

                    // 2-a) Call C API with snapshot copy (always safe)
                    std::string snap = ms.str();
                    c_api_sink(snap.c_str());

                    // 2-b) Or acquire guard and use std::string::c_str() (safe only during guard lifetime)
                    {
                        auto g = ms.guard(); // create guard
                        c_api_sink(g->c_str()); // note: pointer becomes unsafe after guard is destroyed
                    }

                    // ⚠ The following is caught by assert in debug mode (deadlock prevention):
                    // ms.append("X"); // do not call ms.* again inside with()/guard() scope
                }

                std::this_thread::sleep_for(1ms);
            }

            // std::cout << "[reader] 'hello' detected: " << hello_hits << "\n";
            EXPECT_TRUE(hello_hits >= 0 && hello_hits <= 300);

        };

    std::thread t1(writer), t2(reader);
    t1.join(); t2.join();

    // std::cout << "final(ms): " << ms.str() << "\n";
    EXPECT_TRUE(ms == "hello" || ms == "world");

    //std::cout
    //    << "\n[note]\n"
    //    << " - ms.c_str() / guard().guard_cstr() are protected.\n"
    //    << " - Calling other members of the same object during guard lifetime is blocked by assert in debug mode.\n"
    //    << " - Do not pass pointer/iterator/reference outside of guard scope.\n";

}

