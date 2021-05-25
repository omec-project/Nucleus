/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __S10_STRUCTS_H_
#define __S10_STRUCTS_H_

#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nas_structs.h>
#include "s1ap_structs.h"
#include"s11_structs.h"

#define S10_MSGBUF_SIZE 2048

typedef struct tagbstring* bstring;
typedef uint32_t teid_t;


typedef struct macro_enb_id_s {
  unsigned enb_id : 20;
  uint16_t tac;
} macro_enb_id_t;

typedef struct {
  unsigned enb_id : 28;
  uint16_t tac;
} home_enb_id_t;

typedef enum {
  TARGET_ID_RNC_ID = 0,
  TARGET_ID_MACRO_ENB_ID = 1,
  TARGET_ID_CELL_ID = 2,
  TARGET_ID_HOME_ENB_ID = 3
  /* Other values are spare */
} target_type_t;



typedef struct rnc_id_s {
  uint16_t lac;
  uint8_t rac;

  /* Length of RNC Id can be 2 bytes if length of element is 8
   * or 4 bytes long if length is 10.
   */
  uint16_t id;
  uint16_t xid;
} rnc_id_t;


typedef struct target_identification_s {
  /* Common part */
  uint8_t target_type;

  uint16_t mcc;
  uint16_t mnc;
  union {
    rnc_id_t rnc_id;
    macro_enb_id_t macro_enb_id;
    home_enb_id_t home_enb_id;
  } target_id;
} target_identification_t;


typedef uint32_t in_addr_t;


typedef enum SelectionMode_e {
  MS_O_N_P_APN_S_V = 0,  ///< MS or network provided APN, subscribed verified
  MS_P_APN_S_N_V = 1,    ///< MS provided APN, subscription not verified
  N_P_APN_S_N_V = 2,     ///< Network provided APN, subscription not verified
} SelectionMode_t;


typedef uint8_t APNRestriction_t;

struct tagbstring {
  int mlen;
  int slen;
  unsigned char* data;
};


typedef uint64_t bitrate_t;
typedef struct {
  bitrate_t br_ul;
  bitrate_t br_dl;
}ambr_t;

#define MSG_FORWARD_RELOCATION_REQUEST_MAX_PDN_CONNECTIONS 3


typedef struct pdn_connection_s {
  //  char                      apn[APN_MAX_LENGTH + 1]; ///< Access Point Name
  //  protocol_configuration_options_t pco;
  bstring apn_str;
  //  int                       pdn_type;

  APNRestriction_t
      apn_restriction;  ///< This IE shall be included on the S5/S8 and S4/S11
  ///< interfaces in the E-UTRAN initial attach, PDP Context
  ///< Activation and UE Requested PDN connectivity
  ///< procedures.
  ///< This IE shall also be included on S4/S11 during the Gn/Gp
  ///< SGSN to S4 SGSN/MME RAU/TAU procedures.
  ///< This IE denotes the restriction on the combination of types
  ///< of APN for the APN associated with this EPS bearer
  ///< Context.

  SelectionMode_t selection_mode;  ///< Selection Mode
  ///< This IE shall be included on the S4/S11 and S5/S8
  ///< interfaces for an E-UTRAN initial attach, a PDP Context
  ///< Activation and a UE requested PDN connectivity.
  ///< This IE shall be included on the S2b interface for an Initial
  ///< Attach with GTP on S2b and a UE initiated Connectivity to
  ///< Additional PDN with GTP on S2b.
  ///< It shall indicate whether a subscribed APN or a non
  ///< subscribed APN chosen by the MME/SGSN/ePDG was
  ///< selected.
  ///< CO: When available, this IE shall be sent by the MME/SGSN on
  ///< the S11/S4 interface during TAU/RAU/HO with SGW
  ///< relocation.

  //  uint8_t                   ipv4_address[4];
  //  uint8_t                   ipv6_address[16];
  //  pdn_type_value_t pdn_type;
  struct in_addr ipv4_address;
  struct in6_addr ipv6_address;
  uint8_t ipv6_prefix_length;

  uint8_t linked_eps_bearer_id;

  fteid_t pgw_address_for_cp;  ///< PGW S5/S8 address for control plane or PMIP

  bearer_ctx_list_t bearer_context_list;      /// TODO structure need to be created

  ambr_t apn_ambr;

} pdn_connection_t;

typedef struct mme_ue_eps_pdn_connections_s {
  uint8_t num_pdn_connections;
  uint8_t num_processed_pdn_connections;
  pdn_connection_t
      pdn_connection[MSG_FORWARD_RELOCATION_REQUEST_MAX_PDN_CONNECTIONS];
} mme_ue_eps_pdn_connections_t;

typedef struct bearer_context_created_s {
  uint8_t eps_bearer_id;  ///< EPS Bearer ID
  struct gtp_cause cause;

  /* This parameter is used on S11 interface only */
  fteid_t s1u_sgw_fteid;  ///< S1-U SGW F-TEID
  fteid_t s1u_enb_fteid;  ///< S1-U ENB F-TEID

  /* This parameter is used on S4 interface only */
  fteid_t s4u_sgw_fteid;  ///< S4-U SGW F-TEID

  /* This parameter is used on S11 and S5/S8 interface only for a
   * GTP-based S5/S8 interface and during:
   * - E-UTRAN Inintial attch
   * - PDP Context Activation
   * - UE requested PDN connectivity
   */
  fteid_t s5_s8_u_pgw_fteid;  ///< S4-U SGW F-TEID

  /* This parameter is used on S4 interface only and when S12 interface is used
   */
  fteid_t s12_sgw_fteid;  ///< S12 SGW F-TEID

  /* This parameter is received only if the QoS parameters have been modified */
  bearer_qos_t bearer_level_qos;

  //traffic_flow_template_t tft;  ///< Bearer TFT   TODO this IE is skup for now
} bearer_context_created_t;

#define MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS 11
typedef struct bearer_contexts_created_s {
  uint8_t num_bearer_context;
  bearer_context_created_t
      bearer_context[MSG_CREATE_SESSION_REQUEST_MAX_BEARER_CONTEXTS];
} bearer_contexts_created_t;


#ifdef S10_FEATURE
typedef struct mm_context {
    security_mode security_mode;
    E_UTRAN_security_vector sec_vector;
    enum drx_params drx;
    unsigned int dl_count;
    unsigned int ul_count;
    unsigned int security_encryption_algo;
    unsigned int security_integrity_algo;
    ue_add_sec_capabilities ue_add_sec_capab;
    UE_net_capab ue_network;
    bool isNHIpresent;
    struct security_context security_context;
    ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
} mm_context_t;
#endif



#endif /* S10_STRUCTS_H */
