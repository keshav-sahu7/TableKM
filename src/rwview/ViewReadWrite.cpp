#include "ViewReadWrite.hpp"

#include <fstream>
#include <QDebug>

#include "kmx.hpp"

#include <kmt/BasicView.hpp>
#include <kmt/LogMsg.hpp>

#include "../DummyBasicView.hpp"
#include "../misc/KDefines.h"

using SOdr = km::SortingOrder;
using km::err::LogMsg;



void kmxToView(KMXTag *root, km::AbstractTable *parent, QList<km::AbstractView*> &views)
{
    K_RETURN_IF(root == nullptr);
    K_RETURN_IF_NOT(root->getTagName() == "Views");
    for(KMXTag *child_view : root->getChildTagsByName("View"))
    {
        std::string view_name = child_view->getAttributeValue("name");
        std::string sortby = child_view->getAttributeValue("sortby");
        std::string sortingorder = child_view->getAttributeValue("sortingorder");
        std::string exp = child_view->getAttributeValue("expr");
        std::vector<KMXTag*> column_tags = child_view->getChildTagsByName("columns");
        K_CONTINUE_IF_NOT(column_tags.size() == 1);

        std::vector<std::string> column_names;
        for(KMXTag *column_tag : column_tags.front()->getChildTags())
        {
            K_CONTINUE_IF_NOT(column_tag->getTagName() == "column");
            column_names.push_back(column_tag->getAttributeValue("name"));
        }
        K_CONTINUE_IF_NOT(std::atoll(column_tags.front()->getAttributeValue("count").c_str()) == (long long)column_names.size());
        SOdr s_order = sortingorder == "descending" ? SOdr::DESCENDING : SOdr::ASCENDING;

        try{
            km::BasicView *view = new km::BasicView(view_name,parent,column_names,exp,sortby,s_order);
            views.append(view);
            kmxToView(child_view->getChildTagByName("Views"),views.last(),views);
        }
        catch(std::exception &)
        {
            //
        }
    }
}




void viewToKmx(km::AbstractTable *table, KMXTag *root)
{
    root->setTagType(KMXTag::Container);
    for(km::AbstractView *view : table->getDependentViews())
    {
        K_CONTINUE_IF(dynamic_cast<DummyBasicView*>(view));
        KMXTag *view_tag = root->addChildTag("View");
        view_tag->insertAttribute({"name",view->getName()});
        view_tag->insertAttribute({"expr",view->getFilterFormula()});
        view_tag->insertAttribute({"sortingorder",
                                   view->getSortingOrder() == SOdr::ASCENDING ? "ascending" : "descending"
                                  });
        view_tag->insertAttribute({"sortby",view->getColumnMetaData(view->getKeyColumn()).column_name});
        KMXTag *columns = view_tag->addChildTag("columns");
        columns->insertAttribute({"count",std::to_string(view->columnCount())});
        for(km::IndexType i = 0, ccount = view->columnCount(); i < ccount; ++i)
        {
            KMXTag *column = columns->addChildTag("column");
            column->setTagType(KMXTag::Leaf);
            column->insertAttribute({"name",view->getColumnMetaData(i).column_name});
        }

        if(!view->getDependentViews().empty())
        {
            viewToKmx(view,view_tag->addChildTag("Views"));
        }
    }
}





bool loadViews(km::AbstractTable *root_table, const char *filename, QList<km::AbstractView *> &views)
{
    std::ifstream file(filename);
    if(!file.is_open()){
        return true;
    }
    std::string contents;
    file.seekg(0, std::ios::end);
    contents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();

    std::string error;
    KMXTag *root_tag = toKMXTag(contents,error);

    km::err::LockLogFileHandler lock;
    if(root_tag){
        kmxToView(root_tag,root_table, views);
        return true;
    }
    else{
        km::err::addLogMsg(LogMsg("KXML Parser") << error);
        lock.resume();
        km::err::addLogMsg(LogMsg("View Loader") << "file `"
                           << filename << "` contains syntax error");
        return false;
    }
}




bool saveViews(km::AbstractTable *root_table, const char *filename)
{
    std::ofstream file(filename);
    if(!file.is_open())
    {
        km::err::addLogMsg(LogMsg("IO") << "could not open `" << filename << "` for writing!");
        return false;
    }
    KMXTag root_tag("Views");
    viewToKmx(root_table,&root_tag);
    root_tag.writeTo(file,0,4);
    file.close();
    return true;
}


