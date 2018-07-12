
/*! \file eNB_scheduler_ulsch_NB_IoT.c
 * \brief handle UL UE-specific scheduling
 * \author  NTUST BMW Lab./
 * \date 2017
 * \email: 
 * \version 1.0
 *
 */

#include "defs_NB_IoT.h"
#include "proto_NB_IoT.h"
#include "extern_NB_IoT.h"

unsigned char str20[] = "DCI_uss";
unsigned char str21[] = "DATA_uss";

// scheduling UL
int schedule_UL_NB_IoT(eNB_MAC_INST_NB_IoT *mac_inst,UE_TEMPLATE_NB_IoT *UE_info,uint32_t subframe, uint32_t frame, uint32_t H_SFN){

	int i,ndi = 0,check_DCI_result = 0,check_UL_result = 0,candidate;
	uint32_t DL_end;
    //Scheduling resource temp buffer
    sched_temp_DL_NB_IoT_t *NPDCCH_info = (sched_temp_DL_NB_IoT_t*)malloc(sizeof(sched_temp_DL_NB_IoT_t));

	candidate = UE_info->R_max/UE_info->R_dci;
    uint32_t mcs = max_mcs[UE_info->multi_tone];
    uint32_t mappedMcsIndex=UE_info->PHR+(4 * UE_info->multi_tone);
    int TBS = 0;
    int Iru = 0, Nru, I_rep,N_rep,total_ru;
    int dly = 0,uplink_time = 0;

    if(UE_info->ul_total_buffer<=0)
    {
        LOG_D(MAC,"[%04d][UL scheduler][UE:%05d] No UL data in buffer\n", mac_inst->current_subframe, UE_info->rnti);
        return -1;
    }

    TBS=get_TBS_UL_NB_IoT(mcs,UE_info->multi_tone,Iru);

    sched_temp_UL_NB_IoT_t *NPUSCH_info = (sched_temp_UL_NB_IoT_t*)malloc(sizeof(sched_temp_UL_NB_IoT_t));

    DCIFormatN0_t *DCI_N0 = (DCIFormatN0_t*)malloc(sizeof(DCIFormatN0_t));

    //available_resource_DL_t *node;

    // setting of the NDI
    if(UE_info->HARQ_round == 0)
    {
        ndi = 1-UE_info->oldNDI_UL;
        UE_info->oldNDI_UL=ndi;
    }

    for (i = 0; i < candidate; i++)
	{
		/*step 1 : Check DL resource is available for DCI N0 or not*/
		check_DCI_result = check_resource_NPDCCH_NB_IoT(mac_inst,H_SFN, frame, subframe, NPDCCH_info, i, UE_info->R_dci);

        //node = check_resource_DL(mac_inst,);

        //just use to check when there is no DL function
        //NPDCCH_info->sf_start = H_SFN*10240+frame*10 +subframe + i * UE_info->R_dci;
        //NPDCCH_info->sf_end = NPDCCH_info->sf_start + (i+1) * UE_info->R_dci;

        //LOG_D(MAC,"UE : %5d, NPDCCH result: %d ,NPDCCH start: %d,NPDCCH end : %d\n",UE_info->rnti,check_DCI_result,NPDCCH_info->sf_start,NPDCCH_info->sf_end);
        if( check_DCI_result != -1)
		{
			/*step 2 : Determine MCS / TBS / REP / RU number*/
            /*while((mapped_mcs[UE_info->CE_level][mappedMcsIndex]< mcs)||((TBS>UE_info->ul_total_buffer)&&(mcs>=0)))
                {
                    --mcs;
                    TBS=get_TBS_UL_NB_IoT(mcs,UE_info->multi_tone,Iru);
                }*/

            mcs = mapped_mcs[UE_info->CE_level][mappedMcsIndex];

            while((TBS<UE_info->ul_total_buffer)&&(Iru<=7))
                {
                    Iru++;
                    TBS=get_TBS_UL_NB_IoT(mcs,UE_info->multi_tone,Iru);
                }

            //LOG_D(MAC,"TBS : %d UL_buffer: %d\n", TBS, UE_info->ul_total_buffer);

            Nru = RU_table[Iru];
            DL_end = NPDCCH_info->sf_end;
            N_rep = get_N_REP(UE_info->CE_level);
            I_rep = get_I_REP(N_rep);
            total_ru = Nru * N_rep;

            LOG_D(MAC,"[%04d][UL scheduler][UE:%05d] Multi-tone:%d,MCS:%d,TBS:%d,UL_buffer:%d,DL_start:%d,DL_end:%d,N_rep:%d,N_ru:%d,Total_ru:%d\n", mac_inst->current_subframe,UE_info->rnti,UE_info->multi_tone,mcs,TBS,UE_info->ul_total_buffer,NPDCCH_info->sf_start,DL_end,N_rep,Nru,total_ru);

            /*step 3 Check UL resource for Uplink data*/
			// we will loop the scheduling delay here
            for(dly=0;dly<4;dly++)
            {
                uplink_time = DL_end +scheduling_delay[dly];
                check_UL_result = Check_UL_resource(uplink_time,total_ru, NPUSCH_info, UE_info->multi_tone, 0);
                if (check_UL_result != -1)
                {
                    // step 4 : generate DCI content
                    DCI_N0->type = 0;
                    DCI_N0->scind = NPUSCH_info->subcarrier_indication;
                    DCI_N0->ResAssign = Iru;
                    DCI_N0->mcs = mcs;
                    DCI_N0->ndi = ndi;
                    DCI_N0->Scheddly = dly;
                    DCI_N0->RepNum = I_rep;
                    DCI_N0->rv = (UE_info->HARQ_round%2==0)?0:1; // rv will loop 0 & 2
                    DCI_N0->DCIRep = get_DCI_REP(UE_info->R_dci,UE_info->R_max);

                LOG_D(MAC,"[%04d][UL scheduler][UE:%05d] DCI content = scind : %d ResAssign : %d mcs : %d ndi : %d scheddly : %d RepNum : %d rv : %d DCIRep : %d\n", mac_inst->current_subframe,UE_info->rnti,DCI_N0->scind,DCI_N0->ResAssign,DCI_N0->mcs,DCI_N0->ndi,DCI_N0->Scheddly,DCI_N0->RepNum,DCI_N0->rv,DCI_N0->DCIRep);
                LOG_D(MAC,"[%04d][ULSchedulerUSS][%d][Success] complete scheduling with data size %d\n", mac_inst->current_subframe, UE_info->rnti, UE_info->ul_total_buffer);
                LOG_D(MAC,"[%04d][ULSchedulerUSS][%d][Success] DCI content = scind : %d ResAssign : %d mcs : %d ndi : %d scheddly : %d RepNum : %d rv : %d DCIRep : %d\n", mac_inst->current_subframe, UE_info->rnti, DCI_N0->scind,DCI_N0->ResAssign,DCI_N0->mcs,DCI_N0->ndi,DCI_N0->Scheddly,DCI_N0->RepNum,DCI_N0->rv,DCI_N0->DCIRep);
                // step 5 resource allocation and generate scheduling result
                generate_scheduling_result_UL(NPDCCH_info->sf_start, NPDCCH_info->sf_end,NPUSCH_info->sf_start, NPUSCH_info->sf_end,DCI_N0,UE_info->rnti, str20, str21);
                //fill_resource_DL();
                maintain_resource_DL(mac_inst,NPDCCH_info,NULL);

                adjust_UL_resource_list(NPUSCH_info);
                //Change the UE state to idle
                UE_info->direction = -1;
                return 0;
                }
            }

		}
        /*break now, we only loop one candidiate*/
        //break;
	}
    LOG_D(MAC,"[%04d][ULSchedulerUSS][%d][Fail] UL scheduling USS fail\n", mac_inst->current_subframe, UE_info->rnti);
	LOG_D(MAC,"[%04d][UL scheduler][UE:%05d] there is no available UL resource\n", mac_inst->current_subframe, UE_info->rnti);
	return -1;
}

