#ifndef DUMMYBASICVIEW_HPP
#define DUMMYBASICVIEW_HPP

#include <QObject>

#include <kmt/AbstractView.hpp>

#define DBV_CREATE_DEFAULT_IMPL \
    virtual std::optional<std::pair<km::IndexType, km::DataType> > findColumn(const std::string &) const override { return {};}\
    virtual std::optional<std::pair<std::string, km::DataType> > columnAt(km::IndexType) const override {return {};}\
    virtual const km::ColumnMetaData &getColumnMetaData(km::IndexType) const override{\
        static km::ColumnMetaData data("__dummy_column",km::DataType::INT32);\
        return data;\
    }\
    virtual km::SizeType rowCount() const override { return 0;}\
    virtual km::SizeType columnCount() const override {return 0;}\
    virtual std::optional<km::Variant> getData(km::IndexType, km::IndexType) const override { return {};}\
    virtual km::Variant getDataWC(km::IndexType, km::IndexType) const override{\
        return km::Variant();\
    }\
    virtual void sortBy(km::SortingOrder) override {}\
    virtual void sortBy(const std::string &) override {}\
    virtual void sortBy(const std::string &, km::SortingOrder) override {}\
    virtual km::IndexType mapToLocal(km::IndexType) override{\
        return km::INVALID_INDEX;\
    }\
    virtual void refresh() override{};


/**
 * This class is designed only to get notified about views, so that it can notify
 * K2QtTableModel class as if the view will be changed then it will trigger the child
 * views so by making DummyBasicView object a child view, it will be notified about
 * changes with exact locations. Else either we would need to reimplement all those
 * functions and notify K2QtTableModel or we have to scale the parent's indices to
 * local and call non abstract view classes (e.g. km::BasicView)'s functions. Which
 * will need more codes and will be time consuming.
 *
**/
class DummyBasicView final : public QObject, public km::AbstractView
{
    Q_OBJECT

public:
    DummyBasicView(km::AbstractView* view, QObject *parent);
signals:
    void kDataChanged(km::IndexType r, km::IndexType c);
    void kRowInserted(km::IndexType r);
    void kRowDropped(km::IndexType r);
    void kCompleteUpdateNeeded();
    void kColumnTransformed(km::IndexType c);
    void kParentToBeDeleted();
protected:
    virtual void dataUpdated(km::IndexType row_index, km::IndexType column_index, const km::Variant &old_data) override;
    virtual void rowInserted(km::IndexType row_index) override;
    virtual void rowDropped(km::IndexType row_index) override;
    virtual void sourceSorted() override;
    virtual void sourceReversed() override;
    virtual void columnTransformed(km::IndexType column_index) override;
    virtual void sourceRefreshed() override;
    virtual void sourceAboutToBeDestructed() override;
private:
    //ignore these functions
    DBV_CREATE_DEFAULT_IMPL
};

inline DummyBasicView::DummyBasicView(km::AbstractView * view, QObject *parent)
    : QObject(parent), AbstractView("","",km::SortingOrder::ASCENDING)
{
    setSourceTable(view);
}



using ViewChangeNotifier = DummyBasicView;
#endif // DUMMYBASICVIEW_HPP
