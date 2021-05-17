/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/mmeCommonUtils.h>
#include <cmath>
#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <mme_app.h>
#include <msgBuffer.h>
#include <s1ap_structs.h>
#include <utils/defaultMmeProcedureCtxt.h>
#include <random>

using namespace mme;

extern mme_config_t *mme_cfg;

bool MmeCommonUtils::isLocalGuti(const guti &guti_r)
{
	bool rc = false;

	if (guti_r.mme_grp_id == mme_cfg->mme_group_id &&
			guti_r.mme_code == mme_cfg->mme_code)
	{
		rc = true;
	}

	return rc;
}

uint8_t MmeCommonUtils::select_preferred_int_algo(uint8_t &val)
{
	uint8_t result = 0;

	for(int i = 0; i < MAX_ALGO_COUNT; i++)
    {
        if (val & (0x80 >> mme_cfg->integrity_alg_order[i]))
        {
            result = mme_cfg->integrity_alg_order[i];
            break;
        }
    }

	return result;
}

uint8_t MmeCommonUtils::select_preferred_sec_algo(uint8_t &val)
{
	uint8_t result = 0;

	for(int i = 0; i < MAX_ALGO_COUNT; i++)
    {
        if (val & (0x80 >> mme_cfg->ciphering_alg_order[i]))
        {
            result = mme_cfg->ciphering_alg_order[i];
            break;
        }
    }

	return result;
}

uint32_t MmeCommonUtils::allocateMtmsi()
{
	uint32_t tmsi = 0;
	std::default_random_engine generator(time(NULL));
	std::uniform_int_distribution<int> temp_fun(0, 1000000);

	while(1)
	{
		tmsi = temp_fun(generator);

		if (SubsDataGroupManager::Instance()->findCBWithmTmsi(tmsi) == -1)
			break;
	}

	log_msg(LOG_INFO, "MTMSI allocated is %u", tmsi);

	return tmsi;
}

void MmeCommonUtils::formatS1apPlmnId(struct PLMN* plmn_p)
{
	/* Lets update plmnId .... What we received from s1ap is : 214365 and what we need on
     	* s6a/s11/nas interfaces is - 216354*/

	unsigned char plmn_byte2 = plmn_p->idx[1];
	unsigned char plmn_byte3 = plmn_p->idx[2];
	unsigned char mnc3 = plmn_byte3 >> 4; //  mnc3
	unsigned char mnc2 = plmn_byte3 & 0xf; // mnc2
	unsigned char mnc1 = plmn_byte2 >> 4;  // mnc1
	unsigned char mcc3  = plmn_byte2 & 0xf; //mcc3
	// First byte we are not changing mcc2 mcc1
	if(mnc1 != 0x0F)
	{
		plmn_byte2 = (mnc3 << 4) | mcc3; // 2nd byte on NAS - mnc3 mcc3
		plmn_byte3 = (mnc2 << 4) | mnc1; // 3rd byte on NAS - <mnc2 mnc1>
		plmn_p->idx[1] = plmn_byte2;
          	plmn_p->idx[2] = plmn_byte3;
	}

}

void MmeCommonUtils::getS1apPlmnIdFroms11(struct PLMN* plmn_p)
{
	/* we have on s11/nas/s6a - 216354 */
    /*              s1ap need : 214365 */

	unsigned char plmn_byte2 = plmn_p->idx[1];
	unsigned char plmn_byte3 = plmn_p->idx[2];
	unsigned char mnc3 = plmn_byte2 >> 4; //  mnc3
	unsigned char mnc2 = plmn_byte3 >> 4; // mnc2
	unsigned char mnc1 = plmn_byte3 & 0xf; // mnc1
	unsigned char mcc3  = plmn_byte2 & 0xf; //mcc3
	// First byte we are not changing mcc2 mcc1
	if(mnc1 != 0x0F)
	{
		plmn_byte2 = (mnc1 << 4) | mcc3;
		plmn_byte3 = (mnc3 << 4) | mnc2;
		plmn_p->idx[1] = plmn_byte2;
        plmn_p->idx[2] = plmn_byte3;
	}

}

