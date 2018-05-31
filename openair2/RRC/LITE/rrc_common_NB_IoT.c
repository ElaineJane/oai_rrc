/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file rrc_common.c
 * \brief rrc common procedures for eNB and UE
 * \author Navid Nikaein and Raymond Knopp
 * \date 2011 - 2014
 * \version 1.0
 * \company Eurecom
 * \email:  navid.nikaein@eurecom.fr and raymond.knopp@eurecom.fr
 */

//#include "defs_NB_IoT.h"
//#include "extern.h"
//#include "RRC/LITE/extern_NB_IoT.h"
#include "common/utils/collection/tree.h"
#include "LAYER2/MAC/extern_NB_IoT.h"
//#include "COMMON/openair_defs.h"
//#include "COMMON/platform_types.h"
//#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
#include "LAYER2/RLC/rlc.h"
//#include "COMMON/mac_rrc_primitives.h"
#include "UTIL/LOG/log.h"
#include "asn1_msg.h"
#include "pdcp.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
//#include "rrc_eNB_UE_context.h"
//#include "proto_NB_IoT.h"
#include "RRC/LITE/defs_NB_IoT.h"
//#include "RRC/LITE/vars_NB_IoT.h"

/* NB-IoT */
#include "UL-CCCH-Message-NB.h"
#include "UL-DCCH-Message-NB.h"
#include "DL-CCCH-Message-NB.h"
#include "DL-DCCH-Message-NB.h"
#include "T.h"
#include "msc.h"

#ifdef LOCALIZATION
#include <sys/time.h>
#endif

#define DEBUG_RRC 1
extern eNB_MAC_INST *eNB_mac_inst;
extern UE_MAC_INST *UE_mac_inst;

extern mui_t rrc_eNB_mui;

/*missed functions*/
//rrc_top_cleanup
//binary_search_init
//binary_search_float


//--------------
//MP: Most probably is not needed (old code)
//-----------------------------------------------------------------------------
void rrc_t310_expiration_NB_IoT(
  const protocol_ctxt_t* const ctxt_pP,
  const uint8_t                 eNB_index
)
//-----------------------------------------------------------------------------
{

  if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].State != RRC_CONNECTED_NB_IoT) {
    LOG_D(RRC, "Timer 310 expired, going to RRC_IDLE_NB_IoT\n");
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].State = RRC_IDLE_NB_IoT;
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].UE_index = 0xffff;
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[eNB_index].Rx_buffer.payload_size = 0;
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[eNB_index].Tx_buffer.payload_size = 0;
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1[eNB_index].Srb_info.Rx_buffer.payload_size = 0;
    UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1[eNB_index].Srb_info.Tx_buffer.payload_size = 0;

    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Active == 1) {
      msg ("[RRC Inst %d] eNB_index %d, Remove RB %d\n ", ctxt_pP->module_id, eNB_index,
           UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Srb_info.Srb_id);
      rrc_pdcp_config_req (ctxt_pP, // MP
                           SRB_FLAG_YES,
                           CONFIG_ACTION_REMOVE,
                           UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Srb_info.Srb_id,
                           0);

      rrc_rlc_config_req_NB_IoT(
    		  	  	  	  	        ctxt_pP,
							                  SRB_FLAG_YES,
							                  CONFIG_ACTION_REMOVE,
							                  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Srb_info.Srb_id,
							                  Rlc_info_am_NB_IoT);


      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Active = 0;
      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Status = IDLE;
      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb1bis[eNB_index].Next_check_frame = 0;
    }
  } else { // Restablishment procedure
    LOG_D(RRC, "Timer 310 expired, trying RRCRestablishment ...\n");
  }
}




