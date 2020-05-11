/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>
#include <freeDiameter/libfdcore.h>

#include "s6a_fd.h"
#include "s6a.h"

extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;


/**
 * @brief Parse supported features for UE
 * @param[in] avp_ptr - POinter to supported features avp
 * @param[out] supp_features information filled with parsed data
 * @return void
 */
void
parse_supported_features_avp(struct avp *avp_ptr, supported_features *supp_features)
{
        struct avp *avp = NULL;
        struct avp_hdr *hdr = NULL;
        bool rc = false;
	
	CHECK_FCT_DO(fd_msg_browse(avp_ptr, MSG_BRW_FIRST_CHILD, &avp, NULL), return);
        while (avp) {

		fd_msg_avp_hdr (avp, &hdr);

                switch(hdr->avp_code)
                {
                    /*Storing only feature-list of ID 2 for NSA MME:Diameter Protocol Layer Support*/
		    case FEAT_LIST_ID_AVP_CODE:
                    {
			if(hdr->avp_value->u32 != 2) return;
			supp_features->feature_list_id = hdr->avp_value->u32;
		        rc = true;			
		    } break;
                    case FEAT_LIST_AVP_CODE:
                    {
                        if(rc)
			    supp_features->feature_list = hdr->avp_value->u32;
                    } break;
                    default:
                        goto next;
                }
                next:
                    /* Go to next AVP */
                    CHECK_FCT_DO(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL), return);
        }
}