AttachType MmeCommonUtils::getAttachType(UEContext* ueContext_p,
		const struct ue_attach_info& ue_info)
{
	log_msg(LOG_INFO, "deriveAttachType");

	AttachType attachType = maxAttachType_c;

	if(UE_ID_IMSI(ue_info.flags))
	{
		log_msg(LOG_INFO, "IMSI attach received.");

		attachType = imsiAttach_c;
	}
	else if (UE_ID_GUTI(ue_info.flags))
	{
		log_msg(LOG_INFO, "GUTI attach received. mTMSI is %u ",
				ue_info.mi_guti.m_TMSI);

		attachType = unknownGutiAttach_c;

		if (isLocalGuti(ue_info.mi_guti))
		{
			// The guti is allocated by this MME, check if a context exists.
			// If the context does not exist, treat as unknown GUTI attach?
			log_msg(LOG_INFO, "GUTI is local..");

			if (ueContext_p != NULL)
			{
				if (ueContext_p->getMTmsi() == ue_info.mi_guti.m_TMSI)
				{
					log_msg(LOG_INFO, "and known");

					attachType = knownGutiAttach_c;
				}
				else
				{
					log_msg(LOG_INFO, "mTMSI mismatches with UE context. "
							"Treat as unknown GUTI attach");
				}
			}
			else
			{
				log_msg(LOG_INFO, "UE context is null. Unknown GUTI attach triggered");
			}

		}
		else
		{
			log_msg(LOG_INFO, "GUTI is not local..");
		}
	}
	return attachType;
}

