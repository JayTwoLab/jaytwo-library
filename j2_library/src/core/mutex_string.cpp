
#include "j2_library/core/mutex_string.hpp"

namespace j2::core {

#ifndef NDEBUG
thread_local const mutex_string* mutex_string::tls_owner_ = nullptr;
#endif

// ================= Locked implementation =================
mutex_string::Locked::Locked(std::string& s, std::mutex& m, const mutex_string* owner)
    : s_(&s)
    , lock_(m)                 // ✅ initialize lock_ before owner_
#ifndef NDEBUG
    , owner_(owner)
#endif
{
#ifndef NDEBUG
    // set mark to block calling other members of the same object during guard lifetime
    assert(mutex_string::tls_owner_ != owner_ && "no reentrancy on same object (calling ms.* while guard is held)");
    mutex_string::tls_owner_ = owner_;
    mark_set_ = true;
#endif
}

mutex_string::Locked::Locked(const std::string& s, std::mutex& m, const mutex_string* owner)
    : cs_(&s)
    , lock_(m)                 // ✅ initialize lock_ before owner_
#ifndef NDEBUG
    , owner_(owner)
#endif
{
#ifndef NDEBUG
    assert(mutex_string::tls_owner_ != owner_ && "no reentrancy on same object (calling ms.* while guard is held)");
    mutex_string::tls_owner_ = owner_;
    mark_set_ = true;
#endif
}

mutex_string::Locked::~Locked() {
#ifndef NDEBUG
    if (mark_set_ && mutex_string::tls_owner_ == owner_) {
        mutex_string::tls_owner_ = nullptr;
    }
#endif
}

std::string* mutex_string::Locked::operator->() { return s_; }
const std::string* mutex_string::Locked::operator->() const { return cs_ ? cs_ : s_; }
std::string& mutex_string::Locked::operator*() { return *s_; }
const std::string& mutex_string::Locked::operator*() const { return cs_ ? *cs_ : *s_; }

void mutex_string::Locked::unlock() {
    lock_.unlock();
#ifndef NDEBUG
    // if guard is released early, the owner mark is no longer kept
    if (mark_set_ && mutex_string::tls_owner_ == owner_) {
        mutex_string::tls_owner_ = nullptr;
        mark_set_ = false;
    }
#endif
}
bool mutex_string::Locked::owns_lock() const { return lock_.owns_lock(); }

// protected method: only safe during guard lifetime
const char* mutex_string::Locked::guard_cstr() const {
    return (cs_ ? cs_ : s_)->c_str();
}

// ================= CStrGuard implementation =================
mutex_string::CStrGuard::CStrGuard(const std::string& s, std::mutex& m)
    : lock_(m), p_(s.c_str()) {
    // NOTE: p_ is the internal buffer pointer of std::string,
    // it can only be used safely during the CStrGuard lifetime (=while lock is held).
}

// ================= mutex_string core =================
mutex_string::mutex_string(std::string s) : s_(std::move(s)) {}
mutex_string::mutex_string(const char* s) : s_(s ? s : "") {}

mutex_string::mutex_string(const mutex_string& other) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(other.m_);
    s_ = other.s_;
}
mutex_string::mutex_string(mutex_string&& other) noexcept {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(other.m_);
    s_ = std::move(other.s_);
}

mutex_string& mutex_string::operator=(const mutex_string& other) {
    if (this != &other) {
#ifndef NDEBUG
        assert_not_reentrant_();
#endif
        std::scoped_lock lock(m_, other.m_);
        s_ = other.s_;
    }
    return *this;
}

mutex_string& mutex_string::operator=(mutex_string&& other) noexcept {
    if (this != &other) {
#ifndef NDEBUG
        assert_not_reentrant_();
#endif
        std::scoped_lock lock(m_, other.m_);
        s_ = std::move(other.s_);
    }
    return *this;
}

// ===== std::string/char* assignment =====
mutex_string& mutex_string::operator=(const std::string& rhs) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_);
    s_ = rhs;
    return *this;
}
mutex_string& mutex_string::operator=(const char* rhs) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_);
    s_ = (rhs ? rhs : "");
    return *this;
}

// ===== comparison =====
bool mutex_string::operator==(const std::string& rhs) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_);
    return s_ == rhs;
}
bool mutex_string::operator==(const char* rhs) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_);
    return s_ == (rhs ? rhs : "");
}

// ===== capacity/status =====
std::size_t mutex_string::size() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.size();
}
std::size_t mutex_string::length() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.length();
}
bool mutex_string::empty() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.empty();
}
std::size_t mutex_string::capacity() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.capacity();
}
std::size_t mutex_string::max_size() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.max_size();
}
void mutex_string::reserve(std::size_t n) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.reserve(n);
}
void mutex_string::shrink_to_fit() {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.shrink_to_fit();
}

// ===== element access (value return) + setter =====
char mutex_string::at(std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.at(pos);
}
char mutex_string::operator[](std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_[pos];
}
char mutex_string::front() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.front();
}
char mutex_string::back() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.back();
}
void mutex_string::set(std::size_t pos, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.at(pos) = ch;
}
void mutex_string::front(char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.front() = ch;
}
void mutex_string::back(char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.back() = ch;
}

