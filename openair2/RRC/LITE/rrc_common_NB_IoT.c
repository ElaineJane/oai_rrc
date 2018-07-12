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
#include "RRC/LITE/proto_NB_IoT.h"
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
  ResumeIdentity_r13_t              *resumeId = NULL;
  int i;
  struct rrc_eNB_ue_context_NB_IoT_s*        ue_context_p = NULL;
  uint32_t id[2];
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

  LOG_D(RRC, PROTOCOL_RRC_CTXT_UE_FMT" Decoding DL-DCCH Message-NB\n",
        PROTOCOL_RRC_CTXT_UE_ARGS(ctxt_pP));
  dec_rval = uper_decode(
               NULL,
               &asn_DEF_DL_DCCH_Message_NB,
               (void**)&dl_dcch_msg_NB_IoT,
               (uint8_t*)Srb_info->Rx_buffer.Payload,
               1000,
              //Srb_info->Rx_buffer.payload_size,
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

    case DL_DCCH_MessageType_NB__c1_PR_rrcConnectionRelease_r13:
       //printf("[UE] RRCConnectionRelease-NB message received\n" );
      if(dl_dcch_msg_NB_IoT->message.choice.c1.choice.rrcConnectionRelease_r13.criticalExtensions.choice.c1.choice.rrcConnectionRelease_r13.releaseCause_r13 == ReleaseCause_NB_r13_rrc_Suspend)
      {

        printf("[UE] DL DCCH Message Received --- RRCConnectionSuspend-NB\n");
        printf("[UE] eNB request to suspend RRC connection\n");
        resumeId = dl_dcch_msg_NB_IoT->message.choice.c1.choice.rrcConnectionRelease_r13.criticalExtensions.choice.c1.choice.rrcConnectionRelease_r13.resumeIdentity_r13;
        id[0]=resumeId->buf[0];
        id[1] = (resumeId->buf[1]<<24) | (resumeId->buf[1]<<16) | (resumeId->buf[1]<<8) | (resumeId->buf[1]);
        printf("ResumeId=%02x%08x\n", id[0], id[1]);
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].State = RRC_IDLE_NB_IoT;
        LOG_I(RRC,"[UE %d] State = RRC_IDLE (eNB %d)\n",ctxt_pP->module_id,eNB_index);
       rrc_ue_generate_RRCConnectionResumeRequest_NB_IoT(ctxt_pP, eNB_index,resumeId);
      }else
      {
        printf("[UE] DL DCCH Message Received --- RRCConnectionRelease-NB\n");
        printf("[UE] eNB request to release RRC connection\n");
         rrc_ue_process_RRCConnectionRelease_NB_IoT(ctxt_pP, &dl_dcch_msg_NB_IoT->message.choice.c1.choice.rrcConnectionRelease_r13,eNB_index);
      }
      
     
      break;

    case DL_DCCH_MessageType_NB__c1_PR_securityModeCommand_r13:

      /* R2-163262 3GPP NB-IOT Ad-hoc Meeting #2
       * After receiving the SMC and performing the security activation, the UE shall use the SRB1.
       */
    printf("[UE] SecurityModeCommand-NB received\n");
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


     rrc_ue_process_securityModeCommand_NB_IoT(ctxt_pP, &dl_dcch_msg_NB_IoT->message.choice.c1.choice.securityModeCommand_r13, eNB_index);

      break;

    case DL_DCCH_MessageType_NB__c1_PR_rrcConnectionReconfiguration_r13:
      printf("[UE] RRC Connection Reconfiguration Message received\n" );
      //Process Function
      

      break;

   case DL_DCCH_MessageType_NB__c1_PR_rrcConnectionResume_r13:
      printf("[UE] DL DCCH Message Received --- rrcConnectionResume-NB\n" );
      //Process Function
      rrc_ue_process_RRCConnectionResume_NB_IoT(ctxt_pP, &dl_dcch_msg_NB_IoT->message.choice.c1.choice.rrcConnectionResume_r13, eNB_index);
      //generate RRCConnectionResumeComplete
       rrc_ue_generate_RRCConnectionResumeComplete_NB_IoT(ctxt_pP,
          eNB_index,
          dl_dcch_msg_NB_IoT->message.choice.c1.choice.rrcConnectionResume_r13.rrc_TransactionIdentifier);
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

