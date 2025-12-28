#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <filesystem>

#include "j2_library/export.hpp"

namespace j2::xml
{   
    // 텍스트 처리 정책
    enum class text_policy
    {
        // 앞뒤 공백 제거, 공백만 있으면 버림
        trim_and_discard_empty,

        // 공백 포함 원문 그대로 유지
        preserve,

        // 공백만 있으면 " " 한 글자만 저장
        collapse_whitespace_only
    };

    // XML 속성 구조체
    struct J2LIB_API xml_attribute
    {
        std::string name;      // 속성 이름 (로컬 이름)
        std::string value;     // 속성 값
        std::string prefix;    // 네임스페이스 접두사
        std::string ns_uri;    // 네임스페이스 URI
    };

    // XML 노드 구조체 (간단 DOM)
    struct J2LIB_API xml_node
    {
        std::string name;          // 로컬 이름
        std::string prefix;        // 네임스페이스 접두사
        std::string ns_uri;        // 네임스페이스 URI
        std::string text;          // 노드 텍스트

        std::vector<xml_attribute> attributes;
        std::vector<std::unique_ptr<xml_node>> children;

        // 기본 생성자 명시 (unique_ptr 멤버가 있어도 기본 생성자는 필요)
        xml_node() = default;

        // non-copyable, movable: unique_ptr 멤버 때문에 복사는 금지하고 이동만 허용
        xml_node(const xml_node&) = delete;
        xml_node& operator=(const xml_node&) = delete;
        xml_node(xml_node&&) noexcept = default;
        xml_node& operator=(xml_node&&) noexcept = default;

        // 자식 노드를 이름으로 찾기 (네임스페이스는 무시, 로컬 이름 기준)
        xml_node* find_child(const std::string& child_name);

        // 속성을 이름으로 찾기 (네임스페이스는 무시, 로컬 이름 기준)
        const xml_attribute* find_attribute(const std::string& attr_name) const;
    };

    // -----------------------------
    // SAX 스타일 핸들러
    // -----------------------------
    class J2LIB_API sax_handler
    {
    public:
        virtual ~sax_handler();

        virtual void on_start_element(const xml_node& node);
        virtual void on_end_element(const xml_node& node);
        virtual void on_text(const std::string& text);
    };

    // DOM 트리를 순회하며 SAX 이벤트 발생
    J2LIB_API void traverse_sax(const xml_node& node, sax_handler& handler);

    // -----------------------------
    // XML 파서 (네임스페이스 + CDATA + 인코딩)
    // -----------------------------
    class J2LIB_API xml_parser
    {
    public:
        // 기본 생성자
        xml_parser();

        // XML 문자열을 바로 넘기는 생성자
        xml_parser(const std::string& xml,
            text_policy policy = text_policy::trim_and_discard_empty);

        // 현재 xml_ 멤버에 들어 있는 내용을 파싱
        std::unique_ptr<xml_node> parse();

        // 새 XML 문자열과 정책을 넘겨서 곧바로 파싱
        std::unique_ptr<xml_node> parse(const std::string& xml,
            text_policy policy = text_policy::trim_and_discard_empty);

        // 파일 경로(std::filesystem::path)를 받아서 파싱
        std::unique_ptr<xml_node> parse_file(
            const std::filesystem::path& file_path,
            text_policy policy = text_policy::trim_and_discard_empty);

    private:
        std::string xml_;          // 파싱할 XML 내용
        std::size_t pos_;
        std::size_t len_;
        text_policy text_policy_;

        bool peek_char(char c) const;
        char current_char() const;
        char get_char();
        void skip_whitespace();
        bool starts_with(const std::string& s) const;
        void expect(const std::string& s);

        std::string parse_raw_name();
        static void split_qname(const std::string& raw_name,
            std::string& out_prefix,
            std::string& out_local);

        std::string parse_attribute_value();

        static std::string trim(const std::string& s);
        static std::string decode_entities(const std::string& s);

        std::string read_text();
        std::string parse_cdata();

        void skip_optional_xml_declaration_and_misc();
        void skip_processing_instruction();
        void skip_comment();

        void parse_attributes(xml_node& node,
            std::unordered_map<std::string, std::string>& ns_map);

        std::unique_ptr<xml_node> parse_element(std::unordered_map<std::string, std::string> ns_map);

        // 텍스트 버퍼를 정책에 따라 node.text 에 반영
        void flush_text_buffer(std::string& text_buffer, xml_node& node);
    };

    // -----------------------------
    // XML 트리 출력
    // -----------------------------
    J2LIB_API void print_xml_tree(const xml_node& node, int indent = 0);

    // -----------------------------
    // 간단 XPath 스타일 선택
    //   - "root/items/item[@index='2']"
    //   - "/a/b/c"
    //   - [@attr='value'] 필터
    // -----------------------------
    struct J2LIB_API xpath_step
    {
        std::string name;
        std::string attr_name;
        std::string attr_value;
    };

