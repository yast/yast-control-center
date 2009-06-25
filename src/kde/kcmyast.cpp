
#include <KAboutData>

#include <kdecore_export.h>
#include <kgenericfactory.h>


#include "kcmyast.h"

K_PLUGIN_FACTORY(YaSTFactory, registerPlugin<KCMYaST>();)
K_EXPORT_PLUGIN(YaSTFactory("yast", "kcm_yast"))

KCMYaST::KCMYaST(QWidget *parent, const QVariantList &args)
        : KCModule(YaSTFactory::componentData(), parent, args)
{
    KAboutData *about = new KAboutData("kcm_yast", 0, ki18n("YaST"), "1.0");
    setAboutData(about);
//     setButtons(Apply | Help);
}

KCMYaST::~KCMYaST()
{
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return (QString::localeAwareCompare(s1, s2) < 0);
}

void KCMYaST::load()
{
    emit changed(false);
}

void KCMYaST::save()
{
}

void KCMYaST::defaults()
{
//    emit changed(true);
    emit changed(false);
}