void rx_sdu_NB_IoT(module_id_t module_id, int CC_id, frame_t frame, sub_frame_t subframe, uint16_t rnti, uint8_t *sdu, uint16_t  length)
{
    unsigned char  rx_ces[5], num_ce = 0, num_sdu = 0, *payload_ptr, i; // MAX Control element
    unsigned char  rx_lcids[5];//for NB_IoT-IoT, NB_IoT_RB_MAX should be fixed to 5 (2 DRB+ 3SRB) 
  unsigned short rx_lengths[5];
  //int UE_id = 0;
  int BSR_index=0;
  int DVI_index = 0;
  int PHR = 0;
  int ul_total_buffer = 0;
  //mac_NB_IoT_t *mac_inst;
  UE_TEMPLATE_NB_IoT *UE_info;

  //mac_inst = get_mac_inst(module_id);

  // note: if lcid < 25 this is sdu, otherwise this is CE
  payload_ptr = parse_ulsch_header_NB_IoT(sdu, &num_ce, &num_sdu,rx_ces, rx_lcids, rx_lengths, length);

  //LOG_D(MAC,"num_CE= %d, num_sdu= %d, rx_ces[0] = %d, rx_lcids =  %d, rx_lengths[0] = %d, length = %d\n",num_ce,num_sdu,rx_ces[0],rx_lcids[0],rx_lengths[0],length);

  for (i = 0; i < num_ce; i++)
  {
    switch(rx_ces[i])
    {
        case CRNTI:
          // find UE id again, confirm the UE, intial some ue specific parameters
          payload_ptr+=2;
            break;
        case SHORT_BSR:
            // update BSR here
        UE_info = get_ue_from_rnti(mac_inst, rnti);
        BSR_index = payload_ptr[0] & 0x3f;
        UE_info->ul_total_buffer = BSR_table[BSR_index];
            payload_ptr+=1;
            break;
        default:
        LOG_D(MAC,"Received unknown MAC header (0x%02x)\n", rx_ces[i]);
                break;
        }
    }
    for (i = 0; i < num_sdu; i++)
    {
        switch(rx_lcids[i])
        {
            case CCCH_NB_IoT:
                
                // MSG3 content: |R|R|PHR|PHR|DVI|DVI|DVI|DVI|CCCH payload
                PHR = ((payload_ptr[0] >> 5) & 0x01)*2+((payload_ptr[0]>>4) & 0x01);
                DVI_index = (payload_ptr[0] >>3 & 0x01)*8+ (payload_ptr[0] >>2 & 0x01)*4 + (payload_ptr[0] >>1 & 0x01)*2 +(payload_ptr[0] >>0 & 0x01);
          //LOG_D(MAC,"DVI_index= %d\n",DVI_index);
                ul_total_buffer = DV_table[DVI_index];
                LOG_D(MAC,"PHR = %d, ul_total_buffer = %d\n",PHR,ul_total_buffer);
                // go to payload
                payload_ptr+=1; 
                rx_lengths[i]-=1;
                LOG_D(MAC,"rx_lengths : %d\n", rx_lengths[i]);
                //NB_IoT_mac_rrc_data_ind(payload_ptr,mac_inst,rnti);
                //NB_IoT_receive_msg3(mac_inst,rnti,PHR,ul_total_buffer);
          break;
            case DCCH0_NB_IoT:
            case DCCH1_NB_IoT:
                // UE specific here
                //NB_IoT_mac_rlc_data_ind(payload_ptr,mac_inst,rnti);
            
          break;
            // all the DRBS
            case DTCH0_NB_IoT:
            default:
                //NB_IoT_mac_rlc_data_ind(payload_ptr,mac_inst,rnti);
          break;
        }
        payload_ptr+=rx_lengths[i];
    }

   
}

