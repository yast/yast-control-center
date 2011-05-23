
#include "yqmodulesproxymodel.h"

YQModulesProxyModel::YQModulesProxyModel( QObject * parent )
    : KCategorizedSortFilterProxyModel(parent)
{
}

YQModulesProxyModel::~YQModulesProxyModel()
{
}

bool YQModulesProxyModel::subSortLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return KCategorizedSortFilterProxyModel::subSortLessThan(left, right);
}

int YQModulesProxyModel::compareCategories(const QModelIndex &left, const QModelIndex &right) const
{
    return KCategorizedSortFilterProxyModel::compareCategories(left, right);
}



// QVariant YQModulesProxyModel::data( const QModelIndex &index, int role ) const
// {
//    if ( role == CategoryDisplayRole )
//    {
//    }
   
//    return YQDesktopFilesModel::data(index, role);
// }

#include "yqmodulesproxymodel.moc"