//configure  BCCH & CCCH Logical Channels and associated rrc_buffers, configure associated SRBs
//called by openair_rrc_eNB_configuration_NB_IoT
//-----------------------------------------------------------------------------
void openair_eNB_rrc_on_NB_IoT(
  const protocol_ctxt_t* const ctxt_pP
)
//-----------------------------------------------------------------------------
{

  int            CC_id;

    LOG_I(RRC, PROTOCOL_RRC_CTXT_FMT" OPENAIR RRC-NB IN....\n",
          PROTOCOL_RRC_CTXT_ARGS(ctxt_pP));
    for (CC_id = 0; CC_id < MAX_NUM_CCs; CC_id++) {
      rrc_config_buffer_NB_IoT (&eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].SI, BCCH, 1);
      eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].SI.Active = 1;
      rrc_config_buffer_NB_IoT (&eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].Srb0, CCCH, 1);
      eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].Srb0.Active = 1;
    }
    //no UE side
 }



//-----------------------------------------------------------------------------
void rrc_config_buffer_NB_IoT(
  SRB_INFO_NB_IoT* Srb_info,
  uint8_t Lchan_type,
  uint8_t Role
)
//-----------------------------------------------------------------------------
{

  Srb_info->Rx_buffer.payload_size = 0;
  Srb_info->Tx_buffer.payload_size = 0;
}


//-----------------------------------------------------------------------------
//XXX NEW mplementation by Raymond: still used but no more called by MAC/main.c instead directly called by rrc_eNB_nb_iot.c
//XXX maybe this function is no more useful
int rrc_init_global_param_NB_IoT( void )
//-----------------------------------------------------------------------------
{

  //may no more used (defined in rlc_rrc.c)
  rrc_rlc_register_rrc_NB_IoT (rrc_data_ind_NB_IoT, NULL); //register with rlc

  //XXX MP: most probably ALL of this stuff are no more needed (also the one not commented)

  //DCCH_LCHAN_DESC.transport_block_size = 4;....

  //Setting of this values????
  Rlc_info_am_config_NB_IoT.rlc_mode = RLC_MODE_AM; //only allowed for NB-IoT
  Rlc_info_am_config_NB_IoT.rlc.rlc_am_info_NB_IoT.max_retx_threshold_NB_IoT = 50;
  Rlc_info_am_config_NB_IoT.rlc.rlc_am_info_NB_IoT.t_poll_retransmit_NB_IoT = 15;
  Rlc_info_am_config_NB_IoT.rlc.rlc_am_info_NB_IoT.enableStatusReportSN_Gap_NB_IoT = NULL; //should be disabled


#ifndef NO_RRM

if (L3_xface_init_NB_IoT ()) { //XXX to be modified???
    return (-1);
  }

#endif

  return 0;
}


#ifndef NO_RRM
//-----------------------------------------------------------------------------
int L3_xface_init_NB_IoT( //Exact copy of the LTE implementation
  void
)
//-----------------------------------------------------------------------------
{

  int ret = 0;

#ifdef USER_MODE

  int sock;
  LOG_D(RRC, "[L3_XFACE] init de l'interface \n");

  if (open_socket (&S_rrc, RRC_RRM_SOCK_PATH, RRM_RRC_SOCK_PATH, 0) == -1) {
    return (-1);
  }

  if (S_rrc.s == -1) {
    return (-1);
  }

  socket_setnonblocking (S_rrc.s);
  msg ("Interface Connected... RRM-RRC\n");
  return 0;

#else

  ret=rtf_create(RRC2RRM_FIFO,32768);

  if (ret < 0) {
    msg("[openair][MAC][INIT] Cannot create RRC2RRM fifo %d (ERROR %d)\n",RRC2RRM_FIFO,ret);
    return(-1);
  } else {
    msg("[openair][MAC][INIT] Created RRC2RRM fifo %d\n",RRC2RRM_FIFO);
    rtf_reset(RRC2RRM_FIFO);
  }

  ret=rtf_create(RRM2RRC_FIFO,32768);

  if (ret < 0) {
    msg("[openair][MAC][INIT] Cannot create RRM2RRC fifo %d (ERROR %d)\n",RRM2RRC_FIFO,ret);
    return(-1);
  } else {
    msg("[openair][MAC][INIT] Created RRC2RRM fifo %d\n",RRM2RRC_FIFO);
    rtf_reset(RRM2RRC_FIFO);
  }

  return(0);

#endif
}
#endif

