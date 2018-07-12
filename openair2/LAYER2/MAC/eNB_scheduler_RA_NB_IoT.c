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

/*! \file eNB_scheduler_RA_NB_IoT.c
 * \brief functions used in Random access scheduling 
 * \author  NTUST BMW Lab./
 * \date 2017
 * \email: 
 * \version 1.0
 *
 */


#include "assertions.h"
#include "platform_types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "msc.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/extern.h"

#include "LAYER2/MAC/proto.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"

#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"

//#include "LAYER2/MAC/pre_processor.c"
#include "pdcp.h"

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#include "SIMULATION/TOOLS/defs.h" // for taus

#include "T.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "defs_NB_IoT.h"
#include "proto_NB_IoT.h"
#include "extern_NB_IoT.h"

unsigned char str1[] = "rar_dci";
unsigned char str2[] = "rar";
unsigned char str3[] = "msg4_dci";
unsigned char str4[] = "msg4";
unsigned char str5[] = "ack_msg4";
unsigned char str6[] = "msg3_dci(retransmit)";
unsigned char str7[] = "msg3(retransmit)";
unsigned char str8[] = "msg4_dci(retransmit)";
unsigned char str9[] = "msg4(retransmit)";
unsigned char str10[] = "ack_msg4(retransmit)";
unsigned char str11[] = "msg3";
unsigned char str12[] = "msg3(retransmit)";

void init_RA_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, uint8_t preamble_index, ce_level_t ce_level, uint32_t sfn_id, uint16_t ta){

	int i;
	//RA_TEMPLATE_NB_IoT *msg2_list_tail = mac_inst->RA_msg2_list.tail;
	RA_TEMPLATE_NB_IoT *migrate_node;

	static int static_count=0;
	LOG_D(MAC,"[%04d][RA scheduler][MSG1] RX %d\n", mac_inst->current_subframe, static_count++);

	
	for(i=0; i<MAX_NUMBER_OF_UE_MAX_NB_IoT; ++i){
		if(0 == mac_inst->RA_template[i].active){
			migrate_node = &mac_inst->RA_template[i];
			break;
		}
	}

	if(i==MAX_NUMBER_OF_UE_MAX_NB_IoT){
		LOG_D(MAC,"[%04d][RA scheduler][MSG1] number of RA procedures is up to maximum..\n", mac_inst->current_subframe);
		return ;
	}

	migrate_node->active = 1;
	migrate_node->preamble_index = preamble_index;
	migrate_node->ce_level = ce_level;
	migrate_node->ra_rnti = (sfn_id>>2) + 1;
	migrate_node->ta = ta;
	migrate_node->next = (RA_TEMPLATE_NB_IoT *)0;
	migrate_node->prev = (RA_TEMPLATE_NB_IoT *)0;
	LOG_D(MAC,"[%04d][RA scheduler][MSG1][CE%d] Receive MSG1 RNTI %d preamble index %d\n", mac_inst->current_subframe, migrate_node->ce_level, migrate_node->ra_rnti, migrate_node->preamble_index);

	//	insert to end of list
	if((RA_TEMPLATE_NB_IoT *)0 == mac_inst->RA_msg2_list.head){
		mac_inst->RA_msg2_list.head = migrate_node;
	}else{
		//	not empty
		mac_inst->RA_msg2_list.tail->next = migrate_node;
		migrate_node->prev = mac_inst->RA_msg2_list.tail;
	}
	mac_inst->RA_msg2_list.tail = migrate_node;

}

//	triggered time :
//	1) after tx rar, wait msg3 timeout
//	2) number of retransmit msg3 meet maximum or absent of ack/nack
//	3) number of retransmit msg4 meet maximum or absent of ack/nack
void cancel_ra(uint16_t rnti){
	//uint32_t i;
}

uint16_t find_suit_i_delay(uint32_t rmax, uint32_t r, uint32_t dci_candidate){
    uint32_t i;
	uint32_t num_candidates = rmax / r;
	uint32_t left_candidates = num_candidates - dci_candidate - 1;	// 0-7
	uint32_t resource_gap = left_candidates * r;
	resource_gap = ((resource_gap * 10)>>3);	//	x1.125
	for(i=0;i<8;++i){
		if(resource_gap <= get_scheduling_delay(i, rmax)){
			return i;
		}
	}
	return 0;
}


