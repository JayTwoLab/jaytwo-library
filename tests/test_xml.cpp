
#include <filesystem>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "j2_library/xml/xml.hpp"

// xml_node 의 find_child / find_attribute 테스트
TEST(XmlNodeTest, FindChildAndAttribute)
{
    j2::xml::xml_node root;
    root.name = "root";

    auto child = std::make_unique<j2::xml::xml_node>();
    child->name = "hello";

    j2::xml::xml_attribute attr;
    attr.name = "id";
    attr.value = "42";
    child->attributes.push_back(attr);

    j2::xml::xml_node* child_raw = child.get();
    root.children.push_back(std::move(child));

    auto* found_child = root.find_child("hello");
    ASSERT_EQ(found_child, child_raw);

    EXPECT_EQ(root.find_child("world"), nullptr);

    auto* found_attr = found_child->find_attribute("id");
    ASSERT_NE(found_attr, nullptr);
    EXPECT_EQ(found_attr->value, "42");

    EXPECT_EQ(found_child->find_attribute("name"), nullptr);
}

// SAX 핸들러 + traverse_sax 테스트
TEST(SaxTest, TraverseCallsHandlers)
{
    class recording_handler : public j2::xml::sax_handler
    {
    public:
        std::vector<std::string> events;

        void on_start_element(const j2::xml::xml_node& node) override
        {
            events.push_back("start:" + node.name);
        }

        void on_end_element(const j2::xml::xml_node& node) override
        {
            events.push_back("end:" + node.name);
        }

        void on_text(const std::string& text) override
        {
            std::string t = text;
            while (!t.empty() && isspace(static_cast<unsigned char>(t.front()))) t.erase(t.begin());
            while (!t.empty() && isspace(static_cast<unsigned char>(t.back()))) t.pop_back();
            if (!t.empty())
                events.push_back("text:" + t);
        }
    };

    const std::string xml = R"(
        <root>
            <hello>world</hello>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    recording_handler handler;
    j2::xml::traverse_sax(*doc, handler);

    ASSERT_FALSE(handler.events.empty());
}

// xml_parser: 생성자 + parse() 조합 테스트
TEST(XmlParserTest, ParseWithConstructorAndParseMethod)
{
    const std::string xml = R"(
        <root>
            <hello><world a="1">123</world></hello>
        </root>
    )";

    j2::xml::xml_parser parser(xml, j2::xml::text_policy::trim_and_discard_empty);
    auto doc = parser.parse();

    auto v = j2::xml::get_int_by_path(doc.get(), "/root/hello/world");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 123);

    auto a = j2::xml::get_int_by_path(doc.get(), "/root/hello/world/@a");
    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(*a, 1);
}

// xml_parser: text_policy 별 동작 테스트
TEST(XmlParserTest, TextPolicyVariants)
{
    const std::string xml = R"(
        <root>
            <item>   text   </item>
        </root>
    )";

    {
        j2::xml::xml_parser parser;
        auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);
        auto t = j2::xml::get_text_by_path(doc.get(), "/root/item");
        EXPECT_EQ(t, "text");
    }

    {
        j2::xml::xml_parser parser;
        auto doc = parser.parse(xml, j2::xml::text_policy::preserve);
        auto t = j2::xml::get_text_by_path(doc.get(), "/root/item");
        EXPECT_EQ(t, "   text   ");
    }

    {
        const std::string xml2 = R"(
            <root>
                <item>   </item>
            </root>
        )";

        j2::xml::xml_parser parser;
        auto doc = parser.parse(xml2, j2::xml::text_policy::collapse_whitespace_only);
        auto t = j2::xml::get_text_by_path(doc.get(), "/root/item");
        EXPECT_EQ(t, " ");
    }
}

// 네임스페이스 + CDATA 파싱 테스트
TEST(XmlParserTest, NamespaceAndCdataParsing)
{
    const std::string xml = R"(
        <root xmlns:h="http://example.com/hello">
            <h:msg><![CDATA[Hello <World> & all]]></h:msg>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto nodes = j2::xml::xpath_select(doc.get(), "root/msg");
    ASSERT_EQ(nodes.size(), 1u);
    EXPECT_EQ(nodes.front()->name, "msg");
    EXPECT_EQ(nodes.front()->prefix, "h");
    EXPECT_EQ(nodes.front()->text, "Hello <World> & all");
}