//------------------------------------------------------------------------------
//specialized function for the eNB initialization (NB-IoT)
//(OLD was called in MAC/main.c--> mac_top_init)(NEW is called in directly in "openair_rrc_eNB_configuration_NB_IoT")
void openair_rrc_top_init_eNB_NB_IoT(void)//MP: XXX Raymond put this directly the definition on rrc_eNB.c file
//-----------------------------------------------------------------------------
{

  int                 CC_id;

  /* for no gcc warnings */
  (void)CC_id;

//not consider UE part

    eNB_rrc_inst_NB_IoT = (eNB_RRC_INST_NB_IoT*) malloc16(sizeof(eNB_RRC_INST_NB_IoT));
    memset (eNB_rrc_inst_NB_IoT, 0, sizeof(eNB_RRC_INST_NB_IoT));
    LOG_D(RRC, "ALLOCATE %d Bytes for eNB_RRC_INST NB-IoT @ %p\n", (unsigned int)(sizeof(eNB_RRC_INST_NB_IoT)), eNB_rrc_inst_NB_IoT);

//no CBA, no LOcalization, no MBMS flag

    LOG_D(RRC,
          "ALLOCATE %d Bytes for eNB_RRC_INST_NB @ %p\n", (unsigned int)(sizeof(eNB_RRC_INST_NB_IoT)), eNB_rrc_inst_NB_IoT);


}

void
openair_rrc_on_NB_IoT(
  const protocol_ctxt_t* const ctxt_pP
)
//-----------------------------------------------------------------------------
{
  unsigned short i;
  int            CC_id;

  if (ctxt_pP->enb_flag == ENB_FLAG_YES) {
    LOG_I(RRC, PROTOCOL_RRC_CTXT_FMT" OPENAIR RRC IN....\n",
          PROTOCOL_RRC_CTXT_ARGS(ctxt_pP));
    for (CC_id = 0; CC_id < MAX_NUM_CCs; CC_id++) {
      rrc_config_buffer (&eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].SI, BCCH, 1);
      eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].SI.Active = 1;
      rrc_config_buffer (&eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].Srb0, CCCH, 1);
      eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].carrier[CC_id].Srb0.Active = 1;
    }
  } else {
    LOG_I(RRC, PROTOCOL_RRC_CTXT_FMT" OPENAIR RRC IN....\n",
          PROTOCOL_RRC_CTXT_ARGS(ctxt_pP));

    for (i = 0; i < NB_eNB_INST; i++) {
      LOG_D(RRC, PROTOCOL_RRC_CTXT_FMT" Activating CCCH (eNB %d)\n",
            PROTOCOL_RRC_CTXT_ARGS(ctxt_pP), i);
      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[i].Srb_id = CCCH;
      //memcpy (&UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[i].Rx_buffer, &CCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
      //memcpy (&UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[i].Tx_buffer, &CCCH_LCHAN_DESC, LCHAN_DESC_SIZE);
      rrc_config_buffer (&UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[i], CCCH, 1);
      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[i].Active = 1;
    }
  }
}