SM::ControlBlock* MmeCommonUtils::findControlBlock(cmn::utils::MsgBuffer* buf)
{
	SM::ControlBlock *cb = NULL;

	const s1_incoming_msg_header_t* msgData_p = (s1_incoming_msg_header_t*)(buf->getDataPointer());
	if(msgData_p == NULL)
	{
		log_msg(LOG_INFO, "MsgData is NULL .");
		return cb;
	}

	switch (msgData_p->msg_type)
	{
		case attach_request:
		{
			const ue_attach_info_t *ue_info = (ue_attach_info_t *)(msgData_p);
			if(UE_ID_IMSI(ue_info->flags))
			{
				log_msg(LOG_INFO, "IMSI attach received.");

				uint8_t imsi[BINARY_IMSI_LEN] = {0};
				memcpy( imsi, ue_info->IMSI, BINARY_IMSI_LEN );

				uint8_t first = imsi[0] >> 4;
				imsi[0] = (uint8_t)(( first << 4 ) | 0x0f );

				DigitRegister15 IMSIInfo;
				IMSIInfo.convertFromBcdArray(imsi);

				int cbIndex = SubsDataGroupManager::Instance()->findCBWithimsi(IMSIInfo);
				if (cbIndex > 0)
				{
					log_msg(LOG_DEBUG, "existing cb for IMSI. %s ",IMSIInfo.getDigitsArray());
					cb = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
				}
				if (cb == NULL)
				{
					//Deleting the IMSI key, since there is no cb associated with it.
					SubsDataGroupManager::Instance()->deleteimsikey(IMSIInfo);

					log_msg(LOG_INFO, "create new cb for IMSI %s.",
							IMSIInfo.getDigitsArray());

					cb = SubsDataGroupManager::Instance()->allocateCB();
					if(cb == NULL) 
					{
						log_msg(LOG_DEBUG, "create new cb for IMSI failed. %s ",IMSIInfo.getDigitsArray());
						return nullptr;
					}
					cb->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
				}
			}
			else if (UE_ID_GUTI(ue_info->flags))
			{
				log_msg(LOG_INFO, "GUTI attach received.");

				if (isLocalGuti(ue_info->mi_guti))
				{
					log_msg(LOG_INFO, "GUTI is local.");

					int cbIndex = SubsDataGroupManager::Instance()->findCBWithmTmsi(ue_info->mi_guti.m_TMSI);
					if (cbIndex > 0)
					{
						cb = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
						if(cb == NULL)
						{
						    log_msg(LOG_INFO, "Failed to find control block with mTmsi %d", 
								    ue_info->mi_guti.m_TMSI);
						    // Deleting stale mTmsi -> cbIdx mapping.
						    SubsDataGroupManager::Instance()->deletemTmsikey(ue_info->mi_guti.m_TMSI);
						}
					}
					else
					{
						log_msg(LOG_ERROR, "Failed to find control block with mTmsi.");

						// allocate new cb and proceed?
						cb = SubsDataGroupManager::Instance()->allocateCB();
						cb->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
					}
				}
				else
				{
					cb = SubsDataGroupManager::Instance()->allocateCB();
					cb->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
				}
			}
			break;
		}
		case service_request:
		{
			int cbIndex = SubsDataGroupManager::Instance()->findCBWithmTmsi(msgData_p->ue_idx);
			if (cbIndex > 0)
			{
				cb = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
			}
			else
			{
				log_msg(LOG_INFO, "Failed to find control block with mTmsi %d", msgData_p->ue_idx);
			}

			if (cb == NULL)
			{
			    // Deleting stale mTmsi -> cbIdx mapping.
			    SubsDataGroupManager::Instance()->deletemTmsikey(msgData_p->ue_idx);
			    log_msg(LOG_INFO, "Failed to find control block using mtmsi %d."
						" Allocate a temporary control block", msgData_p->ue_idx);
			    
			    // Respond  with Service Reject from default Service Request event handler
			    cb = SubsDataGroupManager::Instance()->allocateCB();
			    cb->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
			}

			break;
		}
		case detach_request:
		{
			const detach_req_Q_msg_t *detach_Req = (const detach_req_Q_msg_t *)(msgData_p);
			int cbIndex = SubsDataGroupManager::Instance()->findCBWithmTmsi(detach_Req->ue_m_tmsi);
			if (cbIndex > 0)
			{
				cb = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
				if(cb == NULL)
				{
				    log_msg(LOG_INFO,"No CB found for UE with mTMSI %d", detach_Req->ue_m_tmsi);
				    // Deleting stale mTmsi -> cbIdx mapping.
				    SubsDataGroupManager::Instance()->deletemTmsikey(detach_Req->ue_m_tmsi);
				}
			}
			else
			{
				log_msg(LOG_INFO, "Failed to find control block with mTmsi. %d", detach_Req->ue_m_tmsi);
			}
			break;
		}
		case tau_request:
		{
			const tauReq_Q_msg_t *tau_Req = (const tauReq_Q_msg_t *)(msgData_p);
			int cbIndex = SubsDataGroupManager::Instance()->findCBWithmTmsi(tau_Req->ue_m_tmsi);
			if (cbIndex > 0)
			{
				cb = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
			}
			else
			{
				log_msg(LOG_INFO, "Failed to find control block with mTmsi %d", tau_Req->ue_m_tmsi);
			}

			if (cb == NULL)
			{
			    // Deleting stale mTmsi -> cbIdx mapping.
			    SubsDataGroupManager::Instance()->deletemTmsikey(tau_Req->ue_m_tmsi);
			    log_msg(LOG_INFO, "Failed to find control block using mtmsi %d."
					    " Allocate a temporary control block", tau_Req->ue_m_tmsi);

			    // Respond  with TAU Reject from default TAU Request event handler
			    cb = SubsDataGroupManager::Instance()->allocateCB();
			    cb->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
			}

			break;
		}
		default:
		{
			log_msg(LOG_INFO, "Unhandled message type %d ", msgData_p->msg_type);
		}
	}

	return cb;
}