// 주석, 처리 지시문, self-closing 태그 테스트
TEST(XmlParserTest, CommentsPiAndSelfClosing)
{
    const std::string xml = R"(
        <?xml version="1.0"?>
        <!-- 주석 -->
        <root>
            <?pi test?>
            <empty id="100" />
            <item>value</item>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto empty_nodes = j2::xml::xpath_select(doc.get(), "root/empty");
    ASSERT_EQ(empty_nodes.size(), 1u);
    EXPECT_TRUE(empty_nodes.front()->children.empty());
    EXPECT_TRUE(empty_nodes.front()->text.empty());

    auto id = j2::xml::get_int_by_path(doc.get(), "/root/empty/@id");
    ASSERT_TRUE(id.has_value());
    EXPECT_EQ(*id, 100);

    auto item_text = j2::xml::get_text_by_path(doc.get(), "/root/item");
    EXPECT_EQ(item_text, "value");
}

// 문자열 기반 기본 테스트
TEST(XmlBasicTest, ParseSimpleNodeAndAttribute)
{
    const std::string xml = R"(
        <root>
            <hello><world a="1">123</world></hello>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto v = j2::xml::get_int_by_path(doc.get(), "/root/hello/world");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 123);

    auto a = j2::xml::get_int_by_path(doc.get(), "/root/hello/world/@a");
    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(*a, 1);
}

// datetime + epoch 테스트
TEST(XmlDatetimeTest, Iso8601AndEpoch)
{
    const std::string xml = R"(
        <root>
            <times>
                <date_only>2025-01-02</date_only>
                <iso>2025-01-02T03:04:05.123Z</iso>
            </times>
            <epoch>
                <sec>1736073600</sec>
                <ms>1736073600123</ms>
            </epoch>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto tp_date = j2::xml::get_datetime_by_path(doc.get(), "/root/times/date_only");
    EXPECT_TRUE(tp_date.has_value());

    auto tp_iso = j2::xml::get_datetime_by_path(doc.get(), "/root/times/iso");
    EXPECT_TRUE(tp_iso.has_value());

    auto tp_sec = j2::xml::get_datetime_from_epoch_sec_by_path(doc.get(), "/root/epoch/sec");
    EXPECT_TRUE(tp_sec.has_value());

    auto tp_ms = j2::xml::get_datetime_from_epoch_millis_by_path(doc.get(), "/root/epoch/ms");
    EXPECT_TRUE(tp_ms.has_value());

    EXPECT_FALSE(j2::xml::parse_iso8601_datetime("abc").has_value());
    EXPECT_FALSE(j2::xml::parse_unix_epoch_seconds("x123").has_value());
    EXPECT_FALSE(j2::xml::parse_unix_epoch_millis("x123").has_value());
}

// validator: 성공 케이스
TEST(XmlValidatorTest, RequiredPaths)
{
    const std::string xml = R"(
        <root>
            <config>
                <server port="8080" />
                <start_time>2025-01-02T03:04:05</start_time>
            </config>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    j2::xml::required_path_validator validator({
        "/root/config/server",
        "/root/config/start_time",
        "/root/config/server/@port"
        });

    std::string error;
    EXPECT_TRUE(validator.validate(*doc, error));
    EXPECT_TRUE(error.empty());
}

// validator: 실패 케이스
TEST(XmlValidatorTest, MissingRequiredPath)
{
    const std::string xml = R"(
        <root>
            <config>
                <server />
            </config>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    j2::xml::required_path_validator validator({
        "/root/config/server",
        "/root/config/start_time"
        });

    std::string error;
    EXPECT_FALSE(validator.validate(*doc, error));
    EXPECT_FALSE(error.empty());
}

