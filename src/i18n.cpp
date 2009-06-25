
#include "i18n.h"

void
set_textdomain (const char* domain)
{
    bindtextdomain (domain, LOCALEDIR);
    textdomain (domain);
    bind_textdomain_codeset (domain, "UTF-8");
}
