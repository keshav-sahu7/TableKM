#include "Kmx.hpp"

#include <stack>
#include <algorithm>



extern std::string placeQuotes(const std::string &str);


bool KMXTag::getAttributeValue(const std::string &attribute_name, std::string &attribute_value) const
{
    auto it = m_properties.find({attribute_name,""});
    if(it != m_properties.end())
    {
        attribute_value = it->value;
        return true;
    }
    return false;
}

std::string KMXTag::getAttributeValue(const std::string &attribute_name) const
{
    auto it = m_properties.find({attribute_name,""});
    if(it != m_properties.end()) return it->value;
    return std::string();
}

KMXTag::~KMXTag()
{
    for(KMXTag *tag : m_child_tags)
        delete tag;
}

namespace parser
{

struct Token
{
    std::string str;
    enum : uint16_t{
        Opening_Tag = 0x01,              // <tag
        Closing_Tag = 0x02,              // </tag>
        Closing_Leaf_Tag = 0x04,         // />
        Attribute_Name = 0x08,            // <tag name = value></tag>
        Attribute_Value = 0x10,           // <tag name = value/>
        Equal_Sign = 0x20,               // =
        Greater_Than_Sign = 0x40,        // >
        Invalid_Token = 0x80             // for validating
        }type;
};

using SConstForwardIterator = std::string::const_iterator;
using SConstForwardIteratorRef = SConstForwardIterator&;
using TokenRef = Token&;

static inline bool isSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

static inline bool isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

static inline bool isWord(char c)
{
    return ((c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z')
            || c == '_'
            );
}


static bool is_opening_tag(SConstForwardIterator begin_, SConstForwardIterator end_, SConstForwardIteratorRef search_end, TokenRef token)
{
    if(*begin_ != '<' || !isWord(*(++begin_)))
        return false;
    token.str.push_back(*begin_);
    while(begin_ != end_ && isWord(*(++begin_)))
    {
        token.str.push_back(*begin_);
    };
    token.type = Token::Opening_Tag;
    search_end = begin_;
    return true;
}

static bool is_closing_tag(SConstForwardIterator begin_, SConstForwardIterator end_, SConstForwardIteratorRef search_end, TokenRef token)
{
    if(*begin_ != '<' || *(++begin_) != '/' || !isWord(*(++begin_)))
        return false;
    token.str.push_back(*begin_);
    while(begin_ != end_ && isWord(*(++begin_))){token.str.push_back(*begin_);};
    if(begin_ == end_ || *begin_ != '>')
        return false;
    search_end = ++begin_;
    token.type = Token::Closing_Tag;
    return true;
}

static bool is_closing_leaf_tag(SConstForwardIterator begin_, SConstForwardIterator end_, SConstForwardIteratorRef search_end,TokenRef token)
{
    if(std::distance(begin_,end_) < 2)
    {
        return false;
    }
    if(*begin_ != '/' || *(++begin_) != '>')
    {
        return false;
    }
    token.str = "/>";
    token.type = Token::Closing_Leaf_Tag;
    search_end = ++begin_;
    return true;
}

static bool is_attribute_name(SConstForwardIterator begin_, SConstForwardIterator end_, SConstForwardIteratorRef search_end,TokenRef token)
{
    if(!isWord(*begin_))
        return false;
    token.str.push_back(*(begin_++));
    while(begin_ != end_ && (isWord(*begin_) || isDigit(*begin_))) token.str.push_back(*(begin_++));
    search_end = begin_;
    token.type = Token::Attribute_Name;
    return true;
}

static bool is_attribute_value(SConstForwardIterator begin_, SConstForwardIterator end_, SConstForwardIteratorRef search_end,TokenRef token)
{
    if(*begin_ != '\"')
        return false;
    ++begin_;
    while(begin_ != end_ && (*begin_ != '\"' && *begin_ != '\n'))
        token.str.push_back(*(begin_++));
    token.str = placeQuotes(token.str);
    if(begin_ == end_ || *begin_ == '\n')
        return false;
    token.type = Token::Attribute_Value;
    search_end = ++begin_;
    return true;
}

static bool is_equal_sign(SConstForwardIterator begin_, SConstForwardIterator /*end_*/, SConstForwardIteratorRef search_end,TokenRef token)
{
    if(*begin_ != '=')
        return false;
    token.str = "=";
    token.type = Token::Equal_Sign;
    search_end = ++begin_;
    return true;
}

static bool is_greater_than_sign(SConstForwardIterator begin_, SConstForwardIterator /*end_*/, SConstForwardIteratorRef search_end,TokenRef token)
{
    if(*begin_ != '>')
        return false;
    token.str = ">";
    token.type = Token::Greater_Than_Sign;
    search_end = ++begin_;
    return true;
}


std::vector<Token> parse(std::string str, std::string &error)
{
    error.clear();
    str += "     ";     //just to avoid early ending
    SConstForwardIterator begin_ = str.cbegin();
    const SConstForwardIterator end_ = str.cend();
    SConstForwardIterator search_ended_at;

    std::vector<Token> tokens;

    Token token;
    bool tag_found;
    bool (*token_scanners[])(SConstForwardIterator, SConstForwardIterator, SConstForwardIteratorRef , Token &) = {
            is_opening_tag,         // <tag
            is_closing_tag,         // </tag>
            is_closing_leaf_tag,    // />
            is_attribute_name,       // name
            is_attribute_value,      // "value"
            is_equal_sign,          // =
            is_greater_than_sign,   // >
    };
    while(begin_ != end_)
    {
        while(begin_ != end_ && isSpace(*begin_)) ++begin_;
        if(begin_ == end_)
            break;
        tag_found = false;
        const int tag_count = 7;
        for(unsigned int i = 0; i < tag_count && !tag_found; ++i)
        {
            token.str.clear();
            tag_found = token_scanners[i](begin_,end_,search_ended_at, token);
        }
        if(!tag_found)
        {
            int row = std::count(str.cbegin(),begin_,'\n') + 1;
            error = std::string("invalid syntax at line ") + std::to_string(row);
            return {};
        }
        tokens.push_back(token);
        begin_ = search_ended_at;
    }
    //
    return tokens;
}

}   //end of parser




KMXTag *toKMXTag(const std::string &str, std::string &error)
{
    using parser::Token;
    std::vector<Token> tokens = parser::parse(str,error);
    if(!error.empty() || !tokens.size())      //if token error occured or it contains nothing
        return nullptr;
    if(tokens[0].type != Token::Opening_Tag)  //if first token is not opening tag
    {
        error = "Token is not opening tag!";
        return nullptr;
    }
    KMXTag* root = nullptr, *temp = nullptr;
    std::stack<KMXTag*> stack;

    root = new KMXTag(tokens[0].str);
    stack.push(root);

    const size_t size = tokens.size();
    tokens.push_back({"",Token::Invalid_Token});

    bool good_token = true;
    KMXAttribute attribute;
    std::size_t i;
    for(i = 1; i < size && good_token; ++i)
    {
        if(stack.empty())
        {
            good_token = false;
            break;
        }
        switch(tokens[i].type)
        {
            case Token::Opening_Tag:
                temp = new KMXTag(tokens[i].str);
                stack.top()->addChildTag(temp);
                stack.push(temp);
                good_token = tokens[i+1].type & (Token::Closing_Leaf_Tag | Token::Attribute_Name | Token::Greater_Than_Sign);
                break;
            case Token::Closing_Tag:
                if(stack.top()->getTagName() != tokens[i].str)
                {
                    good_token = false;
                    break;
                }
                stack.pop();
                good_token = tokens[i+1].type & (Token::Opening_Tag | Token::Closing_Tag | Token::Invalid_Token);
                break;
            case Token::Closing_Leaf_Tag:
                stack.top()->setTagType(KMXTag::Leaf);
                stack.pop();
                good_token = tokens[i+1].type & (Token::Opening_Tag | Token::Closing_Tag | Token::Invalid_Token);
                break;
            case Token::Attribute_Name:
                attribute.name = tokens[i].str;
                good_token = tokens[i+1].type & (Token::Equal_Sign);
                break;
            case Token::Attribute_Value:
                attribute.value = tokens[i].str;
                stack.top()->insertAttribute(attribute);
                good_token = tokens[i+1].type & (Token::Attribute_Name | Token::Closing_Leaf_Tag | Token::Greater_Than_Sign);
                break;
            case Token::Equal_Sign:
                good_token = tokens[i+1].type & (Token::Attribute_Value);
                break;
            case Token::Greater_Than_Sign:
            good_token = tokens[i+1].type & (Token::Opening_Tag | Token::Closing_Tag);
                break;
            default:
                break;
        }
    }
    tokens.pop_back();
    if(!good_token)
    {
        error = "Invalid token is taken somewhere near " + tokens[i].str;
        delete root;
        return nullptr;
    }
    return root;
}