//-----------------------------------------------------------------------------
//XXX MP: most probably is not needed
RRC_status_t rrc_rx_tx_NB_IoT(
  protocol_ctxt_t* const ctxt_pP,
  const uint8_t      enb_indexP,
  const int          CC_id
)
//-----------------------------------------------------------------------------
{
  //uint8_t        UE_id;
  int32_t        current_timestamp_ms, ref_timestamp_ms;
  struct timeval ts;
  struct rrc_eNB_ue_context_NB_IoT_s   *ue_context_p = NULL;
  struct rrc_eNB_ue_context_NB_IoT_s   *ue_to_be_removed = NULL;

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_IN);

  if(ctxt_pP->enb_flag == ENB_FLAG_NO) {  //is an UE
    // check timers

    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_active == 1) {
      if ((UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_cnt % 10) == 0)
        LOG_D(RRC,
              "[UE %d][RAPROC] Frame %d T300 Count %d ms\n", ctxt_pP->module_id, ctxt_pP->frame, UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_cnt);

      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_cnt
          == T300_NB_IoT[UE_rrc_inst_NB_IoT[ctxt_pP->module_id].sib2[enb_indexP]->ue_TimersAndConstants_r13.t300_r13]) {
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_active = 0;
        // ALLOW CCCH to be used
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Srb0[enb_indexP].Tx_buffer.payload_size = 0;
        rrc_ue_generate_RRCConnectionRequest (ctxt_pP, enb_indexP);
        VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
        return (RRC_ConnSetup_failed);
      }

      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T300_cnt++;
    }

    if ((UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].SIStatus&2)>0) {
      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].N310_cnt
          == N310_NB_IoT[UE_rrc_inst_NB_IoT[ctxt_pP->module_id].sib2[enb_indexP]->ue_TimersAndConstants_r13.n310_r13]) {
	LOG_I(RRC,"Activating T310_NB_IoT\n");
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_active = 1;
      }
    } else { // in case we have not received SIB2 yet
      /*      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].N310_cnt == 100) {
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].N310_cnt = 0;

	}*/
      VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
      return RRC_OK;
    }

    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_active == 1) {
      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].N311_cnt
          == N311_NB_IoT[UE_rrc_inst_NB_IoT[ctxt_pP->module_id].sib2[enb_indexP]->ue_TimersAndConstants_r13.n311_r13]) {
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_active = 0;
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].N311_cnt = 0;
      }

      if ((UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_cnt % 10) == 0) {
        LOG_D(RRC, "[UE %d] Frame %d T310_NB_IoT Count %d ms\n", ctxt_pP->module_id, ctxt_pP->frame, UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_cnt);
      }

      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_cnt    == T310_NB_IoT[UE_rrc_inst_NB_IoT[ctxt_pP->module_id].sib2[enb_indexP]->ue_TimersAndConstants_r13.t310_r13]) {
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_active = 0;
        rrc_t310_expiration_NB_IoT (ctxt_pP, enb_indexP); //FIXME: maybe is required a NB_iot version of this function
        VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
	LOG_I(RRC,"Returning RRC_PHY_RESYNCH: T310 expired\n");
        return RRC_PHY_RESYNCH;
      }

      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T310_cnt++;
    }

    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_active==1) {
      if ((UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_cnt % 10) == 0)
        LOG_D(RRC,"[UE %d][RAPROC] Frame %d T304 Count %d ms\n",ctxt_pP->module_id,ctxt_pP->frame,
              UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_cnt);

      if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_cnt == 0) {
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_active = 0;
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].HandoverInfoUe.measFlag = 1;
        LOG_E(RRC,"[UE %d] Handover failure..initiating connection re-establishment procedure... \n",
              ctxt_pP->module_id);
        //Implement 36.331, section 5.3.5.6 here
        VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
        return(RRC_Handover_failed);
      }

      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].T304_cnt--;
    }

    // Layer 3 filtering of RRC measurements
    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].QuantityConfig[0] != NULL) {
      ue_meas_filtering(ctxt_pP,enb_indexP);
    }

    ue_measurement_report_triggering(ctxt_pP,enb_indexP);

    if (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[0].handoverTarget > 0) {
      LOG_I(RRC,"[UE %d] Frame %d : RRC handover initiated\n", ctxt_pP->module_id, ctxt_pP->frame);
    }

    if((UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].State == RRC_HO_EXECUTION_NB_IoT)   &&
        (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].HandoverInfoUe.targetCellId != 0xFF)) {
      UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[enb_indexP].State= RRC_IDLE_NB_IoT;
      VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
      return(RRC_HO_STARTED);
    }

  } else { // eNB

	 //no handover in NB_IoT
    // counter, and get the value and aggregate

    // check for UL failure
    RB_FOREACH(ue_context_p, rrc_ue_tree_NB_IoT_s, &(eNB_rrc_inst_NB_IoT[ctxt_pP->module_id].rrc_ue_head)) {
      if ((ctxt_pP->frame == 0) && (ctxt_pP->subframe==0)) {
	if (ue_context_p->ue_context.Initialue_identity_s_TMSI.presence == TRUE) {
	  LOG_I(RRC,"UE rnti %x:S-TMSI %x failure timer %d/20000\n",
		ue_context_p->ue_context.rnti,
		ue_context_p->ue_context.Initialue_identity_s_TMSI.m_tmsi,
		ue_context_p->ue_context.ul_failure_timer);
	}
	else {
	  LOG_I(RRC,"UE rnti %x failure timer %d/20000\n",
		ue_context_p->ue_context.rnti,
		ue_context_p->ue_context.ul_failure_timer);
	}
      }
      if (ue_context_p->ue_context.ul_failure_timer>0) {
	ue_context_p->ue_context.ul_failure_timer++;
	if (ue_context_p->ue_context.ul_failure_timer >= 20000) {
	  // remove UE after 20 seconds after MAC has indicated UL failure
	  LOG_I(RRC,"Removing UE %x instance\n",ue_context_p->ue_context.rnti);
	  ue_to_be_removed = ue_context_p;
	  break;
	}
      }
      if (ue_context_p->ue_context.ue_release_timer>0) {
	ue_context_p->ue_context.ue_release_timer++;
	if (ue_context_p->ue_context.ue_release_timer >=
	    ue_context_p->ue_context.ue_release_timer_thres) {
	  LOG_I(RRC,"Removing UE %x instance\n",ue_context_p->ue_context.rnti);
	  ue_to_be_removed = ue_context_p;
	  break;
	}
      }
    }
    if (ue_to_be_removed)
    	rrc_eNB_free_UE_NB_IoT(ctxt_pP->module_id,ue_to_be_removed);
//no localization in NB-IoT

    (void)ts; /* remove gcc warning "unused variable" */
    (void)ref_timestamp_ms; /* remove gcc warning "unused variable" */
    (void)current_timestamp_ms; /* remove gcc warning "unused variable" */
  }

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_RX_TX,VCD_FUNCTION_OUT);
  return (RRC_OK);
}