// ===== modifiers =====
void mutex_string::clear() {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.clear();
}
void mutex_string::push_back(char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.push_back(ch);
}
void mutex_string::pop_back() {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.pop_back();
}

void mutex_string::assign(const std::string& s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_ = s;
}
void mutex_string::assign(const char* s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_ = (s ? s : "");
}
void mutex_string::assign(std::size_t count, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.assign(count, ch);
}

mutex_string& mutex_string::append(const std::string& s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.append(s); return *this;
}
mutex_string& mutex_string::append(const char* s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.append(s ? s : ""); return *this;
}
mutex_string& mutex_string::append(std::size_t count, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.append(count, ch); return *this;
}

mutex_string& mutex_string::operator+=(const std::string& s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_ += s; return *this;
}
mutex_string& mutex_string::operator+=(const char* s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_ += (s ? s : ""); return *this;
}
mutex_string& mutex_string::operator+=(char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_ += ch; return *this;
}

mutex_string& mutex_string::insert(std::size_t pos, const std::string& s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.insert(pos, s); return *this;
}
mutex_string& mutex_string::insert(std::size_t pos, const char* s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.insert(pos, s ? s : ""); return *this;
}
mutex_string& mutex_string::insert(std::size_t pos, std::size_t count, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.insert(pos, count, ch); return *this;
}

mutex_string& mutex_string::erase(std::size_t pos, std::size_t count) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.erase(pos, count); return *this;
}

mutex_string& mutex_string::replace(std::size_t pos, std::size_t count, const std::string& s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.replace(pos, count, s); return *this;
}
mutex_string& mutex_string::replace(std::size_t pos, std::size_t count, const char* s) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.replace(pos, count, s ? s : ""); return *this;
}
mutex_string& mutex_string::replace(std::size_t pos, std::size_t count, std::size_t n, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.replace(pos, count, n, ch); return *this;
}

void mutex_string::resize(std::size_t n) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.resize(n);
}
void mutex_string::resize(std::size_t n, char ch) {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); s_.resize(n, ch);
}

void mutex_string::swap(mutex_string& other) {
    if (this == &other) return;
#ifndef NDEBUG
    assert_not_reentrant_();
    other.assert_not_reentrant_();
#endif
    mutex_string* first = this < &other ? this : &other;
    mutex_string* second = this < &other ? &other : this;
    std::scoped_lock lock(first->m_, second->m_);
    s_.swap(other.s_);
}
void mutex_string::swap(std::string& other_str) {
#ifndef NDEBUG
    assert_not_reentrant_(); // if the other std::string is shared, separate sync is required
#endif
    std::scoped_lock lock(m_); s_.swap(other_str);
}

// ===== string operations =====
std::string mutex_string::substr(std::size_t pos, std::size_t count) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.substr(pos, count);
}
std::size_t mutex_string::copy(char* dest, std::size_t count, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.copy(dest, count, pos);
}
int mutex_string::compare(const std::string& s) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.compare(s);
}
int mutex_string::compare(std::size_t pos, std::size_t count, const std::string& s) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.compare(pos, count, s);
}

std::size_t mutex_string::find(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find(s, pos);
}
std::size_t mutex_string::find(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find(s ? s : "", pos);
}
std::size_t mutex_string::find(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find(ch, pos);
}

std::size_t mutex_string::rfind(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.rfind(s, pos);
}
std::size_t mutex_string::rfind(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.rfind(s ? s : "", pos);
}
std::size_t mutex_string::rfind(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.rfind(ch, pos);
}

std::size_t mutex_string::find_first_of(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_of(s, pos);
}
std::size_t mutex_string::find_first_of(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_of(s ? s : "", pos);
}
std::size_t mutex_string::find_first_of(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_of(std::string(1, ch), pos);
}

std::size_t mutex_string::find_last_of(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_of(s, pos);
}
std::size_t mutex_string::find_last_of(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_of(s ? s : "", pos);
}
std::size_t mutex_string::find_last_of(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_of(std::string(1, ch), pos);
}

std::size_t mutex_string::find_first_not_of(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_not_of(s, pos);
}
std::size_t mutex_string::find_first_not_of(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_not_of(s ? s : "", pos);
}
std::size_t mutex_string::find_first_not_of(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_first_not_of(std::string(1, ch), pos);
}

std::size_t mutex_string::find_last_not_of(const std::string& s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_not_of(s, pos);
}
std::size_t mutex_string::find_last_not_of(const char* s, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_not_of(s ? s : "", pos);
}
std::size_t mutex_string::find_last_not_of(char ch, std::size_t pos) const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_.find_last_not_of(std::string(1, ch), pos);
}

// ===== safe convenience =====
std::string mutex_string::str() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    std::scoped_lock lock(m_); return s_;
}

// ===== full API access =====
mutex_string::Locked mutex_string::synchronize() {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    return Locked{ s_, m_, this };
}
mutex_string::Locked mutex_string::synchronize() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    return Locked{ s_, m_, this };
}

// ===== protected: RAII c_str() (not exposed externally) =====
mutex_string::CStrGuard mutex_string::c_str() const {
#ifndef NDEBUG
    assert_not_reentrant_();
#endif
    return CStrGuard{ s_, m_ };
}



} // namespace j2::core