uint8_t *parse_ulsch_header_NB_IoT( uint8_t *mac_header,
                             uint8_t *num_ce,
                             uint8_t *num_sdu,
                             uint8_t *rx_ces,
                             uint8_t *rx_lcids,
                             uint16_t *rx_lengths,
                             uint16_t tb_length ){

uint8_t not_done=1, num_ces=0, num_sdus=0, lcid,num_sdu_cnt;
uint8_t *mac_header_ptr = mac_header;
uint16_t length, ce_len=0;

  while(not_done==1){

    if(((SCH_SUBHEADER_FIXED_NB_IoT*)mac_header_ptr)->E == 0){
      not_done = 0;
    }

    lcid = ((SCH_SUBHEADER_FIXED_NB_IoT*)mac_header_ptr)->LCID;

    if(lcid < EXTENDED_POWER_HEADROOM){
      if (not_done==0) { // last MAC SDU, length is implicit
        mac_header_ptr++;
        length = tb_length-(mac_header_ptr-mac_header)-ce_len;

        for(num_sdu_cnt=0; num_sdu_cnt < num_sdus ; num_sdu_cnt++){
          length -= rx_lengths[num_sdu_cnt];
        }
      }else{
        if(((SCH_SUBHEADER_SHORT_NB_IoT *)mac_header_ptr)->F == 0){
          length = ((SCH_SUBHEADER_SHORT_NB_IoT *)mac_header_ptr)->L;
          mac_header_ptr += 2;//sizeof(SCH_SUBHEADER_SHORT);
        }else{ // F = 1
          length = ((((SCH_SUBHEADER_LONG_NB_IoT *)mac_header_ptr)->L_MSB & 0x7f ) << 8 ) | (((SCH_SUBHEADER_LONG_NB_IoT *)mac_header_ptr)->L_LSB & 0xff);
          mac_header_ptr += 3;//sizeof(SCH_SUBHEADER_LONG);
        }
      }

      rx_lcids[num_sdus] = lcid;
      rx_lengths[num_sdus] = length;
      num_sdus++;
    }else{ // This is a control element subheader POWER_HEADROOM, BSR and CRNTI
      if(lcid == SHORT_PADDING){
        mac_header_ptr++;
      }else{
        rx_ces[num_ces] = lcid;
        num_ces++;
        mac_header_ptr++;

        if(lcid==LONG_BSR){
          ce_len+=3;
        }else if(lcid==CRNTI){
          ce_len+=2;
        }else if((lcid==POWER_HEADROOM) || (lcid==TRUNCATED_BSR)|| (lcid== SHORT_BSR)) {
          ce_len++;
        }else{
          // wrong lcid
        }
      }
    }
  }

  *num_ce = num_ces;
  *num_sdu = num_sdus;

  return(mac_header_ptr);
}

