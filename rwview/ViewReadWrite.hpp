#ifndef VIEWREADWRITE_HPP
#define VIEWREADWRITE_HPP
#include <kmt/AbstractTable.hpp>
#include <kmt/AbstractView.hpp>

#include <QList>

/**
 * @brief loads views from the xml file.
 *
 * It loads views from the xml file pointed by @c filename and stores them
 * in the list @c views. @c root_table is root table that those views belong
 * to. @c root_table is generally a km::Table object as all depenedent views
 * and dependent views of dependent views are stored in xml file.
 *
 * @return if xml file contains any syntax error it will return false. If
 * file could not be opened it will return true. If the file has no syntax
 * error it will load views and will return true, even if it doesn't contain
 * any information about views.
 *
 * @note the parser is not a complete xml parser. It supports only partial
 * features, such as tag names, properties and child tags. But no xml version
 * info, comments and text between tags.
 */
bool loadViews(km::AbstractTable *root_table, const char* filename, QList<km::AbstractView*> &views);

/**
 * @brief writes views to the xml file.
 * It writes all the dependent views of @c root_table to xml file pointed by
 * @c filename.
 *
 * @return returns true if file was opened and data was written; false otherwise.
 */
bool saveViews(km::AbstractTable *root_table, const char *filename);


#endif // VIEWREADWRITE_HPP