void schedule_rar_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, int abs_subframe){
//    printf_FUNCTION_IN("schedule rar");
	RA_TEMPLATE_NB_IoT *msg2_nodes = mac_inst->RA_msg2_list.head;
	//RA_TEMPLATE_NB_IoT *msg3_list_tail = mac_inst->RA_msg3_list.tail;
	RA_TEMPLATE_NB_IoT *migrate_node;
	schedule_result_t *dci_result, *msg2_result;
    DCIFormatN0_t *dci_n0;
    DCIFormatN1_t *dci_n1_rar;
	available_resource_DL_t *dci_node, *msg2_node;//, *msg3_node;
	int rmax, fail, r, res;
	int dci_subframe, dci_end_subframe, dci_first_subframe, num_dci_subframe;
	int msg2_subframe, msg2_end_subframe, msg2_first_subframe, num_msg2_subframe;
	int msg3_subframe;//, msg3_end_subframe;

	int dci_candidate, i, num_candidate;
	int msg2_i_delay;
	int msg3_scheduling_delay;
	static uint16_t tc_rnti = 0x0101;
	int rep=1;
	sched_temp_UL_NB_IoT_t npusch_info;
    int fail_num = 0;
    int flag=0;
	while((RA_TEMPLATE_NB_IoT *)0 != msg2_nodes){
		if(flag==0)
			flag=1;
		fail=0;
		rmax = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg2_nodes->ce_level].mac_npdcch_NumRepetitions_RA_NB_IoT;//32;
		num_candidate = 8;//rmax / r;
		r = rmax/num_candidate;
		num_dci_subframe = r;
		dci_subframe = abs_subframe;//mac_inst->current_subframe;
		for(dci_candidate=0; dci_candidate<num_candidate; ++dci_candidate){
            while(!is_dlsf(mac_inst, dci_subframe)){
                ++dci_subframe;
            }
			dci_node = (available_resource_DL_t *)check_resource_DL(mac_inst, dci_subframe, num_dci_subframe, &dci_end_subframe, &dci_first_subframe);
			if((available_resource_DL_t *)0 != dci_node){
				//dci_subframe += dci_candidate*num_dci_subframe;
				break;
			}
			res = num_dci_subframe;
			while(res != 0){ //  maybe would cost lots of time to search
                if(is_dlsf(mac_inst, dci_subframe)){
                    res--;
                }
                dci_subframe++;
            }
		}
		if(num_candidate==dci_candidate){
			fail|=0x1;
		}
		//	check msg2 resource
		uint32_t TBS, I_tbs, I_mcs, I_sf, Nrep;
		
		I_mcs = get_I_mcs(msg2_nodes->ce_level);
	    I_tbs = I_mcs;
		TBS = get_tbs(7, I_tbs, &I_sf);   //  rar 7 bytes
		Nrep = dl_rep[msg2_nodes->ce_level];
		num_msg2_subframe = get_num_sf(I_sf) * Nrep;
		
		//num_msg2_subframe = 8;
		msg2_i_delay = find_suit_i_delay(rmax, r, dci_candidate);
		for(i=0; i<8; ++i, ++msg2_i_delay){
		    msg2_i_delay = (msg2_i_delay==8)?0:msg2_i_delay;
		    msg2_subframe = dci_end_subframe+4+get_scheduling_delay(msg2_i_delay, rmax);
			msg2_node = (available_resource_DL_t *)check_resource_DL(mac_inst, msg2_subframe, num_msg2_subframe, &msg2_end_subframe, &msg2_first_subframe);
			if((available_resource_DL_t *)0 != msg2_node){
				break;
			}
		}
		if(-1==msg2_i_delay){
			fail|=0x2;
		}
		//	check msg3 resource
		rep = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg2_nodes->ce_level].mac_numRepetitionsPerPreambleAttempt_NB_IoT;
		
		
		uint32_t Iru = 0, mcs, Nru;
		uint32_t mappedMcsIndex = 4;  //  assume all ue supported multi-tone
		mcs = mapped_mcs[msg2_nodes->ce_level][mappedMcsIndex]; //  assume all ue supported multi-tone
		
        TBS = get_TBS_UL_NB_IoT(mcs,1,Iru);
		while((TBS<11)&&(Iru<=7)){    //  88 bits
            Iru++;
            TBS=get_TBS_UL_NB_IoT(mcs,1,Iru);
        }
		Nru = RU_table[Iru];
		
        for(msg3_scheduling_delay=0; msg3_scheduling_delay<4; ++msg3_scheduling_delay){
		    //    36.213 Table 16.3.3-1 Imcs=3'b000 Nru=4
			msg3_subframe = msg2_end_subframe+msg3_scheduling_delay_table[msg3_scheduling_delay]+1;
			if(0==Check_UL_resource(msg3_subframe, Nru*rep, &npusch_info, 1, 0)){	//1: multi-tones 0: single-tone. 1: format 2(ack/nack) 0: format 1
				break;
			}
		}
		if(4==msg3_scheduling_delay){
			fail|=0x4;
		}
		if(0 < fail){
		    fail_num++;
		    LOG_D(MAC,"[%04d][RA scheduler][MSG2] fail vector %d\n", abs_subframe, fail);
		    LOG_D(MAC,"[%04d][RA scheduler][MSG2][CE%d] rnti: %d preamble: %d fail vector %d\n", abs_subframe-1, msg2_nodes->ce_level, msg2_nodes->ra_rnti, msg2_nodes->preamble_index, fail);
		    msg2_nodes = msg2_nodes->next;
		}else{
		    LOG_D(MAC,"[%04d][RA scheduler][MSG2][CE%d] rnti: %d preamble: %d scheduling success\n", abs_subframe-1, msg2_nodes->ce_level, msg2_nodes->ra_rnti, msg2_nodes->preamble_index);
		    dci_result = (schedule_result_t *)calloc(1, sizeof(schedule_result_t));
		    msg2_result = (schedule_result_t *)calloc(1, sizeof(schedule_result_t));
		    dci_n0 = (DCIFormatN0_t *)malloc(sizeof(DCIFormatN0_t));
			dci_n1_rar = (DCIFormatN1_t *)malloc(sizeof(DCIFormatN1_t));
			
		    fill_rar_NB_IoT(mac_inst, msg2_nodes, msg3_scheduling_delay, rep, &npusch_info);
		    
            msg2_nodes->wait_msg3_ack = 1;
			//	dci entity
			dci_n1_rar->type = 1;
			dci_n1_rar->orderIndicator = 0;
			dci_n1_rar->Scheddly = msg2_i_delay;
			dci_n1_rar->ResAssign = 0;
			dci_n1_rar->mcs = 0;	
			dci_n1_rar->RepNum = 0;	//	36.213 table 16.4.1.3-2, 8 candidates
			dci_n1_rar->ndi = 0;	//	ndi is useless in RAR	36.212 says the feild is reserved
			dci_n1_rar->HARQackRes = 0;	//	no HARQ procedure in RAR	36.212 says the feild is reserved
			dci_n1_rar->DCIRep = 0;	//	36.213 table 16.6-1 R=Rmax/8

			//	for dci
		    dci_result->output_subframe = dci_first_subframe;//dci_subframe;
			dci_result->end_subframe = dci_end_subframe;
		    dci_result->sdu_length = 0;
		    dci_result->direction = DL;
		    dci_result->DCI_release = 0;
		    dci_result->channel = NPDCCH;
		    dci_result->rnti = msg2_nodes->ra_rnti;//ra_rnti;
		    dci_result->rnti_type = 1;
		    dci_result->npusch_format = 0; //useless
		    dci_result->R_harq = 0;
		    dci_result->next = (schedule_result_t *)0;
		    dci_result->DCI_pdu = (void *)dci_n1_rar;
		    //dci_result->printf_str = &str1[0];
		    //dci_result->dl_sdly = msg2_subframe - dci_end_subframe;
			//dci_result->ul_sdly = msg3_subframe - msg2_end_subframe;
			//dci_result->num_sf = msg2_end_subframe - msg2_subframe+1;
			
			//	for msg2
		    msg2_result->output_subframe = msg2_first_subframe;//msg2_subframe;
		    msg2_result->end_subframe = msg2_end_subframe;
		    msg2_result->sdu_length = 56;   //  rar size
		    msg2_result->DLSCH_pdu = msg2_nodes->rar_buffer;
		    msg2_result->direction = DL;
		    msg2_result->DCI_release = 1;
		    msg2_result->channel = NPDSCH;
		    msg2_result->rnti = msg2_nodes->ra_rnti;//ra_rnti;
		    msg2_result->rnti_type = 1;
		    msg2_result->npusch_format = 0;	//useless
		    msg2_result->R_harq = 0;
		    msg2_result->next = (schedule_result_t *)0;
		    msg2_result->DCI_pdu = (void *)dci_n1_rar;
			//msg2_result->printf_str = str2;
			msg2_result->rar_buffer = msg2_nodes->rar_buffer;
			//msg2_result->dl_sdly = -1;
			//msg2_result->ul_sdly = -1;

			//	for msg3(fake DCI N0)
			dci_n0->type = 0;
			dci_n0->scind = npusch_info.subcarrier_indication;
			dci_n0->ResAssign = 0;
			dci_n0->mcs = 0;
			dci_n0->ndi = 1;
			dci_n0->Scheddly = msg3_scheduling_delay;
			dci_n0->RepNum = rep;
			dci_n0->rv = 0;
			dci_n0->DCIRep = 1;//get_DCI_REP()
			
			msg2_nodes->ue_rnti = tc_rnti;
			
			LOG_D(MAC,"[%04d][RA scheduler][MSG2] RARDCI %d-%d RAR %d-%d MSG3 %d-%d\n", abs_subframe-1, dci_first_subframe, dci_end_subframe, msg2_first_subframe, msg2_end_subframe, npusch_info.sf_start, npusch_info.sf_end);
			LOG_D(MAC,"[%04d][RA scheduler][MSG2][CE%d] Change RA-RNTI %d->T-CRNTI %d\n", abs_subframe-1, msg2_nodes->ce_level, msg2_nodes->ra_rnti, msg2_nodes->ue_rnti);
			LOG_D(MAC,"[%04d][RA scheduler][MSG2][CE%d] RAR DCI %d-%d RAR %d-%d MSG3 %d-%d\n", abs_subframe-1, msg2_nodes->ce_level, dci_first_subframe, dci_end_subframe, msg2_first_subframe, msg2_end_subframe, npusch_info.sf_start, npusch_info.sf_end);
            
            //	fill dci resource
			fill_resource_DL(mac_inst, dci_node, dci_first_subframe, dci_end_subframe, dci_result);
			//	fill msg2 resource
			fill_resource_DL(mac_inst, msg2_node, msg2_first_subframe, msg2_end_subframe, msg2_result);

			//	fill msg3 resource
			generate_scheduling_result_UL(-1, -1, npusch_info.sf_start, npusch_info.sf_end, dci_n0, tc_rnti, str11, (void *)0);
			adjust_UL_resource_list(&npusch_info);

			//simulate_rx(&simulate_rx_msg3_list, tc_rnti, npusch_info.sf_start);

			migrate_node = msg2_nodes;
			//migrate_node->ue_rnti = tc_rnti;
			tc_rnti++;
	        msg2_nodes = msg2_nodes->next;

			//	maintain list
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->prev){
				//	first node
				mac_inst->RA_msg2_list.head = migrate_node->next;	//	including null
			}else{
				//	not first node
				migrate_node->prev->next = migrate_node->next;		//	including null
			}
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->next){
				//	last node
				mac_inst->RA_msg2_list.tail = migrate_node->prev;	//	including null
			}else{
				//	not last node
				migrate_node->next->prev = migrate_node->prev;		//	including null
			}

			//	migrate to next list
			//	insert to end of list
			migrate_node->next = (RA_TEMPLATE_NB_IoT *)0;
			migrate_node->prev = (RA_TEMPLATE_NB_IoT *)0;
			if((RA_TEMPLATE_NB_IoT *)0 == mac_inst->RA_msg3_list.head){
				mac_inst->RA_msg3_list.head = migrate_node;
			}else{
				//	not empty
				mac_inst->RA_msg3_list.tail->next = migrate_node;
				migrate_node->prev = mac_inst->RA_msg3_list.tail;
			}
			mac_inst->RA_msg3_list.tail = migrate_node;

		}
		
	}
	if(flag==1)
		LOG_D(MAC,"[%04d][RA scheduler][MSG2] failed number: %d\n", abs_subframe-1, fail_num);
	
	
	//printf_FUNCTION_OUT("schedule rar");
	return ;
}