int rrc_ue_decode_dcch_NB_IoT(
const protocol_ctxt_t* const ctxt_pP, 
const SRB_INFO_NB_IoT* const Srb_info, 
const uint8_t eNB_index 
)
{
     asn_dec_rval_t                      dec_rval;
  DL_DCCH_Message_NB_t                  *dl_dcch_msg_NB_IoT = NULL;
  UE_Capability_NB_r13_t              *UE_Capability_NB = NULL;
  int i;
  struct rrc_eNB_ue_context_NB_IoT_s*        ue_context_p = NULL;

  int dedicated_DRB=0;

  T(T_ENB_RRC_DL_DCCH_DATA_IN, T_INT(ctxt_pP->module_id), T_INT(ctxt_pP->frame),
    T_INT(ctxt_pP->subframe), T_INT(ctxt_pP->rnti));

  if ((Srb_info->Srb_id != 1) && (Srb_info->Srb_id != 3)) {
    LOG_E(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Received message on SRB%d, should not have ...\n",
          PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
          Srb_info->Srb_id);
  } else {
    LOG_D(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Received message on SRB%d\n",
          PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
          Srb_info->Srb_id);
  }

  LOG_D(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Decoding UL-DCCH Message-NB\n",
        PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP));
  dec_rval = uper_decode(
               NULL,
               &asn_DEF_DL_DCCH_Message_NB,
               (void**)&dl_dcch_msg_NB_IoT,
               Srb_info->Rx_buffer.Payload,
               Srb_info->Rx_buffer.payload_size,
               0,
               0);

