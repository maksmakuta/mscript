#include "mscript/frontend/Lexer.hpp"

#include <unordered_map>

namespace ms {

    bool isOperator(const char c) {
        switch (c) {
            case '.':
            case ',':
            case '~':
            case '-':
            case '+':
            case '*':
            case '^':
            case ':':
            case '/':
            case '=':
            case '<':
            case '>':
            case '!':
            case '%':
            case '|':
            case '&':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
                return true;
            default:
                return false;
        }
    }

    Lexer::Lexer(const std::string_view input) : m_input(input), m_index(0), m_line(1), m_column(1) {}

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;
        while (m_index < m_input.size()) {
            const auto c = m_input[m_index];
            if (c == '\n') {
                m_line++;
                m_column = 1;
                m_index++;
                continue;
            }
            if ( c == '\r' || c == ' ' || c == '\t') {
                m_column++;
                m_index++;
                continue;
            }
            if (std::isdigit(c)) {
                tokens.push_back(lexNumber());
                continue;
            }
            if (std::isalpha(c) || c == '_') {
                tokens.push_back(lexIdentifier());
                continue;
            }
            if (c == '\'') {
                tokens.push_back(lexCharacter());
                continue;
            }
            if (c == '"') {
                tokens.push_back(lexString());
                continue;
            }
            if (isOperator(c)) {
                tokens.push_back(lexOperator());
                continue;
            }

            m_column++;
            m_index++;
        }
        return tokens;
    }

    Token Lexer::lexNumber() {
        bool is_real = false;
        const auto begin = m_index;
        const auto column = m_column;
        while (m_index < m_input.size() && std::isdigit(m_input[m_index])) {
            m_column++;
            m_index++;
        }
        if (m_index < m_input.size() && m_input[m_index] == '.') { // check for .. (range operator)
            if (m_index + 1 < m_input.size() && m_input[m_index + 1] == '.') {
                //if range operator - return only integer
                return Token{Word::Integer, m_input.substr(begin, m_index - begin), m_line, column};
            }
            m_column++;
            m_index++;
            is_real = true;
            while (m_index < m_input.size() && std::isdigit(m_input[m_index])) {
                m_column++;
                m_index++;
            }
        }
        return Token{
            is_real ? Word::Real : Word::Integer,
            m_input.substr(begin, m_index - begin),
            m_line,
            column
        };
    }

    static auto RESERVED_WORDS = std::unordered_map<std::string_view, Word>{
        {"function",    Word::Function  },
        { "return",     Word::Return    },
        { "while",      Word::While     },
        { "match",      Word::Match     },
        { "break",      Word::Break     },
        { "false",      Word::False     },
        { "null",       Word::Null      },
        { "true",       Word::True      },
        { "else",       Word::Else      },
        { "let",        Word::Let       },
        { "for",        Word::For       },
        { "in",         Word::In        },
        { "if",         Word::If        },
        { "do",         Word::Do        },
    };

    Token Lexer::lexIdentifier() {
        const auto begin = m_index;
        const auto column = m_column;
        while (m_index < m_input.size() && (std::isalnum(m_input[m_index]) || m_input[m_index] == '_')) {
            m_column++;
            m_index++;
        }
        auto word = Word::Identifier;
        const auto value = m_input.substr(begin, m_index - begin);
        if (const auto it = RESERVED_WORDS.find(value); it != RESERVED_WORDS.end()) {
            word = it->second;
        }
        return Token{ word, value, m_line, column };
    }

    Token Lexer::lexCharacter() {
        const auto begin = m_index;
        const auto column = m_column;
        if (m_input[m_index] == '\'') {
            m_column++;
            m_index++;
        }
        while (m_index < m_input.size() && m_input[m_index] != '\'') {
            m_column++;
            m_index++;
        }
        if (m_input[m_index] == '\'') {
            m_column++;
            m_index++;
        }
        return Token{
             Word::Character,
            m_input.substr(begin, m_index - begin),
            m_line,
            column
        };
    }

    Token Lexer::lexString() {
        const auto begin = m_index;
        const auto column = m_column;
        if (m_input[m_index] == '"') {
            m_column++;
            m_index++;
        }
        while (m_index < m_input.size() && m_input[m_index] != '"') {
            if (m_input[m_index] == '\\') {
                m_column++;
                m_index++;
            }
            m_column++;
            m_index++;
        }
        if (m_index < m_input.size() && m_input[m_index] == '"') {
            m_column++;
            m_index++;
        }
        return Token{
            Word::String,
           m_input.substr(begin, m_index - begin),
           m_line,
           column
       };
    }

    static auto RESERVED_OPERATORS = std::unordered_map<std::string_view, Word>{
        { "<<=", Word::LeftShiftAssign     },
        { ">>=", Word::RightShiftAssign    },

        { "&&", Word::And                 },
        { "||", Word::Or                  },
        { "<<", Word::LeftShift           },
        { ">>", Word::RightShift          },
        { "==", Word::Equals              },
        { "!=", Word::NotEquals           },
        { "<=", Word::LessEqual           },
        { ">=", Word::GreaterEqual        },
        { "+=", Word::PlusAssign          },
        { "-=", Word::MinusAssign         },
        { "*=", Word::MultiplyAssign      },
        { "/=", Word::DivideAssign        },
        { "%=", Word::ModulusAssign       },
        { "|=", Word::BOrAssign           },
        { "&=", Word::BAndAssign          },
        { "^=", Word::BXorAssign          },
        { "..", Word::Range               },
        { "->", Word::Arrow               },

        { "<", Word::Less                },
        { ">", Word::Greater             },
        { "=", Word::Assign              },
        { "+", Word::Plus                },
        { "-", Word::Minus               },
        { "*", Word::Multiply            },
        { "/", Word::Divide              },
        { "%", Word::Modulus             },
        { "!", Word::Not                 },
        { "~", Word::Invert              },
        { "|", Word::BOr                 },
        { "&", Word::BAnd                },
        { "^", Word::BXor                },
        { ":", Word::Colon               },
        { ",", Word::Comma               },
        { ".", Word::Dot                 },
        { "[", Word::LeftBracket         },
        { "]", Word::RightBracket        },
        { "{", Word::LeftBrace           },
        { "}", Word::RightBrace          },
        { "(", Word::LeftParen           },
        { ")", Word::RightParen          },
    };

    Token Lexer::lexOperator() {
        const auto begin = m_index;
        const auto column = m_column;
        while (m_index < m_input.size() && isOperator(m_input[m_index])) {
            m_column++;
            m_index++;
        }

        auto value = m_input.substr(begin, m_index - begin);
        while (m_index > begin) {
            if (const auto it = RESERVED_OPERATORS.find(value); it != RESERVED_OPERATORS.end()) {
                return Token{ it->second, value, m_line, column };
            }
            m_column--;
            m_index--;
            value = m_input.substr(begin, m_index - begin);
        }
        return Token{ Word::Unknown, value, m_line, column };
    }

}