void msg3_do_retransmit_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, rnti_t c_rnti){
    RA_TEMPLATE_NB_IoT *msg3_nodes = mac_inst->RA_msg3_list.head;
	//RA_TEMPLATE_NB_IoT *migrate_node;

	if((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes)
	while((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes){
		if(msg3_nodes->ue_rnti == c_rnti){
		    msg3_nodes->wait_msg3_ack = 0;
		    msg3_nodes->msg3_retransmit_count++;
		    return ;
		}
		msg3_nodes = msg3_nodes->next;
	}
	
	return ;
}

void msg4_do_retransmit_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, rnti_t c_rnti){
    RA_TEMPLATE_NB_IoT *msg4_nodes = mac_inst->RA_msg4_list.head;
	//RA_TEMPLATE_NB_IoT *migrate_node;

	if((RA_TEMPLATE_NB_IoT *)0 != msg4_nodes)
	while((RA_TEMPLATE_NB_IoT *)0 != msg4_nodes){
		if(msg4_nodes->ue_rnti == c_rnti){
		    msg4_nodes->wait_msg4_ack = 0;
		    msg4_nodes->msg4_retransmit_count++;
		    return ;
		}
		msg4_nodes = msg4_nodes->next;
	}
	
	return ;
}

void receive_msg3_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, rnti_t c_rnti, uint32_t phr, uint32_t ul_total_buffer){
	//	since successful receive msg3, tc-rnti become c-rnti.

	RA_TEMPLATE_NB_IoT *msg3_nodes = mac_inst->RA_msg3_list.head;
	RA_TEMPLATE_NB_IoT *migrate_node;

	if((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes)
	while((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes){
		if(msg3_nodes->ue_rnti == c_rnti){
			add_ue_NB_IoT(mac_inst, c_rnti, msg3_nodes->ce_level, phr, ul_total_buffer);//	rnti, ce level
			LOG_D(MAC,"[%04d][RA scheduler][MSG3][CE%d] Receive MSG3 T-CRNTI %d Preamble Index %d \n", mac_inst->current_subframe, msg3_nodes->ce_level, msg3_nodes->ue_rnti, msg3_nodes->preamble_index);
			migrate_node = msg3_nodes;

			//	maintain list
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->prev){
				//	first node
				mac_inst->RA_msg3_list.head = migrate_node->next;	//	including null
			}else{
				//	not first node
				migrate_node->prev->next = migrate_node->next;		//	including null
			}
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->next){
				//	last node
				mac_inst->RA_msg3_list.tail = migrate_node->prev;	//	including null
			}else{
				//	not last node
				migrate_node->next->prev = migrate_node->prev;		//	including null
			}

			//	migrate to next list
			//	insert to end of list
			migrate_node->next = (RA_TEMPLATE_NB_IoT *)0;
			migrate_node->prev = (RA_TEMPLATE_NB_IoT *)0;
			if((RA_TEMPLATE_NB_IoT *)0 == mac_inst->RA_msg4_list.head){
				mac_inst->RA_msg4_list.head = migrate_node;
			}else{
				//	not empty
				mac_inst->RA_msg4_list.tail->next = migrate_node;
				migrate_node->prev = mac_inst->RA_msg4_list.tail;
			}
			mac_inst->RA_msg4_list.tail = migrate_node;

			return ;
		}
		msg3_nodes = msg3_nodes->next;
	}

	if((RA_TEMPLATE_NB_IoT *)0 == msg3_nodes){
		LOG_D(MAC,"[%04d][RA scheduler][MSG3] receive msg3.. can't found the ue from crnti %x\n", mac_inst->current_subframe, c_rnti);
		return;
	}
}

