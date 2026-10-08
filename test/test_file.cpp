#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Codec/TestFile.hpp"
#include <doctest.h>

using namespace mentor;
using json = nlohmann::json;

TEST_CASE("testing decoding of TestFile")
{
    auto j         = json::parse(R"({
    "author": "TPPPP",
    "description": "this is a test",
    "id": 99,
    "mode": 1,
    "name": "test",
    "questions": [
        {
            "answer": 0,
            "hint": "h",
            "id": 1,
            "options": [
                "op1",
                "op2"
            ],
            "score": 10,
            "solution": "sol",
            "stem": "s"
        },
        {
            "answers": [
                "12",
                "3"
            ],
            "hint": "h",
            "id": 1,
            "score": 10,
            "solution": "sol",
            "stem": "s"
        }
    ],
    "score": 50
})");
    auto test_file = decodeTestFile(j);
    auto &info     = test_file->info;
    CHECK(info.name == "test");
    CHECK(info.author == "TPPPP");
    CHECK(info.description == "this is a test");
    CHECK(info.mode == TestMode::practice);
    CHECK(info.id == 99);
    CHECK(info.score == 50);

    auto q1 = decode<ChoiceQuestion>(test_file->questions[0]).value();
    CHECK(q1.id == 1);
    CHECK(q1.stem == "s");
    CHECK(q1.solution == "sol");
    CHECK(q1.hint == "h");
    CHECK(q1.options[0] == "op1");
    CHECK(q1.options[1] == "op2");
    CHECK(q1.answer == 0);
    CHECK(q1.score == 10);

    auto q2 = decode<EssayQuestion>(test_file->questions[1]).value();
    CHECK(q2.id == 1);
    CHECK(q2.stem == "s");
    CHECK(q2.solution == "sol");
    CHECK(q2.hint == "h");
    CHECK(q2.answers[0] == "12");
    CHECK(q2.answers[1] == "3");
    CHECK(q2.score == 10);
}
