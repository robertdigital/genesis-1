#ifndef GNS_UTILS_LEXER_H_
#define GNS_UTILS_LEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

 #include <string>
 #include <vector>

namespace genesis {
namespace utils {

inline bool IsWhitespace(const char c)
{
    return (' '  == c) || ('\n' == c) || ('\r' == c) || ('\t' == c) ||
           ('\b' == c) || ('\v' == c) || ('\f' == c) ;
}

inline bool IsLetter(const char c)
{
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || c == '_';
}

inline bool IsDigit(const char c)
{
    return ('0' <= c) && (c <= '9');
}

inline bool IsAlphanum(const char c)
{
    return IsLetter(c) || IsDigit(c);
}

inline bool IsLeftBracket(const char c)
{
    return ('(' == c) || ('[' == c) || ('{' == c);
}

inline bool IsRightBracket(const char c)
{
    return (')' == c) || (']' == c) || ('}' == c);
}

inline bool IsBracket(const char c)
{
    return IsLeftBracket(c) || IsRightBracket(c);
}

inline bool IsSign(const char c)
{
    return ('+' == c) || ('-' == c);
}

inline bool IsOperator(const char c)
{
    return
    ('+' == c) || ('-' == c) ||
    ('*' == c) || ('/' == c) ||
    ('<' == c) || ('>' == c) ||
    ('?' == c) || ('!' == c) ||
    ('^' == c) || ('=' == c) ||
    ('%' == c) || ('&' == c) ||
    ('|' == c) || (',' == c) ||
    (':' == c) || (';' == c);
}

inline bool IsQuotemark(const char c)
{
    return ('"' == c) || ('\'' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool CharMatch(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

/**
 * @brief Represents a token that is outputted by the Lexer.
 *
 * The main types of tokens are:
 *
 * 1. **Symbol**: A named symbol, that starts with a letter, followed by any
 *    number of letters, digits or underscores.
 *
 * 2. **Number**: A number in the format
 *
 *        [+-]123[.456][eE[+-]789].
 *
 * 3. **String**: A literal string, enclosed in either 'abc' or "def". It can
 *    contain escape charaters using a backslash, where \\n, \\t and \\r are
 *    translated into their whitespace representation.
 *
 * 4. **Operator**: An operator out of the set
 *
 *        + - * / < > ? ! ^ = % & | , : ;
 *
 * 5. **Bracket**: Any of these brackets
 *
 *        ( ) [ ] { }
 *
 * Furthermore, there are token types marking whitespaces and comments, which
 * are included when the corresponding options are set in the Lexer class.
 *
 * In case of an error while processing the input, an error token is produced
 * which contains the location of the error.
 */
struct LexerToken
{
    public:
        enum TokenType {
            kUnknown    = 0,
            kError      = 1,
            kEOF        = 8,
            kWhite      = 9,
            kComment    = 10,

            kSymbol     = 20,
            kNumber     = 21,
            kString     = 22,
            kOperator   = 23,
            kBracket    = 24
        };

        /**
         * @brief Constructor that sets the values for this token.
         */
        inline LexerToken
        (
            const TokenType t, const int l, const int c, const std::string v
        ) :
            type_(t), line_(l), column_(c), value_(v)
        {};

        /**
         * @brief Getter for the TokenType of this token.
         */
        inline TokenType type() const {return type_;};

        /** @brief Getter for the line where this token occured. */
        inline size_t line() const {return line_;};

        /** @brief Getter for the column where this token occured. */
        inline size_t column() const {return column_;};

        /** @brief Getter for the string value of this token. */
        inline std::string value() const {return value_;};

        /** Returns whether this token denotes an error in the process. */
        inline bool IsError() const
        {
            return type_ == kError;
        }

        /**
         * @brief Returns whether this token is a given type of operator.
         *
         * Usage: `token.IsOperator('%')` will return true if this token is
         * an operator and if it is the modulo operator.
         */
        inline bool IsOperator(const char c) const
        {
            return type_ == kOperator && value_[0] == c;
        }

        /** @brief Returns whether this token is any operator. */
        inline bool IsOperator() const
        {
            return type_ == kOperator;
        }

        /** @brief Returns whether this token is a given type if bracket.
         *
         * Usage: `token.IsBracket(')')` will return true if this token is
         * an bracket and if it is the closing parenthesis.
         */
        inline bool IsBracket(const char c) const
        {
            return type_ == kBracket && value_[0] == c;
        }

        /** @brief Returns whether this token is any bracket. */
        inline bool IsBracket() const
        {
            return type_ == kBracket;
        }

        /** @brief Converts a TokenType into its string representation. */
        static inline std::string ToStr(const TokenType t)
        {
            switch(t) {
                case kUnknown  : return "Unknown";
                case kError    : return "Error";
                case kEOF      : return "EOF";
                case kWhite    : return "Whitespace";

                case kComment  : return "Comment";
                case kSymbol   : return "Symbol";
                case kNumber   : return "Number";
                case kString   : return "String";
                case kOperator : return "Operator";
                case kBracket  : return "Bracket";

                default        : return "Unknown";
            }
        }

        /**
         * @brief Returns the string representation for the TokenType of
         * this token.
         */
        inline std::string ToStr() const
        {
            return ToStr(type_);
        }

    private:
        const TokenType   type_;
        const int         line_;
        const int         column_;
        const std::string value_;
};

/**
 * @brief %Lexer class that provides an easy way of tokenizing a string.
 *
 * The usage of this class is as follows:
 *
 *     Lexer l;
 *     l.Analyse("1+2=3");
 *
 * The tokens produced with the Lexer::Analyse method are of type LexerToken and
 * can be accessed using an iterator over the Lexer object:
 *
 * TODO
 *
 * The options include_whitespace and include_comments are disabled by default,
 * which helps separating the important content from whites and comments.
 * If needed, they can be enabled to include those tokens also.
 *
 * The option glue_sign_to_number is enabled per default, because this case is
 * more common for analyzing genetic data as opposed to e.g. source code.
 */
class Lexer
{
    public:
        bool Analyze(const std::string& text);
        bool ValidateBrackets();

        std::string Dump();
        void Clear();

        inline bool empty() const
        {
            return tokens_.empty();
        }

        inline std::size_t size() const
        {
            return tokens_.size();
        }

        inline LexerToken operator[](const std::size_t index) const
        {
            if (index < tokens_.size())
                return tokens_[index];
            else
                return LexerToken(LexerToken::kEOF, 0, 0, "");
        }

        /** @brief Determines whether whitespaces are included as tokens. */
        bool include_whitespace = false;

        /** @brief Determines whether comments are included as tokens. */
        bool include_comments   = false;

        /** @brief Determines whether to glue a sign to a number following it.
         *
         * If disabled, a term like `1+2=3` will be parsed into single tokes
         * for each character:
         *
         *     1 + 2 = 3
         *
         * If enabled, signs that preceed a number will be glued to that number,
         * so that a term like `items [1.0, -3.14]` will result in
         *
         *     [ 1.0 , -3.14 ]
         *
         * This is useful when the input is a list or similar data.
         */
         bool glue_sign_to_number = true;

    protected:
        bool ScanToken();
        bool ScanWhitespace();
        bool ScanComment();
        bool ScanSymbol();
        bool ScanNumber();
        bool ScanString();
        bool ScanOperator();
        bool ScanBracket();

    private:
        /** @brief Returns if the iterator is at the end of the text. */
        inline bool IsEnd() const
        {
            return itr_ >= len_;
        }

        /** @brief Returns if a given position is the end of the text. */
        inline bool IsEnd(size_t pos) const
        {
            return pos >= len_;
        }

        /** @brief Extracts a substring of the text betweeen two positions */
        inline std::string GetSubstr (size_t start, size_t end) const
        {
            if (start<end) {
                return std::string(text_+start, end-start);
            } else {
                return std::string("");
            }
        }

        /** @brief Create a token and push it to the list. */
        inline void PushToken (
            const LexerToken::TokenType t, const size_t start, const std::string value
        ) {
            // find previous new line, we need it to tell the column
            size_t bitr = start;
            while (bitr > 0 && text_[bitr-1] != '\n') {
                --bitr;
            }

            // make and push token
            LexerToken tok(t, line_, start-bitr, value);
            tokens_.push_back(tok);
        }

        /** @brief Create a token and push it to the list. */
        inline void PushToken (
            const LexerToken::TokenType t, const size_t start, const size_t end
        ) {
            PushToken(t, start, GetSubstr(start, end));
        }

        // Caveat: the following variables are heavily interweaved during a run
        // of Analyze! They have to stay consistent, otherwise the resulting
        // tokens will contain wrong information.

        /** @brief The text that is being analyzed. */
        const char* text_ = nullptr;

        /** @brief The current position in the text while analyzing. */
        size_t      itr_  = 0;

        /** @brief The length of the text being analyzed. */
        size_t      len_  = 0;

        /** @brief The current line in the text while analyzing. */
        int         line_ = 1;

        /** @brief The list of tokens resulting from the analysis. */
        std::vector<LexerToken> tokens_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