void schedule_msg3_retransimission_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, int abs_subframe){

	RA_TEMPLATE_NB_IoT *msg3_nodes = mac_inst->RA_msg3_list.head;

	available_resource_DL_t *dci_node;//, *msg3_node;
	int rmax, fail, res, r;
	int dci_subframe, dci_end_subframe, dci_first_subframe, num_dci_subframe;
	int msg3_subframe;//, msg3_end_subframe;

	int dci_candidate, num_candidate;
	int msg3_scheduling_delay;
	schedule_result_t *dci_result;//, *msg3_result;

	int rep=1;
	//sched_temp_UL_NB_IoT_t npusch_info;

#if 0
	//	msg3 retransmission pre-processor
	RA_TEMPLATE_NB_IoT *iterator, *iterator1;
	while((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes){
		if(msg3_nodes->wait_msg3_ack == 0){
			iterator = msg3_nodes;
			
			while(iterator->prev != (RA_TEMPLATE_NB_IoT *)0 && iterator != mac_inst->RA_msg3_list.head){
				if(iterator->prev->msg3_retransmit_count < iterator->msg3_retransmit_count){
					//swap
					iterator1 = iterator->prev;
					
					if(iterator->prev == mac_inst->RA_msg3_list.head || iterator1->prev == (RA_TEMPLATE_NB_IoT *)0){	
						mac_inst->RA_msg3_list.head = iterator;
						iterator1->prev = (RA_TEMPLATE_NB_IoT *)0;	//	b->prev
					}else{	
						iterator->prev->prev->next = iterator;	//	a*->next
						iterator->prev = iterator1->prev;	//	b->prev
					}
					if(iterator == mac_inst->RA_msg3_list.tail || iterator->next == (RA_TEMPLATE_NB_IoT *)0){	
						mac_inst->RA_msg3_list.tail = iterator1;
						iterator1->next = (RA_TEMPLATE_NB_IoT *)0;		//	a->next
					}else{	
						iterator->next->prev = iterator->prev;	//b*->prev
						iterator1->next = iterator->next;	//	a->next
					}
					
					iterator1->prev = iterator;	//	a->prev
				
					iterator->next = iterator1;	//	b->next
				
				}else{
					break;
				}
			}
		}
		msg3_nodes = msg3_nodes->next;
	}
#endif 

	msg3_nodes = mac_inst->RA_msg3_list.head;
	while((RA_TEMPLATE_NB_IoT *)0 != msg3_nodes){
		if(msg3_nodes->wait_msg3_ack == 0){
		    
			fail=0;
			//	check dci resource
			rmax = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg3_nodes->ce_level].mac_npdcch_NumRepetitions_RA_NB_IoT;//32;
			num_candidate = 8;//rmax / r;
			r = rmax/num_candidate;
			num_dci_subframe = r;
			dci_subframe = abs_subframe;//mac_inst->current_subframe;
			for(dci_candidate=0; dci_candidate<8; ++dci_candidate){
                while(!is_dlsf(mac_inst, dci_subframe)){
                    ++dci_subframe;
                }
				dci_node = (available_resource_DL_t *)check_resource_DL(mac_inst, dci_subframe, num_dci_subframe, &dci_end_subframe, &dci_first_subframe);
				if((available_resource_DL_t *)0 != dci_node){
					break;
				}
				res = num_dci_subframe;
    			while(res != 0){ //  cost lot of time to search
                    if(is_dlsf(mac_inst, dci_subframe)){
                        res--;
                    }
                    dci_subframe++;
                }
			}
			if(8==dci_candidate){
				//failed
				fail|=0x1;
			}
			//	check msg3 resource
			
			rep = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg3_nodes->ce_level].mac_numRepetitionsPerPreambleAttempt_NB_IoT;
			sched_temp_UL_NB_IoT_t npusch_info;
		
        	uint32_t Iru = 0, mcs, Nru;
			uint32_t mappedMcsIndex = 4;  //  assume all ue supported multi-tone
			mcs = mapped_mcs[msg3_nodes->ce_level][mappedMcsIndex]; //  assume all ue supported multi-tone
			int TBS = get_TBS_UL_NB_IoT(mcs,1,Iru);
			while((TBS<11)&&(Iru<=7)){
				Iru++;
				TBS=get_TBS_UL_NB_IoT(mcs,1,Iru);
			}
			Nru = RU_table[Iru];
			
			for(msg3_scheduling_delay=0; msg3_scheduling_delay<4; ++msg3_scheduling_delay){
				msg3_subframe = 8+dci_end_subframe+msg3_scheduling_delay_table[msg3_scheduling_delay];
				if(0==Check_UL_resource(msg3_subframe+1, Nru*rep, &npusch_info, 1, 0)){	//1: multi-tones 0: single-tone. 1: format 2(ack/nack) 0: format 1
					break;
				}
			}
			if(4==msg3_scheduling_delay){
				//failed
				fail|=0x2;
			}
	        
			if(0 == fail){
				
				msg3_nodes->wait_msg3_ack = 1;
				DCIFormatN0_t *dci_n0_msg3 = (DCIFormatN0_t *)malloc(sizeof(DCIFormatN0_t));
				//	dci entity
				dci_n0_msg3->type = 0;
				dci_n0_msg3->scind = npusch_info.subcarrier_indication;
				dci_n0_msg3->ResAssign = 0;
				dci_n0_msg3->mcs = 0;
				dci_n0_msg3->ndi = 0;	//	retrnasmit
				dci_n0_msg3->Scheddly = msg3_scheduling_delay;
				dci_n0_msg3->RepNum = rep;
				dci_n0_msg3->rv = 0;
				dci_n0_msg3->DCIRep = 1;//get_DCI_REP()
				//	for dci
				dci_result = (schedule_result_t *)malloc(sizeof(schedule_result_t));
			    dci_result->output_subframe = dci_first_subframe;//dci_subframe;
			    dci_result->end_subframe = dci_end_subframe;
			    dci_result->sdu_length = 0;
			    dci_result->direction = UL;
			    dci_result->DCI_release = 0;
			    dci_result->channel = NPDCCH;
			    dci_result->rnti = msg3_nodes->ue_rnti;
			    dci_result->rnti_type = 1;
			    dci_result->npusch_format = 0; //useless
			    dci_result->R_harq = 0;
			    dci_result->next = (schedule_result_t *)0;
			    dci_result->DCI_pdu = (void *)dci_n0_msg3;
//		        dci_result->printf_str = str6;
	           // dci_result->dl_sdly = msg3_subframe - dci_end_subframe + 1;
			//	dci_result->ul_sdly = -1;
				//dci_result->num_sf = -1;
				//dci_result->harq_round = msg3_nodes->msg3_retransmit_count;
				
       		    //simulate_rx(&simulate_rx_msg3_list, msg3_nodes->ue_rnti, npusch_info.sf_start);

			    //	fill dci resource
			    fill_resource_DL(mac_inst, dci_node, dci_first_subframe, dci_end_subframe, dci_result);
			    //	fill msg3 resource
			    generate_scheduling_result_UL(-1, -1, npusch_info.sf_start, npusch_info.sf_end, dci_n0_msg3, msg3_nodes->ue_rnti, str12, (void *)0);	//	rnti
				adjust_UL_resource_list(&npusch_info);
	            LOG_D(MAC,"[%04d][RA scheduler][MSG3 re] MSG3DCI %d-%d MSG3 %d-%d\n", abs_subframe, dci_first_subframe, dci_end_subframe, npusch_info.sf_start, npusch_info.sf_end );
			}else{
				LOG_D(MAC,"[%04d][RA scheduler][MSG3 re] fail vector %d\n", abs_subframe, fail );
			}

			//++msg3_nodes->msg3_retransmit_count;
		}
		msg3_nodes = msg3_nodes->next;
	}
	return ;
}


