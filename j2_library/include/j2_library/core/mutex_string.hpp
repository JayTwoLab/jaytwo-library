#pragma once
//
// NOTE: Do not use non-English commnets in public header files for character encoding compatibility.
//

#include <string>
#include <mutex>
#include <utility>
#include <type_traits>
#include <cassert>

#include "j2_library/export.hpp" // for J2LIB_API macro definition

namespace j2
{
    namespace core
    {

        // thread-safe string wrapper
        // - only members are std::string and std::mutex
        // - std::string API is provided with identical/similar signatures as much as possible
        // - returning pointer/iterator/reference directly is dangerous, so access only via guard() or snapshot (str)
        J2LIB_API class mutex_string {
        public:
            // locked view (guard): holds the mutex during lifetime and provides direct access to internal std::string
            class Locked {
            public:
                // for reentrancy detection, the pointer to owner (protected target object) is also passed
                Locked(std::string& s, std::mutex& m, const mutex_string* owner);
                Locked(const std::string& s, std::mutex& m, const mutex_string* owner);
                ~Locked(); // release reentrancy mark in debug mode

                // internal std::string full API can be used during guard lifetime
                std::string* operator->();
                const std::string* operator->() const;
                std::string& operator*();
                const std::string& operator*() const;

                // early unlock and status check if needed
                [[deprecated("Avoid using unlock() unless in special cases. Narrow down guard lifetime.")]]
                void unlock();
                bool owns_lock() const;

            protected:
                // protected: block helpers that extract pointer directly from external code (prevent misuse)
                const char* guard_cstr() const;  // == (cs_ ? cs_ : s_)->c_str()

                // internal state
                std::string* s_ = nullptr;
                const std::string* cs_ = nullptr;
                std::unique_lock<std::mutex> lock_;

#ifndef NDEBUG
                // debug-only reentrancy control
                const mutex_string* owner_ = nullptr;
                bool mark_set_ = false;
#endif

                friend class mutex_string; // mutex_string can access internally
            };

            // RAII pointer guard (internal use)
            // - provides get()/operator const char*()
            // - keeps lock during object lifetime safe to pass directly as function arguments
            class CStrGuard {
            public:
                CStrGuard(const std::string& s, std::mutex& m);
                const char* get() const { return p_; }
                operator const char* () const { return p_; } // allow direct argument passing
                CStrGuard(const CStrGuard&) = delete;
                CStrGuard& operator=(const CStrGuard&) = delete;
            private:
                std::unique_lock<std::mutex> lock_;
                const char* p_ = nullptr;
            };

        public:
            // ===== constructors/assignments =====
            mutex_string() = default;                 // empty string

            // explicit removed allows "j2::mutex_string ms = \"start\";" / "jstr ms = \"start\";"
            mutex_string(std::string s);
            mutex_string(const char* s);

            mutex_string(const mutex_string& other);
            mutex_string(mutex_string&& other) noexcept;
            mutex_string& operator=(const mutex_string& other);
            mutex_string& operator=(mutex_string&& other) noexcept;

            // assignment from std::string/char* (write)
            mutex_string& operator=(const std::string& rhs);
            mutex_string& operator=(const char* rhs);

            // comparison (read)
            bool operator==(const std::string& rhs) const;
            bool operator==(const char* rhs) const;
            bool operator!=(const std::string& rhs) const { return !(*this == rhs); }
            bool operator!=(const char* rhs) const { return !(*this == rhs); }
            friend inline bool operator==(const std::string& lhs, const mutex_string& rhs) { return rhs == lhs; }
            friend inline bool operator==(const char* lhs, const mutex_string& rhs) { return rhs == lhs; }
            friend inline bool operator!=(const std::string& lhs, const mutex_string& rhs) { return !(rhs == lhs); }
            friend inline bool operator!=(const char* lhs, const mutex_string& rhs) { return !(rhs == lhs); }

            // ===== capacity/status =====
            std::size_t size() const;
            std::size_t length() const;
            bool empty() const;
            std::size_t capacity() const;
            std::size_t max_size() const;
            void reserve(std::size_t n);
            void shrink_to_fit();

            // ===== element access (read-only value return) + short setters =====
            // note: returning reference (operator[], at with char&) is not provided due to safety concerns
            char at(std::size_t pos) const;
            char operator[](std::size_t pos) const;     // read-only value return
            char front() const;                          // getter
            char back() const;                           // getter

            void set(std::size_t pos, char ch);         // set_at alternative (short name)
            void front(char ch);                         // setter overload
            void back(char ch);                          // setter overload

            // ===== modifiers =====
            void clear();
            void push_back(char ch);
            void pop_back();

            void assign(const std::string& s);
            void assign(const char* s);
            void assign(std::size_t count, char ch);

            // append (chained: returns mutex_string&)
            mutex_string& append(const std::string& s);
            mutex_string& append(const char* s);
            mutex_string& append(std::size_t count, char ch);

            // operator+=
            mutex_string& operator+=(const std::string& s);
            mutex_string& operator+=(const char* s);
            mutex_string& operator+=(char ch);

            // insert
            mutex_string& insert(std::size_t pos, const std::string& s);
            mutex_string& insert(std::size_t pos, const char* s);
            mutex_string& insert(std::size_t pos, std::size_t count, char ch);

            // erase
            mutex_string& erase(std::size_t pos = 0, std::size_t count = std::string::npos);

            // replace
            mutex_string& replace(std::size_t pos, std::size_t count, const std::string& s);
            mutex_string& replace(std::size_t pos, std::size_t count, const char* s);
            mutex_string& replace(std::size_t pos, std::size_t count, std::size_t n, char ch);

            // resize
            void resize(std::size_t n);
            void resize(std::size_t n, char ch);

            // swap
            void swap(mutex_string& other);          // between mutex_strings
            void swap(std::string& other_str);      // between internal string and std::string

            // ===== string operations =====
            std::string substr(std::size_t pos = 0, std::size_t count = std::string::npos) const;
            std::size_t copy(char* dest, std::size_t count, std::size_t pos = 0) const;

            int compare(const std::string& s) const;
            int compare(std::size_t pos, std::size_t count, const std::string& s) const;

            std::size_t find(const std::string& s, std::size_t pos = 0) const;
            std::size_t find(const char* s, std::size_t pos = 0) const;
            std::size_t find(char ch, std::size_t pos = 0) const;

            std::size_t rfind(const std::string& s, std::size_t pos = std::string::npos) const;
            std::size_t rfind(const char* s, std::size_t pos = std::string::npos) const;
            std::size_t rfind(char ch, std::size_t pos = std::string::npos) const;

            std::size_t find_first_of(const std::string& s, std::size_t pos = 0) const;
            std::size_t find_first_of(const char* s, std::size_t pos = 0) const;
            std::size_t find_first_of(char ch, std::size_t pos = 0) const;

            std::size_t find_last_of(const std::string& s, std::size_t pos = std::string::npos) const;
            std::size_t find_last_of(const char* s, std::size_t pos = std::string::npos) const;
            std::size_t find_last_of(char ch, std::size_t pos = std::string::npos) const;

            std::size_t find_first_not_of(const std::string& s, std::size_t pos = 0) const;
            std::size_t find_first_not_of(const char* s, std::size_t pos = 0) const;
            std::size_t find_first_not_of(char ch, std::size_t pos = 0) const;

            std::size_t find_last_not_of(const std::string& s, std::size_t pos = std::string::npos) const;
            std::size_t find_last_not_of(const char* s, std::size_t pos = std::string::npos) const;
            std::size_t find_last_not_of(char ch, std::size_t pos = std::string::npos) const;

            // ===== safe convenience =====
            std::string str() const;

            // run lock scope with lambda: with_lock()/with() (short alias)
            // in debug mode: calling other members of the same object inside with() scope will trigger assert
            template <typename Fn>
            auto with_lock(Fn&& f) -> decltype(std::forward<Fn>(f)(std::declval<std::string&>())) {
#ifndef NDEBUG
                assert_not_reentrant_();               // prevent reentrancy on same thread
                ReentrancyMark _rmk{ this };             // mark "this object lock held" during with() lifetime
#endif
                std::scoped_lock lock(m_);
                return std::forward<Fn>(f)(s_);
            }
            template <typename Fn>
            auto with_lock(Fn&& f) const -> decltype(std::forward<Fn>(f)(std::declval<const std::string&>())) {
#ifndef NDEBUG
                assert_not_reentrant_();
                ReentrancyMark _rmk{ this };
#endif
                std::scoped_lock lock(m_);
                return std::forward<Fn>(f)(s_);
            }
            template <typename Fn>
            auto with(Fn&& f) -> decltype(std::forward<Fn>(f)(std::declval<std::string&>())) {
                return with_lock(std::forward<Fn>(f));
            }
            template <typename Fn>
            auto with(Fn&& f) const -> decltype(std::forward<Fn>(f)(std::declval<const std::string&>())) {
                return with_lock(std::forward<Fn>(f));
            }

            // full API access (including iterators/pointers)
            [[nodiscard]] Locked synchronize();
            [[nodiscard]] Locked synchronize() const;

            // short alias
            [[nodiscard]] Locked guard() { return synchronize(); }
            [[nodiscard]] Locked guard() const { return synchronize(); }

        protected:
            // protected: RAII c_str() helper is not exposed externally (prevent misuse)
            CStrGuard c_str() const;

#ifndef NDEBUG
            // debug-only reentrancy check helper/mark
            static thread_local const mutex_string* tls_owner_;
            void assert_not_reentrant_() const {
                // if already inside this object's lock context in the same thread no reentrancy
                assert(tls_owner_ != this && "reentrancy detected: do not call ms.* again inside with()/guard() scope. "
                    "Inside with(), only manipulate the provided std::string(s).");
            }
            struct ReentrancyMark {
                const mutex_string* prev;
                ReentrancyMark(const mutex_string* self) : prev(tls_owner_) {
                    // even if another object is marked, only prevent reentrancy for the same object
                    assert(tls_owner_ != self && "no reentrancy for same object");
                    tls_owner_ = self;
                }
                ~ReentrancyMark() { tls_owner_ = prev; }
            };
#endif

            // accessible directly by derived classes
            std::string        s_;
            mutable std::mutex m_;
        };

        // non-member swap (ADL target)
        inline void swap(mutex_string& a, mutex_string& b) { a.swap(b); }



    } // namespace core
} // namespace j2

// alias: to use shortly in global scope (e.g., jstr ms = "start";)
// using jstr = j2::mutex_string;
