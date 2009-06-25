/*---------------------------------------------------------------------\
|                                                                      |
|                        __   __    _____ _____                        |
|                        \ \ / /   /  ___|_   _|                       |
|                         \ V /__ _\ `--.  | |                         |
|                          \ // _` |`--. \ | |                         |
|                          | | (_| /\__/ / | |                         |
|                          \_/\__,_\____/  \_/                         |
|                                                                      |
\---------------------------------------------------------------------*/

#ifndef YQMODULESPROXYMODEL_H
#define YQMODULESPROXYMODEL_H

#include <QStringList>
#include "kcategorizedsortfilterproxymodel.h"

/**
 * Proxy Model to sort by categories
 *
 * @author Duncan Mac-Vicar P. <dmacvicar@suse.de>
 */
class YQModulesProxyModel : public KCategorizedSortFilterProxyModel
{
Q_OBJECT
public:

  YQModulesProxyModel( QObject * parent = 0 );
  ~YQModulesProxyModel();

  /**
   * This method has a similar purpose as lessThan() has on QSortFilterProxyModel.
   * It is used for sorting items that are in the same category.
   *
   * @return Returns true if the item @p left is less than the item @p right when sorting.
   */
  virtual bool subSortLessThan(const QModelIndex &left, const QModelIndex &right) const;

  /**
   * This method compares the category of the @p left index with the category
   * of the @p right index.
   *
   * Internally and if not reimplemented, this method will ask for @p left and
   * @p right models for role CategorySortRole. In order to correctly sort
   * categories, the data() metod of the model should return a qlonglong (or numeric) value, or
   * a QString object. QString objects will be sorted with QString::localeAwareCompare if
   * sortCategoriesByNaturalComparison() is true.
   *
   */
   virtual int compareCategories(const QModelIndex &left, const QModelIndex &right) const;

   //virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
};

#endif