void receive_msg4_ack_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, rnti_t rnti){
	int i;
	RA_TEMPLATE_NB_IoT *migrate_node = mac_inst->RA_msg4_list.head;

	UE_TEMPLATE_NB_IoT *ue_info = (UE_TEMPLATE_NB_IoT *)0;
	if((RA_TEMPLATE_NB_IoT *)0 != migrate_node)
	while((RA_TEMPLATE_NB_IoT *)0 != migrate_node){
		if(migrate_node->ue_rnti == rnti){
            
			ue_info = mac_inst->UE_list_spec[(uint32_t)migrate_node->ce_level].UE_template_NB_IoT;
			//	maintain list
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->prev){
				//	first node
				mac_inst->RA_msg4_list.head = migrate_node->next;	//	including null
			}else{
				//	not first node
				migrate_node->prev->next = migrate_node->next;		//	including null
			}
			if((RA_TEMPLATE_NB_IoT *)0 == migrate_node->next){
				//	last node
				mac_inst->RA_msg4_list.tail = migrate_node->prev;	//	including null
			}else{
				//	not last node
				migrate_node->next->prev = migrate_node->prev;		//	including null
			}

			//	release ra template
            migrate_node->active = 0;
			migrate_node->ce_level = 0;
			migrate_node->msg3_retransmit_count = 0;
			migrate_node->msg4_retransmit_count = 0;
			migrate_node->next = (RA_TEMPLATE_NB_IoT *)0;
			migrate_node->prev = (RA_TEMPLATE_NB_IoT *)0;
			migrate_node->ta = 0;
			migrate_node->preamble_index = 0;
			migrate_node->ue_rnti = 0x0;
			migrate_node->ra_rnti = 0x0;
			migrate_node->wait_msg4_ack = 0;
			migrate_node->wait_msg3_ack = 0;
			break ;
		}
		migrate_node = migrate_node->next;
	}
	
	for(i=0; i<MAX_NUMBER_OF_UE_MAX_NB_IoT; ++i){
		if(ue_info[i].rnti == rnti){
		    
			if(ue_info[i].ul_total_buffer>0)
			{
				ue_info[i].direction = 0;
			}
			else
			{
				ue_info[i].direction = 1;
			}
			ue_info[i].RRC_connected = 1;
			LOG_D(MAC,"[%04d][RA scheduler][MSG4] received UE:%d direction: %d \n", mac_inst->current_subframe, rnti, ue_info[i].direction );
			break;
		}
	}
	
	return ;
}

