#include <Base/Type.hpp>
#include <nlohmann/json.hpp>

namespace mentor
{

using json = nlohmann::json;

void to_json(json &j, const Question &q)
{
    j = json{
        {"id", q.id},
        {"type", q.type},
        {"stem", q.stem},
        {"hint", q.hint},
        {"options", q.options},
        {"answers", q.answers},
        {"solution", q.solution},
        {"score", q.score}};
}

void from_json(const json &j, Question &q)
{
    j.at("id").get_to(q.id);
    j.at("type").get_to(q.type);
    j.at("stem").get_to(q.stem);
    j.at("hint").get_to(q.hint);
    j.at("options").get_to(q.options);
    j.at("answers").get_to(q.answers);
    j.at("solution").get_to(q.solution);
    q.score = j.value("score", 0);
}

void to_json(json &j, const Test &t)
{
    j = json{
        {"id", t.id},
        {"name", t.name},
        {"mode", t.mode},
        {"score", t.score},
        {"questions", t.questions}};
}

void from_json(const json &j, Test &t)
{
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    t.mode  = j.value("mode", TestMode::Unspec);
    t.score = j.value("score", 0);
    j.at("questions").get_to(t.questions);
}

} // namespace mentor