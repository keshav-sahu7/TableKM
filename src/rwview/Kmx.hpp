#ifndef RWVIEW_KMX_HPP
#define RWVIEW_KMX_HPP

#include <string>
#include <vector>
#include <set>
#include <algorithm>


extern std::string replaceQuotes(const std::string &str);

struct KMXAttribute
{
    std::string name;
    std::string value;
    bool operator < (const KMXAttribute &rhs) const;   //for std::set
    bool operator == (const KMXAttribute &rhs) const;
    bool operator <= (const KMXAttribute &rhs) const;
};

inline bool KMXAttribute::operator <(const KMXAttribute &rhs) const
{
    return name < rhs.name;
}

inline bool KMXAttribute::operator ==(const KMXAttribute &rhs) const
{
    return name == rhs.name;
}

inline bool KMXAttribute::operator <=(const KMXAttribute &rhs) const
{
    return name <= rhs.name;
}

class KMXTag
{
public:
    enum TagType{
        Container,Leaf
    };
public:
    KMXTag(const std::string &tag_name);
    KMXTag(const KMXTag&) = delete;
    const std::string &getTagName() const;
    void insertAttribute(const KMXAttribute &attribute);
    const std::set<KMXAttribute> &getProperties() const;
    void addChildTag(KMXTag *tag);
    KMXTag *addChildTag(const std::string &tagname);
    const std::vector<KMXTag*> &getChildTags() const;
    std::vector<KMXTag*> getChildTagsByName(const std::string &name);
    KMXTag *getChildTagByName(const std::string &name);
    bool getAttributeValue(const std::string &attribute_name, std::string &attribute_value) const;
    std::string getAttributeValue(const std::string &attribute_name) const;
    template <class Stream>
    void writeTo(Stream &stream, int tabs, int spaces) const;
    ~KMXTag();
    TagType tagType() const;
    void setTagType(TagType newType);

private:
    std::string m_tag_name;
    std::set<KMXAttribute> m_properties;
    std::vector<KMXTag*> m_child_tags;
    TagType m_type;
};

/**
 * tabs -> number of tabs
 * spaces -> number of spaces in tabs
 */
template<class Stream>
void KMXTag::writeTo(Stream &stream, int tabs, int spaces) const
{
    stream << std::string(tabs * spaces, ' ');
    stream << "<" << m_tag_name;
    for(const KMXAttribute &attribute : m_properties)
    {
        stream << ' ' << attribute.name << "=\""
               << replaceQuotes(attribute.value) << "\"";
    }
    if(m_type == TagType::Container)
    {
        stream << ">\n";
        for(KMXTag *child_tag : m_child_tags)
        {
            child_tag->writeTo(stream,tabs+1,spaces);
        }
        stream << std::string(tabs * spaces, ' ') << "</" << m_tag_name << ">";
    }
    else        //leaf tag
    {
        stream << "/>";
    }
    stream << "\n";
    stream.flush();
}


inline KMXTag::KMXTag(const std::string &tag_name) : m_tag_name(tag_name), m_type(Container)
{
    //
}

inline const std::string &KMXTag::getTagName() const
{
    return m_tag_name;
}

inline void KMXTag::insertAttribute(const KMXAttribute &attribute)
{
    m_properties.insert(attribute);
}

inline const std::set<KMXAttribute> &KMXTag::getProperties() const
{
    return m_properties;
}

inline void KMXTag::addChildTag(KMXTag *tag)
{
    m_child_tags.push_back(tag);
}

inline KMXTag *KMXTag::addChildTag(const std::string &tagname)
{
    KMXTag *tag = new KMXTag(tagname);
    addChildTag(tag);
    return tag;
}

inline const std::vector<KMXTag *> &KMXTag::getChildTags() const
{
    return m_child_tags;
}

inline KMXTag *KMXTag::getChildTagByName(const std::string &name)
{
    auto it = std::find_if(m_child_tags.begin(),m_child_tags.end(),[name](KMXTag *tag){
            return tag->m_tag_name == name;
    });
    return (it != m_child_tags.end())?(*it):nullptr;
}

inline std::vector<KMXTag*> KMXTag::getChildTagsByName(const std::string &name)
{
    std::vector<KMXTag*> tags;
    std::for_each(m_child_tags.begin(),m_child_tags.end(),[name,&tags](KMXTag *tag){
            if(tag->m_tag_name == name)
                tags.push_back(tag);
    });
    return tags;
}

inline KMXTag::TagType KMXTag::tagType() const
{
    return m_type;
}

inline void KMXTag::setTagType(TagType newType)
{
    m_type = newType;
}

KMXTag *toKMXTag(const std::string& str, std::string &error);

#endif //RWVIEW_KMX_HPP