//	msg4 scheduling: both first time or retransmit would be scheduled in this function(msg4_list).
void schedule_msg4_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst, int abs_subframe){
    //printf_FUNCTION_IN("[SCHEDULER RA MSG4]");
	RA_TEMPLATE_NB_IoT *msg4_nodes = mac_inst->RA_msg4_list.head;//, *migrate_node;

	available_resource_DL_t *dci_node, *msg4_node;
	int rmax, fail, r;
	int dci_subframe, dci_end_subframe, dci_first_subframe, num_dci_subframe;
	int msg4_subframe, msg4_end_subframe, msg4_first_subframe, num_msg4_subframe;
	int harq_subframe, harq_end_subframe;

	int dci_candidate, num_candidate;
	int msg4_i_delay, i, res, rep;
	int end_flagHARQ, HARQ_delay;
	sched_temp_UL_NB_IoT_t HARQ_info;
	schedule_result_t *dci_result;
	schedule_result_t *msg4_result;
	schedule_result_t *harq_result;

#if 0
	//	msg4 pre-processor
	RA_TEMPLATE_NB_IoT *iterator, *iterator1;
	while((RA_TEMPLATE_NB_IoT *)0 != msg4_nodes){
		if(msg4_nodes->wait_msg4_ack == 0){
			iterator = msg4_nodes;
			
			while(iterator->prev != (RA_TEMPLATE_NB_IoT *)0 && iterator != mac_inst->RA_msg4_list.head){
				if(iterator->prev->msg4_retransmit_count < iterator->msg4_retransmit_count){
					//swap
					iterator1 = iterator->prev;
					
					if(iterator->prev == mac_inst->RA_msg4_list.head || iterator1->prev == (RA_TEMPLATE_NB_IoT *)0){	
						mac_inst->RA_msg4_list.head = iterator;
						iterator1->prev = (RA_TEMPLATE_NB_IoT *)0;	//	b->prev
					}else{	
						iterator->prev->prev->next = iterator;	//	a*->next
						iterator->prev = iterator1->prev;	//	b->prev
					}
					if(iterator == mac_inst->RA_msg4_list.tail || iterator->next == (RA_TEMPLATE_NB_IoT *)0){	
						mac_inst->RA_msg4_list.tail = iterator1;
						iterator1->next = (RA_TEMPLATE_NB_IoT *)0;		//	a->next
					}else{	
						iterator->next->prev = iterator->prev;	//b*->prev
						iterator1->next = iterator->next;	//	a->next
					}
					
					iterator1->prev = iterator;	//	a->prev
				
					iterator->next = iterator1;	//	b->next
				
				}else{
					break;
				}
			}
		}
		msg4_nodes = msg4_nodes->next;
	}
#endif
	msg4_node = (available_resource_DL_t *)0;
	msg4_nodes = mac_inst->RA_msg4_list.head;
	while((RA_TEMPLATE_NB_IoT *)0 != msg4_nodes){

		if(msg4_nodes->wait_msg4_ack == 0){
			fail=0;
			
			//printf_FUNCTION_IN("[SCHEDULER RA MSG4 DCI]");
			//	check dci resource
			rmax = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg4_nodes->ce_level].mac_npdcch_NumRepetitions_RA_NB_IoT;//32;
    	    num_candidate = 8;//rmax / r;
    		r = rmax/num_candidate;
    		num_dci_subframe = r;
    		dci_subframe = abs_subframe;//mac_inst->current_subframe;
			for(dci_candidate=0; dci_candidate<num_candidate; ++dci_candidate){
	            while(!is_dlsf(mac_inst, dci_subframe)){
                    ++dci_subframe;
                }
				dci_node = (available_resource_DL_t *)check_resource_DL(mac_inst, dci_subframe, num_dci_subframe, &dci_end_subframe, &dci_first_subframe);
				if((available_resource_DL_t *)0 != dci_node){
					LOG_D(MAC,"%d msg4 dci %d - %d\n", abs_subframe, dci_first_subframe, dci_end_subframe);
					break;
				}
				res = num_dci_subframe;
    			while(res != 0){ //  cost lot of time to search
                    if(is_dlsf(mac_inst, dci_subframe)){
                        res--;
                    }
                    dci_subframe++;
                }
			}
			if(num_candidate==dci_candidate){
				//failed
				fail|=1;
			}
			//printf_FUNCTION_OUT("[SCHEDULER RA MSG4 DCI]");
			
			//printf_FUNCTION_IN("[SCHEDULER RA MSG4 PAYLOAD]");
			//	check msg4 resource
			rep = dl_rep[msg4_nodes->ce_level];
			num_msg4_subframe = 1*rep;   //  8 subframes
			msg4_i_delay = find_suit_i_delay(rmax, r, dci_candidate);
			for(i=msg4_i_delay; i<8; ++i){
				msg4_i_delay = (msg4_i_delay==8)?0:msg4_i_delay;
				msg4_subframe = dci_end_subframe+4+get_scheduling_delay(msg4_i_delay, rmax);
				msg4_node = (available_resource_DL_t *)check_resource_DL(mac_inst, msg4_subframe, num_msg4_subframe*rep, &msg4_end_subframe, &msg4_first_subframe);
				if((available_resource_DL_t *)0 != msg4_node){
					LOG_D(MAC,"%d msg4 %d - %d\n", abs_subframe, msg4_first_subframe, msg4_end_subframe);
					break;
				}
			}
			if(8==i){
				//failed
				fail|=2;
			}
            //printf_FUNCTION_OUT("[SCHEDULER RA MSG4 PAYLOAD]");
            
            //printf_FUNCTION_IN("[SCHEDULER RA MSG4 HARQ]");
            rep = mac_inst->rrc_config.mac_NPRACH_ConfigSIB[msg4_nodes->ce_level].mac_numRepetitionsPerPreambleAttempt_NB_IoT;
			for(HARQ_delay=0;HARQ_delay<4;++HARQ_delay){
	            end_flagHARQ=Check_UL_resource(msg4_end_subframe+get_HARQ_delay(1, HARQ_delay), rep, &HARQ_info, 0, 1);	//	RA_template->R
				if(0 == end_flagHARQ){
					harq_subframe = msg4_end_subframe + get_HARQ_delay(1, HARQ_delay);
					harq_end_subframe = harq_subframe + 2*rep -1;
					HARQ_info.ACK_NACK_resource_field=get_resource_field_value(HARQ_info.subcarrier_indication, get_scheduling_delay(HARQ_delay, rmax));
					break;
				}
			}
			if(4 == HARQ_delay){
				fail |= 4;
			}
			//printf_FUNCTION_OUT("[SCHEDULER RA MSG4 HARQ]");

			if(0==fail){
			    LOG_D(MAC,"[%04d][RA scheduler][MSG4][CE%d] rnti: %d scheduling success\n", abs_subframe-1, msg4_nodes->ce_level, msg4_nodes->ue_rnti);
			    msg4_nodes->wait_msg4_ack = 1;	
				DCIFormatN1_t *dci_n1_msg4 = (DCIFormatN1_t *)malloc(sizeof(DCIFormatN1_t));
				//	dci entity
				dci_n1_msg4->type = 1;
				dci_n1_msg4->orderIndicator = 0;
				dci_n1_msg4->Scheddly = msg4_i_delay;
				dci_n1_msg4->ResAssign = 0;
				dci_n1_msg4->mcs = 0;
				dci_n1_msg4->RepNum = 1;
				dci_n1_msg4->ndi = 1;
				dci_n1_msg4->HARQackRes = HARQ_info.ACK_NACK_resource_field;
				dci_n1_msg4->DCIRep = 1;

				//	for dci
				dci_result = (schedule_result_t *)malloc(sizeof(schedule_result_t));
				dci_result->output_subframe = dci_first_subframe;//dci_subframe;
				dci_result->end_subframe = dci_end_subframe;
				dci_result->sdu_length = 0;
				dci_result->direction = DL;
				dci_result->DCI_release = 0;
				dci_result->channel = NPDCCH;
				dci_result->rnti = msg4_nodes->ue_rnti;
				dci_result->rnti_type = 1;
				dci_result->npusch_format = 0; //useless
				dci_result->R_harq = 0;
				dci_result->next = (schedule_result_t *)0;
				dci_result->DCI_pdu = (void *)dci_n1_msg4;
				//dci_result->dl_sdly = msg4_subframe - dci_end_subframe;
				//dci_result->ul_sdly = harq_subframe - msg4_end_subframe;
				//dci_result->num_sf = msg4_end_subframe - msg4_subframe+1;
				//dci_result->harq_round = msg4_nodes->msg4_retransmit_count;
				
				//	for msg4
				msg4_result = (schedule_result_t *)malloc(sizeof(schedule_result_t));
				msg4_result->output_subframe = msg4_first_subframe;// msg4_subframe;
				msg4_result->end_subframe = msg4_end_subframe;
				msg4_result->sdu_length = 0;
				msg4_result->direction = DL;
				msg4_result->DCI_release = 0;
				msg4_result->channel = NPDSCH;
				msg4_result->rnti = msg4_nodes->ue_rnti;
		    	msg4_result->rnti_type = 1;
		    	msg4_result->npusch_format = 0;	//useless
		    	msg4_result->R_harq = 0;
				msg4_result->next = (schedule_result_t *)0;
				msg4_result->DCI_pdu = (void *)dci_n1_msg4;
				//msg4_result->harq_round = msg4_nodes->msg4_retransmit_count;

				harq_result = (schedule_result_t *)malloc(sizeof(schedule_result_t));
				harq_result->rnti = msg4_nodes->ue_rnti;
			    harq_result->output_subframe = harq_subframe;
			    harq_result->end_subframe = harq_end_subframe;
			    harq_result->sdu_length = 0;
			    harq_result->direction = UL;
			    harq_result->rnti_type = 3;
			    harq_result->DLSCH_pdu = NULL;
			    harq_result->DCI_pdu = (void *)dci_n1_msg4;
			    harq_result->DCI_release = 1;
			    harq_result->channel = NPUSCH;
			    harq_result->next = (schedule_result_t *)0;
				
				/*if(msg4_nodes->msg4_retransmit_count==0){
			        dci_result->printf_str = str3;
			        msg4_result->printf_str = str4;
			        harq_result->printf_str = str5;
                }else{
                    dci_result->printf_str = str8;
			        msg4_result->printf_str = str9;
			        harq_result->printf_str = str10;
                }*/
				
				//simulate_rx(&simulate_rx_msg4_list, msg4_nodes->ue_rnti, harq_subframe);
				
				LOG_D(MAC,"[%04d][RA scheduler][MSG4] UE:%x MSG4DCI %d-%d MSG4 %d-%d HARQ %d-%d\n", abs_subframe-1, msg4_nodes->ue_rnti, dci_first_subframe, dci_end_subframe, msg4_first_subframe, msg4_end_subframe, HARQ_info.sf_start, HARQ_info.sf_end);
	            LOG_D(MAC,"[%04d][RA scheduler][MSG4][CE%d] MSG4 DCI %d-%d MSG4 %d-%d HARQ %d-%d\n", abs_subframe-1, msg4_nodes->ce_level, dci_first_subframe, dci_end_subframe, msg4_first_subframe, msg4_end_subframe, HARQ_info.sf_start, HARQ_info.sf_end);
	            msg4_nodes->msg4_retransmit_count++;
	            
				//	fill dci resource
				fill_resource_DL(mac_inst, dci_node, dci_first_subframe, dci_end_subframe, dci_result);

                //	fill msg4 resource
				fill_resource_DL(mac_inst, msg4_node, msg4_first_subframe, msg4_end_subframe, msg4_result);

				//	fill ack/nack resource
				insert_schedule_result(&schedule_result_list_UL, harq_subframe, harq_result);
				adjust_UL_resource_list(&HARQ_info);
                
				//	active ue_list ul/dl
				UE_list_NB_IoT_t *UE_list = mac_inst->UE_list_spec;
				for(i=0; i<MAX_NUMBER_OF_UE_MAX_NB_IoT; ++i){
					if(UE_list->UE_template_NB_IoT[i].active == 1 && UE_list->UE_template_NB_IoT[i].rnti == msg4_nodes->ue_rnti){
						UE_list->UE_template_NB_IoT[i].direction = rand()&1;
					}
				}	
			}else{
			    LOG_D(MAC,"[%04d][RA scheduler][MSG4] fail vector %d\n", abs_subframe, fail );
			    LOG_D(MAC,"[%04d][RA scheduler][MSG4] rnti: %d preamble: %d fail vector %d\n", abs_subframe-1, msg4_nodes->ra_rnti, msg4_nodes->preamble_index, fail);
            
	       }
		}
        msg4_nodes = msg4_nodes->next;
	}
	
	//printf_FUNCTION_OUT("[SCHEDULER RA MSG4]");
	return ;
}

