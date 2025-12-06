#pragma once

#include <gtest/gtest.h>
 
// 구버전 GoogleTest(예: 1.8.x) 에는 GTEST_SKIP 매크로가 없습니다.
// 이 경우 "skip" 을 그냥 "성공 처리"로 다운그레이드해서 컴파일 에러만 막습니다.
#ifndef GTEST_SKIP
#  define GTEST_SKIP() GTEST_SUCCEED()
#endif