void rrc_ue_process_RRCConnectionResume_NB_IoT( const protocol_ctxt_t* const ctxt_pP, RRCConnectionResume_NB_t* const rrcConnectionResume, const uint8_t eNB_index )
{
  //Resume UE context and radio bearer
      LOG_I(RRC,
              "[UE%d][RAPROC] Frame %d : Logical Channel DL-DCCH (SRB1), Received RRCConnectionResume-NB\n",
              ctxt_pP->module_id,
              ctxt_pP->frame);
        // Get configuration
        //printf("[UE]RRCConnectionResume Message Received\n");
        // Release T300 timer 
        //not sure if needed
        //UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].T300_active = 0;
        //FIXME:it seems like i never start T300
        rrc_ue_process_radioResourceConfigDedicated_NB_IoT(
          ctxt_pP,
          eNB_index,
          &rrcConnectionResume->criticalExtensions.choice.c1.choice.rrcConnectionResume_r13.radioResourceConfigDedicated_r13);

        //rrc_set_state_NB_IoT (ctxt_pP->module_id, RRC_STATE_CONNECTED_NB_IoT);
        //rrc_set_sub_state_NB_IoT (ctxt_pP->module_id, RRC_SUB_STATE_CONNECTED_NB_IoT);
        //UE_rrc_inst_NB_IoT[ctxt_pP->module_id].Info[eNB_index].rnti = ctxt_pP->rnti;
  

}

void rrc_ue_process_securityModeCommand_NB_IoT( const protocol_ctxt_t* const ctxt_pP, SecurityModeCommand_t* const securityModeCommand, const uint8_t eNB_index )
{
  asn_enc_rval_t enc_rval;

  UL_DCCH_Message_NB_t ul_dcch_msg;
  //SecurityModeCommand_t securityModeCommand;
  uint8_t buffer[200];
  int i, securityMode;

  LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1bis (DL-DCCH), Processing securityModeCommand (eNB %d)\n",
        ctxt_pP->module_id,ctxt_pP->frame,eNB_index);

  switch (securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.cipheringAlgorithm) {
  case CipheringAlgorithm_r12_eea0:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea0\n",
          ctxt_pP->module_id);
    securityMode= CipheringAlgorithm_r12_eea0;
    break;

  case CipheringAlgorithm_r12_eea1:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea1\n",ctxt_pP->module_id);
    securityMode= CipheringAlgorithm_r12_eea1;
    break;

  case CipheringAlgorithm_r12_eea2:
    LOG_I(RRC,"[UE %d] Security algorithm is set to eea2\n",
          ctxt_pP->module_id);
    securityMode = CipheringAlgorithm_r12_eea2;
    break;

  default:
    LOG_I(RRC,"[UE %d] Security algorithm is set to none\n",ctxt_pP->module_id);
    securityMode = CipheringAlgorithm_r12_spare1;
    break;
  }

  switch (securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.integrityProtAlgorithm) {
  case SecurityAlgorithmConfig__integrityProtAlgorithm_eia1:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to eia1\n",ctxt_pP->module_id);
    securityMode |= 1 << 5;
    break;

  case SecurityAlgorithmConfig__integrityProtAlgorithm_eia2:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to eia2\n",ctxt_pP->module_id);
    securityMode |= 1 << 6;
    break;

  default:
    LOG_I(RRC,"[UE %d] Integrity protection algorithm is set to none\n",ctxt_pP->module_id);
    securityMode |= 0x70 ;
    break;
  }

  LOG_D(RRC,"[UE %d] security mode is %x \n",ctxt_pP->module_id, securityMode);

  /* Store the parameters received */
  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].ciphering_algorithm =
    securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.cipheringAlgorithm;
  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].integrity_algorithm =
    securityModeCommand->criticalExtensions.choice.c1.choice.securityModeCommand_r8.securityConfigSMC.securityAlgorithmConfig.integrityProtAlgorithm;

  memset((void *)&ul_dcch_msg,0,sizeof(UL_DCCH_Message_NB_t));
  //memset((void *)&SecurityModeCommand,0,sizeof(SecurityModeCommand_t));

  ul_dcch_msg.message.present           = UL_DCCH_MessageType_NB_PR_c1;

  if (securityMode >= NO_SECURITY_MODE) {
    ul_dcch_msg.message.choice.c1.present = UL_DCCH_MessageType_NB__c1_PR_securityModeComplete_r13;
  } else {
    ul_dcch_msg.message.choice.c1.present = UL_DCCH_MessageType_NB__c1_PR_securityModeFailure_r13;
  }