void schedule_RA_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst){
    //printf_FUNCTION_IN("[SCHEDULER RA]");
    uint32_t schedule_subframe = mac_inst->current_subframe + 1;
    schedule_subframe = schedule_subframe % 1048576;    //  20 bits, 10 bits + 10 bits
    
    //	this is the priority order in current stage.
    schedule_msg3_retransimission_NB_IoT(mac_inst, schedule_subframe);
    schedule_rar_NB_IoT(mac_inst, schedule_subframe);
	schedule_msg4_NB_IoT(mac_inst, schedule_subframe);

    //printf_FUNCTION_OUT("[SCHEDULER RA]");
	return ;
}

//  7bytes	TODO: CHECK
void fill_rar_NB_IoT(
	eNB_MAC_INST_NB_IoT *inst, 
	RA_TEMPLATE_NB_IoT *ra_template,
	uint8_t msg3_schedule_delay,
	uint8_t msg3_rep,
	sched_temp_UL_NB_IoT_t *schedule_template
)
//------------------------------------------------------------------------------
{
	uint8_t *dlsch_buffer = &ra_template->rar_buffer[0];
	RA_HEADER_RAPID_NB_IoT *rarh = (RA_HEADER_RAPID_NB_IoT *)dlsch_buffer;
	int i;
//,ra_idx = -1;
//	uint16_t rballoc;
//	uint8_t mcs,TPC,ULdelay,cqireq;
	
	for(i=0; i<7; ++i){
	    dlsch_buffer[i] = 0x0;
    }
	
	// subheader fixed
	rarh->E                     = 0; // First and last RAR
	rarh->T                     = 1; // 0 for E/T/R/R/BI subheader, 1 for E/T/RAPID subheader
	rarh->RAPID                 = ra_template->preamble_index; // Respond to Preamble 0 only for the moment
	
	uint8_t *rar = (uint8_t *)(dlsch_buffer+1);
	//	ta
	ra_template->ta >>= 4;
	rar[0] = (uint8_t)(ra_template->ta>>(2+4)); // 7 MSBs of timing advance + divide by 4
	rar[1] = (uint8_t)(ra_template->ta<<(4-2))&0xf0; // 4 LSBs of timing advance + divide by 4
	
	//	msg3 grant (15bits)
	//	subcarrier spacing:1 subcarrier indication:6 scheduling delay:2 msg3 repetition:3 MCS index: 3
	uint8_t subcarrier_spacing = 1;	// 1bit 15kHz
	uint8_t subcarrier_indication = schedule_template->subcarrier_indication; // 6bits
	uint8_t i_delay = msg3_schedule_delay; // 2bits
	uint8_t msg3_repetition = msg3_rep;// 3bit
	uint8_t mcs_index = 0;//3bit, msg3 88bits 3'b000
	
	rar[1] |= (subcarrier_spacing<<4) | (subcarrier_indication>>3);
	rar[2] = (uint8_t)(subcarrier_indication<<5) | (i_delay<<3) | msg3_repetition;
	rar[3] = (mcs_index<<5)&0xe0;  //  maped
	
	//	tc-rnti
	rar[4] = (uint8_t)(ra_template->ue_rnti>>8);
	rar[5] = (uint8_t)(ra_template->ue_rnti&0xff);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// NB-IoT testing /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void initiate_ra_proc_NB_IoT(module_id_t module_idP, int CC_id,frame_t frameP, uint16_t preamble_index,int16_t timing_offset,uint8_t sect_id,sub_frame_t subframeP,
                      uint8_t f_id)
{

  uint8_t i;
  RA_TEMPLATE *RA_template = (RA_TEMPLATE *)&eNB_mac_inst[module_idP].common_channels[CC_id].RA_template[0];

  printf("xxxxxxxxxxxxxxxx NB-IoT xxxxxxxxxxxxxx");
  for (i=0; i<NB_RA_PROC_MAX; i++) {
    if (RA_template[i].RA_active==FALSE &&
        RA_template[i].wait_ack_Msg4 == 0) {
     // int loop = 0;
      RA_template[i].RA_active=TRUE;
      RA_template[i].generate_rar=1;
      RA_template[i].generate_Msg4=0;
      RA_template[i].wait_ack_Msg4=0;
      RA_template[i].timing_offset=timing_offset;
      RA_template[i].RA_rnti = 1 + (frameP/4);
      RA_template[i].preamble_index = preamble_index;
      /* TODO: find better procedure to allocate RNTI */
    /*  do {
        RA_template[i].rnti = taus();
        loop++;
      } while (loop != 100 &&
               // TODO: this is not correct, the rnti may be in use without
               // being in the MAC yet. To be refined.
                //
      ///         !(find_UE_id(module_idP, RA_template[i].rnti) == -1 &&
                 // 1024 and 60000 arbirarily chosen, not coming from standard //
   //              RA_template[i].rnti >= 1024 && RA_template[i].rnti < 60000));
      if (loop == 100) {
       printf("%s:%d:%s: FATAL ERROR! contact the authors\n", __FILE__, __LINE__, __FUNCTION__); abort(); }

      RA_template[i].RA_rnti = 1 + (frameP/4);
      RA_template[i].preamble_index = preamble_index;   /// preamble_index=000000;

      LOG_D(MAC,"[eNB %d][RAPROC] CC_id %d Frame %d Activating RAR generation for process %d, rnti %x, RA_active %d\n",
            module_idP,CC_id,frameP,i,RA_template[i].rnti,
            RA_template[i].RA_active);*/

      return;
    }
  }

  LOG_E(MAC,"[eNB %d][RAPROC] FAILURE: CC_id %d Frame %d Initiating RA procedure for preamble index %d\n",module_idP,CC_id,frameP,preamble_index);
}


