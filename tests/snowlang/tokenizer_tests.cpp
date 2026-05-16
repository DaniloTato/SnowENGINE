#include <catch2/catch_test_macros.hpp>
#include "Tokenizer.hpp"
#include <iostream>

TEST_CASE("Tokenizer parses identifiers") {
    Snowlang::Tokenizer tokenizer("hello world");

    auto tokens = tokenizer.tokenize();
    std::cout << tokens.size() << "\n";
    /* It includes EOF token.*/
    REQUIRE(tokens.size() == 3);
}