#if defined(ENABLE_SECURITY)
  uint8_t *kRRCenc = NULL;
  uint8_t *kUPenc = NULL;
  uint8_t *kRRCint = NULL;
  pdcp_t *pdcp_p = NULL;
  hash_key_t key = HASHTABLE_NOT_A_KEY_VALUE;
  hashtable_rc_t h_rc;

  key = PDCP_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti,
      ctxt_pP->enb_flag, DCCH, SRB_FLAG_YES);
  h_rc = hashtable_get(pdcp_coll_p, key, (void**) &pdcp_p);

  if (h_rc == HASH_TABLE_OK) {
    LOG_D(RRC, "PDCP_COLL_KEY_VALUE() returns valid key = %ld\n", key);

    LOG_D(RRC, "driving kRRCenc, kRRCint and kUPenc from KeNB="
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x"
        "%02x%02x%02x%02x\n",
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[0],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[1],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[2],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[3],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[4],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[5],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[6],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[7],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[8],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[9],  UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[10], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[11],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[12], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[13], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[14], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[15],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[16], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[17], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[18], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[19],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[20], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[21], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[22], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[23],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[24], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[25], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[26], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[27],
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[28], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[29], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[30], UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb[31]);

    derive_key_rrc_enc(UE_rrc_inst_NB_IoT[ctxt_pP->module_id].ciphering_algorithm,
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb, &kRRCenc);
    derive_key_rrc_int(UE_rrc_inst_NB_IoT[ctxt_pP->module_id].integrity_algorithm,
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb, &kRRCint);
    derive_key_up_enc(UE_rrc_inst_NB_IoT[ctxt_pP->module_id].ciphering_algorithm,
        UE_rrc_inst_NB_IoT[ctxt_pP->module_id].kenb, &kUPenc);

    if (securityMode != 0xff) {
      pdcp_config_set_security(ctxt_pP, pdcp_p, 0, 0,
          UE_rrc_inst_NB_IoT[ctxt_pP->module_id].ciphering_algorithm
              | (UE_rrc_inst_NB_IoT[ctxt_pP->module_id].integrity_algorithm << 4),
          kRRCenc, kRRCint, kUPenc);
    } else {
      LOG_W(RRC, "skipped pdcp_config_set_security() as securityMode == 0x%02x",
          securityMode);
    }
  } else {
    LOG_W(RRC, "Could not get PDCP instance where key=0x%ld\n", key);
  }

