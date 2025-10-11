#pragma once

// NOTICE: Use C++17 or higher version

// C Standard
#include <cassert>
#include <cstdint>
#include <cstring>

// C++ Standard
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <bitset>
#include <string>
#include <iomanip>

#include "j2_library/export.hpp"

namespace j2::core {

class bit_array {

public:

    // 기본 생성자
    bit_array();

    // 초기 생성자
    explicit bit_array(const std::vector<uint8_t>& bytes, size_t bits);

    // 초기화
    void clear();

    // 비트 크기 반환
    size_t size() const;

    // 바이트 배열로 반환
    std::vector<uint8_t> data() const;

    // 특정 비트 오프셋과 길이로 하위 비트 어레이 추출
    bit_array get(size_t bitOffset, size_t bitLength) const;

    // 특정 비트 오프셋과 길이로 값을 병합
    bool merge(const bit_array& input, size_t bitOffset); 

    // 연산자 +를 통해 비트 어레이 병합
    bit_array operator+(const bit_array& other) const; 

    // 바이트 배열로 초기화
    void from(const std::vector<uint8_t>& bytes, size_t bits); 

    // 지정된 바이트 크기로 0으로 채운 배열을 설정
    void setBytes(size_t byteSize); 

    // 지정된 비트 크기로 0으로 채운 배열을 설정
    void setBits(size_t bitSize); 

    // 비트 배열을 boolean 배열(std::vector<bool>)로 반환 (0:false, 1:true)
    std::vector<bool> toArray() const; 

    // 비트 이동 <<
    bit_array operator<<(size_t shift) const; 

    // 비트 이동 >>
    bit_array operator>>(size_t shift) const; 

    // reverse byte array
    void reverser(); 

    // 디버그용 출력
    void print(bool split4bit = true) const;

    // 헥스 덤프 출력
    void dump() const;

protected:

    // 데이터
    std::vector<uint8_t> _data;

    // 비트 크기
    size_t _bitSize;

};

} // namespace j2::core
