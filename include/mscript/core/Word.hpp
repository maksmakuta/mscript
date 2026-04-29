#ifndef MSCRIPT_WORD_HPP
#define MSCRIPT_WORD_HPP

#include <cstdint>
#include <string>

namespace ms {

    enum class Word : uint8_t{
        Unknown,

        Identifier,
        Character,
        Integer,
        String,
        Real,

        Function,
        Return,
        While,
        Match,
        Break,
        False,
        Null,
        True,
        Else,
        Let,
        For,
        In,
        If,
        Do,

        And,
        Or,

        Plus,
        Minus,
        Multiply,
        Divide,
        Modulus,

        Not,
        Invert,

        BOr,
        BAnd,
        BXor,

        LeftShift,
        RightShift,

        Equals,
        NotEquals,
        Less,
        Greater,
        LessEqual,
        GreaterEqual,

        PlusAssign,
        MinusAssign,
        MultiplyAssign,
        DivideAssign,
        ModulusAssign,

        NotAssign,
        InvertAssign,

        LeftShiftAssign,
        RightShiftAssign,

        BOrAssign,
        BAndAssign,
        BXorAssign,

        Assign,
        Range,
        Arrow,
        Colon,
        Comma,
        Dot,

        LeftBracket,
        RightBracket,
        LeftBrace,
        RightBrace,
        LeftParen,
        RightParen,

        EndOfFile = 255
    };

    std::string to_string(Word w);

}

#endif //MSCRIPT_WORD_HPP