ControlBlock* MmeCommonUtils::findControlBlockForS11Msg(cmn::utils::MsgBuffer* msg_p)
{
    ControlBlock* cb_p = NULL;

    const gtp_incoming_msg_data_t* msgData_p = (gtp_incoming_msg_data_t*)(msg_p->getDataPointer());
    if(msgData_p == NULL)
    {
        log_msg(LOG_INFO, "GTP message data is NULL .");
        return cb_p;
    }

    switch (msgData_p->msg_type)
    {
        case downlink_data_notification:
        {
	    const struct ddn_Q_msg* ddn = (const struct ddn_Q_msg*) (msg_p->getDataPointer());
            if (ddn->s11_mme_cp_teid == 0)
            {
                log_msg(LOG_INFO, "UE Index in DDN message data is 0.");
                return cb_p;
            }

            cb_p = SubsDataGroupManager::Instance()->findControlBlock(ddn->s11_mme_cp_teid);
            if (cb_p == NULL)
            {
                log_msg(LOG_INFO, "Failed to find control block using index %d."
                        " Allocate a temporary control block", ddn->s11_mme_cp_teid);

                // Respond  with DDN failure from default DDN event handler
                cb_p = SubsDataGroupManager::Instance()->allocateCB();
                cb_p->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
            }
        }break;
	case create_bearer_request:
        {
	    const struct cb_req_Q_msg * cbr = (const struct cb_req_Q_msg *) (msg_p->getDataPointer());
            if (cbr->s11_mme_cp_teid == 0)
            {
                log_msg(LOG_INFO, "UE Index in CB Req message data is 0.");
                return cb_p;
            }

            cb_p = SubsDataGroupManager::Instance()->findControlBlock(cbr->s11_mme_cp_teid);
            if (cb_p == NULL)
            {
                log_msg(LOG_INFO, "Failed to find control block using index %d."
                        " Allocate a temporary control block", cbr->s11_mme_cp_teid);

                // Respond  with CB Resp from default CB Req event handler
                cb_p = SubsDataGroupManager::Instance()->allocateCB();
                cb_p->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
            }
        }break;
	case delete_bearer_request:
        {
            const struct db_req_Q_msg * dbr = (const struct db_req_Q_msg *) (msg_p->getDataPointer());
            if (dbr->s11_mme_cp_teid == 0)
            {
                log_msg(LOG_INFO, "UE Index in DB Req message data is 0.");
                return cb_p;
            }

            cb_p = SubsDataGroupManager::Instance()->findControlBlock(dbr->s11_mme_cp_teid);
            if (cb_p == NULL)
            {
                log_msg(LOG_INFO, "Failed to find control block using index %d."
                        " Allocate a temporary control block", dbr->s11_mme_cp_teid);

                // Respond  with DB Resp from default DB Req event handler
                cb_p = SubsDataGroupManager::Instance()->allocateCB();
                cb_p->addTempDataBlock(DefaultMmeProcedureCtxt::Instance());
            }
        }break;
        default:
        {
            log_msg(LOG_INFO, "Unhandled message type");
        }
    }
    return cb_p;
}

bool MmeCommonUtils::isEmmInfoRequired(ControlBlock& cb, UEContext& ueCtxt, MmeProcedureCtxt& procCtxt)
{
	bool rc = false;
	if (procCtxt.getCtxtType() == attach_c)
	{
    		MmeAttachProcedureCtxt& attachCtxt = dynamic_cast<MmeAttachProcedureCtxt &>(procCtxt);
    		if (attachCtxt.getAttachType() == imsiAttach_c)
    		{
        		rc = true;
    		}
	}
	return rc;
}

bool MmeCommonUtils::isUeNRCapable(UEContext &ueCtxt)
{
    bool rc;

    if (!ueCtxt.getUeNetCapab().ue_net_capab_m.u.bits.dcnr) {
        log_msg(LOG_DEBUG, "UE does not support dual connectivity");
        rc = false;
    } else if (!mme_cfg->feature_list.dcnr_support) {
        log_msg(LOG_DEBUG,"MME local config does not allow dual connectivity");
        rc = false;
    } else if (!(ueCtxt.getHssFeatList2().feature_list & nrAsSecRatBitMask_c)) {
        log_msg(LOG_DEBUG,"HSS does not support dual connectivity feature");
        rc = false;
    } else if (ueCtxt.getAccessRestrictionData() & nrAsSecRatInEutranNotAllowedBitMask_c) {
        log_msg(LOG_DEBUG,"hss informed about access restriction for this UE");
        rc = false;
    } else {
        log_msg(LOG_DEBUG,"All well, this UE can use DCNR");
        rc = true;
    }

    return rc;
}