// 특정 파일 경로에서 XML 을 읽어 파싱하는 테스트
TEST(XmlFileTest, ParseFromFilesystemPath)
{
    namespace fs = std::filesystem;

    fs::path xml_path = fs::path("test_sample.xml");

    if (fs::exists(xml_path))
    {
        fs::remove(xml_path);
    }

    {
        std::ofstream ofs(xml_path);
        ASSERT_TRUE(ofs.good());

        ofs << R"(<?xml version="1.0" encoding="UTF-8"?>
<root>
    <config>
        <server port="9090" enabled="true" />
        <start_time>2025-01-02T03:04:05</start_time>
    </config>
</root>
)";
    }

    ASSERT_TRUE(fs::exists(xml_path));
    ASSERT_TRUE(fs::is_regular_file(xml_path));

    j2::xml::xml_parser parser;
    auto doc = parser.parse_file(xml_path, j2::xml::text_policy::trim_and_discard_empty);

    auto port = j2::xml::get_int_by_path(doc.get(), "/root/config/server/@port");
    ASSERT_TRUE(port.has_value());
    EXPECT_EQ(*port, 9090);

    auto enabled = j2::xml::get_bool_by_path(doc.get(), "/root/config/server/@enabled");
    ASSERT_TRUE(enabled.has_value());
    EXPECT_TRUE(*enabled);

    auto start_tp = j2::xml::get_datetime_by_path(doc.get(), "/root/config/start_time");
    EXPECT_TRUE(start_tp.has_value());

    std::error_code ec;
    fs::remove(xml_path, ec);
}

// xml_file_loader 의 read_file_binary / parse_file_with_auto_encoding 테스트
TEST(XmlFileLoaderTest, ReadBinaryAndParseWithAutoEncoding)
{
    namespace fs = std::filesystem;

    fs::path xml_path = fs::path("test_loader.xml");
    if (fs::exists(xml_path))
        fs::remove(xml_path);

    {
        std::ofstream ofs(xml_path, std::ios::binary);
        ASSERT_TRUE(ofs.good());
        ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        ofs << "<root><value>123</value></root>\n";
    }

    auto raw = j2::xml::xml_file_loader::read_file_binary(xml_path.string());
    ASSERT_FALSE(raw.empty());

    auto doc = j2::xml::xml_file_loader::parse_file_with_auto_encoding(
        xml_path.string(), j2::xml::text_policy::trim_and_discard_empty);

    auto v = j2::xml::get_int_by_path(doc.get(), "/root/value");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 123);

    std::error_code ec;
    fs::remove(xml_path, ec);
}

// XPath 단계 처리 테스트
TEST(XPathTest, StepsAndAttributeFilter)
{
    const std::string xml = R"(
        <root>
            <items>
                <item index="1">one</item>
                <item index="2">two</item>
                <item index="3">three</item>
            </items>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto nodes = j2::xml::xpath_select(doc.get(), "root/items/item");
    ASSERT_EQ(nodes.size(), 3u);

    auto nodes2 = j2::xml::xpath_select(doc.get(), "root/items/item[@index=\"2\"]");
    ASSERT_EQ(nodes2.size(), 1u);
    EXPECT_EQ(nodes2.front()->text, "two");
}

// 인코딩 관련 테스트 (UTF-8 경로 중심)
TEST(EncodingTest, DetectEncodingUtf8AndEucKr)
{
    {
        std::string xml = "<root>test</root>";
        auto enc = j2::xml::detect_xml_encoding(xml);
        EXPECT_EQ(enc, "UTF-8");
    }

    {
        std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root/>";
        auto enc = j2::xml::detect_xml_encoding(xml);
        EXPECT_EQ(enc, "UTF-8");
    }

    {
        std::string xml = "<?xml version=\"1.0\" encoding=\"EUC-KR\"?><root/>";
        auto enc = j2::xml::detect_xml_encoding(xml);
        EXPECT_EQ(enc, "EUC-KR");
    }
}

TEST(EncodingTest, ConvertXmlToUtf8PassthroughUtf8)
{
    std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root>abc</root>";
    auto converted = j2::xml::convert_xml_to_utf8(xml);
    EXPECT_EQ(converted, xml);
}

