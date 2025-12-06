
#include "j2_library/xml/xml_parser.hpp"

#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifndef _WIN32
#include <iconv.h>
#include <errno.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace j2::xml
{
    // -----------------------------
    // 공용 공백 제거 함수
    // -----------------------------
    static std::string trim_spaces(const std::string& s)
    {
        std::size_t begin = 0;
        while (begin < s.size() &&
            std::isspace(static_cast<unsigned char>(s[begin])))
        {
            ++begin;
        }
        if (begin == s.size())
            return {};

        std::size_t end = s.size();
        while (end > begin &&
            std::isspace(static_cast<unsigned char>(s[end - 1])))
        {
            --end;
        }
        return s.substr(begin, end - begin);
    }

    // -----------------------------
    // xml_node 멤버
    // -----------------------------
    xml_node* xml_node::find_child(const std::string& child_name)
    {
        for (auto& c : children)
        {
            if (c->name == child_name)
                return c.get();
        }
        return nullptr;
    }

    const xml_attribute* xml_node::find_attribute(const std::string& attr_name) const
    {
        for (const auto& a : attributes)
        {
            if (a.name == attr_name)
                return &a;
        }
        return nullptr;
    }

    // -----------------------------
    // sax_handler 기본 구현
    // -----------------------------
    sax_handler::~sax_handler() = default;

    void sax_handler::on_start_element(const xml_node& node)
    {
        (void)node;
    }

    void sax_handler::on_end_element(const xml_node& node)
    {
        (void)node;
    }

    void sax_handler::on_text(const std::string& text)
    {
        (void)text;
    }

    // -----------------------------
    // DOM → SAX 변환
    // -----------------------------
    void traverse_sax(const xml_node& node, sax_handler& handler)
    {
        handler.on_start_element(node);

        if (!node.text.empty())
        {
            handler.on_text(node.text);
        }

        for (const auto& child : node.children)
        {
            traverse_sax(*child, handler);
        }

        handler.on_end_element(node);
    }

    // -----------------------------
    // xml_parser 구현 (B안)
    // -----------------------------

    // 기본 생성자
    xml_parser::xml_parser()
        : xml_(),
        pos_(0),
        len_(0),
        text_policy_(text_policy::trim_and_discard_empty)
    {
    }

    // XML 문자열을 바로 넘기는 생성자
    xml_parser::xml_parser(const std::string& xml, text_policy policy)
        : xml_(xml),
        pos_(0),
        len_(xml.size()),
        text_policy_(policy)
    {
    }

    bool xml_parser::peek_char(char c) const
    {
        return (pos_ < len_ && xml_[pos_] == c);
    }

    char xml_parser::current_char() const
    {
        if (pos_ >= len_)
            throw std::runtime_error("입력을 벗어났습니다.");
        return xml_[pos_];
    }

    char xml_parser::get_char()
    {
        if (pos_ >= len_)
            throw std::runtime_error("입력을 벗어났습니다.");
        return xml_[pos_++];
    }

    void xml_parser::skip_whitespace()
    {
        while (pos_ < len_ && std::isspace(static_cast<unsigned char>(xml_[pos_])))
        {
            ++pos_;
        }
    }

    bool xml_parser::starts_with(const std::string& s) const
    {
        if (pos_ + s.size() > len_)
            return false;
        return std::equal(s.begin(), s.end(), xml_.begin() + static_cast<std::ptrdiff_t>(pos_));
    }

    void xml_parser::expect(const std::string& s)
    {
        if (!starts_with(s))
            throw std::runtime_error("예상한 토큰이 아닙니다: " + s);
        pos_ += s.size();
    }

    std::string xml_parser::parse_raw_name()
    {
        if (pos_ >= len_)
            throw std::runtime_error("이름 파싱 중 입력 종료");

        std::size_t start = pos_;

        auto is_name_char = [](char ch) {
            return std::isalnum(static_cast<unsigned char>(ch)) ||
                ch == '_' || ch == '-' || ch == ':' || ch == '.';
            };

        if (!is_name_char(xml_[pos_]))
            throw std::runtime_error("유효하지 않은 이름 시작 문자");

        ++pos_;
        while (pos_ < len_ && is_name_char(xml_[pos_]))
            ++pos_;

        return xml_.substr(start, pos_ - start);
    }

    void xml_parser::split_qname(const std::string& raw_name,
        std::string& out_prefix,
        std::string& out_local)
    {
        auto pos = raw_name.find(':');
        if (pos == std::string::npos)
        {
            out_prefix.clear();
            out_local = raw_name;
        }
        else
        {
            out_prefix = raw_name.substr(0, pos);
            out_local = raw_name.substr(pos + 1);
        }
    }

    std::string xml_parser::parse_attribute_value()
    {
        skip_whitespace();
        if (pos_ >= len_)
            throw std::runtime_error("속성 값 시작을 찾을 수 없습니다.");

        char quote = xml_[pos_];
        if (quote != '"' && quote != '\'')
            throw std::runtime_error("속성 값은 따옴표로 둘러싸여야 합니다.");

        ++pos_;
        std::size_t start = pos_;

        while (pos_ < len_ && xml_[pos_] != quote)
            ++pos_;

        if (pos_ >= len_)
            throw std::runtime_error("속성 값이 닫히지 않았습니다.");

        std::string value = xml_.substr(start, pos_ - start);
        ++pos_;

        return decode_entities(value);
    }

    std::string xml_parser::trim(const std::string& s)
    {
        return trim_spaces(s);
    }

    std::string xml_parser::decode_entities(const std::string& s)
    {
        std::string result;
        result.reserve(s.size());

        for (std::size_t i = 0; i < s.size();)
        {
            if (s[i] == '&')
            {
                if (s.compare(i, 4, "&lt;") == 0)
                {
                    result.push_back('<');
                    i += 4;
                }
                else if (s.compare(i, 4, "&gt;") == 0)
                {
                    result.push_back('>');
                    i += 4;
                }
                else if (s.compare(i, 5, "&amp;") == 0)
                {
                    result.push_back('&');
                    i += 5;
                }
                else if (s.compare(i, 6, "&quot;") == 0)
                {
                    result.push_back('"');
                    i += 6;
                }
                else if (s.compare(i, 6, "&apos;") == 0)
                {
                    result.push_back('\'');
                    i += 6;
                }
                else
                {
                    result.push_back(s[i]);
                    ++i;
                }
            }
            else
            {
                result.push_back(s[i]);
                ++i;
            }
        }

        return result;
    }

    std::string xml_parser::read_text()
    {
        std::size_t start = pos_;
        while (pos_ < len_ && xml_[pos_] != '<')
            ++pos_;
        return xml_.substr(start, pos_ - start);
    }

    std::string xml_parser::parse_cdata()
    {
        expect("<![CDATA[");
        std::size_t start = pos_;
        while (pos_ + 2 < len_ && !starts_with("]]>"))
            ++pos_;

        if (pos_ + 2 >= len_)
            throw std::runtime_error("CDATA 섹션이 닫히지 않았습니다.");

        std::string value = xml_.substr(start, pos_ - start);
        expect("]]>");
        return value;
    }

    void xml_parser::skip_optional_xml_declaration_and_misc()
    {
        while (true)
        {
            skip_whitespace();
            if (!peek_char('<'))
                return;

            if (starts_with("<?"))
            {
                skip_processing_instruction();
            }
            else if (starts_with("<!--"))
            {
                skip_comment();
            }
            else
            {
                return;
            }
        }
    }

    void xml_parser::skip_processing_instruction()
    {
        expect("<?");
        while (pos_ + 1 < len_ && !starts_with("?>"))
            ++pos_;

        if (pos_ + 1 >= len_)
            throw std::runtime_error("처리 지시문이 닫히지 않았습니다.");

        expect("?>");
    }

    void xml_parser::skip_comment()
    {
        expect("<!--");
        while (pos_ + 2 < len_ && !starts_with("-->"))
            ++pos_;

        if (pos_ + 2 >= len_)
            throw std::runtime_error("주석이 닫히지 않았습니다.");

        expect("-->");
    }

    void xml_parser::parse_attributes(xml_node& node,
        std::unordered_map<std::string, std::string>& ns_map)
    {
        while (true)
        {
            skip_whitespace();
            if (pos_ >= len_)
                throw std::runtime_error("태그 끝을 찾기 전에 입력 종료");

            char c = xml_[pos_];
            if (c == '/' || c == '>')
                break;

            std::string raw_name = parse_raw_name();
            skip_whitespace();

            if (!peek_char('='))
                throw std::runtime_error("속성에 '=' 이 없습니다.");

            get_char();
            std::string value = parse_attribute_value();

            if (raw_name == "xmlns")
            {
                ns_map[""] = value;
            }
            else if (raw_name.rfind("xmlns:", 0) == 0)
            {
                std::string prefix = raw_name.substr(6);
                ns_map[prefix] = value;
            }
            else
            {
                xml_attribute attr;
                std::string attr_prefix;
                std::string attr_local;

                split_qname(raw_name, attr_prefix, attr_local);

                attr.name = attr_local;
                attr.prefix = attr_prefix;
                if (!attr_prefix.empty())
                {
                    auto it = ns_map.find(attr_prefix);
                    if (it != ns_map.end())
                        attr.ns_uri = it->second;
                }
                attr.value = value;
                node.attributes.push_back(std::move(attr));
            }
        }
    }

    void xml_parser::flush_text_buffer(std::string& text_buffer, xml_node& node)
    {
        if (text_buffer.empty())
            return;

        std::string raw = text_buffer;
        text_buffer.clear();

        switch (text_policy_)
        {
        case text_policy::trim_and_discard_empty:
        {
            std::string trimmed = trim(raw);
            if (trimmed.empty())
                return;

            std::string decoded = decode_entities(trimmed);
            if (!node.text.empty())
                node.text.push_back(' ');
            node.text += decoded;
            break;
        }
        case text_policy::preserve:
        {
            std::string decoded = decode_entities(raw);
            node.text += decoded;
            break;
        }
        case text_policy::collapse_whitespace_only:
        {
            bool all_space = true;
            for (char ch : raw)
            {
                if (!std::isspace(static_cast<unsigned char>(ch)))
                {
                    all_space = false;
                    break;
                }
            }

            if (all_space)
            {
                if (!node.text.empty())
                    node.text.push_back(' ');
                else
                    node.text.push_back(' ');
            }
            else
            {
                std::string trimmed = trim(raw);
                if (trimmed.empty())
                    return;
                std::string decoded = decode_entities(trimmed);
                if (!node.text.empty())
                    node.text.push_back(' ');
                node.text += decoded;
            }
            break;
        }
        }
    }

    std::unique_ptr<xml_node> xml_parser::parse_element(std::unordered_map<std::string, std::string> ns_map)
    {
        if (!peek_char('<'))
            throw std::runtime_error("엘리먼트 시작 '<' 를 찾을 수 없습니다.");

        get_char(); // '<'

        if (peek_char('/'))
            throw std::runtime_error("잘못된 위치에서 종료 태그를 만났습니다.");

        std::string raw_name = parse_raw_name();
        std::string prefix;
        std::string local_name;
        split_qname(raw_name, prefix, local_name);

        auto node = std::make_unique<xml_node>();
        node->name = local_name;
        node->prefix = prefix;

        skip_whitespace();
        parse_attributes(*node, ns_map);

        {
            std::string key = node->prefix;
            auto it = ns_map.find(key);
            if (it != ns_map.end())
                node->ns_uri = it->second;
        }

        if (starts_with("/>"))
        {
            expect("/>");
            return node;
        }

        if (!peek_char('>'))
            throw std::runtime_error("태그의 '>' 가 필요합니다.");

        get_char(); // '>'

        std::string text_buffer;

        while (pos_ < len_)
        {
            if (peek_char('<'))
            {
                flush_text_buffer(text_buffer, *node);

                if (starts_with("</"))
                {
                    expect("</");
                    std::string end_raw = parse_raw_name();
                    std::string end_prefix;
                    std::string end_local;
                    split_qname(end_raw, end_prefix, end_local);

                    skip_whitespace();
                    if (!peek_char('>'))
                        throw std::runtime_error("종료 태그에 '>' 가 필요합니다.");
                    get_char(); // '>'

                    if (end_local != node->name)
                        throw std::runtime_error("시작/종료 태그 이름이 일치하지 않습니다: " +
                            node->name + " vs " + end_local);

                    break;
                }
                else if (starts_with("<!--"))
                {
                    skip_comment();
                }
                else if (starts_with("<?"))
                {
                    skip_processing_instruction();
                }
                else if (starts_with("<![CDATA["))
                {
                    std::string cdata_text = parse_cdata();
                    if (!node->text.empty())
                        node->text.push_back(' ');
                    node->text += cdata_text;
                }
                else
                {
                    auto child = parse_element(ns_map);
                    node->children.push_back(std::move(child));
                }
            }
            else
            {
                text_buffer += read_text();
            }
        }

        flush_text_buffer(text_buffer, *node);

        return node;
    }

    // 현재 xml_ 멤버에 들어 있는 내용을 파싱
    std::unique_ptr<xml_node> xml_parser::parse()
    {
        if (xml_.empty())
            throw std::runtime_error("파싱할 XML 문자열이 비어 있습니다.");

        pos_ = 0;
        len_ = xml_.size();

        skip_whitespace();
        skip_optional_xml_declaration_and_misc();
        skip_whitespace();

        if (!peek_char('<'))
            throw std::runtime_error("루트 태그를 찾을 수 없습니다.");

        std::unordered_map<std::string, std::string> ns_map;
        auto root = parse_element(ns_map);
        skip_whitespace();
        return root;
    }

    // 새 XML 문자열과 정책을 넘겨서 곧바로 파싱
    std::unique_ptr<xml_node> xml_parser::parse(const std::string& xml,
        text_policy policy)
    {
        xml_ = xml;
        text_policy_ = policy;
        return parse();
    }

    // 파일 경로(std::filesystem::path)를 받아서 파싱
    std::unique_ptr<xml_node> xml_parser::parse_file(
        const std::filesystem::path& file_path,
        text_policy policy)
    {
        namespace fs = std::filesystem;

        const std::string path_str = file_path.string();

        if (!fs::exists(file_path))
        {
            throw std::runtime_error("XML 파일이 존재하지 않습니다: " + path_str);
        }
        if (!fs::is_regular_file(file_path))
        {
            throw std::runtime_error("XML 경로가 일반 파일이 아닙니다: " + path_str);
        }

        std::string raw_xml;
        try
        {
            raw_xml = xml_file_loader::read_file_binary(path_str);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error(
                std::string("XML 파일을 읽는 중 오류가 발생했습니다: ") +
                path_str + " / 상세: " + ex.what());
        }

        std::string utf8_xml;
        try
        {
            utf8_xml = convert_xml_to_utf8(raw_xml);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error(
                std::string("XML 인코딩 변환 중 오류가 발생했습니다: ") +
                path_str + " / 상세: " + ex.what());
        }

        return parse(utf8_xml, policy);
    }

    // -----------------------------
    // 트리 출력
    // -----------------------------
    void print_xml_tree(const xml_node& node, int indent)
    {
        std::string indent_str(static_cast<std::size_t>(indent) * 2, ' ');

        std::cout << indent_str << "<";
        if (!node.prefix.empty())
            std::cout << node.prefix << ":";
        std::cout << node.name;

        for (const auto& attr : node.attributes)
        {
            std::cout << " ";
            if (!attr.prefix.empty())
                std::cout << attr.prefix << ":";
            std::cout << attr.name << "=\"" << attr.value << "\"";
        }

        if (node.children.empty() && node.text.empty())
        {
            std::cout << " />\n";
            return;
        }

        std::cout << ">";

        if (!node.text.empty())
            std::cout << node.text;

        if (!node.children.empty())
            std::cout << "\n";

        for (const auto& child : node.children)
        {
            print_xml_tree(*child, indent + 1);
        }

        if (!node.children.empty())
            std::cout << indent_str;

        std::cout << "</";
        if (!node.prefix.empty())
            std::cout << node.prefix << ":";
        std::cout << node.name << ">\n";
    }

    // -----------------------------
    // XPath 비슷한 기능
    // -----------------------------
    xpath_step parse_xpath_step(const std::string& step_str)
    {
        xpath_step step;

        auto lb = step_str.find('[');
        if (lb == std::string::npos)
        {
            step.name = step_str;
            return step;
        }

        step.name = step_str.substr(0, lb);
        auto rb = step_str.find(']', lb);
        if (rb == std::string::npos)
            throw std::runtime_error("xpath step 대괄호가 닫히지 않았습니다.");

        std::string cond = step_str.substr(lb + 1, rb - lb - 1);
        cond.erase(std::remove_if(cond.begin(), cond.end(), ::isspace), cond.end());

        if (cond.size() < 5 || cond[0] != '@')
            throw std::runtime_error("지원하지 않는 xpath 조건입니다: " + cond);

        auto eq_pos = cond.find('=');
        if (eq_pos == std::string::npos)
            throw std::runtime_error("xpath 조건에 '=' 이 없습니다: " + cond);

        step.attr_name = cond.substr(1, eq_pos - 1);

        char quote = cond[eq_pos + 1];
        if (quote != '"' && quote != '\'')
            throw std::runtime_error("xpath 조건 값에 따옴표가 필요합니다: " + cond);

        auto q2 = cond.find(quote, eq_pos + 2);
        if (q2 == std::string::npos)
            throw std::runtime_error("xpath 조건 값이 닫히지 않았습니다: " + cond);

        step.attr_value = cond.substr(eq_pos + 2, q2 - (eq_pos + 2));

        return step;
    }

    std::vector<xpath_step> parse_xpath(const std::string& expr)
    {
        std::vector<xpath_step> steps;

        std::string tmp;
        for (char ch : expr)
        {
            if (ch == '/')
            {
                if (!tmp.empty())
                {
                    steps.push_back(parse_xpath_step(tmp));
                    tmp.clear();
                }
            }
            else
            {
                tmp.push_back(ch);
            }
        }

        if (!tmp.empty())
            steps.push_back(parse_xpath_step(tmp));

        return steps;
    }

    std::vector<xml_node*> match_step_in_children(xml_node* parent, const xpath_step& step)
    {
        std::vector<xml_node*> result;

        for (auto& child_uptr : parent->children)
        {
            xml_node* child = child_uptr.get();
            if (child->name != step.name)
                continue;

            if (!step.attr_name.empty())
            {
                const xml_attribute* attr = child->find_attribute(step.attr_name);
                if (!attr || attr->value != step.attr_value)
                    continue;
            }

            result.push_back(child);
        }

        return result;
    }

    std::vector<xml_node*> xpath_select(xml_node* root, const std::string& expr)
    {
        std::string trimmed = trim_spaces(expr);

        // 선행 '/' 제거
        while (!trimmed.empty() && trimmed.front() == '/')
            trimmed.erase(trimmed.begin());

        if (trimmed.empty())
            return { root };

        auto steps = parse_xpath(trimmed);

        std::vector<xml_node*> current;
        current.push_back(root);

        // ★ 첫 step이 루트 노드 이름과 같으면,
        //    이미 그 노드 위에서 시작하고 있으므로 건너뜁니다.
        std::size_t start_index = 0;
        if (!steps.empty() && steps[0].name == root->name)
        {
            start_index = 1;
        }

        for (std::size_t i = start_index; i < steps.size(); ++i)
        {
            const auto& step = steps[i];
            std::vector<xml_node*> next;

            for (auto* node : current)
            {
                auto matched = match_step_in_children(node, step);
                next.insert(next.end(), matched.begin(), matched.end());
            }

            current.swap(next);
            if (current.empty())
                break;
        }

        return current;
    }

 
    // -----------------------------
    // debug_sax_handler
    // -----------------------------
    void debug_sax_handler::on_start_element(const xml_node& node)
    {
        std::cout << "[SAX] START: ";
        if (!node.prefix.empty())
            std::cout << node.prefix << ":";
        std::cout << node.name << "\n";
    }

    void debug_sax_handler::on_end_element(const xml_node& node)
    {
        std::cout << "[SAX] END  : ";
        if (!node.prefix.empty())
            std::cout << node.prefix << ":";
        std::cout << node.name << "\n";
    }

    void debug_sax_handler::on_text(const std::string& text)
    {
        std::string trimmed = trim_spaces(text);
        if (!trimmed.empty())
        {
            std::cout << "[SAX] TEXT : " << trimmed << "\n";
        }
    }

    // -----------------------------
    // 인코딩 처리
    // -----------------------------
    std::string detect_xml_encoding(const std::string& raw_xml)
    {
        std::string default_encoding = "UTF-8";

        std::size_t pos = raw_xml.find("<?xml");
        if (pos == std::string::npos)
            return default_encoding;

        std::size_t end = raw_xml.find("?>", pos);
        if (end == std::string::npos)
            end = std::min(raw_xml.size(), pos + 256);

        std::string decl = raw_xml.substr(pos, end - pos);

        std::size_t enc_pos = decl.find("encoding");
        if (enc_pos == std::string::npos)
            return default_encoding;

        enc_pos = decl.find('=', enc_pos);
        if (enc_pos == std::string::npos)
            return default_encoding;

        enc_pos = decl.find_first_of("\"'", enc_pos + 1);
        if (enc_pos == std::string::npos)
            return default_encoding;

        char quote = decl[enc_pos];
        std::size_t enc_start = enc_pos + 1;
        std::size_t enc_end = decl.find(quote, enc_start);
        if (enc_end == std::string::npos)
            return default_encoding;

        std::string encoding = decl.substr(enc_start, enc_end - enc_start);

        for (char& ch : encoding)
        {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }

        return encoding;
    }

    std::string convert_text_encoding_to_utf8(const std::string& input,
        const std::string& encoding)
    {
        std::string upper = encoding;
        for (char& ch : upper)
        {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
        if (upper == "UTF-8" || upper == "UTF8")
            return input;

#ifdef _WIN32
        UINT code_page = 0;

        if (upper == "EUC-KR" || upper == "CP949" || upper == "KS_C_5601-1987")
        {
            code_page = 949;
        }
        else
        {
            throw std::runtime_error("지원하지 않는 인코딩(Windows): " + encoding);
        }

        int wide_len = MultiByteToWideChar(code_page, 0,
            input.data(),
            static_cast<int>(input.size()),
            nullptr, 0);
        if (wide_len <= 0)
            throw std::runtime_error("MultiByteToWideChar 실패");

        std::wstring wide(static_cast<std::size_t>(wide_len), 0);
        wide_len = MultiByteToWideChar(code_page, 0,
            input.data(),
            static_cast<int>(input.size()),
            wide.data(), wide_len);
        if (wide_len <= 0)
            throw std::runtime_error("MultiByteToWideChar 실패(2)");

        int utf8_len = WideCharToMultiByte(CP_UTF8, 0,
            wide.data(), wide_len,
            nullptr, 0,
            nullptr, nullptr);
        if (utf8_len <= 0)
            throw std::runtime_error("WideCharToMultiByte 실패");

        std::string utf8(static_cast<std::size_t>(utf8_len), 0);
        utf8_len = WideCharToMultiByte(CP_UTF8, 0,
            wide.data(), wide_len,
            utf8.data(), utf8_len,
            nullptr, nullptr);
        if (utf8_len <= 0)
            throw std::runtime_error("WideCharToMultiByte 실패(2)");

        return utf8;

#else
        iconv_t cd = iconv_open("UTF-8", encoding.c_str());
        if (cd == (iconv_t)-1)
        {
            throw std::runtime_error("iconv_open 실패: " + encoding);
        }

        std::size_t in_bytes_left = input.size();
        std::size_t out_bytes_left = in_bytes_left * 4 + 16;
        std::string output(out_bytes_left, 0);

        char* inbuf = const_cast<char*>(input.data());
        char* outbuf = output.data();

        while (in_bytes_left > 0)
        {
            std::size_t res = iconv(cd, &inbuf, &in_bytes_left, &outbuf, &out_bytes_left);
            if (res == static_cast<std::size_t>(-1))
            {
                iconv_close(cd);
                throw std::runtime_error("iconv 변환 실패 (errno=" + std::to_string(errno) + ")");
            }
        }

        iconv_close(cd);

        output.resize(output.size() - out_bytes_left);
        return output;
#endif
    }

    std::string convert_xml_to_utf8(const std::string& raw_xml)
    {
        std::string encoding = detect_xml_encoding(raw_xml);

        std::string upper = encoding;
        for (char& ch : upper)
        {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }

        if (upper == "UTF-8" || upper == "UTF8")
        {
            return raw_xml;
        }

        return convert_text_encoding_to_utf8(raw_xml, encoding);
    }

    std::unique_ptr<xml_node> parse_with_auto_encoding(const std::string& raw_xml,
        text_policy policy)
    {
        std::string utf8_xml = convert_xml_to_utf8(raw_xml);

        xml_parser parser(utf8_xml, policy);
        return parser.parse();
    }

    // -----------------------------
    // xml_file_loader
    // -----------------------------
    std::string xml_file_loader::read_file_binary(const std::string& path)
    {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs)
            throw std::runtime_error("파일을 열 수 없습니다: " + path);

        std::ostringstream oss;
        oss << ifs.rdbuf();
        return oss.str();
    }

    std::unique_ptr<xml_node> xml_file_loader::parse_file_with_auto_encoding(
        const std::string& path,
        text_policy policy)
    {
        std::string raw_xml = read_file_binary(path);
        return parse_with_auto_encoding(raw_xml, policy);
    }

    // -----------------------------
    // 속성 전용 경로 헬퍼 (/.../@attr)
    // -----------------------------
    std::string get_attr_value_by_path(xml_node* root,
        const std::string& full_path)
    {
        if (!root)
            return {};

        std::string path = trim_spaces(full_path);

        while (!path.empty() && path.front() == '/')
            path.erase(path.begin());

        if (path.empty())
            return {};

        std::string node_path;
        std::string attr_part;

        auto pos = path.rfind('/');
        if (pos == std::string::npos)
        {
            node_path = "";
            attr_part = path;
        }
        else
        {
            node_path = path.substr(0, pos);
            attr_part = path.substr(pos + 1);
        }

        attr_part = trim_spaces(attr_part);
        if (attr_part.empty() || attr_part[0] != '@')
            return {};

        std::string attr_name = attr_part.substr(1);
        if (attr_name.empty())
            return {};

        xml_node* target_node = nullptr;

        if (node_path.empty())
        {
            target_node = root;
        }
        else
        {
            auto nodes = xpath_select(root, node_path);
            if (nodes.empty())
                return {};
            target_node = nodes.front();
        }

        const xml_attribute* attr = target_node->find_attribute(attr_name);
        if (!attr)
            return {};

        return attr->value;
    }

    std::optional<int> get_attr_int_by_path(xml_node* root,
        const std::string& full_path)
    {
        std::string value = get_attr_value_by_path(root, full_path);
        if (value.empty())
            return std::nullopt;

        try
        {
            int v = std::stoi(value);
            return std::optional<int>(v);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    std::optional<bool> get_attr_bool_by_path(xml_node* root,
        const std::string& full_path)
    {
        std::string value = get_attr_value_by_path(root, full_path);
        if (value.empty())
            return std::nullopt;

        std::string lower;
        lower.reserve(value.size());
        for (char ch : value)
        {
            lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }

        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
            return std::optional<bool>(true);
        if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
            return std::optional<bool>(false);

        return std::nullopt;
    }

    // -----------------------------
    // 내부: 경로가 속성 경로인지 판별
    //   - "/a/b/@x" → true
    //   - "/a/b"    → false
    // -----------------------------
    static bool is_attr_path(const std::string& path)
    {
        std::string trimmed = trim_spaces(path);
        if (trimmed.empty())
            return false;

        std::string tmp = trimmed;
        while (!tmp.empty() && tmp.front() == '/')
            tmp.erase(tmp.begin());

        if (tmp.empty())
            return false;

        auto pos = tmp.rfind('/');
        std::string last = (pos == std::string::npos) ? tmp : tmp.substr(pos + 1);
        last = trim_spaces(last);

        return (!last.empty() && last[0] == '@');
    }

    // -----------------------------
    // 경로 기반 텍스트/속성 공통 헬퍼
    // -----------------------------
    std::string get_text_by_path(xml_node* root,
        const std::string& path)
    {
        if (!root)
            return {};

        if (is_attr_path(path))
        {
            return get_attr_value_by_path(root, path);
        }

        auto nodes = xpath_select(root, path);
        if (nodes.empty())
            return {};

        return nodes.front()->text;
    }

    std::optional<std::string> get_text_opt_by_path(xml_node* root,
        const std::string& path)
    {
        if (!root)
            return std::nullopt;

        std::string v = get_text_by_path(root, path);
        if (v.empty())
            return std::nullopt;

        return std::optional<std::string>(v);
    }

    std::optional<int> get_int_by_path(xml_node* root,
        const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        try
        {
            int v = std::stoi(*opt_text);
            return std::optional<int>(v);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    std::optional<bool> get_bool_by_path(xml_node* root,
        const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        std::string lower;
        lower.reserve(opt_text->size());
        for (char ch : *opt_text)
        {
            lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }

        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
            return std::optional<bool>(true);
        if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
            return std::optional<bool>(false);

        return std::nullopt;
    }

    std::optional<double> get_double_by_path(xml_node* root,
        const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        try
        {
            double v = std::stod(*opt_text);
            return std::optional<double>(v);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    // -----------------------------
    // ISO-8601 datetime 파서
    // -----------------------------
    std::optional<std::chrono::system_clock::time_point>
        parse_iso8601_datetime(const std::string& text)
    {
        std::string s = trim_spaces(text);
        if (s.empty())
            return std::nullopt;

        if (s.size() < 10)
            return std::nullopt;

        if (s[4] != '-' || s[7] != '-')
            return std::nullopt;

        int year = 0;
        int month = 0;
        int day = 0;

        try
        {
            year = std::stoi(s.substr(0, 4));
            month = std::stoi(s.substr(5, 2));
            day = std::stoi(s.substr(8, 2));
        }
        catch (...)
        {
            return std::nullopt;
        }

        std::tm tm{};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;

        std::size_t pos = 10;

        if (pos >= s.size())
        {
            std::time_t tt = std::mktime(&tm);
            if (tt == static_cast<std::time_t>(-1))
                return std::nullopt;
            auto tp = std::chrono::system_clock::from_time_t(tt);
            return std::optional<std::chrono::system_clock::time_point>(tp);
        }

        if (s[pos] == 'T' || s[pos] == 't' || s[pos] == ' ')
        {
            ++pos;
        }
        else
        {
            return std::nullopt;
        }

        if (pos + 8 > s.size())
            return std::nullopt;

        if (s[pos + 2] != ':' || s[pos + 5] != ':')
            return std::nullopt;

        int hour = 0;
        int min = 0;
        int sec = 0;

        try
        {
            hour = std::stoi(s.substr(pos, 2));
            min = std::stoi(s.substr(pos + 3, 2));
            sec = std::stoi(s.substr(pos + 6, 2));
        }
        catch (...)
        {
            return std::nullopt;
        }

        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;

        pos += 8;

        long long fractional_ns = 0;
        if (pos < s.size() && s[pos] == '.')
        {
            ++pos;
            std::size_t start_frac = pos;
            while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos])))
            {
                ++pos;
            }

            std::size_t frac_len = pos - start_frac;
            if (frac_len > 0)
            {
                std::string frac_str = s.substr(start_frac, frac_len);
                if (frac_len > 9)
                    frac_str = frac_str.substr(0, 9);
                frac_len = frac_str.size();

                try
                {
                    long long frac_val = std::stoll(frac_str);
                    for (std::size_t i = frac_len; i < 9; ++i)
                        frac_val *= 10;
                    fractional_ns = frac_val;
                }
                catch (...)
                {
                    fractional_ns = 0;
                }
            }
        }

        if (pos < s.size())
        {
            char tz_ch = s[pos];
            if (tz_ch == 'Z' || tz_ch == 'z')
            {
                ++pos;
            }
            else if (tz_ch == '+' || tz_ch == '-')
            {
                ++pos;
                if (pos + 2 <= s.size() &&
                    std::isdigit(static_cast<unsigned char>(s[pos])) &&
                    std::isdigit(static_cast<unsigned char>(s[pos + 1])))
                {
                    pos += 2;
                }
                if (pos < s.size() && s[pos] == ':')
                    ++pos;
                if (pos + 2 <= s.size() &&
                    std::isdigit(static_cast<unsigned char>(s[pos])) &&
                    std::isdigit(static_cast<unsigned char>(s[pos + 1])))
                {
                    pos += 2;
                }
                // 실제 시간대 보정은 생략 (로컬 시간 기준으로 처리)
            }
        }

        std::time_t tt = std::mktime(&tm);
        if (tt == static_cast<std::time_t>(-1))
            return std::nullopt;

        auto base_tp = std::chrono::system_clock::from_time_t(tt);

        if (fractional_ns != 0)
        {
            // system_clock::duration 으로 캐스팅하여 타입을 맞춘 뒤 더해줌
            auto frac = std::chrono::duration_cast<std::chrono::system_clock::duration>(
                std::chrono::nanoseconds(fractional_ns));
            auto tp = base_tp + frac;
            return std::optional<std::chrono::system_clock::time_point>(tp);
        }

        return std::optional<std::chrono::system_clock::time_point>(base_tp);
    }

    std::optional<std::chrono::system_clock::time_point>
        get_datetime_by_path(xml_node* root,
            const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        return parse_iso8601_datetime(*opt_text);
    }

    // -----------------------------
    // Unix epoch 파서 (수정 포인트)
    // -----------------------------
    std::optional<std::chrono::system_clock::time_point>
        parse_unix_epoch_seconds(const std::string& text)
    {
        std::string s = trim_spaces(text);
        if (s.empty())
            return std::nullopt;

        try
        {
            long long sec = std::stoll(s);

            // seconds → system_clock::duration 으로 변환
            auto dur = std::chrono::seconds(sec);
            auto sys_dur = std::chrono::duration_cast<std::chrono::system_clock::duration>(dur);
            std::chrono::system_clock::time_point tp(sys_dur);

            return std::optional<std::chrono::system_clock::time_point>(tp);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }


    std::optional<std::chrono::system_clock::time_point>
        parse_unix_epoch_millis(const std::string& text)
    {
        std::string s = trim_spaces(text);
        if (s.empty())
            return std::nullopt;

        try
        {
            long long ms = std::stoll(s);

            // milliseconds → system_clock::duration 으로 변환
            auto dur_ms = std::chrono::milliseconds(ms);
            auto sys_dur = std::chrono::duration_cast<std::chrono::system_clock::duration>(dur_ms);
            std::chrono::system_clock::time_point tp(sys_dur);

            return std::optional<std::chrono::system_clock::time_point>(tp);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }


    std::optional<std::chrono::system_clock::time_point>
        get_datetime_from_epoch_sec_by_path(xml_node* root,
            const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        return parse_unix_epoch_seconds(*opt_text);
    }

    std::optional<std::chrono::system_clock::time_point>
        get_datetime_from_epoch_millis_by_path(xml_node* root,
            const std::string& path)
    {
        auto opt_text = get_text_opt_by_path(root, path);
        if (!opt_text)
            return std::nullopt;

        return parse_unix_epoch_millis(*opt_text);
    }

    // -----------------------------
    // XML 검증 모듈 구현
    // -----------------------------
    required_path_validator::required_path_validator(std::vector<std::string> required_paths)
        : required_paths_(std::move(required_paths))
    {
    }

    bool required_path_validator::validate(const xml_node& root,
        std::string& error_message) const
    {
        xml_node* root_non_const = const_cast<xml_node*>(&root);

        for (const auto& path : required_paths_)
        {
            // 1) 먼저, 이 경로가 "/a/b/@x" 같은 속성 경로인지 확인
            if (is_attr_path(path))
            {
                // get_attr_value_by_path 를 그대로 쓰면
                // 값이 빈 문자열인 속성과 "없는" 속성을 구분 못하므로
                // 여기서 직접 파싱해서 존재 여부만 확인합니다.

                std::string full = trim_spaces(path);

                // 선행 '/' 제거
                while (!full.empty() && full.front() == '/')
                    full.erase(full.begin());

                if (full.empty())
                {
                    error_message = "필수 경로가 존재하지 않습니다: " + path;
                    return false;
                }

                std::string node_path;
                std::string attr_part;

                auto pos = full.rfind('/');
                if (pos == std::string::npos)
                {
                    node_path = "";
                    attr_part = full;
                }
                else
                {
                    node_path = full.substr(0, pos);
                    attr_part = full.substr(pos + 1);
                }

                attr_part = trim_spaces(attr_part);
                if (attr_part.empty() || attr_part[0] != '@')
                {
                    error_message = "필수 경로 형식이 잘못되었습니다: " + path;
                    return false;
                }

                std::string attr_name = attr_part.substr(1);
                if (attr_name.empty())
                {
                    error_message = "필수 경로 형식이 잘못되었습니다: " + path;
                    return false;
                }

                // 노드 찾기
                xml_node* target_node = nullptr;
                if (node_path.empty())
                {
                    // "/@attr" 같은 형태면 루트 노드 자체에서 찾음
                    target_node = root_non_const;
                }
                else
                {
                    auto nodes = xpath_select(root_non_const, node_path);
                    if (nodes.empty())
                    {
                        error_message = "필수 경로가 존재하지 않습니다: " + path;
                        return false;
                    }
                    target_node = nodes.front();
                }

                // 속성 존재 여부 확인
                const xml_attribute* attr = target_node->find_attribute(attr_name);
                if (!attr)
                {
                    error_message = "필수 속성이 존재하지 않습니다: " + path;
                    return false;
                }

                // 이 경로 OK → 다음 required path 검사
                continue;
            }

            // 2) 일반 노드 경로 처리
            auto nodes = xpath_select(root_non_const, path);
            if (nodes.empty())
            {
                error_message = "필수 경로가 존재하지 않습니다: " + path;
                return false;
            }
        }

        return true;
    }


} // namespace j2::xml
