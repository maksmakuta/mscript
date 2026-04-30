#include <gtest/gtest.h>
#include <vector>
#include "mscript/frontend/Lexer.hpp"

using namespace ms;

void ExpectTokens(std::string_view source, const std::vector<Word>& expected_types) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), expected_types.size() + 1)
        << "Token count mismatch for source: " << source;

    for (size_t i = 0; i < expected_types.size(); ++i) {
        EXPECT_EQ(tokens[i].word, expected_types[i])
            << "Mismatch at token index " << i << " (Lexeme: '" << tokens[i].value << "')";
    }
    
    EXPECT_EQ(tokens.back().word, Word::EndOfFile);
}

TEST(LexerTest, ParsesKeywordsAndIdentifiers) {
    ExpectTokens("let a = function", {
        Word::Let, Word::Identifier, Word::Assign, Word::Function
    });
}

TEST(LexerTest, ParsesNumbersAndRanges) {
    ExpectTokens("10 10.5 0..5", {
        Word::Integer, Word::Real, Word::Integer, Word::Range, Word::Integer
    });
}

TEST(LexerTest, MaximalMunchOperators) {
    ExpectTokens("< <= << <<=", {
        Word::Less, Word::LessEqual, Word::LeftShift, Word::LeftShiftAssign
    });
}

TEST(LexerTest, IgnoresWhitespace) {
    ExpectTokens("let a  = 5", {
        Word::Let, Word::Identifier, Word::Assign, Word::Integer
    });
}

TEST(LexerTest, TracksLineAndColumnAccurately) {
    Lexer lexer("let\na");
    const auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens.size(),3);

    EXPECT_EQ(tokens[0].line, 1);
    EXPECT_EQ(tokens[0].column, 1);
    EXPECT_EQ(tokens[1].line, 2);
    EXPECT_EQ(tokens[1].column, 1);
}

TEST(LexerTest, ParsesWithoutSpaces) {
    ExpectTokens("let a=b+c*(10..20)", {
        Word::Let, Word::Identifier, Word::Assign, Word::Identifier,
        Word::Plus, Word::Identifier, Word::Multiply, Word::LeftParen,
        Word::Integer, Word::Range, Word::Integer, Word::RightParen
    });
}

TEST(LexerTest, ParsesStringsWithEscapeCharacters) {
    ExpectTokens(R"("He said \"Hello\"")", {
        Word::String
    });
}

TEST(LexerTest, ParsesComplexIdentifiers) {
    ExpectTokens("let _private_var123 = null", {
        Word::Let, Word::Identifier, Word::Assign, Word::Null
    });
}

TEST(LexerTest, IgnoresComments) {
    ExpectTokens("let a = 5 // this is a comment\nlet b = 10", {
        Word::Let, Word::Identifier, Word::Assign, Word::Integer,
        Word::Let, Word::Identifier, Word::Assign, Word::Integer
    });
}