// get_*_by_path 테스트 (텍스트/속성, int/bool/double)
TEST(GetByPathTest, TextAttributeIntBoolDouble)
{
    const std::string xml = R"(
        <root>
            <node a="10" flag="true"  pi="3.14">20</node>
            <node a="30" flag="false" pi="6.28">40</node>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto text = j2::xml::get_text_by_path(doc.get(), "/root/node");
    EXPECT_EQ(text, "20"); // first node value 

    auto attr_text = j2::xml::get_text_by_path(doc.get(), "/root/node/@a");
    EXPECT_EQ(attr_text, "10");

    auto v_node = j2::xml::get_int_by_path(doc.get(), "/root/node");
    ASSERT_TRUE(v_node.has_value());
    EXPECT_EQ(*v_node, 20);

    auto v_attr = j2::xml::get_int_by_path(doc.get(), "/root/node/@a");
    ASSERT_TRUE(v_attr.has_value());
    EXPECT_EQ(*v_attr, 10);

    auto v_bool = j2::xml::get_bool_by_path(doc.get(), "/root/node/@flag");
    ASSERT_TRUE(v_bool.has_value());
    EXPECT_TRUE(*v_bool);

    auto v_double = j2::xml::get_double_by_path(doc.get(), "/root/node/@pi");
    ASSERT_TRUE(v_double.has_value());
    EXPECT_DOUBLE_EQ(*v_double, 3.14);
}

// get_*_by_path 실패 케이스
TEST(GetByPathTest, MissingValuesReturnNulloptOrEmpty)
{
    const std::string xml = R"(
        <root>
            <node a="10">20</node>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto text = j2::xml::get_text_by_path(doc.get(), "/root/missing");
    EXPECT_TRUE(text.empty());

    auto v_int = j2::xml::get_int_by_path(doc.get(), "/root/missing");
    EXPECT_FALSE(v_int.has_value());

    auto attr_text = j2::xml::get_text_by_path(doc.get(), "/root/node/@b");
    EXPECT_TRUE(attr_text.empty());

    auto v_attr_int = j2::xml::get_int_by_path(doc.get(), "/root/node/@b");
    EXPECT_FALSE(v_attr_int.has_value());
}

// parse_file 예외 케이스 테스트
TEST(XmlFileTest, ParseFileThrowsOnInvalidPath)
{
    j2::xml::xml_parser parser;
    std::filesystem::path p = "no_such_file_xyz_123.xml";

    EXPECT_THROW(
        {
            parser.parse_file(p, j2::xml::text_policy::trim_and_discard_empty);
        },
        std::runtime_error
    );
}

// debug_sax_handler 테스트
TEST(DebugSaxHandlerTest, TraverseDoesNotThrow)
{
    const std::string xml = R"(
        <root>
            <child> text </child>
        </root>
    )";

    j2::xml::xml_parser parser;
    auto doc = parser.parse(xml, j2::xml::text_policy::trim_and_discard_empty);

    j2::xml::debug_sax_handler handler;
    ASSERT_NO_THROW(j2::xml::traverse_sax(*doc, handler));
}

// parse_with_auto_encoding 테스트 (UTF-8 케이스)
TEST(XmlParserTest, ParseWithAutoEncoding)
{
    const std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<root><val>100</val></root>";

    auto doc = j2::xml::parse_with_auto_encoding(
        xml, j2::xml::text_policy::trim_and_discard_empty);

    auto v = j2::xml::get_int_by_path(doc.get(), "/root/val");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 100);
}

// Bookstore sample parsing test
TEST(XmlParserTest, ParseBookstoreSample)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<bookstore>
    <book category="fiction">
        <title lang="en">The Lord of the Rings</title>
        <author>J. R. R. Tolkien</author>
        <year>1954</year>
        <price currency="USD">29.99</price>
    </book>

    <book category="programming">
        <title lang="en">The C++ Programming Language</title>
        <author>Bjarne Stroustrup</author>
        <year>2013</year>
        <price currency="USD">49.99</price>
    </book>