#endif //#if defined(ENABLE_SECURITY)

  if (securityModeCommand->criticalExtensions.present == SecurityModeCommand__criticalExtensions_PR_c1) {
    if (securityModeCommand->criticalExtensions.choice.c1.present == SecurityModeCommand__criticalExtensions__c1_PR_securityModeCommand_r8) {

      ul_dcch_msg.message.choice.c1.choice.securityModeComplete_r13.rrc_TransactionIdentifier = securityModeCommand->rrc_TransactionIdentifier;
      ul_dcch_msg.message.choice.c1.choice.securityModeComplete_r13.criticalExtensions.present = SecurityModeCommand__criticalExtensions_PR_c1;
      ul_dcch_msg.message.choice.c1.choice.securityModeComplete_r13.criticalExtensions.choice.securityModeComplete_r8.nonCriticalExtension =NULL;

      LOG_I(RRC,"[UE %d] Frame %d: Receiving from SRB1 (DL-DCCH), encoding securityModeComplete (eNB %d)\n",
            ctxt_pP->module_id,ctxt_pP->frame,eNB_index);

      enc_rval = uper_encode_to_buffer(&asn_DEF_UL_DCCH_Message_NB,
                                       (void*)&ul_dcch_msg,
                                       buffer,
                                       100);
      AssertFatal (enc_rval.encoded > 0, "ASN1 message encoding failed (%s, %jd)!\n",
                   enc_rval.failed_type->name, enc_rval.encoded);

#ifdef XER_PRINT
      xer_fprint(stdout, &asn_DEF_UL_DCCH_Message_NB, (void*)&ul_dcch_msg);
#endif
#if defined(ENABLE_ITTI)

    MessageDef                         *message_p;

    message_p = itti_alloc_new_message(TASK_RRC_UE, RRC_MAC_DCCH_DATA_IND);
         RRC_MAC_DCCH_DATA_IND (message_p).frame     = get_NB_IoT_frame();
          RRC_MAC_DCCH_DATA_IND (message_p).sub_frame = get_NB_IoT_subframe();
          RRC_MAC_DCCH_DATA_IND (message_p).sdu_size  = enc_rval.encoded;
          RRC_MAC_DCCH_DATA_IND (message_p).enb_index = 0;
          RRC_MAC_DCCH_DATA_IND (message_p).rnti      = get_NB_IoT_rnti();
          memset (RRC_MAC_DCCH_DATA_IND (message_p).sdu, 0, DCCH_SDU_SIZE);
          memcpy (RRC_MAC_DCCH_DATA_IND (message_p).sdu, buffer,enc_rval.encoded);
          


    itti_send_msg_to_task(TASK_RRC_ENB, ctxt_pP->instance, message_p);
    printf("[UE] RRC_MAC_DCCH_DATA_IND message(SecurityModeComplete) has been sent to eNB\n");

  #endif

/*#if defined(ENABLE_ITTI)
# if !defined(DISABLE_XER_SPRINT)
      {
        char        message_string[20000];
        size_t      message_string_size;

        if ((message_string_size = xer_sprint(message_string, sizeof(message_string), &asn_DEF_UL_DCCH_Message_NB, (void *) &ul_dcch_msg)) > 0) {
          MessageDef *msg_p;

          msg_p = itti_alloc_new_message_sized (TASK_RRC_UE, RRC_UL_DCCH, message_string_size + sizeof (IttiMsgText));
          msg_p->ittiMsg.rrc_ul_dcch.size = message_string_size;
          memcpy(&msg_p->ittiMsg.rrc_ul_dcch.text, message_string, message_string_size);

          itti_send_msg_to_task(TASK_UNKNOWN, ctxt_pP->instance, msg_p);
        }
      }
# endif
#endif*/

#ifdef USER_MODE
      LOG_D(RRC, "securityModeComplete Encoded %zd bits (%zd bytes)\n", enc_rval.encoded, (enc_rval.encoded+7)/8);
#endif

      for (i = 0; i < (enc_rval.encoded + 7) / 8; i++) {
        LOG_T(RRC, "%02x.", buffer[i]);
      }

      LOG_T(RRC, "\n");
     /* rrc_data_req (
        ctxt_pP,
        DCCH,
        rrc_mui++,
        SDU_CONFIRM_NO,
        (enc_rval.encoded + 7) / 8,
        buffer,
        PDCP_TRANSMISSION_MODE_CONTROL);*/
    }
  }
}