//#if defined(ENABLE_ITTI)
//#   if defined(DISABLE_ITTI_XER_PRINT)

  
    for (i = 0; i < Srb_info->Rx_buffer.payload_size; i++) {
      LOG_T(RRC, "%x.", Srb_info->Rx_buffer.Payload[i]);
    }

    LOG_T(RRC, "\n");
  

  if ((dec_rval.code != RC_OK) && (dec_rval.consumed == 0)) {
    LOG_E(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Failed to decode UL-DCCH (%zu bytes)\n",
          PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
          dec_rval.consumed);
    return -1;
  }

  ue_context_p = rrc_eNB_get_ue_context_NB_IoT(
                   &eNB_rrc_inst_NB_IoT[ctxt_pP->module_id],
                   ctxt_pP->rnti);

  if (dl_dcch_msg_NB_IoT->message.present == DL_DCCH_MessageType_NB_PR_c1) {

    switch (dl_dcch_msg_NB_IoT->message.choice.c1.present) {
    case DL_DCCH_MessageType_NB__c1_PR_NOTHING:   /* No components present */
      break;

      //no measurement reports


    case DL_DCCH_MessageType_NB__c1_PR_securityModeCommand_r13:

      /* R2-163262 3GPP NB-IOT Ad-hoc Meeting #2
       * After receiving the SMC and performing the security activation, the UE shall use the SRB1.
       */
    printf("[UE] SecurityModeCommand Received\n");
      T(T_ENB_RRC_SECURITY_MODE_COMPLETE, T_INT(ctxt_pP->module_id), T_INT(ctxt_pP->frame),
        T_INT(ctxt_pP->subframe), T_INT(ctxt_pP->rnti));

#ifdef RRC_MSG_PRINT
      LOG_F(RRC,"[MSG] RRC SecurityModeComplete-NB\n");

/*      for (i = 0; i < Srb_info->Rx_buffer.payload_size; i++) eNB->pusch_vars[UE_id]{
        LOG_F(RRC,"%02x ", ((uint8_t*)Srb_info->Rx_buffer.Payload)[i]);
      }*/

      LOG_F(RRC,"\n");
#endif

      

      LOG_I(RRC,
            PROTOCOL_RRC_CTXT_UE_FMT" received securityModeComplete-NB on UL-DCCH %d from UE\n",
            PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
            DCCH0_NB_IoT);
      LOG_D(RRC,
            PROTOCOL_RRC_CTXT_UE_FMT" RLC RB %02d --- RLC_DATA_IND %d bytes "
            "(securityModeComplete-NB) ---> RRC_eNB\n",
            PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
            DCCH0_NB_IoT,//through SRB1bis
            Srb_info->Rx_buffer.payload_size);
#ifdef XER_PRINT
      xer_fprint(stdout, &asn_DEF_DL_DCCH_Message_NB, (void *)dl_dcch_msg_NB_IoT);
#endif


      //rrc_ue_generate_SecurityModeComplete_NB_IoT(ctxt_pP, ue_context_p);

      break;


    default:

      T(T_ENB_RRC_UNKNOW_MESSAGE, T_INT(ctxt_pP->module_id), T_INT(ctxt_pP->frame),
        T_INT(ctxt_pP->subframe), T_INT(ctxt_pP->rnti));

      LOG_E(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Unknown message %s:%u\n",
            PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
            __FILE__, __LINE__);
      return -1;
    }

    return 0;
  } else {
    LOG_E(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Unknown error %s:%u\n",
          PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP),
          __FILE__, __LINE__);
    return -1;
  }
}
