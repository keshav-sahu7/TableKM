#include "DummyBasicView.hpp"

void DummyBasicView::dataUpdated(km::IndexType row_index, km::IndexType column_index, const km::Variant &old_data)
{
    Q_UNUSED(row_index);
    Q_UNUSED(column_index);
    Q_UNUSED(old_data);
    kDataChanged(row_index, column_index);
}

void DummyBasicView::rowInserted(km::IndexType row_index)
{
    kRowInserted(row_index);
}

void DummyBasicView::rowDropped(km::IndexType row_index)
{
    kRowDropped(row_index);
}

void DummyBasicView::sourceSorted()
{
    kCompleteUpdateNeeded();
}

void DummyBasicView::sourceReversed()
{
    //actually this will never be called.
    kCompleteUpdateNeeded();
}

void DummyBasicView::columnTransformed(km::IndexType column_index)
{
    kColumnTransformed(column_index);
}

void DummyBasicView::sourceRefreshed()
{
    //actually this will never be called.
    kCompleteUpdateNeeded();
}

void DummyBasicView::sourceAboutToBeDestructed()
{
    kParentToBeDeleted();
}

