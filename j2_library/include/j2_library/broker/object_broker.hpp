#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <string>
#include <shared_mutex>

#include "j2_library/export.hpp"
 
namespace j2::broker {

    /**
     * @brief 객체 중개자 (object_broker)
     * 인스턴스의 등록(registration)과 조회(lookup)를 관리하는 크로스 플랫폼 유틸리티입니다.
     */
    class J2LIB_API object_broker {
    public:
        /**
         * @brief 내부 키 구조체
         * 타입 정보와 이름을 조합하여 객체를 고유하게 식별합니다.
         */
        struct key {
            std::type_index type;
            std::string name;

            bool operator==(const key& other) const {
                return type == other.type && name == other.name;
            }
        };

        /**
         * @brief unordered_map을 위한 커스텀 해시 계산기
         */
        struct key_hash {
            std::size_t operator()(const key& k) const {
                return k.type.hash_code() ^ (std::hash<std::string>{}(k.name) << 1);
            }
        };

    public:
        /**
         * @brief 기본 인스턴스 등록
         * @tparam T 등록할 객체의 타입
         * @param instance 공유할 객체의 shared_ptr
         */
        template <typename T>
        static void register_instance(std::shared_ptr<T> instance) {
            register_instance<T>("__default__", instance);
        }

        /**
         * @brief 이름을 지정하여 인스턴스 등록
         * @tparam T 등록할 객체의 타입
         * @param name 객체 식별을 위한 고유 별칭
         * @param instance 공유할 객체의 shared_ptr
         */
        template <typename T>
        static void register_instance(const std::string& name, std::shared_ptr<T> instance) {
            std::unique_lock lock(get_instance().mutex_);
            get_instance().storage_[{typeid(T), name}] = instance;
        }

        /**
         * @brief 등록된 인스턴스 조회
         * @tparam T 가져올 객체의 타입
         * @param name 조회할 객체의 별칭 (기본값 사용 시 생략 가능)
         * @return std::shared_ptr<T> 조회된 객체 (없을 경우 nullptr 반환)
         */
        template <typename T>
        static std::shared_ptr<T> get(const std::string& name = "__default__") {
            std::shared_lock lock(get_instance().mutex_);
            auto& s = get_instance().storage_;

            auto it = s.find({ typeid(T), name });
            if (it != s.end()) {
                try {
                    return std::any_cast<std::shared_ptr<T>>(it->second);
                }
                catch (const std::bad_any_cast&) {
                    return nullptr;
                }
            }
            return nullptr;
        }

        /**
         * @brief 특정 인스턴스 제거
         */
        template <typename T>
        static void unregister_instance(const std::string& name = "__default__") {
            std::unique_lock lock(get_instance().mutex_);
            get_instance().storage_.erase({ typeid(T), name });
        }

        /**
         * @brief 모든 중개 목록 초기화
         */
        static void clear() {
            std::unique_lock lock(get_instance().mutex_);
            get_instance().storage_.clear();
        }

    private:
        object_broker() = default;
        ~object_broker() = default;

        // 싱글톤 속성 유지를 위해 복사 및 이동 제한
        object_broker(const object_broker&) = delete;
        object_broker& operator=(const object_broker&) = delete;

        /**
         * @brief 내부 싱글톤 인스턴스 반환
         */
        static object_broker& get_instance() {
            static object_broker instance;
            return instance;
        }

        std::unordered_map<key, std::any, key_hash> storage_;
        mutable std::shared_mutex mutex_;
    };

} // namespace j2::broker