</bookstore>)";

    auto doc = j2::xml::parse_with_auto_encoding(xml, j2::xml::text_policy::trim_and_discard_empty);

    auto books = j2::xml::xpath_select(doc.get(), "bookstore/book");
    ASSERT_EQ(books.size(), 2u);

    // First book
    {
        auto b = books[0];
        auto cat = b->find_attribute("category");
        ASSERT_NE(cat, nullptr);
        EXPECT_EQ(cat->value, "fiction");

        auto title = b->find_child("title");
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(title->text, "The Lord of the Rings");
        auto lang = title->find_attribute("lang");
        ASSERT_NE(lang, nullptr);
        EXPECT_EQ(lang->value, "en");

        auto author = b->find_child("author");
        ASSERT_NE(author, nullptr);
        EXPECT_EQ(author->text, "J. R. R. Tolkien");

        auto year = b->find_child("year");
        ASSERT_NE(year, nullptr);
        EXPECT_EQ(std::stoi(year->text), 1954);

        auto price = b->find_child("price");
        ASSERT_NE(price, nullptr);
        EXPECT_DOUBLE_EQ(std::stod(price->text), 29.99);
        auto currency = price->find_attribute("currency");
        ASSERT_NE(currency, nullptr);
        EXPECT_EQ(currency->value, "USD");
    }

    // Second book
    {
        auto b = books[1];
        auto cat = b->find_attribute("category");
        ASSERT_NE(cat, nullptr);
        EXPECT_EQ(cat->value, "programming");

        auto title = b->find_child("title");
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(title->text, "The C++ Programming Language");
        auto lang = title->find_attribute("lang");
        ASSERT_NE(lang, nullptr);
        EXPECT_EQ(lang->value, "en");

        auto author = b->find_child("author");
        ASSERT_NE(author, nullptr);
        EXPECT_EQ(author->text, "Bjarne Stroustrup");

        auto year = b->find_child("year");
        ASSERT_NE(year, nullptr);
        EXPECT_EQ(std::stoi(year->text), 2013);

        auto price = b->find_child("price");
        ASSERT_NE(price, nullptr);
        EXPECT_DOUBLE_EQ(std::stod(price->text), 49.99);
        auto currency = price->find_attribute("currency");
        ASSERT_NE(currency, nullptr);
        EXPECT_EQ(currency->value, "USD");
    }

    //-------------------------------------------
    // Expected data for each book
    struct expected_book {
        std::string category;
        std::string title;
        std::string lang;
        std::string author;
        int year;
        double price;
        std::string currency;
    };

    std::vector<expected_book> expect = {
        {"fiction",
         "The Lord of the Rings",
         "en",
         "J. R. R. Tolkien",
         1954,
         29.99,
         "USD"},
        {"programming",
         "The C++ Programming Language",
         "en",
         "Bjarne Stroustrup",
         2013,
         49.99,
         "USD"}
    };

    ASSERT_EQ(books.size(), expect.size());

    for (std::size_t i = 0; i < books.size(); ++i)
    {
        const auto& b = books[i];
        const auto& e = expect[i];

        auto cat = b->find_attribute("category");
        ASSERT_NE(cat, nullptr);
        EXPECT_EQ(cat->value, e.category);

        auto title = b->find_child("title");
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(title->text, e.title);
        auto lang = title->find_attribute("lang");
        ASSERT_NE(lang, nullptr);
        EXPECT_EQ(lang->value, e.lang);

        auto author = b->find_child("author");
        ASSERT_NE(author, nullptr);
        EXPECT_EQ(author->text, e.author);

        auto year = b->find_child("year");
        ASSERT_NE(year, nullptr);
        EXPECT_EQ(std::stoi(year->text), e.year);

        auto price = b->find_child("price");
        ASSERT_NE(price, nullptr);
        EXPECT_DOUBLE_EQ(std::stod(price->text), e.price);
        auto currency = price->find_attribute("currency");
        ASSERT_NE(currency, nullptr);
        EXPECT_EQ(currency->value, e.currency);
    }
}
