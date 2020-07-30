#ifndef _INCLUDE_MME_APP_PROM_CLIENT_H
#define _INCLUDE_MME_APP_PROM_CLIENT_H

#include "contextManager/dataBlocks.h"

void promThreadSetup(void);

class statistics {
    public:
        statistics();
        ~statistics() {}
        static statistics* Instance();
        void promThreadSetup(void);
        void ue_attached(mme::UEContext *ue);
        void ue_detached(mme::UEContext *ue);
    public:
#if 1
        Family<Gauge> &ue_info;
        Gauge &current_sub;
#endif
};

#endif
