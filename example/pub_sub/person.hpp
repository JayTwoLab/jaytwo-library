#pragma once

#include <string>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

struct Person { // example struct
    std::string name;
    std::optional<int> age;
    std::vector<std::string> tags;
};

inline void to_json(nlohmann::json& j, const Person& p) {
    j = nlohmann::json{
        {"name", p.name},
        {"tags", p.tags}
    };
    if (p.age.has_value()) {
        j["age"] = *p.age;
    }
    else {
        j["age"] = nullptr; // or omit the key entirely if you prefer
    }
}

inline void from_json(const nlohmann::json& j, Person& p) {
    j.at("name").get_to(p.name);

    auto it_age = j.find("age");
    if (it_age != j.end() && !it_age->is_null()) {
        p.age = it_age->get<int>();
    }
    else {
        p.age.reset();
    }

    auto it_tags = j.find("tags");
    if (it_tags != j.end() && !it_tags->is_null()) {
        it_tags->get_to(p.tags);
    }
    else {
        p.tags.clear();
    }
}

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, name, age, tags)
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE 는 놀먼 json 3.9.0 이후 버전에서 제공되는 매크로.
// Rockey8 의 놀먼 json 은 3.6.1 이므로 위의 주석 처리된 to_json/from_json 함수를 사용.
