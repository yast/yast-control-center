
#ifndef YCCModuleInfoProvider_H
#define YCCModuleInfoProvider_H

#include "yqmodulesmodel.h"

class YCCModuleInfoProvider
{
public:
  YCCModuleInfoProvider();
  ~YCCModuleInfoProvider();
  
  QString provideInfo( const YQModulesModel *model, const QModelIndex &module ) const;

private:
};

#endif