    J2LIB_API xpath_step parse_xpath_step(const std::string& step_str);
    J2LIB_API std::vector<xpath_step> parse_xpath(const std::string& expr);
    J2LIB_API std::vector<xml_node*> match_step_in_children(xml_node* parent, const xpath_step& step);
    J2LIB_API std::vector<xml_node*> xpath_select(xml_node* root, const std::string& expr);

    // -----------------------------
    // 디버그용 SAX 핸들러
    // -----------------------------
    class J2LIB_API debug_sax_handler : public sax_handler
    {
    public:
        void on_start_element(const xml_node& node) override;
        void on_end_element(const xml_node& node) override;
        void on_text(const std::string& text) override;
    };

    // -----------------------------
    // 인코딩 자동 처리
    // -----------------------------
    // XML 선언에서 encoding 추출 (없으면 "UTF-8")
    J2LIB_API std::string detect_xml_encoding(const std::string& raw_xml);

    // 특정 인코딩 → UTF-8 변환
    J2LIB_API std::string convert_text_encoding_to_utf8(const std::string& input,
        const std::string& encoding);

    // XML 전체를 UTF-8 로 변환
    J2LIB_API std::string convert_xml_to_utf8(const std::string& raw_xml);

    // 인코딩 자동 처리 + 파싱
    J2LIB_API std::unique_ptr<xml_node> parse_with_auto_encoding(const std::string& raw_xml,
        text_policy policy = text_policy::trim_and_discard_empty);

    // -----------------------------
    // 파일 로더
    // -----------------------------
    class J2LIB_API xml_file_loader
    {
    public:
        // 파일 전체를 바이너리로 읽기
        static std::string read_file_binary(const std::string& path);

        // 파일에서 XML 읽고 인코딩 자동 처리 후 파싱
        static std::unique_ptr<xml_node> parse_file_with_auto_encoding(
            const std::string& path,
            text_policy policy = text_policy::trim_and_discard_empty);
    };

    // -----------------------------
    // 경로 기반 "속성" 접근 헬퍼
    //   - "/hello/world/@a" 형태
    // -----------------------------
    J2LIB_API std::string get_attr_value_by_path(xml_node* root,
        const std::string& full_path);

    J2LIB_API std::optional<int> get_attr_int_by_path(xml_node* root,
        const std::string& full_path);

    J2LIB_API std::optional<bool> get_attr_bool_by_path(xml_node* root,
        const std::string& full_path);

    // -----------------------------
    // 경로 기반 "텍스트/속성 공통" 접근 헬퍼
    //   - "/hello/world"       → world 노드 텍스트
    //   - "/hello/world/@a"    → world 노드 a 속성 값
    // -----------------------------
    J2LIB_API std::string get_text_by_path(xml_node* root,
        const std::string& path);

    J2LIB_API std::optional<std::string> get_text_opt_by_path(xml_node* root,
        const std::string& path);

    J2LIB_API std::optional<int> get_int_by_path(xml_node* root,
        const std::string& path);

    J2LIB_API std::optional<bool> get_bool_by_path(xml_node* root,
        const std::string& path);

    J2LIB_API std::optional<double> get_double_by_path(xml_node* root,
        const std::string& path);

    // -----------------------------
    // ISO-8601 datetime 파서 + 경로 헬퍼
    //   지원 예:
    //     "2025-01-02"
    //     "2025-01-02T03:04:05"
    //     "2025-01-02T03:04:05.123"
    //     "2025-01-02T03:04:05.123Z"
    // -----------------------------
    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        parse_iso8601_datetime(const std::string& text);

    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        get_datetime_by_path(xml_node* root,
            const std::string& path);

    // -----------------------------
    // Unix epoch 기반 datetime 헬퍼
    //   - "1736073600"    : 초 단위 epoch
    //   - "1736073600123" : 밀리초 단위 epoch
    // -----------------------------
    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        parse_unix_epoch_seconds(const std::string& text);

    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        parse_unix_epoch_millis(const std::string& text);

    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        get_datetime_from_epoch_sec_by_path(xml_node* root,
            const std::string& path);

    J2LIB_API std::optional<std::chrono::system_clock::time_point>
        get_datetime_from_epoch_millis_by_path(xml_node* root,
            const std::string& path);

    // -----------------------------
    // XML 검증용 인터페이스 (간단 스키마/DTD 대체)
    // -----------------------------
    class J2LIB_API xml_validator
    {
    public:
        virtual ~xml_validator() = default;

        // root 노드를 검증하고, 실패 시 error_message 에 이유를 채운다
        virtual bool validate(const xml_node& root,
            std::string& error_message) const = 0;
    };

    // 필수 경로(노드/속성)가 존재하는지 검사하는 검증기
    class J2LIB_API required_path_validator : public xml_validator
    {
    public:
        explicit required_path_validator(std::vector<std::string> required_paths);

        bool validate(const xml_node& root,
            std::string& error_message) const override;

    private:
        std::vector<std::string> required_paths_;
    };

} // namespace j2::xml
