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

/*! \file PHY/LTE_TRANSPORT/dlsch_demodulation.c
 * \brief Top-level routines for demodulating the PDSCH physical channel from 36-211, V8.6 2009-03
 * \author R. Knopp, F. Kaltenberger,A. Bhamri, S. Aubert, X. Xiang
 * \date 2011
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr,ankit.bhamri@eurecom.fr,sebastien.aubert@eurecom.fr
 * \note
 * \warning
 */
//#include "PHY/defs.h"
#include "PHY/extern.h"
#include "defs.h"
#include "extern.h"
#include "PHY/sse_intrin.h"
#include "T.h"

#ifndef USER_MODE
#define NOCYGWIN_STATIC static
#else
#define NOCYGWIN_STATIC
#endif

/* dynamic shift for LLR computation for TM3/4
 * set as command line argument, see lte-softmodem.c
 * default value: 0
 */
int16_t dlsch_demod_shift = 0;

//#define DEBUG_HARQ

//#undef LOG_D
//#define LOG_D LOG_I

//#define DEBUG_PHY 1
//#define DEBUG_DLSCH_DEMOD 1



// [MCS][i_mod (0,1,2) = (2,4,6)]
unsigned char offset_mumimo_llr_drange_fix=0;
uint8_t interf_unaw_shift0=0;
uint8_t interf_unaw_shift1=0;
uint8_t interf_unaw_shift=0;
//inferference-free case
unsigned char interf_unaw_shift_tm4_mcs[29]={5, 3, 4, 3, 3, 2, 1, 1, 2, 0, 1, 1, 1, 1, 0, 0,
                                             1, 1, 1, 1, 0, 2, 1, 0, 1, 0, 1, 0, 0} ;
unsigned char interf_unaw_shift_tm1_mcs[29]={5, 5, 4, 3, 3, 3, 2, 2, 4, 4, 2, 3, 3, 3, 1, 1,
                                             0, 1, 1, 2, 5, 4, 4, 6, 5, 1, 0, 5, 6} ; // mcs 21, 26, 28 seem to be errorneous

/*
//original values from sebastion + same hand tuning
unsigned char offset_mumimo_llr_drange[29][3]={{8,8,8},{7,7,7},{7,7,7},{7,7,7},{6,6,6},{6,6,6},{6,6,6},{5,5,5},{4,4,4},{1,2,4}, // QPSK
{5,5,4},{5,5,5},{5,5,5},{3,3,3},{2,2,2},{2,2,2},{2,2,2}, // 16-QAM
{2,2,1},{3,3,3},{3,3,3},{3,3,1},{2,2,2},{2,2,2},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}; //64-QAM
*/
 /*
 //first optimization try
 unsigned char offset_mumimo_llr_drange[29][3]={{7, 8, 7},{6, 6, 7},{6, 6, 7},{6, 6, 6},{5, 6, 6},{5, 5, 6},{5, 5, 6},{4, 5, 4},{4, 3, 4},{3, 2, 2},{6, 5, 5},{5, 4, 4},{5, 5, 4},{3, 3, 2},{2, 2, 1},{2, 1, 1},{2, 2, 2},{3, 3, 3},{3, 3, 2},{3, 3, 2},{3, 2, 1},{2, 2, 2},{2, 2, 2},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};
 */
 //second optimization try
 /*
   unsigned char offset_mumimo_llr_drange[29][3]={{5, 8, 7},{4, 6, 8},{3, 6, 7},{7, 7, 6},{4, 7, 8},{4, 7, 4},{6, 6, 6},{3, 6, 6},{3, 6, 6},{1, 3, 4},{1, 1, 0},{3, 3, 2},{3, 4, 1},{4, 0, 1},{4, 2, 2},{3, 1, 2},{2, 1, 0},{2, 1, 1},{1, 0, 1},{1, 0, 1},{0, 0, 0},{1, 0, 0},{0, 0, 0},{0, 1, 0},{1, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};  w
 */
unsigned char offset_mumimo_llr_drange[29][3]= {{0, 6, 5},{0, 4, 5},{0, 4, 5},{0, 5, 4},{0, 5, 6},{0, 5, 3},{0, 4, 4},{0, 4, 4},{0, 3, 3},{0, 1, 2},{1, 1, 0},{1, 3, 2},{3, 4, 1},{2, 0, 0},{2, 2, 2},{1, 1, 1},{2, 1, 0},{2, 1, 1},{1, 0, 1},{1, 0, 1},{0, 0, 0},{1, 0, 0},{0, 0, 0},{0, 1, 0},{1, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}};


extern void print_shorts(char *s,int16_t *x);


int rx_pdsch(PHY_VARS_UE *ue,
             PDSCH_t type,
             unsigned char eNB_id,
             unsigned char eNB_id_i, //if this == ue->n_connected_eNB, we assume MU interference
             uint32_t frame,
             uint8_t subframe,
             unsigned char symbol,
             unsigned char first_symbol_flag,
             RX_type_t rx_type,
             unsigned char i_mod,
             unsigned char harq_pid)
{

  LTE_UE_COMMON *common_vars  = &ue->common_vars;
  LTE_UE_PDSCH **pdsch_vars;
  LTE_DL_FRAME_PARMS *frame_parms    = &ue->frame_parms;
  PHY_MEASUREMENTS *measurements = &ue->measurements;
  LTE_UE_DLSCH_t   **dlsch;

  int avg[4];
  int avg_0[2];
  int avg_1[2];

  unsigned char aatx,aarx;

  unsigned short nb_rb = 0, round;
  int avgs, rb;
  LTE_DL_UE_HARQ_t *dlsch0_harq,*dlsch1_harq = 0;

  uint8_t beamforming_mode;
  uint32_t *rballoc;

  int32_t **rxdataF_comp_ptr;
  int32_t **dl_ch_mag_ptr;
  int32_t codeword_TB0;
  int32_t codeword_TB1;



  switch (type) {
  case SI_PDSCH:
    pdsch_vars = &ue->pdsch_vars_SI[eNB_id];
    dlsch = &ue->dlsch_SI[eNB_id];
    dlsch0_harq = dlsch[0]->harq_processes[harq_pid];
    beamforming_mode  = 0;
    break;

  case RA_PDSCH:
    pdsch_vars = &ue->pdsch_vars_ra[eNB_id];
    dlsch = &ue->dlsch_ra[eNB_id];
    dlsch0_harq = dlsch[0]->harq_processes[harq_pid];
    beamforming_mode  = 0;
    break;

  case PDSCH:
    pdsch_vars = ue->pdsch_vars[subframe&0x1];
    dlsch = ue->dlsch[subframe&0x1][eNB_id];
    LOG_D(PHY,"AbsSubframe %d.%d / Sym %d harq_pid %d,  harq status %d.%d \n",
                   frame,subframe,symbol,harq_pid,
                   dlsch[0]->harq_processes[harq_pid]->status,
                   dlsch[1]->harq_processes[harq_pid]->status);

    if ((dlsch[0]->harq_processes[harq_pid]->status == ACTIVE) &&
        (dlsch[1]->harq_processes[harq_pid]->status == ACTIVE)){
      codeword_TB0 = dlsch[0]->harq_processes[harq_pid]->codeword;
      codeword_TB1 = dlsch[1]->harq_processes[harq_pid]->codeword;
      dlsch0_harq = dlsch[codeword_TB0]->harq_processes[harq_pid];
      dlsch1_harq = dlsch[codeword_TB1]->harq_processes[harq_pid];
    }
     else if ((dlsch[0]->harq_processes[harq_pid]->status == ACTIVE) &&
              (dlsch[1]->harq_processes[harq_pid]->status != ACTIVE) ) {
      codeword_TB0 = dlsch[0]->harq_processes[harq_pid]->codeword;
      dlsch0_harq = dlsch[0]->harq_processes[harq_pid];
      dlsch1_harq = NULL;
      codeword_TB1 = -1;
    }
     else if ((dlsch[0]->harq_processes[harq_pid]->status != ACTIVE) &&
              (dlsch[1]->harq_processes[harq_pid]->status == ACTIVE) ){
      codeword_TB1 = dlsch[1]->harq_processes[harq_pid]->codeword;
      dlsch0_harq  = dlsch[1]->harq_processes[harq_pid];
      dlsch1_harq  = NULL;
      codeword_TB0 = -1;
    }
    else {
      LOG_E(PHY,"[UE][FATAL] Frame %d subframe %d: no active DLSCH\n",ue->proc.proc_rxtx[0].frame_rx,subframe);
      return(-1);
    }
    beamforming_mode  = ue->transmission_mode[eNB_id]<7?0:ue->transmission_mode[eNB_id];
    break;

  default:
    LOG_E(PHY,"[UE][FATAL] Frame %d subframe %d: Unknown PDSCH format %d\n",ue->proc.proc_rxtx[0].frame_rx,subframe,type);
    return(-1);
    break;
  }
#ifdef DEBUG_HARQ
  printf("[DEMOD] MIMO mode = %d\n", dlsch0_harq->mimo_mode);
  printf("[DEMOD] cw for TB0 = %d, cw for TB1 = %d\n", codeword_TB0, codeword_TB1);
#endif

  DevAssert(dlsch0_harq);
  round = dlsch0_harq->round;

  if (eNB_id > 2) {
    LOG_W(PHY,"dlsch_demodulation.c: Illegal eNB_id %d\n",eNB_id);
    return(-1);
  }

  if (!common_vars) {
    LOG_W(PHY,"dlsch_demodulation.c: Null common_vars\n");
    return(-1);
  }

  if (!dlsch[0]) {
    LOG_W(PHY,"dlsch_demodulation.c: Null dlsch_ue pointer\n");
    return(-1);
  }

  if (!pdsch_vars) {
    LOG_W(PHY,"dlsch_demodulation.c: Null pdsch_vars pointer\n");
    return(-1);
  }

  if (!frame_parms) {
    LOG_W(PHY,"dlsch_demodulation.c: Null frame_parms\n");
    return(-1);
  }

  if (((frame_parms->Ncp == NORMAL) && (symbol>=7)) ||
      ((frame_parms->Ncp == EXTENDED) && (symbol>=6)))
    rballoc = dlsch0_harq->rb_alloc_odd;
  else
    rballoc = dlsch0_harq->rb_alloc_even;


  if (dlsch0_harq->mimo_mode>DUALSTREAM_PUSCH_PRECODING) {
    LOG_E(PHY,"This transmission mode is not yet supported!\n");
    return(-1);
  }



  if ((dlsch0_harq->mimo_mode==LARGE_CDD) || ((dlsch0_harq->mimo_mode>=DUALSTREAM_UNIFORM_PRECODING1) && (dlsch0_harq->mimo_mode<=DUALSTREAM_PUSCH_PRECODING)))  {
    DevAssert(dlsch1_harq);
    if (eNB_id!=eNB_id_i) {
      LOG_E(PHY,"TM3/TM4 requires to set eNB_id==eNB_id_i!\n");
      return(-1);
    }
  }

  // Pointers to data of OFDM symbol
  int32_t** rxdataF           = common_vars->common_vars_rx_data_per_thread[subframe&0x1].rxdataF;
  int32_t** dl_ch_estimates   = common_vars->common_vars_rx_data_per_thread[subframe&0x1].dl_ch_estimates[eNB_id];
  int32_t** dl_ch_estimates_i = common_vars->common_vars_rx_data_per_thread[subframe&0x1].dl_ch_estimates[eNB_id_i];
  int32_t** rxdataF_ext            = pdsch_vars[eNB_id  ]->rxdataF_ext[symbol];
  int32_t** rxdataF_ext_i          = pdsch_vars[eNB_id_i]->rxdataF_ext[symbol];
  int32_t** dl_ch_estimates_ext    = pdsch_vars[eNB_id  ]->dl_ch_estimates_ext[symbol];
  int32_t** dl_ch_estimates_ext_i  = pdsch_vars[eNB_id_i]->dl_ch_estimates_ext[symbol];
  int32_t** dl_bf_ch_estimates_ext = pdsch_vars[eNB_id  ]->dl_bf_ch_estimates_ext[symbol];
  int32_t** dl_bf_ch_estimates     = pdsch_vars[eNB_id  ]->dl_bf_ch_estimates[symbol];

  int32_t** dl_ch_mag0      = pdsch_vars[eNB_id]->dl_ch_mag0[symbol];
  int32_t** dl_ch_magb0     = pdsch_vars[eNB_id]->dl_ch_magb0[symbol];
  int32_t** rxdataF_comp0   = pdsch_vars[eNB_id]->rxdataF_comp0[symbol];
  int32_t** rho             = pdsch_vars[eNB_id]->rho[symbol];
  int32_t** dl_ch_mag0_i    = pdsch_vars[eNB_id_i]->dl_ch_mag0[symbol];
  int32_t** dl_ch_magb0_i   = pdsch_vars[eNB_id_i]->dl_ch_magb0[symbol];
  int32_t** rxdataF_comp0_i = pdsch_vars[eNB_id_i]->rxdataF_comp0[symbol];
  int32_t** rho_i           = pdsch_vars[eNB_id_i]->rho[symbol];

  if (frame_parms->nb_antenna_ports_eNB>1 && beamforming_mode==0) {
#ifdef DEBUG_DLSCH_MOD
    LOG_I(PHY,"dlsch: using pmi %x (%p), rb_alloc %x\n",pmi2hex_2Ar1(dlsch0_harq->pmi_alloc),dlsch[0],dlsch0_harq->rb_alloc_even[0]);
#endif

    nb_rb = dlsch_extract_rbs_dual(rxdataF,
                                   dl_ch_estimates,
                                   rxdataF_ext,
                                   dl_ch_estimates_ext,
                                   dlsch0_harq->pmi_alloc,
                                   pdsch_vars[eNB_id]->pmi_ext,
                                   rballoc,
                                   symbol,
                                   subframe,
                                   ue->high_speed_flag,
                                   frame_parms,
                                   dlsch0_harq->mimo_mode);
//#ifdef DEBUG_DLSCH_MOD
    /*   printf("dlsch: using pmi %lx, rb_alloc %x, pmi_ext ",pmi2hex_2Ar1(dlsch0_harq->pmi_alloc),*rballoc);
       for (rb=0;rb<nb_rb;rb++)
          printf("%d",pdsch_vars[eNB_id]->pmi_ext[rb]);
       printf("\n");*/
//#endif

   if (rx_type >= rx_IC_single_stream)
   {
       if (eNB_id_i<ue->n_connected_eNB)
       {// we are in TM5
           nb_rb = dlsch_extract_rbs_dual(rxdataF,
                                          dl_ch_estimates_i,
                                          rxdataF_ext_i,
                                          dl_ch_estimates_ext_i,
                                          dlsch0_harq->pmi_alloc,
                                          pdsch_vars[eNB_id_i]->pmi_ext,
                                          rballoc,
                                          symbol,
                                          subframe,
                                          ue->high_speed_flag,
                                          frame_parms,
                                          dlsch0_harq->mimo_mode);
       }
       else
       {
           nb_rb = dlsch_extract_rbs_dual(rxdataF,
                                          dl_ch_estimates,
                                          rxdataF_ext_i,
                                          dl_ch_estimates_ext_i,
                                          dlsch0_harq->pmi_alloc,
                                          pdsch_vars[eNB_id_i]->pmi_ext,
                                          rballoc,
                                          symbol,
                                          subframe,
                                          ue->high_speed_flag,
                                          frame_parms,
                                          dlsch0_harq->mimo_mode);
       }
   }
  }
  else if (beamforming_mode==0)
  { //else if nb_antennas_ports_eNB==1 && beamforming_mode == 0
      nb_rb = dlsch_extract_rbs_single(rxdataF,
                                       dl_ch_estimates,
                                       rxdataF_ext,
                                       dl_ch_estimates_ext,
                                       dlsch0_harq->pmi_alloc,
                                       pdsch_vars[eNB_id_i]->pmi_ext,
                                       rballoc,
                                       symbol,
                                       subframe,
                                       ue->high_speed_flag,
                                       frame_parms);

      if (rx_type==rx_IC_single_stream)
      {
          if (eNB_id_i < ue->n_connected_eNB)
          {
              nb_rb = dlsch_extract_rbs_single(rxdataF,
                                               dl_ch_estimates_i,
                                               rxdataF_ext_i,
                                               dl_ch_estimates_ext_i,
                                               dlsch0_harq->pmi_alloc,
                                               pdsch_vars[eNB_id_i]->pmi_ext,
                                               rballoc,
                                               symbol,
                                               subframe,
                                               ue->high_speed_flag,
                                               frame_parms);
          }
          else
          {
              nb_rb = dlsch_extract_rbs_single(rxdataF,
                                               dl_ch_estimates,
                                               rxdataF_ext_i,
                                               dl_ch_estimates_ext_i,
                                               dlsch0_harq->pmi_alloc,
                                               pdsch_vars[eNB_id_i]->pmi_ext,
                                               rballoc,
                                               symbol,
                                               subframe,
                                               ue->high_speed_flag,
                                               frame_parms);
          }
      }
  }
  else if (beamforming_mode==7)
  { //else if beamforming_mode == 7
      nb_rb = dlsch_extract_rbs_TM7(rxdataF,
                                    dl_bf_ch_estimates,
                                    rxdataF_ext,
                                    dl_bf_ch_estimates_ext,
                                    rballoc,
                                    symbol,
                                    subframe,
                                    ue->high_speed_flag,
                                    frame_parms);

  }
  else if (beamforming_mode > 7)
  {
      LOG_W(PHY,"dlsch_demodulation: beamforming mode not supported yet.\n");
  }

  //printf("nb_rb = %d, eNB_id %d\n",nb_rb,eNB_id);
  if (nb_rb == 0)
  {
      LOG_D(PHY,"dlsch_demodulation.c: nb_rb=0\n");
      return(-1);
  }


#ifdef DEBUG_PHY
  LOG_D(PHY,"[DLSCH] log2_maxh = %d (%d,%d)\n",pdsch_vars[eNB_id]->log2_maxh,avg[0],avgs);
  LOG_D(PHY,"[DLSCH] mimo_mode = %d\n", dlsch0_harq->mimo_mode);
#endif

  aatx = frame_parms->nb_antenna_ports_eNB;
  aarx = frame_parms->nb_antennas_rx;

  dlsch_scale_channel(dl_ch_estimates_ext,
                      frame_parms,
                      dlsch,
                      symbol,
                      nb_rb);

  if ((dlsch0_harq->mimo_mode<DUALSTREAM_UNIFORM_PRECODING1) &&
      (rx_type==rx_IC_single_stream) &&
      (eNB_id_i==ue->n_connected_eNB) &&
      (dlsch0_harq->dl_power_off==0)
      )
  {
      // TM5 two-user
      dlsch_scale_channel(dl_ch_estimates_ext_i,
                          frame_parms,
                          dlsch,
                          symbol,
                          nb_rb);
  }

  if (first_symbol_flag==1)
  {
      if (beamforming_mode==0)
      {
          if (dlsch0_harq->mimo_mode<LARGE_CDD)
          {
              dlsch_channel_level(dl_ch_estimates_ext,
                                  frame_parms,
                                  avg,
                                  symbol,
                                  nb_rb);
              avgs = 0;
              for (aatx=0;aatx<frame_parms->nb_antenna_ports_eNB;aatx++)
              {
                  for (aarx=0;aarx<frame_parms->nb_antennas_rx;aarx++)
                  {
                      avgs = cmax(avgs,avg[(aatx<<1)+aarx]);
                  }
              }

              pdsch_vars[eNB_id]->log2_maxh = (log2_approx(avgs)/2)+1;
          }
          else if ((dlsch0_harq->mimo_mode == LARGE_CDD) ||
                   ((dlsch0_harq->mimo_mode >=DUALSTREAM_UNIFORM_PRECODING1) &&
                    (dlsch0_harq->mimo_mode <=DUALSTREAM_PUSCH_PRECODING)))
          {
              dlsch_channel_level_TM34(dl_ch_estimates_ext,
                                       frame_parms,
                                       pdsch_vars[eNB_id]->pmi_ext,
                                       avg_0,
                                       avg_1,
                                       symbol,
                                       nb_rb,
                                       dlsch0_harq->mimo_mode);

              LOG_D(PHY,"Channel Level TM34  avg_0 %d, avg_1 %d, rx_type %d, rx_standard %d, interf_unaw_shift %d \n", avg_0[0],
                    avg_1[0], rx_type, rx_standard, interf_unaw_shift);
              if (rx_type>rx_standard)
              {
                  avg_0[0] = (log2_approx(avg_0[0])/2) + dlsch_demod_shift;// + 2 ;//+ 4;
                  avg_1[0] = (log2_approx(avg_1[0])/2) + dlsch_demod_shift;// + 2 ;//+ 4;
                  pdsch_vars[eNB_id]->log2_maxh0 = cmax(avg_0[0],0);
                  pdsch_vars[eNB_id]->log2_maxh1 = cmax(avg_1[0],0);
                  //printf("TM4 I-A log2_maxh0 = %d\n", pdsch_vars[eNB_id]->log2_maxh0);
                  //printf("TM4 I-A log2_maxh1 = %d\n", pdsch_vars[eNB_id]->log2_maxh1);
              }
              else
              {
                  avg_0[0] = (log2_approx(avg_0[0])/2) - 13 + interf_unaw_shift;
                  avg_1[0] = (log2_approx(avg_1[0])/2) - 13 + interf_unaw_shift;
                  pdsch_vars[eNB_id]->log2_maxh0 = cmax(avg_0[0],0);
                  pdsch_vars[eNB_id]->log2_maxh1 = cmax(avg_1[0],0);
                  //printf("TM4 I-UA log2_maxh0 = %d\n", pdsch_vars[eNB_id]->log2_maxh0);
                  //printf("TM4 I-UA log2_maxh1 = %d\n", pdsch_vars[eNB_id]->log2_maxh1);
              }
          }
          else if (dlsch0_harq->mimo_mode<DUALSTREAM_UNIFORM_PRECODING1)
          { // single-layer precoding (TM5, TM6)
              if ((rx_type==rx_IC_single_stream) && (eNB_id_i==ue->n_connected_eNB) && (dlsch0_harq->dl_power_off==0))
              {
                  dlsch_channel_level_TM56(dl_ch_estimates_ext,
                                           frame_parms,
                                           pdsch_vars[eNB_id]->pmi_ext,
                                           avg,
                                           symbol,
                                           nb_rb);

                  avg[0] = log2_approx(avg[0]) - 13 + offset_mumimo_llr_drange[dlsch0_harq->mcs][(i_mod>>1)-1];
                  pdsch_vars[eNB_id]->log2_maxh = cmax(avg[0],0);
              }
              else if (dlsch0_harq->dl_power_off==1)
              { //TM6
                  dlsch_channel_level(dl_ch_estimates_ext,
                                      frame_parms,
                                      avg,
                                      symbol,
                                      nb_rb);

                  avgs = 0;
                  for (aatx=0;aatx<frame_parms->nb_antenna_ports_eNB;aatx++)
                  {
                      for (aarx=0;aarx<frame_parms->nb_antennas_rx;aarx++)
                      {
                          avgs = cmax(avgs,avg[(aatx<<1)+aarx]);
                      }
                  }

                  pdsch_vars[eNB_id]->log2_maxh = (log2_approx(avgs)/2) + 1;
                  pdsch_vars[eNB_id]->log2_maxh++;
              }
          }
      }
      else if (beamforming_mode==7)
      {
          dlsch_channel_level_TM7(dl_bf_ch_estimates_ext,
                                  frame_parms,
                                  avg,
                                  symbol,
                                  nb_rb);
      }

#ifdef DEBUG_PHY
      LOG_I(PHY,"[DLSCH] log2_maxh = %d [log2_maxh0 %d log2_maxh1 %d] (%d,%d)\n",pdsch_vars[eNB_id]->log2_maxh,
            pdsch_vars[eNB_id]->log2_maxh0,
            pdsch_vars[eNB_id]->log2_maxh1,
            avg[0],avgs);
      LOG_I(PHY,"[DLSCH] mimo_mode = %d\n", dlsch0_harq->mimo_mode);
#endif
  }

#if T_TRACER
    if (type == PDSCH)
    {
      T(T_UE_PHY_PDSCH_ENERGY, T_INT(eNB_id),  T_INT(0), T_INT(frame%1024), T_INT(subframe),
                               T_INT(avg[0]), T_INT(avg[1]),    T_INT(avg[2]),             T_INT(avg[3]));
    }
#endif

// Now channel compensation
    if (dlsch0_harq->mimo_mode < LARGE_CDD)
    {
        dlsch_channel_compensation(rxdataF_ext,
                                   dl_ch_estimates_ext,
                                   dl_ch_mag0,
                                   dl_ch_magb0,
                                   rxdataF_comp0,
                                   (aatx>1) ? rho : NULL,
                                   frame_parms,
                                   symbol,
                                   first_symbol_flag,
                                   dlsch0_harq->Qm,
                                   nb_rb,
                                   pdsch_vars[eNB_id]->log2_maxh,
                                   measurements); // log2_maxh+I0_shift

    if ((rx_type==rx_IC_single_stream) && (eNB_id_i < ue->n_connected_eNB))
    {
        dlsch_channel_compensation(rxdataF_ext_i,
                                   dl_ch_estimates_ext_i,
                                   dl_ch_mag0_i,
                                   dl_ch_magb0_i,
                                   rxdataF_comp0_i,
                                   (aatx>1) ? rho_i : NULL,
                                   frame_parms,
                                   symbol,
                                   first_symbol_flag,
                                   i_mod,
                                   nb_rb,
                                   pdsch_vars[eNB_id]->log2_maxh,
                                   measurements); // log2_maxh+I0_shift
#ifdef DEBUG_PHY
        if (symbol == 5)
        {
            write_output("rxF_comp_d.m","rxF_c_d",&pdsch_vars[eNB_id]->rxdataF_comp0[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);
            write_output("rxF_comp_i.m","rxF_c_i",&pdsch_vars[eNB_id_i]->rxdataF_comp0[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);
        }
#endif

        dlsch_dual_stream_correlation(frame_parms,
                                      symbol,
                                      nb_rb,
                                      dl_ch_estimates_ext,
                                      dl_ch_estimates_ext_i,
                                      pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                      pdsch_vars[eNB_id]->log2_maxh);
    }
    }
    else if ((dlsch0_harq->mimo_mode == LARGE_CDD) || ((dlsch0_harq->mimo_mode >=DUALSTREAM_UNIFORM_PRECODING1) &&
            (dlsch0_harq->mimo_mode <=DUALSTREAM_PUSCH_PRECODING)))
    {
        dlsch_channel_compensation_TM34(frame_parms,
                                        pdsch_vars[eNB_id],
                                        measurements,
                                        eNB_id,
                                        symbol,
                                        dlsch0_harq->Qm,
                                        dlsch1_harq->Qm,
                                        harq_pid,
                                        dlsch0_harq->round,
                                        dlsch0_harq->mimo_mode,
                                        nb_rb,
                                        pdsch_vars[eNB_id]->log2_maxh0,
                                        pdsch_vars[eNB_id]->log2_maxh1);

  /*   if (symbol == 5) {
     write_output("rxF_comp_d00.m","rxF_c_d00",&pdsch_vars[eNB_id]->rxdataF_comp0[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);// should be QAM
     write_output("rxF_comp_d01.m","rxF_c_d01",&pdsch_vars[eNB_id]->rxdataF_comp0[1][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be almost 0
     write_output("rxF_comp_d10.m","rxF_c_d10",&pdsch_vars[eNB_id]->rxdataF_comp1[harq_pid][round][0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be almost 0
     write_output("rxF_comp_d11.m","rxF_c_d11",&pdsch_vars[eNB_id]->rxdataF_comp1[harq_pid][round][1][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be QAM
        } */
        // compute correlation between signal and interference channels (rho12 and rho21)
        dlsch_dual_stream_correlation(frame_parms, // this is doing h11'*h12 and h21'*h22
                                      symbol,
                                      nb_rb,
                                      dl_ch_estimates_ext[symbol],
                                      &(pdsch_vars[eNB_id]->dl_ch_estimates_ext[2]),
                                      pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                      pdsch_vars[eNB_id]->log2_maxh0);
        //printf("rho stream1 =%d\n", &pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round] );
      //to be optimized (just take complex conjugate)
      dlsch_dual_stream_correlation(frame_parms, // this is doing h12'*h11 and h22'*h21
                                    symbol,
                                    nb_rb,
                                    &(pdsch_vars[eNB_id]->dl_ch_estimates_ext[2]),
                                    dl_ch_estimates_ext,
                                    pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                    pdsch_vars[eNB_id]->log2_maxh1);
    //  printf("rho stream2 =%d\n",&pdsch_vars[eNB_id]->dl_ch_rho2_ext );
      //printf("TM3 log2_maxh : %d\n",pdsch_vars[eNB_id]->log2_maxh);
  /*     if (symbol == 5) {
     write_output("rho0_0.m","rho0_0",&pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round][0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);// should be QAM
     write_output("rho2_0.m","rho2_0",&pdsch_vars[eNB_id]->dl_ch_rho2_ext[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be almost 0
     write_output("rho0_1.m.m","rho0_1",&pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round][1][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be almost 0
     write_output("rho2_1.m","rho2_1",&pdsch_vars[eNB_id]->dl_ch_rho2_ext[1][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be QAM
        } */

    }
    else if (dlsch0_harq->mimo_mode<DUALSTREAM_UNIFORM_PRECODING1)
    { // single-layer precoding (TM5, TM6)
        if ((rx_type==rx_IC_single_stream) && (eNB_id_i==ue->n_connected_eNB) && (dlsch0_harq->dl_power_off==0))
        {
            dlsch_channel_compensation_TM56(rxdataF_ext,
                                            dl_ch_estimates_ext,
                                            dl_ch_mag0,
                                            dl_ch_magb0,
                                            rxdataF_comp0,
                                            pdsch_vars[eNB_id]->pmi_ext,
                                            frame_parms,
                                            measurements,
                                            eNB_id,
                                            symbol,
                                            dlsch0_harq->Qm,
                                            nb_rb,
                                            pdsch_vars[eNB_id]->log2_maxh,
                                            dlsch0_harq->dl_power_off);

            for (rb=0; rb<nb_rb; rb++)
            {
                switch(pdsch_vars[eNB_id]->pmi_ext[rb])
                {
                case 0:
                    pdsch_vars[eNB_id_i]->pmi_ext[rb]=1;
                    break;
                case 1:
                    pdsch_vars[eNB_id_i]->pmi_ext[rb]=0;
                    break;
                case 2:
                    pdsch_vars[eNB_id_i]->pmi_ext[rb]=3;
                    break;
                case 3:
                    pdsch_vars[eNB_id_i]->pmi_ext[rb]=2;
                    break;
                }
                //  if (rb==0)
                //    printf("pmi %d, pmi_i %d\n",pdsch_vars[eNB_id]->pmi_ext[rb],pdsch_vars[eNB_id_i]->pmi_ext[rb]);
            }

            dlsch_channel_compensation_TM56(rxdataF_ext_i,
                                            dl_ch_estimates_ext_i,
                                            dl_ch_mag0_i,
                                            dl_ch_magb0_i,
                                            rxdataF_comp0_i,
                                            pdsch_vars[eNB_id_i]->pmi_ext,
                                            frame_parms,
                                            measurements,
                                            eNB_id_i,
                                            symbol,
                                            i_mod,
                                            nb_rb,
                                            pdsch_vars[eNB_id]->log2_maxh,
                                            dlsch0_harq->dl_power_off);
#ifdef DEBUG_PHY
      if (symbol==5)
      {
          write_output("rxF_comp_d.m","rxF_c_d",&pdsch_vars[eNB_id]->rxdataF_comp0[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);
          write_output("rxF_comp_i.m","rxF_c_i",&pdsch_vars[eNB_id_i]->rxdataF_comp0[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);
      }
#endif

      dlsch_dual_stream_correlation(frame_parms,
                                    symbol,
                                    nb_rb,
                                    dl_ch_estimates_ext,
                                    dl_ch_estimates_ext_i,
                                    pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                    pdsch_vars[eNB_id]->log2_maxh);

        }
        else if (dlsch0_harq->dl_power_off==1)
        {
            dlsch_channel_compensation_TM56(rxdataF_ext,
                                            dl_ch_estimates_ext,
                                            dl_ch_mag0,
                                            dl_ch_magb0,
                                            rxdataF_comp0,
                                            pdsch_vars[eNB_id]->pmi_ext,
                                            frame_parms,
                                            measurements,
                                            eNB_id,
                                            symbol,
                                            dlsch0_harq->Qm,
                                            nb_rb,
                                            pdsch_vars[eNB_id]->log2_maxh,
                                            1);
        }
    } else if (dlsch0_harq->mimo_mode==TM7)
    { //TM7
        dlsch_channel_compensation(rxdataF_ext,
                                   dl_bf_ch_estimates_ext,
                                   dl_ch_mag0,
                                   dl_ch_magb0,
                                   rxdataF_comp0,
                                   (aatx>1) ? rho : NULL,
                                   frame_parms,
                                   symbol,
                                   first_symbol_flag,
                                   get_Qm(dlsch0_harq->mcs),
                                   nb_rb,
                                   //9,
                                   pdsch_vars[eNB_id]->log2_maxh,
                                   measurements); // log2_maxh+I0_shift
    }

// MRC
    if (frame_parms->nb_antennas_rx > 1)
    {
        if ((dlsch0_harq->mimo_mode == LARGE_CDD) ||
            ((dlsch0_harq->mimo_mode >=DUALSTREAM_UNIFORM_PRECODING1) &&
             (dlsch0_harq->mimo_mode <=DUALSTREAM_PUSCH_PRECODING)))
        { // TM3 or TM4
            if (frame_parms->nb_antenna_ports_eNB == 2)
            {
                dlsch_detection_mrc_TM34(frame_parms,
                                         pdsch_vars[eNB_id],
                                         harq_pid,
                                         dlsch0_harq->round,
                                         symbol,
                                         nb_rb,
                                         1);
                /*   if (symbol == 5) {
                     write_output("rho0_mrc.m","rho0_0",&pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round][0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);// should be QAM
                     write_output("rho2_mrc.m","rho2_0",&pdsch_vars[eNB_id]->dl_ch_rho2_ext[0][symbol*frame_parms->N_RB_DL*12],frame_parms->N_RB_DL*12,1,1);//should be almost 0
                     } */
            }
        }
        else
        {
            dlsch_detection_mrc(frame_parms,
                                rxdataF_comp0,
                                rxdataF_comp0_i,
                                rho,
                                pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                dl_ch_mag0,
                                dl_ch_magb0,
                                dl_ch_mag0,
                                dl_ch_magb0,
                                symbol,
                                nb_rb,
                                rx_type==rx_IC_single_stream);
        }
    }
    //  printf("Combining");
    if ((dlsch0_harq->mimo_mode == SISO) ||
        ((dlsch0_harq->mimo_mode >= UNIFORM_PRECODING11) &&
         (dlsch0_harq->mimo_mode <= PUSCH_PRECODING0)) ||
        (dlsch0_harq->mimo_mode == TM7))
    {
        /*
          dlsch_siso(frame_parms,
          pdsch_vars[eNB_id]->rxdataF_comp,
          pdsch_vars[eNB_id_i]->rxdataF_comp,
          symbol,
          nb_rb);
        */
    }
    else if (dlsch0_harq->mimo_mode == ALAMOUTI)
    {
        dlsch_alamouti(frame_parms,
                       rxdataF_comp0,
                       dl_ch_mag0,
                       dl_ch_magb0,
                       symbol,
                       nb_rb);
    }

  //    printf("LLR");
    if ((dlsch0_harq->mimo_mode == LARGE_CDD) ||
        ((dlsch0_harq->mimo_mode >=DUALSTREAM_UNIFORM_PRECODING1) &&
         (dlsch0_harq->mimo_mode =DUALSTREAM_PUSCH_PRECODING)))
    {
        rxdataF_comp_ptr = pdsch_vars[eNB_id]->rxdataF_comp1[harq_pid][round];
        dl_ch_mag_ptr    = pdsch_vars[eNB_id]->dl_ch_mag1[harq_pid][round];
    }
    else
    {
        rxdataF_comp_ptr = rxdataF_comp0_i;
        dl_ch_mag_ptr    = dl_ch_mag0_i;
        //i_mod should have been passed as a parameter
    }

    switch (dlsch0_harq->Qm)
    {
    case 2 :
        if ((rx_type==rx_standard) || (codeword_TB0 == -1) || (codeword_TB1 == -1))
        {
            dlsch_qpsk_llr(frame_parms,
                           rxdataF_comp0[0],
                           pdsch_vars[eNB_id]->llr[0],
                           symbol,
                           first_symbol_flag,
                           nb_rb,
                           adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,2,subframe,symbol),
                           pdsch_vars[eNB_id]->llr128,
                           beamforming_mode);
        }
        else if (rx_type >= rx_IC_single_stream)
        {
            if (dlsch1_harq->Qm == 2)
            {
                dlsch_qpsk_qpsk_llr(frame_parms,
                                    rxdataF_comp0,
                                    rxdataF_comp_ptr,
                                    pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                    pdsch_vars[eNB_id]->llr[0],
                                    symbol,first_symbol_flag,nb_rb,
                                    adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,2,subframe,symbol),
                                    pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_qpsk_qpsk_llr(frame_parms,
                                        rxdataF_comp_ptr,
                                        rxdataF_comp0,
                                        pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                        pdsch_vars[eNB_id]->llr[1],
                                        symbol,first_symbol_flag,nb_rb,
                                        adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,2,subframe,symbol),
                                        pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
            else if (dlsch1_harq->Qm == 4)
            {
                dlsch_qpsk_16qam_llr(frame_parms,
                                     rxdataF_comp0,
                                     rxdataF_comp_ptr,//i
                                     dl_ch_mag_ptr,//i
                                     pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                     pdsch_vars[eNB_id]->llr[0],
                                     symbol,first_symbol_flag,nb_rb,
                                     adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,2,subframe,symbol),
                                     pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_16qam_qpsk_llr(frame_parms,
                                         rxdataF_comp_ptr,
                                         rxdataF_comp0,//i
                                         dl_ch_mag_ptr,
                                         pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                         pdsch_vars[eNB_id]->llr[1],
                                         symbol,first_symbol_flag,nb_rb,
                                         adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,4,subframe,symbol),
                                         pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
            else
            {
                dlsch_qpsk_64qam_llr(frame_parms,
                                     rxdataF_comp0,
                                     rxdataF_comp_ptr,//i
                                     dl_ch_mag_ptr,//i
                                     pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                     pdsch_vars[eNB_id]->llr[0],
                                     symbol,first_symbol_flag,nb_rb,
                                     adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,2,subframe,symbol),
                                     pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_64qam_qpsk_llr(frame_parms,
                                         rxdataF_comp_ptr,
                                         rxdataF_comp0,//i
                                         dl_ch_mag_ptr,
                                         pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                         pdsch_vars[eNB_id]->llr[1],
                                         symbol,first_symbol_flag,nb_rb,
                                         adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,6,subframe,symbol),
                                         pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
        }
        break;
    case 4 :
        if ((rx_type==rx_standard ) || (codeword_TB0 == -1) || (codeword_TB1 == -1))
        {
            dlsch_16qam_llr(frame_parms,
                            rxdataF_comp0[0],
                            pdsch_vars[eNB_id]->llr[0],
                            dl_ch_mag0[0],
                            symbol,first_symbol_flag,nb_rb,
                            adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,4,subframe,symbol),
                            pdsch_vars[eNB_id]->llr128,
                            beamforming_mode);
        }
        else if (rx_type >= rx_IC_single_stream)
        {
            if (dlsch1_harq->Qm == 2)
            {
                dlsch_16qam_qpsk_llr(frame_parms,
                                     rxdataF_comp0,
                                     rxdataF_comp_ptr,//i
                                     dl_ch_mag0,
                                     pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                     pdsch_vars[eNB_id]->llr[0],
                                     symbol,first_symbol_flag,nb_rb,
                                     adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,4,subframe,symbol),
                                     pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_qpsk_16qam_llr(frame_parms,
                                         rxdataF_comp_ptr,
                                         rxdataF_comp0,//i
                                         dl_ch_mag0,//i
                                         pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                         pdsch_vars[eNB_id]->llr[1],
                                         symbol,first_symbol_flag,nb_rb,
                                         adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,2,subframe,symbol),
                                         pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
      else if (dlsch1_harq->Qm == 4)
      {
          dlsch_16qam_16qam_llr(frame_parms,
                                rxdataF_comp0,
                                rxdataF_comp_ptr,//i
                                dl_ch_mag0,
                                dl_ch_mag_ptr,//i
                                pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                pdsch_vars[eNB_id]->llr[0],
                                symbol,first_symbol_flag,nb_rb,
                                adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,4,subframe,symbol),
                                pdsch_vars[eNB_id]->llr128);

          if (rx_type==rx_IC_dual_stream)
          {
              dlsch_16qam_16qam_llr(frame_parms,
                                    rxdataF_comp_ptr,
                                    rxdataF_comp0,//i
                                    dl_ch_mag_ptr,
                                    dl_ch_mag0,//i
                                    pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                    pdsch_vars[eNB_id]->llr[1],
                                    symbol,first_symbol_flag,nb_rb,
                                    adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,4,subframe,symbol),
                                    pdsch_vars[eNB_id]->llr128_2ndstream);
          }
      }
      else
      {
          dlsch_16qam_64qam_llr(frame_parms,
                                rxdataF_comp0,
                                rxdataF_comp_ptr,//i
                                dl_ch_mag0,
                                dl_ch_mag_ptr,//i
                                pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                pdsch_vars[eNB_id]->llr[0],
                                symbol,first_symbol_flag,nb_rb,
                                adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,4,subframe,symbol),
                                pdsch_vars[eNB_id]->llr128);

        if (rx_type==rx_IC_dual_stream)
        {
            dlsch_64qam_16qam_llr(frame_parms,
                                  rxdataF_comp_ptr,
                                  rxdataF_comp0,
                                  dl_ch_mag_ptr,
                                  dl_ch_mag0,
                                  pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                  pdsch_vars[eNB_id]->llr[1],
                                  symbol,first_symbol_flag,nb_rb,
                                  adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,6,subframe,symbol),
                                  pdsch_vars[eNB_id]->llr128_2ndstream);
        }
      }
        }
    break;
    case 6 :
        if ((rx_type==rx_standard) || (codeword_TB0 == -1) || (codeword_TB1 == -1))
        {
            dlsch_64qam_llr(frame_parms,
                            rxdataF_comp0[0],
                            pdsch_vars[eNB_id]->llr[0],
                            dl_ch_mag0[0],
                            dl_ch_magb0[0],
                            symbol,first_symbol_flag,nb_rb,
                            adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,6,subframe,symbol),
                            pdsch_vars[eNB_id]->llr128,
                            beamforming_mode);
        }
        else if (rx_type >= rx_IC_single_stream)
        {
            if (dlsch1_harq->Qm == 2)
            {
                dlsch_64qam_qpsk_llr(frame_parms,
                                     rxdataF_comp0,
                                     rxdataF_comp_ptr,//i
                                     dl_ch_mag0,
                                     pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                     pdsch_vars[eNB_id]->llr[0],
                                     symbol,first_symbol_flag,nb_rb,
                                     adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,6,subframe,symbol),
                                     pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_qpsk_64qam_llr(frame_parms,
                                         rxdataF_comp_ptr,
                                         rxdataF_comp0,//i
                                         dl_ch_mag0,
                                         pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                         pdsch_vars[eNB_id]->llr[1],
                                         symbol,first_symbol_flag,nb_rb,
                                         adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,2,subframe,symbol),
                                         pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
            else if (dlsch1_harq->Qm == 4)
            {
                dlsch_64qam_16qam_llr(frame_parms,
                                      rxdataF_comp0,
                                      rxdataF_comp_ptr,//i
                                      dl_ch_mag0,
                                      dl_ch_mag_ptr,//i
                                      pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                      pdsch_vars[eNB_id]->llr[0],
                                      symbol,first_symbol_flag,nb_rb,
                                      adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,6,subframe,symbol),
                                      pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_16qam_64qam_llr(frame_parms,
                                          rxdataF_comp_ptr,
                                          rxdataF_comp0,//i
                                          dl_ch_mag_ptr,
                                          dl_ch_mag0,//i
                                          pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                          pdsch_vars[eNB_id]->llr[1],
                                          symbol,first_symbol_flag,nb_rb,
                                          adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,4,subframe,symbol),
                                          pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
            else
            {
                dlsch_64qam_64qam_llr(frame_parms,
                                      rxdataF_comp0,
                                      rxdataF_comp_ptr,//i
                                      dl_ch_mag0,
                                      dl_ch_mag_ptr,//i
                                      pdsch_vars[eNB_id]->dl_ch_rho2_ext,
                                      pdsch_vars[eNB_id]->llr[0],
                                      symbol,first_symbol_flag,nb_rb,
                                      adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,6,subframe,symbol),
                                      pdsch_vars[eNB_id]->llr128);

                if (rx_type==rx_IC_dual_stream)
                {
                    dlsch_64qam_64qam_llr(frame_parms,
                                          rxdataF_comp_ptr,
                                          rxdataF_comp0,//i
                                          dl_ch_mag_ptr,
                                          dl_ch_mag0,//i
                                          pdsch_vars[eNB_id]->dl_ch_rho_ext[harq_pid][round],
                                          pdsch_vars[eNB_id]->llr[1],
                                          symbol,first_symbol_flag,nb_rb,
                                          adjust_G2(frame_parms,dlsch1_harq->rb_alloc_even,6,subframe,symbol),
                                          pdsch_vars[eNB_id]->llr128_2ndstream);
                }
            }
        }
        break;
    default:
        LOG_W(PHY,"rx_dlsch.c : Unknown mod_order!!!!\n");
        return(-1);
        break;
    }

    if (dlsch1_harq)
    {
        switch (get_Qm(dlsch1_harq->mcs))
        {
        case 2 :
            if (rx_type==rx_standard) {
                dlsch_qpsk_llr(frame_parms,
                               rxdataF_comp0[0],
                               pdsch_vars[eNB_id]->llr[0],
                               symbol,first_symbol_flag,nb_rb,
                               adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,2,subframe,symbol),
                               pdsch_vars[eNB_id]->llr128,
                               beamforming_mode);
            }
            break;
        case 4:
            if (rx_type==rx_standard)
            {
                dlsch_16qam_llr(frame_parms,
                                rxdataF_comp0[0],
                                pdsch_vars[eNB_id]->llr[0],
                                dl_ch_mag0[0],
                                symbol,first_symbol_flag,nb_rb,
                                adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,4,subframe,symbol),
                                pdsch_vars[eNB_id]->llr128,
                                beamforming_mode);
            }
            break;
        case 6 :
            if (rx_type==rx_standard)
            {
                dlsch_64qam_llr(frame_parms,
                                rxdataF_comp0[0],
                                pdsch_vars[eNB_id]->llr[0],
                                dl_ch_mag0[0],
                                dl_ch_magb0[0],
                                symbol,first_symbol_flag,nb_rb,
                                adjust_G2(frame_parms,dlsch0_harq->rb_alloc_even,6,subframe,symbol),
                                pdsch_vars[eNB_id]->llr128,
                                beamforming_mode);
            }
            break;
        default:
            LOG_W(PHY,"rx_dlsch.c : Unknown mod_order!!!!\n");
            return(-1);
            break;
        }
    }

// Please keep it: useful for debugging
#if 0
    if( (symbol == 13) && (dlsch0_harq->mimo_mode == 2) )
    {
        LOG_E(PHY,"Dump Phy Chan Est \n");
        if(subframe&0x1)
        {
#if 1
            //write_output("rxdataF0.m", "rxdataF0", rxdataF[0], 14*frame_parms->ofdm_symbol_size, 1, 1);
            //write_output("rxdataF1.m", "rxdataF1", rxdataF[1], 14*frame_parms->ofdm_symbol_size, 1, 1);
            //write_output("dl_ch_estimates00.m", "dl_ch_estimates00", dl_ch_estimates[0], 14*frame_parms->ofdm_symbol_size, 1, 1);
            //write_output("dl_ch_estimates01.m", "dl_ch_estimates01", dl_ch_estimates[1], 14*frame_parms->ofdm_symbol_size, 1, 1);
            //write_output("dl_ch_estimates10.m", "dl_ch_estimates10", dl_ch_estimates[2], 14*frame_parms->ofdm_symbol_size, 1, 1);
            //write_output("dl_ch_estimates11.m", "dl_ch_estimates11", dl_ch_estimates[3], 14*frame_parms->ofdm_symbol_size, 1, 1);

            uint32_t l;
            for (l=0; l < 14; l++)
            {
                //write_output("rxdataF_ext00.m", "rxdataF_ext00", pdsch_vars[eNB_id]->rxdataF_ext[l][0], frame_parms->N_RB_DL*12, 10, 1);
                //write_output("rxdataF_ext01.m", "rxdataF_ext01", pdsch_vars[eNB_id]->rxdataF_ext[l][1], frame_parms->N_RB_DL*12, 10, 1);
                //write_output("rxdataF_ext10.m", "rxdataF_ext10", pdsch_vars[eNB_id]->rxdataF_ext[l][2], frame_parms->N_RB_DL*12, 10, 1);
                //write_output("rxdataF_ext11.m", "rxdataF_ext11", pdsch_vars[eNB_id]->rxdataF_ext[l][3], frame_parms->N_RB_DL*12, 10, 1);

                write_output("dl_ch_estimates_ext00.m", "dl_ch_estimates_ext00", pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][0], frame_parms->N_RB_DL*12, 10, 1);
                write_output("dl_ch_estimates_ext01.m", "dl_ch_estimates_ext01", pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][1], frame_parms->N_RB_DL*12, 10, 1);
                write_output("dl_ch_estimates_ext10.m", "dl_ch_estimates_ext10", pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][2], frame_parms->N_RB_DL*12, 10, 1);
                write_output("dl_ch_estimates_ext11.m", "dl_ch_estimates_ext11", pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][3], frame_parms->N_RB_DL*12, 10, 1);

                write_output("rxdataF_comp00.m", "rxdataF_comp00", &pdsch_vars[eNB_id]->rxdataF_comp0[l][0], frame_parms->N_RB_DL*12, 10, 1);
                write_output("rxdataF_comp01.m", "rxdataF_comp01", &pdsch_vars[eNB_id]->rxdataF_comp0[l][1], frame_parms->N_RB_DL*12, 10, 1);
            }

            write_output("rxdataF_comp10.m", "rxdataF_comp10", &pdsch_vars[eNB_id]->rxdataF_comp1[harq_pid][round][0][0],14*frame_parms->N_RB_DL*12,1,1);
            write_output("rxdataF_comp11.m", "rxdataF_comp11", &pdsch_vars[eNB_id]->rxdataF_comp1[harq_pid][round][1][0],14*frame_parms->N_RB_DL*12,1,1);
#endif

            write_output("llr0.m","llr0",  &pdsch_vars[eNB_id]->llr[0][0],(14*nb_rb*12*dlsch1_harq->Qm) - 4*(nb_rb*4*dlsch1_harq->Qm),1,0);
            write_output("llr1.m","llr1",  &pdsch_vars[eNB_id]->llr[1][0],(14*nb_rb*12*dlsch1_harq->Qm) - 4*(nb_rb*4*dlsch1_harq->Qm),1,0);

            AssertFatal(0," ");
        }
    }
#endif

#if T_TRACER
  T(T_UE_PHY_PDSCH_IQ, T_INT(eNB_id), T_INT(ue->Mod_id), T_INT(frame%1024),
    T_INT(subframe), T_INT(nb_rb),
    T_INT(frame_parms->N_RB_UL), T_INT(frame_parms->symbols_per_tti),
    T_BUFFER(&pdsch_vars[eNB_id]->rxdataF_comp0[eNB_id][0],
             2 * /* ulsch[UE_id]->harq_processes[harq_pid]->nb_rb */ frame_parms->N_RB_UL *12*frame_parms->symbols_per_tti*2));
#endif

  return(0);

}

//==============================================================================================
// Pre-processing for LLR computation
//==============================================================================================

void dlsch_channel_compensation(int **rxdataF_ext,
                                int **dl_ch_estimates_ext,
                                int **dl_ch_mag,
                                int **dl_ch_magb,
                                int **rxdataF_comp,
                                int **rho,
                                LTE_DL_FRAME_PARMS *frame_parms,
                                unsigned char symbol,
                                uint8_t first_symbol_flag,
                                unsigned char mod_order,
                                unsigned short nb_rb,
                                unsigned char output_shift,
                                PHY_MEASUREMENTS *measurements)
{

#if defined(__i386) || defined(__x86_64)

    unsigned short rb;
    unsigned char aatx,aarx,symbol_mod,pilots=0;
    __m128i *dl_ch128,*dl_ch128_2,*dl_ch_mag128,*dl_ch_mag128b,*rxdataF128,*rxdataF_comp128,*rho128;
    __m128i mmtmpD0,mmtmpD1,mmtmpD2,mmtmpD3,QAM_amp128,QAM_amp128b;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
    {
        if (frame_parms->mode1_flag==1)
        { // 10 out of 12 so don't reduce size
            nb_rb=1+(5*nb_rb/6);
        }
        else
        {
            pilots=1;
        }
    }

    for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
    {
        if (mod_order == 4)
        {
            QAM_amp128 = _mm_set1_epi16(QAM16_n1);  // 2/sqrt(10)
            QAM_amp128b = _mm_setzero_si128();
        }
        else if (mod_order == 6)
        {
            QAM_amp128  = _mm_set1_epi16(QAM64_n1); //
            QAM_amp128b = _mm_set1_epi16(QAM64_n2);
        }

        // printf("comp: rxdataF_comp %p, symbol %d\n",rxdataF_comp[0], symbol);
        
        for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
        {
            rxdataF128      = (__m128i *)rxdataF_ext        [aarx];
            dl_ch128        = (__m128i *)dl_ch_estimates_ext[(aatx<<1)+aarx];
            dl_ch_mag128    = (__m128i *)dl_ch_mag          [(aatx<<1)+aarx];
            dl_ch_mag128b   = (__m128i *)dl_ch_magb         [(aatx<<1)+aarx];
            rxdataF_comp128 = (__m128i *)rxdataF_comp       [(aatx<<1)+aarx];
            
            for (rb=0; rb < nb_rb; rb++)
            {
                if (mod_order>2)
                {
                    // get channel amplitude if not QPSK
                    mmtmpD0 = _mm_madd_epi16(dl_ch128[0],dl_ch128[0]);
                    mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                    
                    mmtmpD1 = _mm_madd_epi16(dl_ch128[1],dl_ch128[1]);
                    mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                    
                    mmtmpD0 = _mm_packs_epi32(mmtmpD0,mmtmpD1);
                    
                    // store channel magnitude here in a new field of dlsch
                    
                    dl_ch_mag128[0] = _mm_unpacklo_epi16(mmtmpD0,mmtmpD0);
                    dl_ch_mag128b[0] = dl_ch_mag128[0];
                    dl_ch_mag128[0] = _mm_mulhi_epi16(dl_ch_mag128[0],QAM_amp128);
                    dl_ch_mag128[0] = _mm_slli_epi16(dl_ch_mag128[0],1);
                    //print_ints("Re(ch):",(int16_t*)&mmtmpD0);
                    //print_shorts("QAM_amp:",(int16_t*)&QAM_amp128);
                    //print_shorts("mag:",(int16_t*)&dl_ch_mag128[0]);
                    dl_ch_mag128[1] = _mm_unpackhi_epi16(mmtmpD0,mmtmpD0);
                    dl_ch_mag128b[1] = dl_ch_mag128[1];
                    dl_ch_mag128[1] = _mm_mulhi_epi16(dl_ch_mag128[1],QAM_amp128);
                    dl_ch_mag128[1] = _mm_slli_epi16(dl_ch_mag128[1],1);
                    
                    if (pilots==0)
                    {
                        mmtmpD0 = _mm_madd_epi16(dl_ch128[2],dl_ch128[2]);
                        mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                        mmtmpD1 = _mm_packs_epi32(mmtmpD0,mmtmpD0);
                        
                        dl_ch_mag128[2] = _mm_unpacklo_epi16(mmtmpD1,mmtmpD1);
                        dl_ch_mag128b[2] = dl_ch_mag128[2];
                        
                        dl_ch_mag128[2] = _mm_mulhi_epi16(dl_ch_mag128[2],QAM_amp128);
                        dl_ch_mag128[2] = _mm_slli_epi16(dl_ch_mag128[2],1);
                    }
                    
                    dl_ch_mag128b[0] = _mm_mulhi_epi16(dl_ch_mag128b[0],QAM_amp128b);
                    dl_ch_mag128b[0] = _mm_slli_epi16(dl_ch_mag128b[0],1);
                    
                    dl_ch_mag128b[1] = _mm_mulhi_epi16(dl_ch_mag128b[1],QAM_amp128b);
                    dl_ch_mag128b[1] = _mm_slli_epi16(dl_ch_mag128b[1],1);
                    
                    if (pilots==0)
                    {
                        dl_ch_mag128b[2] = _mm_mulhi_epi16(dl_ch_mag128b[2],QAM_amp128b);
                        dl_ch_mag128b[2] = _mm_slli_epi16(dl_ch_mag128b[2],1);
                    }
                }
                
                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[0],rxdataF128[0]);
                //  print_ints("re",&mmtmpD0);
                
                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[0],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
                //  print_ints("im",&mmtmpD1);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[0]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                //  print_ints("re(shift)",&mmtmpD0);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                //  print_ints("im(shift)",&mmtmpD1);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
                //        print_ints("c0",&mmtmpD2);
                //  print_ints("c1",&mmtmpD3);
                rxdataF_comp128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
                //  print_shorts("rx:",rxdataF128);
                //  print_shorts("ch:",dl_ch128);
                //  print_shorts("pack:",rxdataF_comp128);
                
                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[1],rxdataF128[1]);
                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[1],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[1]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
                
                rxdataF_comp128[1] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
                //  print_shorts("rx:",rxdataF128+1);
                //  print_shorts("ch:",dl_ch128+1);
                //  print_shorts("pack:",rxdataF_comp128+1);
                
                if (pilots==0)
                {
                    // multiply by conjugated channel
                    mmtmpD0 = _mm_madd_epi16(dl_ch128[2],rxdataF128[2]);
                    // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                    mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[2],_MM_SHUFFLE(2,3,0,1));
                    mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                    mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
                    mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[2]);
                    // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                    mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                    mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                    mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                    mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
                    
                    rxdataF_comp128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
                    // print_shorts("rx:",rxdataF128+2);
                    // print_shorts("ch:",dl_ch128+2);
                    // print_shorts("pack:",rxdataF_comp128+2);
                    
                    dl_ch128+=3;
                    dl_ch_mag128+=3;
                    dl_ch_mag128b+=3;
                    rxdataF128+=3;
                    rxdataF_comp128+=3;
                }
                else
                { // we have a smaller PDSCH in symbols with pilots so skip last group of 4 REs and increment less
                    dl_ch128+=2;
                    dl_ch_mag128+=2;
                    dl_ch_mag128b+=2;
                    rxdataF128+=2;
                    rxdataF_comp128+=2;
                }
            } // loop over resource blocks
        } // loop over rx antennas
    } // loop over tx antennas

    if (rho)
    {
        for (aarx=0; aarx < frame_parms->nb_antennas_rx; aarx++)
        {
            rho128     = (__m128i *)rho[aarx];
            dl_ch128   = (__m128i *)dl_ch_estimates_ext[aarx];
            dl_ch128_2 = (__m128i *)dl_ch_estimates_ext[2+aarx];

            for (rb=0; rb<nb_rb; rb++)
            {
                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[0],dl_ch128_2[0]);
                //  print_ints("re",&mmtmpD0);

                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[0],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
                //  print_ints("im",&mmtmpD1);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128_2[0]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                //  print_ints("re(shift)",&mmtmpD0);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                //  print_ints("im(shift)",&mmtmpD1);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
                //        print_ints("c0",&mmtmpD2);
                //  print_ints("c1",&mmtmpD3);
                rho128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);

                //print_shorts("rx:",dl_ch128_2);
                //print_shorts("ch:",dl_ch128);
                //print_shorts("pack:",rho128);

                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[1],dl_ch128_2[1]);
                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[1],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128_2[1]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

                rho128[1] =_mm_packs_epi32(mmtmpD2,mmtmpD3);
                //print_shorts("rx:",dl_ch128_2+1);
                //print_shorts("ch:",dl_ch128+1);
                //print_shorts("pack:",rho128+1);
                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[2],dl_ch128_2[2]);
                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[2],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128_2[2]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

                rho128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
                //print_shorts("rx:",dl_ch128_2+2);
                //print_shorts("ch:",dl_ch128+2);
                //print_shorts("pack:",rho128+2);

                dl_ch128+=3;
                dl_ch128_2+=3;
                rho128+=3;
            } // loop over resource blocks

            if (first_symbol_flag==1)
            {
                measurements->rx_correlation[0][aarx] = signal_energy(&rho[aarx][symbol*frame_parms->N_RB_DL*12],rb*12);
            }

        } // loop over rx antennas
    } // if rho

  _mm_empty();
  _m_empty();

#elif defined(__arm__)

  unsigned short rb;
  unsigned char aatx,aarx,symbol_mod,pilots=0;

  int16x4_t *dl_ch128,*dl_ch128_2,*rxdataF128;
  int32x4_t mmtmpD0,mmtmpD1,mmtmpD0b,mmtmpD1b;
  int16x8_t *dl_ch_mag128,*dl_ch_mag128b,mmtmpD2,mmtmpD3,mmtmpD4;
  int16x8_t QAM_amp128,QAM_amp128b;
  int16x4x2_t *rxdataF_comp128,*rho128;

  int16_t conj[4]__attribute__((aligned(16))) = {1,-1,1,-1};
  int32x4_t output_shift128 = vmovq_n_s32(-(int32_t)output_shift);

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp))) {
    if (frame_parms->mode1_flag==1) { // 10 out of 12 so don't reduce size
      nb_rb=1+(5*nb_rb/6);
    }
    else {
      pilots=1;
    }
  }

  for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++) {
    if (mod_order == 4) {
      QAM_amp128  = vmovq_n_s16(QAM16_n1);  // 2/sqrt(10)
      QAM_amp128b = vmovq_n_s16(0);
    } else if (mod_order == 6) {
      QAM_amp128  = vmovq_n_s16(QAM64_n1); //
      QAM_amp128b = vmovq_n_s16(QAM64_n2);
    }
    //    printf("comp: rxdataF_comp %p, symbol %d\n",rxdataF_comp[0],symbol);

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {
        dl_ch128          = (int16x4_t*)dl_ch_estimates_ext[(aatx<<1)+aarx];
        dl_ch_mag128      = (int16x8_t*)dl_ch_mag[(aatx<<1)+aarx];
        dl_ch_mag128b     = (int16x8_t*)dl_ch_magb[(aatx<<1)+aarx];
        rxdataF128        = (int16x4_t*)rxdataF_ext[aarx];
        rxdataF_comp128   = (int16x4x2_t*)rxdataF_comp[(aatx<<1)+aarx];

      for (rb=0; rb<nb_rb; rb++) {
  if (mod_order>2) {
    // get channel amplitude if not QPSK
    mmtmpD0 = vmull_s16(dl_ch128[0], dl_ch128[0]);
    // mmtmpD0 = [ch0*ch0,ch1*ch1,ch2*ch2,ch3*ch3];
    mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
    // mmtmpD0 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3]>>output_shift128 on 32-bits
    mmtmpD1 = vmull_s16(dl_ch128[1], dl_ch128[1]);
    mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
    mmtmpD2 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
    // mmtmpD2 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3,ch4*ch4 + ch5*ch5,ch4*ch4 + ch5*ch5,ch6*ch6 + ch7*ch7,ch6*ch6 + ch7*ch7]>>output_shift128 on 16-bits
    mmtmpD0 = vmull_s16(dl_ch128[2], dl_ch128[2]);
    mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
    mmtmpD1 = vmull_s16(dl_ch128[3], dl_ch128[3]);
    mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
    mmtmpD3 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
    if (pilots==0) {
      mmtmpD0 = vmull_s16(dl_ch128[4], dl_ch128[4]);
      mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
      mmtmpD1 = vmull_s16(dl_ch128[5], dl_ch128[5]);
      mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
      mmtmpD4 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
    }

    dl_ch_mag128b[0] = vqdmulhq_s16(mmtmpD2,QAM_amp128b);
    dl_ch_mag128b[1] = vqdmulhq_s16(mmtmpD3,QAM_amp128b);
    dl_ch_mag128[0] = vqdmulhq_s16(mmtmpD2,QAM_amp128);
    dl_ch_mag128[1] = vqdmulhq_s16(mmtmpD3,QAM_amp128);

    if (pilots==0) {
      dl_ch_mag128b[2] = vqdmulhq_s16(mmtmpD4,QAM_amp128b);
      dl_ch_mag128[2]  = vqdmulhq_s16(mmtmpD4,QAM_amp128);
    }
  }

  mmtmpD0 = vmull_s16(dl_ch128[0], rxdataF128[0]);
  //mmtmpD0 = [Re(ch[0])Re(rx[0]) Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1]) Im(ch[1])Im(ch[1])]
  mmtmpD1 = vmull_s16(dl_ch128[1], rxdataF128[1]);
  //mmtmpD1 = [Re(ch[2])Re(rx[2]) Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3]) Im(ch[3])Im(ch[3])]
  mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
  //mmtmpD0 = [Re(ch[0])Re(rx[0])+Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1])+Im(ch[1])Im(ch[1]) Re(ch[2])Re(rx[2])+Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3])+Im(ch[3])Im(ch[3])]

  mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[0],*(int16x4_t*)conj)), rxdataF128[0]);
  //mmtmpD0 = [-Im(ch[0])Re(rx[0]) Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1]) Re(ch[1])Im(rx[1])]
  mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[1],*(int16x4_t*)conj)), rxdataF128[1]);
  //mmtmpD0 = [-Im(ch[2])Re(rx[2]) Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3]) Re(ch[3])Im(rx[3])]
  mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));
  //mmtmpD1 = [-Im(ch[0])Re(rx[0])+Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1])+Re(ch[1])Im(rx[1]) -Im(ch[2])Re(rx[2])+Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3])+Re(ch[3])Im(rx[3])]

  mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
  mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
  rxdataF_comp128[0] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
  mmtmpD0 = vmull_s16(dl_ch128[2], rxdataF128[2]);
  mmtmpD1 = vmull_s16(dl_ch128[3], rxdataF128[3]);
  mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
  mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[2],*(int16x4_t*)conj)), rxdataF128[2]);
  mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[3],*(int16x4_t*)conj)), rxdataF128[3]);
  mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));
  mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
  mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
  rxdataF_comp128[1] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

  if (pilots==0) {
    mmtmpD0 = vmull_s16(dl_ch128[4], rxdataF128[4]);
    mmtmpD1 = vmull_s16(dl_ch128[5], rxdataF128[5]);
    mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
         vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

    mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[4],*(int16x4_t*)conj)), rxdataF128[4]);
    mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[5],*(int16x4_t*)conj)), rxdataF128[5]);
    mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
         vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));


    mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
    mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
    rxdataF_comp128[2] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


    dl_ch128+=6;
    dl_ch_mag128+=3;
    dl_ch_mag128b+=3;
    rxdataF128+=6;
    rxdataF_comp128+=3;

  } else { // we have a smaller PDSCH in symbols with pilots so skip last group of 4 REs and increment less
    dl_ch128+=4;
    dl_ch_mag128+=2;
    dl_ch_mag128b+=2;
    rxdataF128+=4;
    rxdataF_comp128+=2;
  }
      }
    }
  }

  if (rho) {
    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {
        rho128        = (int16x4x2_t*)rho[aarx];
        dl_ch128      = (int16x4_t*)dl_ch_estimates_ext[aarx];
        dl_ch128_2    = (int16x4_t*)dl_ch_estimates_ext[2+aarx];
      for (rb=0; rb<nb_rb; rb++) {
  mmtmpD0 = vmull_s16(dl_ch128[0], dl_ch128_2[0]);
  mmtmpD1 = vmull_s16(dl_ch128[1], dl_ch128_2[1]);
  mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
  mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[0],*(int16x4_t*)conj)), dl_ch128_2[0]);
  mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[1],*(int16x4_t*)conj)), dl_ch128_2[1]);
  mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

  mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
  mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
  rho128[0] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

  mmtmpD0 = vmull_s16(dl_ch128[2], dl_ch128_2[2]);
  mmtmpD1 = vmull_s16(dl_ch128[3], dl_ch128_2[3]);
  mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
  mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[2],*(int16x4_t*)conj)), dl_ch128_2[2]);
  mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[3],*(int16x4_t*)conj)), dl_ch128_2[3]);
  mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

  mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
  mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
  rho128[1] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

  mmtmpD0 = vmull_s16(dl_ch128[0], dl_ch128_2[0]);
  mmtmpD1 = vmull_s16(dl_ch128[1], dl_ch128_2[1]);
  mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
  mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[4],*(int16x4_t*)conj)), dl_ch128_2[4]);
  mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch128[5],*(int16x4_t*)conj)), dl_ch128_2[5]);
  mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

  mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
  mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
  rho128[2] = vzip_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


  dl_ch128+=6;
  dl_ch128_2+=6;
  rho128+=3;
      }

      if (first_symbol_flag==1) {
  measurements->rx_correlation[0][aarx] = signal_energy(&rho[aarx][symbol*frame_parms->N_RB_DL*12],rb*12);
      }
    }
  }
#endif
}

#if defined(__x86_64__) || defined(__i386__)

void prec2A_TM56_128(unsigned char pmi,__m128i *ch0,__m128i *ch1)
{

  __m128i amp;
  amp = _mm_set1_epi16(ONE_OVER_SQRT2_Q15);

  switch (pmi) {

  case 0 :   // +1 +1
    //    print_shorts("phase 0 :ch0",ch0);
    //    print_shorts("phase 0 :ch1",ch1);
    ch0[0] = _mm_adds_epi16(ch0[0],ch1[0]);
    break;

  case 1 :   // +1 -1
    //    print_shorts("phase 1 :ch0",ch0);
    //    print_shorts("phase 1 :ch1",ch1);
    ch0[0] = _mm_subs_epi16(ch0[0],ch1[0]);
    //    print_shorts("phase 1 :ch0-ch1",ch0);
    break;

  case 2 :   // +1 +j
    ch1[0] = _mm_sign_epi16(ch1[0],*(__m128i*)&conjugate[0]);
    ch1[0] = _mm_shufflelo_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch1[0] = _mm_shufflehi_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch0[0] = _mm_subs_epi16(ch0[0],ch1[0]);

    break;   // +1 -j

  case 3 :
    ch1[0] = _mm_sign_epi16(ch1[0],*(__m128i*)&conjugate[0]);
    ch1[0] = _mm_shufflelo_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch1[0] = _mm_shufflehi_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch0[0] = _mm_adds_epi16(ch0[0],ch1[0]);
    break;
  }

  ch0[0] = _mm_mulhi_epi16(ch0[0],amp);
  ch0[0] = _mm_slli_epi16(ch0[0],1);

  _mm_empty();
  _m_empty();
}
#elif defined(__arm__)
void prec2A_TM56_128(unsigned char pmi,__m128i *ch0,__m128i *ch1) {

  // sqrt(2) is already taken into account in computation sqrt_rho_a, sqrt_rho_b,
  //so removed it

  //__m128i amp;
  //amp = _mm_set1_epi16(ONE_OVER_SQRT2_Q15);

  switch (pmi) {

  case 0 :   // +1 +1
    //    print_shorts("phase 0 :ch0",ch0);
    //    print_shorts("phase 0 :ch1",ch1);
    ch0[0] = _mm_adds_epi16(ch0[0],ch1[0]);
    break;
  case 1 :   // +1 -1
    //    print_shorts("phase 1 :ch0",ch0);
    //    print_shorts("phase 1 :ch1",ch1);
    ch0[0] = _mm_subs_epi16(ch0[0],ch1[0]);
    //    print_shorts("phase 1 :ch0-ch1",ch0);
    break;
  case 2 :   // +1 +j
    ch1[0] = _mm_sign_epi16(ch1[0],*(__m128i*)&conjugate[0]);
    ch1[0] = _mm_shufflelo_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch1[0] = _mm_shufflehi_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch0[0] = _mm_subs_epi16(ch0[0],ch1[0]);

    break;   // +1 -j
  case 3 :
    ch1[0] = _mm_sign_epi16(ch1[0],*(__m128i*)&conjugate[0]);
    ch1[0] = _mm_shufflelo_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch1[0] = _mm_shufflehi_epi16(ch1[0],_MM_SHUFFLE(2,3,0,1));
    ch0[0] = _mm_adds_epi16(ch0[0],ch1[0]);
    break;
  }

  //ch0[0] = _mm_mulhi_epi16(ch0[0],amp);
  //ch0[0] = _mm_slli_epi16(ch0[0],1);

  _mm_empty();
  _m_empty();
}
#endif
// precoding is stream 0 .5(1,1)  .5(1,-1) .5(1,1)  .5(1,-1)
//              stream 1 .5(1,-1) .5(1,1)  .5(1,-1) .5(1,1)
// store "precoded" channel for stream 0 in ch0, stream 1 in ch1

short TM3_prec[8]__attribute__((aligned(16))) = {1,1,-1,-1,1,1,-1,-1} ;

void prec2A_TM3_128(__m128i *ch0,__m128i *ch1) {

  __m128i amp = _mm_set1_epi16(ONE_OVER_SQRT2_Q15);

  __m128i tmp0,tmp1;

  //_mm_mulhi_epi16
  //  print_shorts("prec2A_TM3 ch0 (before):",ch0);
  //  print_shorts("prec2A_TM3 ch1 (before):",ch1);

  tmp0 = ch0[0];
  tmp1  = _mm_sign_epi16(ch1[0],((__m128i*)&TM3_prec)[0]);
  //  print_shorts("prec2A_TM3 ch1*s (mid):",(__m128i*)TM3_prec);

  ch0[0] = _mm_adds_epi16(ch0[0],tmp1);
  ch1[0] = _mm_subs_epi16(tmp0,tmp1);

  ch0[0] = _mm_mulhi_epi16(ch0[0],amp);
  ch0[0] = _mm_slli_epi16(ch0[0],1);

  ch1[0] = _mm_mulhi_epi16(ch1[0],amp);
  ch1[0] = _mm_slli_epi16(ch1[0],1);

  //  print_shorts("prec2A_TM3 ch0 (mid):",&tmp0);
  //  print_shorts("prec2A_TM3 ch1 (mid):",ch1);

  //ch0[0] = _mm_mulhi_epi16(ch0[0],amp);
  //ch0[0] = _mm_slli_epi16(ch0[0],1);
  //ch1[0] = _mm_mulhi_epi16(ch1[0],amp);
  //ch1[0] = _mm_slli_epi16(ch1[0],1);

  //ch0[0] = _mm_srai_epi16(ch0[0],1);
  //ch1[0] = _mm_srai_epi16(ch1[0],1);

  //  print_shorts("prec2A_TM3 ch0 (after):",ch0);
  //  print_shorts("prec2A_TM3 ch1 (after):",ch1);

  _mm_empty();
  _m_empty();
}

// pmi = 0 => stream 0 (1,1), stream 1 (1,-1)
// pmi = 1 => stream 0 (1,j), stream 2 (1,-j)

void prec2A_TM4_128(int pmi,__m128i *ch0,__m128i *ch1) {

// sqrt(2) is already taken into account in computation sqrt_rho_a, sqrt_rho_b,
//so divide by 2 is replaced by divide by sqrt(2).

 // printf ("demod pmi=%d\n", pmi);
 __m128i amp;
 amp = _mm_set1_epi16(ONE_OVER_SQRT2_Q15);
  __m128i tmp0,tmp1;

 // print_shorts("prec2A_TM4 ch0 (before):",ch0);
 // print_shorts("prec2A_TM4 ch1 (before):",ch1);

  if (pmi == 0) { //[1 1;1 -1]
    tmp0 = ch0[0];
    tmp1 = ch1[0];
    ch0[0] = _mm_adds_epi16(tmp0,tmp1);
    ch1[0] = _mm_subs_epi16(tmp0,tmp1);
  }
  else { //ch0+j*ch1 ch0-j*ch1
    tmp0 = ch0[0];
    tmp1   = _mm_sign_epi16(ch1[0],*(__m128i*)&conjugate[0]);
    tmp1   = _mm_shufflelo_epi16(tmp1,_MM_SHUFFLE(2,3,0,1));
    tmp1   = _mm_shufflehi_epi16(tmp1,_MM_SHUFFLE(2,3,0,1));
    ch0[0] = _mm_subs_epi16(tmp0,tmp1);
    ch1[0] = _mm_add_epi16(tmp0,tmp1);
  }

  //print_shorts("prec2A_TM4 ch0 (middle):",ch0);
  //print_shorts("prec2A_TM4 ch1 (middle):",ch1);

  ch0[0] = _mm_mulhi_epi16(ch0[0],amp);
  ch0[0] = _mm_slli_epi16(ch0[0],1);
  ch1[0] = _mm_mulhi_epi16(ch1[0],amp);
  ch1[0] = _mm_slli_epi16(ch1[0],1);


 // ch0[0] = _mm_srai_epi16(ch0[0],1); //divide by 2
 // ch1[0] = _mm_srai_epi16(ch1[0],1); //divide by 2
  //print_shorts("prec2A_TM4 ch0 (end):",ch0);
  //print_shorts("prec2A_TM4 ch1 (end):",ch1);
  _mm_empty();
  _m_empty();
 // print_shorts("prec2A_TM4 ch0 (end):",ch0);
  //print_shorts("prec2A_TM4 ch1 (end):",ch1);
}

void dlsch_channel_compensation_TM56(int **rxdataF_ext,
                                     int **dl_ch_estimates_ext,
                                     int **dl_ch_mag,
                                     int **dl_ch_magb,
                                     int **rxdataF_comp,
                                     unsigned char *pmi_ext,
                                     LTE_DL_FRAME_PARMS *frame_parms,
                                     PHY_MEASUREMENTS *measurements,
                                     int eNB_id,
                                     unsigned char symbol,
                                     unsigned char mod_order,
                                     unsigned short nb_rb,
                                     unsigned char output_shift,
                                     unsigned char dl_power_off)
{

#if defined(__x86_64__) || defined(__i386__)

    unsigned short rb,Nre;
    __m128i *dl_ch0_128,*dl_ch1_128,*dl_ch_mag128,*dl_ch_mag128b,*rxdataF128,*rxdataF_comp128;
    unsigned char aarx=0,symbol_mod,pilots=0;
    int precoded_signal_strength=0;
    __m128i mmtmpD0,mmtmpD1,mmtmpD2,mmtmpD3,QAM_amp128,QAM_amp128b;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
    {
        pilots=1;
    }


    //printf("comp prec: symbol %d, pilots %d\n",symbol, pilots);

    if (mod_order == 4)
    {
        QAM_amp128 = _mm_set1_epi16(QAM16_n1);
        QAM_amp128b = _mm_setzero_si128();
    }
    else if (mod_order == 6)
    {
        QAM_amp128  = _mm_set1_epi16(QAM64_n1);
        QAM_amp128b = _mm_set1_epi16(QAM64_n2);
    }

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
    {
        dl_ch0_128 = (__m128i *)dl_ch_estimates_ext[aarx];
        dl_ch1_128 = (__m128i *)dl_ch_estimates_ext[2+aarx];

        dl_ch_mag128    = (__m128i *)dl_ch_mag   [aarx];
        dl_ch_mag128b   = (__m128i *)dl_ch_magb  [aarx];
        rxdataF128      = (__m128i *)rxdataF_ext [aarx];
        rxdataF_comp128 = (__m128i *)rxdataF_comp[aarx];

        for (rb=0; rb<nb_rb; rb++)
        {
            // combine TX channels using precoder from pmi
#ifdef DEBUG_DLSCH_DEMOD
            printf("mode 6 prec: rb %d, pmi->%d\n",rb,pmi_ext[rb]);
#endif
            // Apply precoder
            prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128[0],&dl_ch1_128[0]);
            prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128[1],&dl_ch1_128[1]);

            if (pilots==0)
            {
                prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128[2],&dl_ch1_128[2]);
            }
        }

        if (mod_order>2)
        {
            // get channel amplitude if not QPSK
            mmtmpD0 = _mm_madd_epi16(dl_ch0_128[0],dl_ch0_128[0]);
            mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);

            mmtmpD1 = _mm_madd_epi16(dl_ch0_128[1],dl_ch0_128[1]);
            mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);

            mmtmpD0 = _mm_packs_epi32(mmtmpD0,mmtmpD1);

            dl_ch_mag128[0] = _mm_unpacklo_epi16(mmtmpD0,mmtmpD0);
            dl_ch_mag128b[0] = dl_ch_mag128[0];
            dl_ch_mag128[0] = _mm_mulhi_epi16(dl_ch_mag128[0],QAM_amp128);
            dl_ch_mag128[0] = _mm_slli_epi16(dl_ch_mag128[0],1);

            //print_shorts("dl_ch_mag128[0]=",&dl_ch_mag128[0]);
            //print_shorts("dl_ch_mag128[0]=",&dl_ch_mag128[0]);

            dl_ch_mag128[1] = _mm_unpackhi_epi16(mmtmpD0,mmtmpD0);
            dl_ch_mag128b[1] = dl_ch_mag128[1];
            dl_ch_mag128[1] = _mm_mulhi_epi16(dl_ch_mag128[1],QAM_amp128);
            dl_ch_mag128[1] = _mm_slli_epi16(dl_ch_mag128[1],1);

            if (pilots==0)
            {
                mmtmpD0 = _mm_madd_epi16(dl_ch0_128[2],dl_ch0_128[2]);
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);

                mmtmpD1 = _mm_packs_epi32(mmtmpD0,mmtmpD0);

                dl_ch_mag128[2] = _mm_unpacklo_epi16(mmtmpD1,mmtmpD1);
                dl_ch_mag128b[2] = dl_ch_mag128[2];

                dl_ch_mag128[2] = _mm_mulhi_epi16(dl_ch_mag128[2],QAM_amp128);
                dl_ch_mag128[2] = _mm_slli_epi16(dl_ch_mag128[2],1);
            }

            dl_ch_mag128b[0] = _mm_mulhi_epi16(dl_ch_mag128b[0],QAM_amp128b);
            dl_ch_mag128b[0] = _mm_slli_epi16(dl_ch_mag128b[0],1);

            //print_shorts("dl_ch_mag128b[0]=",&dl_ch_mag128b[0]);

            dl_ch_mag128b[1] = _mm_mulhi_epi16(dl_ch_mag128b[1],QAM_amp128b);
            dl_ch_mag128b[1] = _mm_slli_epi16(dl_ch_mag128b[1],1);

            if (pilots==0)
            {
                dl_ch_mag128b[2] = _mm_mulhi_epi16(dl_ch_mag128b[2],QAM_amp128b);
                dl_ch_mag128b[2] = _mm_slli_epi16(dl_ch_mag128b[2],1);
            }
        }

        // MF multiply by conjugated channel
        mmtmpD0 = _mm_madd_epi16(dl_ch0_128[0],rxdataF128[0]);
        //        print_ints("re",&mmtmpD0);

        // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
        mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[0],_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);

        //        print_ints("im",&mmtmpD1);
        mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[0]);
        // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
        mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
        //        print_ints("re(shift)",&mmtmpD0);
        mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
        //        print_ints("im(shift)",&mmtmpD1);
        mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
        mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
        //        print_ints("c0",&mmtmpD2);
        //        print_ints("c1",&mmtmpD3);
        rxdataF_comp128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
        //        print_shorts("rx:",rxdataF128);
        //        print_shorts("ch:",dl_ch128);
        //        print_shorts("pack:",rxdataF_comp128);

        // multiply by conjugated channel
        mmtmpD0 = _mm_madd_epi16(dl_ch0_128[1],rxdataF128[1]);
        // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
        mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[1],_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
        mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[1]);
        // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
        mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
        mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
        mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
        mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

        rxdataF_comp128[1] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
        //  print_shorts("rx:",rxdataF128+1);
        //  print_shorts("ch:",dl_ch128+1);
        //  print_shorts("pack:",rxdataF_comp128+1);

        if (pilots==0)
        {
            // multiply by conjugated channel
            mmtmpD0 = _mm_madd_epi16(dl_ch0_128[2],rxdataF128[2]);
            // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
            mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[2],_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
            mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[2]);
            // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
            mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
            mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
            mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
            mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

            rxdataF_comp128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
            //  print_shorts("rx:",rxdataF128+2);
            //  print_shorts("ch:",dl_ch128+2);
            //        print_shorts("pack:",rxdataF_comp128+2);

            dl_ch0_128+=3;
            dl_ch1_128+=3;
            dl_ch_mag128+=3;
            dl_ch_mag128b+=3;
            rxdataF128+=3;
            rxdataF_comp128+=3;
        }
        else
        {
            dl_ch0_128+=2;
            dl_ch1_128+=2;
            dl_ch_mag128+=2;
            dl_ch_mag128b+=2;
            rxdataF128+=2;
            rxdataF_comp128+=2;
        }

        Nre = (pilots==0) ? 12 : 8;

        precoded_signal_strength += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*Nre],
                                                         (nb_rb*Nre))) - (measurements->n0_power[aarx]));
    } // loop over rx antennas

    measurements->precoded_cqi_dB[eNB_id][0] = dB_fixed2(precoded_signal_strength,measurements->n0_power_tot);

    //printf("eNB_id %d, symbol %d: precoded CQI %d dB\n",eNB_id,symbol,
    //   measurements->precoded_cqi_dB[eNB_id][0]);

#elif defined(__arm__)

  uint32_t rb,Nre;
  uint32_t aarx,symbol_mod,pilots=0;

  int16x4_t *dl_ch0_128,*dl_ch1_128,*rxdataF128;
  int16x8_t *dl_ch0_128b,*dl_ch1_128b;
  int32x4_t mmtmpD0,mmtmpD1,mmtmpD0b,mmtmpD1b;
  int16x8_t *dl_ch_mag128,*dl_ch_mag128b,mmtmpD2,mmtmpD3,mmtmpD4,*rxdataF_comp128;
  int16x8_t QAM_amp128,QAM_amp128b;

  int16_t conj[4]__attribute__((aligned(16))) = {1,-1,1,-1};
  int32x4_t output_shift128 = vmovq_n_s32(-(int32_t)output_shift);
  int32_t precoded_signal_strength=0;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp))) {
    if (frame_parms->mode1_flag==1) // 10 out of 12 so don't reduce size
      { nb_rb=1+(5*nb_rb/6); }

    else
      { pilots=1; }
  }


  if (mod_order == 4) {
    QAM_amp128  = vmovq_n_s16(QAM16_n1);  // 2/sqrt(10)
    QAM_amp128b = vmovq_n_s16(0);

  } else if (mod_order == 6) {
    QAM_amp128  = vmovq_n_s16(QAM64_n1); //
    QAM_amp128b = vmovq_n_s16(QAM64_n2);
  }

  //    printf("comp: rxdataF_comp %p, symbol %d\n",rxdataF_comp[0],symbol);

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {



      dl_ch0_128          = (int16x4_t*)&dl_ch_estimates_ext[aarx];
      dl_ch1_128          = (int16x4_t*)&dl_ch_estimates_ext[2+aarx];
      dl_ch0_128b         = (int16x8_t*)&dl_ch_estimates_ext[aarx];
      dl_ch1_128b         = (int16x8_t*)&dl_ch_estimates_ext[2+aarx];
      dl_ch_mag128        = (int16x8_t*)&dl_ch_mag[aarx];
      dl_ch_mag128b       = (int16x8_t*)&dl_ch_magb[aarx];
      rxdataF128          = (int16x4_t*)&rxdataF_ext[aarx];
      rxdataF_comp128     = (int16x8_t*)&rxdataF_comp[aarx];

    for (rb=0; rb<nb_rb; rb++) {
#ifdef DEBUG_DLSCH_DEMOD
      printf("mode 6 prec: rb %d, pmi->%d\n",rb,pmi_ext[rb]);
#endif
      prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128b[0],&dl_ch1_128b[0]);
      prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128b[1],&dl_ch1_128b[1]);

      if (pilots==0) {
        prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128b[2],&dl_ch1_128b[2]);
      }

      if (mod_order>2) {
        // get channel amplitude if not QPSK
        mmtmpD0 = vmull_s16(dl_ch0_128[0], dl_ch0_128[0]);
        // mmtmpD0 = [ch0*ch0,ch1*ch1,ch2*ch2,ch3*ch3];
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        // mmtmpD0 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3]>>output_shift128 on 32-bits
        mmtmpD1 = vmull_s16(dl_ch0_128[1], dl_ch0_128[1]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD2 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
        // mmtmpD2 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3,ch4*ch4 + ch5*ch5,ch4*ch4 + ch5*ch5,ch6*ch6 + ch7*ch7,ch6*ch6 + ch7*ch7]>>output_shift128 on 16-bits
        mmtmpD0 = vmull_s16(dl_ch0_128[2], dl_ch0_128[2]);
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        mmtmpD1 = vmull_s16(dl_ch0_128[3], dl_ch0_128[3]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD3 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
        if (pilots==0) {
          mmtmpD0 = vmull_s16(dl_ch0_128[4], dl_ch0_128[4]);
          mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
          mmtmpD1 = vmull_s16(dl_ch0_128[5], dl_ch0_128[5]);
          mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
          mmtmpD4 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


        }

        dl_ch_mag128b[0] = vqdmulhq_s16(mmtmpD2,QAM_amp128b);
        dl_ch_mag128b[1] = vqdmulhq_s16(mmtmpD3,QAM_amp128b);
        dl_ch_mag128[0] = vqdmulhq_s16(mmtmpD2,QAM_amp128);
        dl_ch_mag128[1] = vqdmulhq_s16(mmtmpD3,QAM_amp128);


        if (pilots==0) {
          dl_ch_mag128b[2] = vqdmulhq_s16(mmtmpD4,QAM_amp128b);
          dl_ch_mag128[2]  = vqdmulhq_s16(mmtmpD4,QAM_amp128);
        }
      }
      mmtmpD0 = vmull_s16(dl_ch0_128[0], rxdataF128[0]);
      //mmtmpD0 = [Re(ch[0])Re(rx[0]) Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1]) Im(ch[1])Im(ch[1])]
      mmtmpD1 = vmull_s16(dl_ch0_128[1], rxdataF128[1]);
      //mmtmpD1 = [Re(ch[2])Re(rx[2]) Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3]) Im(ch[3])Im(ch[3])]
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
      //mmtmpD0 = [Re(ch[0])Re(rx[0])+Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1])+Im(ch[1])Im(ch[1]) Re(ch[2])Re(rx[2])+Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3])+Im(ch[3])Im(ch[3])]

      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[0],*(int16x4_t*)conj)), rxdataF128[0]);
      //mmtmpD0 = [-Im(ch[0])Re(rx[0]) Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1]) Re(ch[1])Im(rx[1])]
      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[1],*(int16x4_t*)conj)), rxdataF128[1]);
      //mmtmpD0 = [-Im(ch[2])Re(rx[2]) Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3]) Re(ch[3])Im(rx[3])]
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));
      //mmtmpD1 = [-Im(ch[0])Re(rx[0])+Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1])+Re(ch[1])Im(rx[1]) -Im(ch[2])Re(rx[2])+Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3])+Re(ch[3])Im(rx[3])]

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp128[0] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      mmtmpD0 = vmull_s16(dl_ch0_128[2], rxdataF128[2]);
      mmtmpD1 = vmull_s16(dl_ch0_128[3], rxdataF128[3]);
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[2],*(int16x4_t*)conj)), rxdataF128[2]);
      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[3],*(int16x4_t*)conj)), rxdataF128[3]);
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp128[1] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      if (pilots==0) {
        mmtmpD0 = vmull_s16(dl_ch0_128[4], rxdataF128[4]);
        mmtmpD1 = vmull_s16(dl_ch0_128[5], rxdataF128[5]);
        mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                               vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

        mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[4],*(int16x4_t*)conj)), rxdataF128[4]);
        mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[5],*(int16x4_t*)conj)), rxdataF128[5]);
        mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                               vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));


        mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
        mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
        rxdataF_comp128[2] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


        dl_ch0_128+=6;
        dl_ch1_128+=6;
        dl_ch_mag128+=3;
        dl_ch_mag128b+=3;
        rxdataF128+=6;
        rxdataF_comp128+=3;

      } else { // we have a smaller PDSCH in symbols with pilots so skip last group of 4 REs and increment less
        dl_ch0_128+=4;
        dl_ch1_128+=4;
        dl_ch_mag128+=2;
        dl_ch_mag128b+=2;
        rxdataF128+=4;
        rxdataF_comp128+=2;
      }
    }

    Nre = (pilots==0) ? 12 : 8;


    precoded_signal_strength += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*Nre],

                                                     (nb_rb*Nre))) - (measurements->n0_power[aarx]));
    // rx_antennas
  }
  measurements->precoded_cqi_dB[eNB_id][0] = dB_fixed2(precoded_signal_strength,measurements->n0_power_tot);

  //printf("eNB_id %d, symbol %d: precoded CQI %d dB\n",eNB_id,symbol,
  //     measurements->precoded_cqi_dB[eNB_id][0]);

#endif
  _mm_empty();
  _m_empty();
}

void dlsch_channel_compensation_TM34(LTE_DL_FRAME_PARMS *frame_parms,
                                    LTE_UE_PDSCH *pdsch_vars,
                                    PHY_MEASUREMENTS *measurements,
                                    int eNB_id,
                                    unsigned char symbol,
                                    unsigned char mod_order0,
                                    unsigned char mod_order1,
                                    int harq_pid,
                                    int round,
                                    MIMO_mode_t mimo_mode,
                                    unsigned short nb_rb,
                                    unsigned char output_shift0,
                                    unsigned char output_shift1) {

#if defined(__x86_64__) || defined(__i386__)

  unsigned short rb,Nre;
  __m128i *dl_ch0_128,*dl_ch1_128,*dl_ch_mag0_128,*dl_ch_mag1_128,*dl_ch_mag0_128b,*dl_ch_mag1_128b,*rxdataF128,*rxdataF_comp0_128,*rxdataF_comp1_128;
  unsigned char aarx=0,symbol_mod,pilots=0;
  int precoded_signal_strength0=0,precoded_signal_strength1=0;
  int rx_power_correction;

  int **rxdataF_ext           = pdsch_vars->rxdataF_ext;
  int **dl_ch_estimates_ext   = pdsch_vars->dl_ch_estimates_ext;
  int **dl_ch_mag0            = pdsch_vars->dl_ch_mag0;
  int **dl_ch_mag1            = pdsch_vars->dl_ch_mag1[harq_pid][round];
  int **dl_ch_magb0           = pdsch_vars->dl_ch_magb0;
  int **dl_ch_magb1           = pdsch_vars->dl_ch_magb1[harq_pid][round];
  int **rxdataF_comp0         = pdsch_vars->rxdataF_comp0;
  int **rxdataF_comp1         = pdsch_vars->rxdataF_comp1[harq_pid][round];
  unsigned char *pmi_ext      = pdsch_vars->pmi_ext;
  __m128i mmtmpD0,mmtmpD1,mmtmpD2,mmtmpD3,QAM_amp0_128,QAM_amp0_128b,QAM_amp1_128,QAM_amp1_128b;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
  {
      pilots=1;
  }

  rx_power_correction = 1;

 // printf("comp prec: symbol %d, pilots %d\n",symbol, pilots);

  if (mod_order0 == 4)
  {
      QAM_amp0_128  = _mm_set1_epi16(QAM16_n1);
      QAM_amp0_128b = _mm_setzero_si128();
  }
  else if (mod_order0 == 6)
  {
      QAM_amp0_128  = _mm_set1_epi16(QAM64_n1);
      QAM_amp0_128b = _mm_set1_epi16(QAM64_n2);
  }

  if (mod_order1 == 4)
  {
      QAM_amp1_128  = _mm_set1_epi16(QAM16_n1);
      QAM_amp1_128b = _mm_setzero_si128();
  }
  else if (mod_order1 == 6)
  {
      QAM_amp1_128  = _mm_set1_epi16(QAM64_n1);
      QAM_amp1_128b = _mm_set1_epi16(QAM64_n2);
  }

  for (aarx=0; aarx < frame_parms->nb_antennas_rx; aarx++)
  {
      /* if (aarx==0) {
         output_shift=output_shift0;
         }
         else {
         output_shift=output_shift1;
         } */

     // printf("antenna %d\n", aarx);
      // printf("symbol %d, rx antenna %d\n", symbol, aarx);

      dl_ch0_128 = (__m128i *)dl_ch_estimates_ext[aarx];   // this is h11
      dl_ch1_128 = (__m128i *)dl_ch_estimates_ext[2+aarx]; // this is h12

      dl_ch_mag0_128    = (__m128i *)dl_ch_mag0[aarx];    //responsible for x1
      dl_ch_mag0_128b   = (__m128i *)dl_ch_magb0[aarx];   //responsible for x1
      dl_ch_mag1_128    = (__m128i *)dl_ch_mag1[aarx];    //responsible for x2. always coming from tx2
      dl_ch_mag1_128b   = (__m128i *)dl_ch_magb1[aarx];   //responsible for x2. always coming from tx2
      rxdataF128        = (__m128i *)rxdataF_ext[aarx];   //received signal on antenna of interest h11*x1+h12*x2
      rxdataF_comp0_128 = (__m128i *)rxdataF_comp0[aarx]; //result of multipl with MF x1 on antenna of interest
      rxdataF_comp1_128 = (__m128i *)rxdataF_comp1[aarx]; //result of multipl with MF x2 on antenna of interest

      for (rb=0; rb < nb_rb; rb++)
      {
          // combine TX channels using precoder from pmi
          if (mimo_mode==LARGE_CDD)
          {
              prec2A_TM3_128(&dl_ch0_128[0],&dl_ch1_128[0]);
              prec2A_TM3_128(&dl_ch0_128[1],&dl_ch1_128[1]);

              if (pilots==0)
              {
                  prec2A_TM3_128(&dl_ch0_128[2],&dl_ch1_128[2]);
              }
          }
          else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
          {
              prec2A_TM4_128(0,&dl_ch0_128[0],&dl_ch1_128[0]);
              prec2A_TM4_128(0,&dl_ch0_128[1],&dl_ch1_128[1]);

              if (pilots==0)
              {
                  prec2A_TM4_128(0,&dl_ch0_128[2],&dl_ch1_128[2]);
              }
          }
          else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
          {
              prec2A_TM4_128(1,&dl_ch0_128[0],&dl_ch1_128[0]);
              prec2A_TM4_128(1,&dl_ch0_128[1],&dl_ch1_128[1]);

              if (pilots==0)
              {
                  prec2A_TM4_128(1,&dl_ch0_128[2],&dl_ch1_128[2]);
              }
          }
          else if (mimo_mode==DUALSTREAM_PUSCH_PRECODING)
          {
              prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128[0],&dl_ch1_128[0]);
              prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128[1],&dl_ch1_128[1]);

              if (pilots==0)
              {
                  prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128[2],&dl_ch1_128[2]);
              }
          }
          else
          {
              LOG_E(PHY,"Unknown MIMO mode\n");
              return;
          }

          if (mod_order0>2)
          {
              // get channel amplitude if not QPSK
              mmtmpD0 = _mm_madd_epi16(dl_ch0_128[0],dl_ch0_128[0]);
              mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift0);

              mmtmpD1 = _mm_madd_epi16(dl_ch0_128[1],dl_ch0_128[1]);
              mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift0);

              mmtmpD0 = _mm_packs_epi32(mmtmpD0,mmtmpD1);

              dl_ch_mag0_128[0] = _mm_unpacklo_epi16(mmtmpD0,mmtmpD0);
              dl_ch_mag0_128b[0] = dl_ch_mag0_128[0];
              dl_ch_mag0_128[0] = _mm_mulhi_epi16(dl_ch_mag0_128[0],QAM_amp0_128);
              dl_ch_mag0_128[0] = _mm_slli_epi16(dl_ch_mag0_128[0],1);

              //  print_shorts("dl_ch_mag0_128[0]=",&dl_ch_mag0_128[0]);

              dl_ch_mag0_128[1] = _mm_unpackhi_epi16(mmtmpD0,mmtmpD0);
              dl_ch_mag0_128b[1] = dl_ch_mag0_128[1];
              dl_ch_mag0_128[1] = _mm_mulhi_epi16(dl_ch_mag0_128[1],QAM_amp0_128);
              dl_ch_mag0_128[1] = _mm_slli_epi16(dl_ch_mag0_128[1],1);

              if (pilots==0)
              {
                  mmtmpD0 = _mm_madd_epi16(dl_ch0_128[2],dl_ch0_128[2]);
                  mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift0);

                  mmtmpD1 = _mm_packs_epi32(mmtmpD0,mmtmpD0);

                  dl_ch_mag0_128[2] = _mm_unpacklo_epi16(mmtmpD1,mmtmpD1);
                  dl_ch_mag0_128b[2] = dl_ch_mag0_128[2];

                  dl_ch_mag0_128[2] = _mm_mulhi_epi16(dl_ch_mag0_128[2],QAM_amp0_128);
                  dl_ch_mag0_128[2] = _mm_slli_epi16(dl_ch_mag0_128[2],1);
              }

              dl_ch_mag0_128b[0] = _mm_mulhi_epi16(dl_ch_mag0_128b[0],QAM_amp0_128b);
              dl_ch_mag0_128b[0] = _mm_slli_epi16(dl_ch_mag0_128b[0],1);

              // print_shorts("dl_ch_mag0_128b[0]=",&dl_ch_mag0_128b[0]);

              dl_ch_mag0_128b[1] = _mm_mulhi_epi16(dl_ch_mag0_128b[1],QAM_amp0_128b);
              dl_ch_mag0_128b[1] = _mm_slli_epi16(dl_ch_mag0_128b[1],1);

              if (pilots==0)
              {
                  dl_ch_mag0_128b[2] = _mm_mulhi_epi16(dl_ch_mag0_128b[2],QAM_amp0_128b);
                  dl_ch_mag0_128b[2] = _mm_slli_epi16(dl_ch_mag0_128b[2],1);
              }
          }

          if (mod_order1>2)
          {
              // get channel amplitude if not QPSK

              mmtmpD0 = _mm_madd_epi16(dl_ch1_128[0],dl_ch1_128[0]);
              mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift1);

              mmtmpD1 = _mm_madd_epi16(dl_ch1_128[1],dl_ch1_128[1]);
              mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift1);

              mmtmpD0 = _mm_packs_epi32(mmtmpD0,mmtmpD1);

              dl_ch_mag1_128[0] = _mm_unpacklo_epi16(mmtmpD0,mmtmpD0);
              dl_ch_mag1_128b[0] = dl_ch_mag1_128[0];
              dl_ch_mag1_128[0] = _mm_mulhi_epi16(dl_ch_mag1_128[0],QAM_amp1_128);
              dl_ch_mag1_128[0] = _mm_slli_epi16(dl_ch_mag1_128[0],1);

              // print_shorts("dl_ch_mag1_128[0]=",&dl_ch_mag1_128[0]);

              dl_ch_mag1_128[1] = _mm_unpackhi_epi16(mmtmpD0,mmtmpD0);
              dl_ch_mag1_128b[1] = dl_ch_mag1_128[1];
              dl_ch_mag1_128[1] = _mm_mulhi_epi16(dl_ch_mag1_128[1],QAM_amp1_128);
              dl_ch_mag1_128[1] = _mm_slli_epi16(dl_ch_mag1_128[1],1);

              if (pilots==0)
              {
                  mmtmpD0 = _mm_madd_epi16(dl_ch1_128[2],dl_ch1_128[2]);
                  mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift1);

                  mmtmpD1 = _mm_packs_epi32(mmtmpD0,mmtmpD0);

                  dl_ch_mag1_128[2] = _mm_unpacklo_epi16(mmtmpD1,mmtmpD1);
                  dl_ch_mag1_128b[2] = dl_ch_mag1_128[2];

                  dl_ch_mag1_128[2] = _mm_mulhi_epi16(dl_ch_mag1_128[2],QAM_amp1_128);
                  dl_ch_mag1_128[2] = _mm_slli_epi16(dl_ch_mag1_128[2],1);
              }

              dl_ch_mag1_128b[0] = _mm_mulhi_epi16(dl_ch_mag1_128b[0],QAM_amp1_128b);
              dl_ch_mag1_128b[0] = _mm_slli_epi16(dl_ch_mag1_128b[0],1);

              // print_shorts("dl_ch_mag1_128b[0]=",&dl_ch_mag1_128b[0]);

              dl_ch_mag1_128b[1] = _mm_mulhi_epi16(dl_ch_mag1_128b[1],QAM_amp1_128b);
              dl_ch_mag1_128b[1] = _mm_slli_epi16(dl_ch_mag1_128b[1],1);

              if (pilots==0)
              {
                  dl_ch_mag1_128b[2] = _mm_mulhi_epi16(dl_ch_mag1_128b[2],QAM_amp1_128b);
                  dl_ch_mag1_128b[2] = _mm_slli_epi16(dl_ch_mag1_128b[2],1);
              }
          }

          // layer 0
          // MF multiply by conjugated channel
          mmtmpD0 = _mm_madd_epi16(dl_ch0_128[0],rxdataF128[0]);
          //  print_ints("re",&mmtmpD0);

          // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
          mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[0],_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
          mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[0]);
          // print_ints("im",&mmtmpD1);
          // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
          mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift0);
          // printf("Shift: %d\n",output_shift);
          // print_ints("re(shift)",&mmtmpD0);
          mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift0);
          // print_ints("im(shift)",&mmtmpD1);
          mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
          mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
          //  print_ints("c0",&mmtmpD2);
          // print_ints("c1",&mmtmpD3);
          rxdataF_comp0_128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);

          // print_shorts("rx:",rxdataF128);
          // print_shorts("ch:",dl_ch0_128);
          // print_shorts("pack:",rxdataF_comp0_128);

          // multiply by conjugated channel
          mmtmpD0 = _mm_madd_epi16(dl_ch0_128[1],rxdataF128[1]);
          // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
          mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[1],_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
          mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[1]);
          // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
          mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift0);
          mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift0);
          mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
          mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

          rxdataF_comp0_128[1] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
          //  print_shorts("rx:",rxdataF128+1);
          //  print_shorts("ch:",dl_ch0_128+1);
          // print_shorts("pack:",rxdataF_comp0_128+1);

          if (pilots==0)
          {
              // multiply by conjugated channel
              mmtmpD0 = _mm_madd_epi16(dl_ch0_128[2],rxdataF128[2]);
              // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
              mmtmpD1 = _mm_shufflelo_epi16(dl_ch0_128[2],_MM_SHUFFLE(2,3,0,1));
              mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
              mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
              mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[2]);
              // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
              mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift0);
              mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift0);
              mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
              mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

              rxdataF_comp0_128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
              //   print_shorts("rx:",rxdataF128+2);
              //   print_shorts("ch:",dl_ch0_128+2);
              //  print_shorts("pack:",rxdataF_comp0_128+2);
          }

          // layer 1
          // MF multiply by conjugated channel
          mmtmpD0 = _mm_madd_epi16(dl_ch1_128[0],rxdataF128[0]);
           //  print_ints("re",&mmtmpD0);

          // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
          mmtmpD1 = _mm_shufflelo_epi16(dl_ch1_128[0],_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
          //  print_ints("im",&mmtmpD1);
          mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[0]);
          // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
          mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift1);
          // print_ints("re(shift)",&mmtmpD0);
          mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift1);
          // print_ints("im(shift)",&mmtmpD1);
          mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
          mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
          // print_ints("c0",&mmtmpD2);
          // print_ints("c1",&mmtmpD3);
          rxdataF_comp1_128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
          // print_shorts("rx:",rxdataF128);
          //  print_shorts("ch:",dl_ch1_128);
          // print_shorts("pack:",rxdataF_comp1_128);

          // multiply by conjugated channel
          mmtmpD0 = _mm_madd_epi16(dl_ch1_128[1],rxdataF128[1]);
          // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
          mmtmpD1 = _mm_shufflelo_epi16(dl_ch1_128[1],_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
          mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
          mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[1]);
          // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
          mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift1);
          mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift1);
          mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
          mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

          rxdataF_comp1_128[1] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
          //  print_shorts("rx:",rxdataF128+1);
          // print_shorts("ch:",dl_ch1_128+1);
          // print_shorts("pack:",rxdataF_comp1_128+1);

          if (pilots==0)
          {
              // multiply by conjugated channel
              mmtmpD0 = _mm_madd_epi16(dl_ch1_128[2],rxdataF128[2]);
              // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
              mmtmpD1 = _mm_shufflelo_epi16(dl_ch1_128[2],_MM_SHUFFLE(2,3,0,1));
              mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
              mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
              mmtmpD1 = _mm_madd_epi16(mmtmpD1,rxdataF128[2]);
              // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
              mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift1);
              mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift1);
              mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
              mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);

              rxdataF_comp1_128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
              //   print_shorts("rx:",rxdataF128+2);
              //  print_shorts("ch:",dl_ch1_128+2);
              //         print_shorts("pack:",rxdataF_comp1_128+2);

              dl_ch0_128+=3;
              dl_ch1_128+=3;
              dl_ch_mag0_128+=3;
              dl_ch_mag1_128+=3;
              dl_ch_mag0_128b+=3;
              dl_ch_mag1_128b+=3;
              rxdataF128+=3;
              rxdataF_comp0_128+=3;
              rxdataF_comp1_128+=3;
          }
          else
          {
              dl_ch0_128+=2;
              dl_ch1_128+=2;
              dl_ch_mag0_128+=2;
              dl_ch_mag1_128+=2;
              dl_ch_mag0_128b+=2;
              dl_ch_mag1_128b+=2;
              rxdataF128+=2;
              rxdataF_comp0_128+=2;
              rxdataF_comp1_128+=2;
          }
      } // rb loop
      Nre = (pilots==0) ? 12 : 8;

      precoded_signal_strength0 += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*Nre],
                                                        (nb_rb*Nre))*rx_power_correction) - (measurements->n0_power[aarx]));

      precoded_signal_strength1 += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx+2][symbol*frame_parms->N_RB_DL*Nre],
                                                        (nb_rb*Nre))*rx_power_correction) - (measurements->n0_power[aarx]));
  } // rx_antennas

  measurements->precoded_cqi_dB[eNB_id][0] = dB_fixed2(precoded_signal_strength0,measurements->n0_power_tot);
  measurements->precoded_cqi_dB[eNB_id][1] = dB_fixed2(precoded_signal_strength1,measurements->n0_power_tot);

  // printf("eNB_id %d, symbol %d: precoded CQI %d dB\n",eNB_id,symbol,
  //  measurements->precoded_cqi_dB[eNB_id][0]);

  _mm_empty();
  _m_empty();

  #elif defined(__arm__)

  unsigned short rb,Nre;
  unsigned char aarx,symbol_mod,pilots=0;
  int precoded_signal_strength0=0,precoded_signal_strength1=0, rx_power_correction;
  int16x4_t *dl_ch0_128,*rxdataF128;
  int16x4_t *dl_ch1_128;
  int16x8_t *dl_ch0_128b,*dl_ch1_128b;

  int32x4_t mmtmpD0,mmtmpD1,mmtmpD0b,mmtmpD1b;
  int16x8_t *dl_ch_mag0_128,*dl_ch_mag0_128b,*dl_ch_mag1_128,*dl_ch_mag1_128b,mmtmpD2,mmtmpD3,mmtmpD4,*rxdataF_comp0_128,*rxdataF_comp1_128;
  int16x8_t QAM_amp0_128,QAM_amp0_128b,QAM_amp1_128,QAM_amp1_128b;
  int32x4_t output_shift128 = vmovq_n_s32(-(int32_t)output_shift);

  int **rxdataF_ext           = pdsch_vars->rxdataF_ext;
  int **dl_ch_estimates_ext   = pdsch_vars->dl_ch_estimates_ext;
  int **dl_ch_mag0            = pdsch_vars->dl_ch_mag0;
  int **dl_ch_mag1            = pdsch_vars->dl_ch_mag1[harq_pid][round];
  int **dl_ch_magb0           = pdsch_vars->dl_ch_magb0;
  int **dl_ch_magb1           = pdsch_vars->dl_ch_magb1[harq_pid][round];
  int **rxdataF_comp0         = pdsch_vars->rxdataF_comp0;
  int **rxdataF_comp1         = pdsch_vars->rxdataF_comp1[harq_pid][round];

  int16_t conj[4]__attribute__((aligned(16))) = {1,-1,1,-1};

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp))) {
    if (frame_parms->mode1_flag==1) // 10 out of 12 so don't reduce size
      { nb_rb=1+(5*nb_rb/6); }

    else
      { pilots=1; }
  }

  rx_power_correction=1;

  if (mod_order0 == 4) {
    QAM_amp0_128  = vmovq_n_s16(QAM16_n1);  // 2/sqrt(10)
    QAM_amp0_128b = vmovq_n_s16(0);

  } else if (mod_order0 == 6) {
    QAM_amp0_128  = vmovq_n_s16(QAM64_n1); //
    QAM_amp0_128b = vmovq_n_s16(QAM64_n2);
  }

  if (mod_order1 == 4) {
    QAM_amp1_128  = vmovq_n_s16(QAM16_n1);  // 2/sqrt(10)
    QAM_amp1_128b = vmovq_n_s16(0);

  } else if (mod_order1 == 6) {
    QAM_amp1_128  = vmovq_n_s16(QAM64_n1); //
    QAM_amp1_128b = vmovq_n_s16(QAM64_n2);
  }

  //    printf("comp: rxdataF_comp %p, symbol %d\n",rxdataF_comp[0],symbol);

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {



    dl_ch0_128          = (int16x4_t*)&dl_ch_estimates_ext[aarx];
    dl_ch1_128          = (int16x4_t*)&dl_ch_estimates_ext[2+aarx];
    dl_ch0_128b          = (int16x8_t*)&dl_ch_estimates_ext[aarx];
    dl_ch1_128b          = (int16x8_t*)&dl_ch_estimates_ext[2+aarx];
    dl_ch_mag0_128      = (int16x8_t*)&dl_ch_mag0[aarx];
    dl_ch_mag0_128b     = (int16x8_t*)&dl_ch_magb0[aarx];
    dl_ch_mag1_128      = (int16x8_t*)&dl_ch_mag1[aarx];
    dl_ch_mag1_128b     = (int16x8_t*)&dl_ch_magb1[aarx];
    rxdataF128          = (int16x4_t*)&rxdataF_ext[aarx];
    rxdataF_comp0_128   = (int16x8_t*)&rxdataF_comp0[aarx];
    rxdataF_comp1_128   = (int16x8_t*)&rxdataF_comp1[aarx];

    for (rb=0; rb<nb_rb; rb++) {
      // combine TX channels using precoder from pmi
      if (mimo_mode==LARGE_CDD) {
        prec2A_TM3_128(&dl_ch0_128[0],&dl_ch1_128[0]);
        prec2A_TM3_128(&dl_ch0_128[1],&dl_ch1_128[1]);


        if (pilots==0) {
          prec2A_TM3_128(&dl_ch0_128[2],&dl_ch1_128[2]);
        }
      }
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1) {
        prec2A_TM4_128(0,&dl_ch0_128[0],&dl_ch1_128[0]);
        prec2A_TM4_128(0,&dl_ch0_128[1],&dl_ch1_128[1]);

        if (pilots==0) {
          prec2A_TM4_128(0,&dl_ch0_128[2],&dl_ch1_128[2]);
        }
      }
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj) {
        prec2A_TM4_128(1,&dl_ch0_128[0],&dl_ch1_128[0]);
        prec2A_TM4_128(1,&dl_ch0_128[1],&dl_ch1_128[1]);

        if (pilots==0) {
          prec2A_TM4_128(1,&dl_ch0_128[2],&dl_ch1_128[2]);
        }
      }
      else {
        LOG_E(PHY,"Unknown MIMO mode\n");
        return;
      }


      if (mod_order0>2) {
        // get channel amplitude if not QPSK
        mmtmpD0 = vmull_s16(dl_ch0_128[0], dl_ch0_128[0]);
        // mmtmpD0 = [ch0*ch0,ch1*ch1,ch2*ch2,ch3*ch3];
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        // mmtmpD0 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3]>>output_shift128 on 32-bits
        mmtmpD1 = vmull_s16(dl_ch0_128[1], dl_ch0_128[1]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD2 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
        // mmtmpD2 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3,ch4*ch4 + ch5*ch5,ch4*ch4 + ch5*ch5,ch6*ch6 + ch7*ch7,ch6*ch6 + ch7*ch7]>>output_shift128 on 16-bits
        mmtmpD0 = vmull_s16(dl_ch0_128[2], dl_ch0_128[2]);
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        mmtmpD1 = vmull_s16(dl_ch0_128[3], dl_ch0_128[3]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD3 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

        if (pilots==0) {
          mmtmpD0 = vmull_s16(dl_ch0_128[4], dl_ch0_128[4]);
          mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
          mmtmpD1 = vmull_s16(dl_ch0_128[5], dl_ch0_128[5]);
          mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
          mmtmpD4 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


        }

        dl_ch_mag0_128b[0] = vqdmulhq_s16(mmtmpD2,QAM_amp0_128b);
        dl_ch_mag0_128b[1] = vqdmulhq_s16(mmtmpD3,QAM_amp0_128b);
        dl_ch_mag0_128[0] = vqdmulhq_s16(mmtmpD2,QAM_amp0_128);
        dl_ch_mag0_128[1] = vqdmulhq_s16(mmtmpD3,QAM_amp0_128);


        if (pilots==0) {
          dl_ch_mag0_128b[2] = vqdmulhq_s16(mmtmpD4,QAM_amp0_128b);
          dl_ch_mag0_128[2]  = vqdmulhq_s16(mmtmpD4,QAM_amp0_128);
        }
      }

      if (mod_order1>2) {
        // get channel amplitude if not QPSK
        mmtmpD0 = vmull_s16(dl_ch1_128[0], dl_ch1_128[0]);
        // mmtmpD0 = [ch0*ch0,ch1*ch1,ch2*ch2,ch3*ch3];
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        // mmtmpD0 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3]>>output_shift128 on 32-bits
        mmtmpD1 = vmull_s16(dl_ch1_128[1], dl_ch1_128[1]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD2 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
        // mmtmpD2 = [ch0*ch0 + ch1*ch1,ch0*ch0 + ch1*ch1,ch2*ch2 + ch3*ch3,ch2*ch2 + ch3*ch3,ch4*ch4 + ch5*ch5,ch4*ch4 + ch5*ch5,ch6*ch6 + ch7*ch7,ch6*ch6 + ch7*ch7]>>output_shift128 on 16-bits
        mmtmpD0 = vmull_s16(dl_ch1_128[2], dl_ch1_128[2]);
        mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
        mmtmpD1 = vmull_s16(dl_ch1_128[3], dl_ch1_128[3]);
        mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
        mmtmpD3 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

        if (pilots==0) {
          mmtmpD0 = vmull_s16(dl_ch1_128[4], dl_ch1_128[4]);
          mmtmpD0 = vqshlq_s32(vqaddq_s32(mmtmpD0,vrev64q_s32(mmtmpD0)),output_shift128);
          mmtmpD1 = vmull_s16(dl_ch1_128[5], dl_ch1_128[5]);
          mmtmpD1 = vqshlq_s32(vqaddq_s32(mmtmpD1,vrev64q_s32(mmtmpD1)),output_shift128);
          mmtmpD4 = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));


        }

        dl_ch_mag1_128b[0] = vqdmulhq_s16(mmtmpD2,QAM_amp1_128b);
        dl_ch_mag1_128b[1] = vqdmulhq_s16(mmtmpD3,QAM_amp1_128b);
        dl_ch_mag1_128[0] = vqdmulhq_s16(mmtmpD2,QAM_amp1_128);
        dl_ch_mag1_128[1] = vqdmulhq_s16(mmtmpD3,QAM_amp1_128);


        if (pilots==0) {
          dl_ch_mag1_128b[2] = vqdmulhq_s16(mmtmpD4,QAM_amp1_128b);
          dl_ch_mag1_128[2]  = vqdmulhq_s16(mmtmpD4,QAM_amp1_128);
        }
      }

      mmtmpD0 = vmull_s16(dl_ch0_128[0], rxdataF128[0]);
      //mmtmpD0 = [Re(ch[0])Re(rx[0]) Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1]) Im(ch[1])Im(ch[1])]
      mmtmpD1 = vmull_s16(dl_ch0_128[1], rxdataF128[1]);
      //mmtmpD1 = [Re(ch[2])Re(rx[2]) Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3]) Im(ch[3])Im(ch[3])]
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
      //mmtmpD0 = [Re(ch[0])Re(rx[0])+Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1])+Im(ch[1])Im(ch[1]) Re(ch[2])Re(rx[2])+Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3])+Im(ch[3])Im(ch[3])]

      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[0],*(int16x4_t*)conj)), rxdataF128[0]);
      //mmtmpD0 = [-Im(ch[0])Re(rx[0]) Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1]) Re(ch[1])Im(rx[1])]
      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[1],*(int16x4_t*)conj)), rxdataF128[1]);
      //mmtmpD0 = [-Im(ch[2])Re(rx[2]) Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3]) Re(ch[3])Im(rx[3])]
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));
      //mmtmpD1 = [-Im(ch[0])Re(rx[0])+Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1])+Re(ch[1])Im(rx[1]) -Im(ch[2])Re(rx[2])+Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3])+Re(ch[3])Im(rx[3])]

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp0_128[0] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      mmtmpD0 = vmull_s16(dl_ch0_128[2], rxdataF128[2]);
      mmtmpD1 = vmull_s16(dl_ch0_128[3], rxdataF128[3]);
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[2],*(int16x4_t*)conj)), rxdataF128[2]);
      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[3],*(int16x4_t*)conj)), rxdataF128[3]);
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp0_128[1] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      // second stream
      mmtmpD0 = vmull_s16(dl_ch1_128[0], rxdataF128[0]);
      mmtmpD1 = vmull_s16(dl_ch1_128[1], rxdataF128[1]);
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));
      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[0],*(int16x4_t*)conj)), rxdataF128[0]);

      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[1],*(int16x4_t*)conj)), rxdataF128[1]);
      //mmtmpD0 = [-Im(ch[2])Re(rx[2]) Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3]) Re(ch[3])Im(rx[3])]
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));
      //mmtmpD1 = [-Im(ch[0])Re(rx[0])+Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1])+Re(ch[1])Im(rx[1]) -Im(ch[2])Re(rx[2])+Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3])+Re(ch[3])Im(rx[3])]

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp1_128[0] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      mmtmpD0 = vmull_s16(dl_ch1_128[2], rxdataF128[2]);
      mmtmpD1 = vmull_s16(dl_ch1_128[3], rxdataF128[3]);
      mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                             vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

      mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[2],*(int16x4_t*)conj)), rxdataF128[2]);
      mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[3],*(int16x4_t*)conj)), rxdataF128[3]);
      mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                             vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));

      mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
      mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
      rxdataF_comp1_128[1] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));

      if (pilots==0) {
        mmtmpD0 = vmull_s16(dl_ch0_128[4], rxdataF128[4]);
        mmtmpD1 = vmull_s16(dl_ch0_128[5], rxdataF128[5]);
        mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                               vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

        mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[4],*(int16x4_t*)conj)), rxdataF128[4]);
        mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch0_128[5],*(int16x4_t*)conj)), rxdataF128[5]);
        mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                               vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));


        mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
        mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
        rxdataF_comp0_128[2] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
        mmtmpD0 = vmull_s16(dl_ch1_128[4], rxdataF128[4]);
        mmtmpD1 = vmull_s16(dl_ch1_128[5], rxdataF128[5]);
        mmtmpD0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0),vget_high_s32(mmtmpD0)),
                               vpadd_s32(vget_low_s32(mmtmpD1),vget_high_s32(mmtmpD1)));

        mmtmpD0b = vmull_s16(vrev32_s16(vmul_s16(dl_ch1_128[4],*(int16x4_t*)conj)), rxdataF128[4]);
        mmtmpD1b = vmull_s16(vrev32_s16(vmul_s16(dl_ch1_128[5],*(int16x4_t*)conj)), rxdataF128[5]);
        mmtmpD1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpD0b),vget_high_s32(mmtmpD0b)),
                               vpadd_s32(vget_low_s32(mmtmpD1b),vget_high_s32(mmtmpD1b)));


        mmtmpD0 = vqshlq_s32(mmtmpD0,output_shift128);
        mmtmpD1 = vqshlq_s32(mmtmpD1,output_shift128);
        rxdataF_comp1_128[2] = vcombine_s16(vmovn_s32(mmtmpD0),vmovn_s32(mmtmpD1));
      }
    }

    Nre = (pilots==0) ? 12 : 8;


  }// rx_antennas

  Nre = (pilots==0) ? 12 : 8;

  precoded_signal_strength0 += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*Nre],
                                                        (nb_rb*Nre))*rx_power_correction) - (measurements->n0_power[aarx]));
  precoded_signal_strength1 += ((signal_energy_nodc(&dl_ch_estimates_ext[aarx+2][symbol*frame_parms->N_RB_DL*Nre],
                                                        (nb_rb*Nre))*rx_power_correction) - (measurements->n0_power[aarx]));

  measurements->precoded_cqi_dB[eNB_id][0] = dB_fixed2(precoded_signal_strength0,measurements->n0_power_tot);
  measurements->precoded_cqi_dB[eNB_id][1] = dB_fixed2(precoded_signal_strength1,measurements->n0_power_tot);

#endif
}


void dlsch_dual_stream_correlation(LTE_DL_FRAME_PARMS *frame_parms,
                                   unsigned char symbol,
                                   unsigned short nb_rb,
                                   int **dl_ch_estimates_ext,
                                   int **dl_ch_estimates_ext_i,
                                   int **dl_ch_rho_ext,
                                   unsigned char output_shift)
{

#if defined(__x86_64__)||defined(__i386__)

    unsigned short rb;
    __m128i *dl_ch128,*dl_ch128i,*dl_ch_rho128,mmtmpD0,mmtmpD1,mmtmpD2,mmtmpD3;
    unsigned char aarx,symbol_mod,pilots=0;

    //    printf("dlsch_dual_stream_correlation: symbol %d\n",symbol);

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
    {
        pilots=1;
    }

    //  printf("Dual stream correlation (%p)\n",dl_ch_estimates_ext_i);

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
    {
        //printf ("antenna %d", aarx);
        dl_ch128 = (__m128i *)dl_ch_estimates_ext[aarx];

        if (dl_ch_estimates_ext_i == NULL)
        {
          // TM3/4
            dl_ch128i = (__m128i *)dl_ch_estimates_ext[2+aarx];
        }
        else
        {
            dl_ch128i = (__m128i *)dl_ch_estimates_ext_i[aarx];
        }

        dl_ch_rho128 = (__m128i *)&dl_ch_rho_ext[aarx][symbol*frame_parms->N_RB_DL*12];

        for (rb=0; rb<nb_rb; rb++)
        {
            // multiply by conjugated channel
            mmtmpD0 = _mm_madd_epi16(dl_ch128[0],dl_ch128i[0]);
            //      print_ints("re",&mmtmpD0);
            // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
            mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[0],_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
            mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[0]);
            //      print_ints("im",&mmtmpD1);
            // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
            mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
            //      print_ints("re(shift)",&mmtmpD0);
            mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
            //      print_ints("im(shift)",&mmtmpD1);
            mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
            mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
            //      print_ints("c0",&mmtmpD2);
            //      print_ints("c1",&mmtmpD3);
            dl_ch_rho128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
            // print_shorts("rho 0:",dl_ch_rho128);
            // multiply by conjugated channel
            mmtmpD0 = _mm_madd_epi16(dl_ch128[1],dl_ch128i[1]);
            // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
            mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[1],_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
            mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
            mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[1]);
            // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
            mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
            mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
            mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
            mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
            dl_ch_rho128[1] =_mm_packs_epi32(mmtmpD2,mmtmpD3);

            if (pilots==0)
            {
                // multiply by conjugated channel
                mmtmpD0 = _mm_madd_epi16(dl_ch128[2],dl_ch128i[2]);
                // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
                mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[2],_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
                mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
                mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[2]);
                // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
                mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
                mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
                mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
                mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
                dl_ch_rho128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);

                dl_ch128+=3;
                dl_ch128i+=3;
                dl_ch_rho128+=3;
            }
            else
            {
                dl_ch128+=2;
                dl_ch128i+=2;
                dl_ch_rho128+=2;
            }
        } // loop over rb
    } // loop over rx antennas

  _mm_empty();
  _m_empty();

#elif defined(__arm__)

#endif
}


/*void dlsch_dual_stream_correlationTM34(LTE_DL_FRAME_PARMS *frame_parms,
                                   unsigned char symbol,
                                   unsigned short nb_rb,
                                   int **dl_ch_estimates_ext,
                                   int **dl_ch_estimates_ext_i,
                                   int **dl_ch_rho_ext,
                                   unsigned char output_shift0,
                                   unsigned char output_shift1)
{

#if defined(__x86_64__)||defined(__i386__)

  unsigned short rb;
  __m128i *dl_ch128,*dl_ch128i,*dl_ch_rho128,mmtmpD0,mmtmpD1,mmtmpD2,mmtmpD3;
  unsigned char aarx,symbol_mod,pilots=0;
  int output_shift;

  //    printf("dlsch_dual_stream_correlation: symbol %d\n",symbol);

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp))) {
    pilots=1;
  }

  //  printf("Dual stream correlation (%p)\n",dl_ch_estimates_ext_i);

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

       if (aarx==0) {
      output_shift=output_shift0;
    }
      else {
        output_shift=output_shift1;
      }

 //printf ("antenna %d", aarx);
    dl_ch128          = (__m128i *)dl_ch_estimates_ext[aarx];

    if (dl_ch_estimates_ext_i == NULL) // TM3/4
      dl_ch128i         = (__m128i *)dl_ch_estimates_ext[2+aarx];
    else
      dl_ch128i         = (__m128i *)dl_ch_estimates_ext_i[aarx];

    dl_ch_rho128      = (__m128i *)&dl_ch_rho_ext[aarx];


    for (rb=0; rb<nb_rb; rb++) {
      // multiply by conjugated channel
      mmtmpD0 = _mm_madd_epi16(dl_ch128[0],dl_ch128i[0]);
      //      print_ints("re",&mmtmpD0);
      // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[0],_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)&conjugate[0]);
      mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[0]);
      //      print_ints("im",&mmtmpD1);
      // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
      //      print_ints("re(shift)",&mmtmpD0);
      mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
      //      print_ints("im(shift)",&mmtmpD1);
      mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
      mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
      //      print_ints("c0",&mmtmpD2);
      //      print_ints("c1",&mmtmpD3);
      dl_ch_rho128[0] = _mm_packs_epi32(mmtmpD2,mmtmpD3);
    // print_shorts("rho 0:",dl_ch_rho128);
      // multiply by conjugated channel
      mmtmpD0 = _mm_madd_epi16(dl_ch128[1],dl_ch128i[1]);
      // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[1],_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
      mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
      mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[1]);
      // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
      mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
      mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
      mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
      dl_ch_rho128[1] =_mm_packs_epi32(mmtmpD2,mmtmpD3);


      if (pilots==0) {

        // multiply by conjugated channel
        mmtmpD0 = _mm_madd_epi16(dl_ch128[2],dl_ch128i[2]);
        // mmtmpD0 contains real part of 4 consecutive outputs (32-bit)
        mmtmpD1 = _mm_shufflelo_epi16(dl_ch128[2],_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_shufflehi_epi16(mmtmpD1,_MM_SHUFFLE(2,3,0,1));
        mmtmpD1 = _mm_sign_epi16(mmtmpD1,*(__m128i*)conjugate);
        mmtmpD1 = _mm_madd_epi16(mmtmpD1,dl_ch128i[2]);
        // mmtmpD1 contains imag part of 4 consecutive outputs (32-bit)
        mmtmpD0 = _mm_srai_epi32(mmtmpD0,output_shift);
        mmtmpD1 = _mm_srai_epi32(mmtmpD1,output_shift);
        mmtmpD2 = _mm_unpacklo_epi32(mmtmpD0,mmtmpD1);
        mmtmpD3 = _mm_unpackhi_epi32(mmtmpD0,mmtmpD1);
        dl_ch_rho128[2] = _mm_packs_epi32(mmtmpD2,mmtmpD3);

       dl_ch128+=3;
        dl_ch128i+=3;
        dl_ch_rho128+=3;
      } else {

        dl_ch128+=2;
        dl_ch128i+=2;
        dl_ch_rho128+=2;
      }
    }

  }

  _mm_empty();
  _m_empty();

#elif defined(__arm__)

#endif
}
*/

void dlsch_detection_mrc(LTE_DL_FRAME_PARMS *frame_parms,
                         int **rxdataF_comp,
                         int **rxdataF_comp_i,
                         int **rho,
                         int **rho_i,
                         int **dl_ch_mag,
                         int **dl_ch_magb,
                         int **dl_ch_mag_i,
                         int **dl_ch_magb_i,
                         unsigned char symbol,
                         unsigned short nb_rb,
                         unsigned char dual_stream_UE)
{

#if defined(__x86_64__)||defined(__i386__)

    unsigned char aatx;
    int i;
    __m128i *rxdataF_comp128_0,*rxdataF_comp128_1,*rxdataF_comp128_i0,*rxdataF_comp128_i1,*dl_ch_mag128_0,*dl_ch_mag128_1,*dl_ch_mag128_0b,*dl_ch_mag128_1b,*rho128_0,*rho128_1,*rho128_i0,*rho128_i1,
        *dl_ch_mag128_i0,*dl_ch_mag128_i1,*dl_ch_mag128_i0b,*dl_ch_mag128_i1b;

    if (frame_parms->nb_antennas_rx > 1)
    {
        for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
        {
            rxdataF_comp128_0 = (__m128i *)rxdataF_comp[(aatx<<1)];
            rxdataF_comp128_1 = (__m128i *)rxdataF_comp[(aatx<<1)+1];
            dl_ch_mag128_0    = (__m128i *)dl_ch_mag[(aatx<<1)];
            dl_ch_mag128_1    = (__m128i *)dl_ch_mag[(aatx<<1)+1];
            dl_ch_mag128_0b   = (__m128i *)dl_ch_magb[(aatx<<1)];
            dl_ch_mag128_1b   = (__m128i *)dl_ch_magb[(aatx<<1)+1];

            // MRC on each re of rb, both on MF output and magnitude (for 16QAM/64QAM llr computation)
            for (i=0; i<nb_rb*3; i++)
            {
                rxdataF_comp128_0[i] = _mm_adds_epi16(_mm_srai_epi16(rxdataF_comp128_0[i],1),_mm_srai_epi16(rxdataF_comp128_1[i],1));
                dl_ch_mag128_0[i]    = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_0[i],1),_mm_srai_epi16(dl_ch_mag128_1[i],1));
                dl_ch_mag128_0b[i]   = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_0b[i],1),_mm_srai_epi16(dl_ch_mag128_1b[i],1));
                // print_shorts("mrc comp0:",&rxdataF_comp128_0[i]);
                // print_shorts("mrc mag0:",&dl_ch_mag128_0[i]);
                // print_shorts("mrc mag0b:",&dl_ch_mag128_0b[i]);
                // print_shorts("mrc rho1:",&rho128_1[i]);
            }
        } // loop over tx antennas

        if (rho)
        {
            rho128_0 = (__m128i *) rho[0];
            rho128_1 = (__m128i *) rho[1];

            for (i=0;i<nb_rb*3;i++)
            {
                // print_shorts("mrc rho0:",&rho128_0[i]);
                // print_shorts("mrc rho1:",&rho128_1[i]);
                rho128_0[i] = _mm_adds_epi16(_mm_srai_epi16(rho128_0[i],1),_mm_srai_epi16(rho128_1[i],1));
            }
        }

        if (dual_stream_UE == 1)
        {
            rho128_i0 = (__m128i *) rho_i[0];
            rho128_i1 = (__m128i *) rho_i[1];
            rxdataF_comp128_i0 = (__m128i *)rxdataF_comp_i[0];
            rxdataF_comp128_i1 = (__m128i *)rxdataF_comp_i[1];
            dl_ch_mag128_i0    = (__m128i *)dl_ch_mag_i[0];
            dl_ch_mag128_i1    = (__m128i *)dl_ch_mag_i[1];
            dl_ch_mag128_i0b   = (__m128i *)dl_ch_magb_i[0];
            dl_ch_mag128_i1b   = (__m128i *)dl_ch_magb_i[1];

            for (i=0; i<nb_rb*3; i++)
            {
                rxdataF_comp128_i0[i] = _mm_adds_epi16(_mm_srai_epi16(rxdataF_comp128_i0[i],1),_mm_srai_epi16(rxdataF_comp128_i1[i],1));
                rho128_i0[i]          = _mm_adds_epi16(_mm_srai_epi16(rho128_i0[i],1),_mm_srai_epi16(rho128_i1[i],1));

                dl_ch_mag128_i0[i]    = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_i0[i],1),_mm_srai_epi16(dl_ch_mag128_i1[i],1));
                dl_ch_mag128_i0b[i]   = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_i0b[i],1),_mm_srai_epi16(dl_ch_mag128_i1b[i],1));
            }
        }
    }

  _mm_empty();
  _m_empty();

#elif defined(__arm__)

  unsigned char aatx;
  int i;
  int16x8_t *rxdataF_comp128_0,*rxdataF_comp128_1,*rxdataF_comp128_i0,*rxdataF_comp128_i1,*dl_ch_mag128_0,*dl_ch_mag128_1,*dl_ch_mag128_0b,*dl_ch_mag128_1b,*rho128_0,*rho128_1,*rho128_i0,*rho128_i1,*dl_ch_mag128_i0,*dl_ch_mag128_i1,*dl_ch_mag128_i0b,*dl_ch_mag128_i1b;

  if (frame_parms->nb_antennas_rx>1) {

    for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++) {

        rxdataF_comp128_0 = (int16x8_t *)rxdataF_comp[(aatx<<1)];
        rxdataF_comp128_1 = (int16x8_t *)rxdataF_comp[(aatx<<1)+1];
        dl_ch_mag128_0    = (int16x8_t *)dl_ch_mag[(aatx<<1)];
        dl_ch_mag128_1    = (int16x8_t *)dl_ch_mag[(aatx<<1)+1];
        dl_ch_mag128_0b   = (int16x8_t *)dl_ch_magb[(aatx<<1)];
        dl_ch_mag128_1b   = (int16x8_t *)dl_ch_magb[(aatx<<1)+1];

      // MRC on each re of rb, both on MF output and magnitude (for 16QAM/64QAM llr computation)
      for (i=0; i<nb_rb*3; i++) {
        rxdataF_comp128_0[i] = vhaddq_s16(rxdataF_comp128_0[i],rxdataF_comp128_1[i]);
        dl_ch_mag128_0[i]    = vhaddq_s16(dl_ch_mag128_0[i],dl_ch_mag128_1[i]);
        dl_ch_mag128_0b[i]   = vhaddq_s16(dl_ch_mag128_0b[i],dl_ch_mag128_1b[i]);
      }
    }

    if (rho) {
        rho128_0 = (int16x8_t *) rho[0];
        rho128_1 = (int16x8_t *) rho[1];

      for (i=0; i<nb_rb*3; i++) {
        //  print_shorts("mrc rho0:",&rho128_0[i]);
        //  print_shorts("mrc rho1:",&rho128_1[i]);
        rho128_0[i] = vhaddq_s16(rho128_0[i],rho128_1[i]);
      }
    }


    if (dual_stream_UE == 1) {
        rho128_i0 = (int16x8_t *) rho_i[0];
        rho128_i1 = (int16x8_t *) rho_i[1];
        rxdataF_comp128_i0 = (int16x8_t *)rxdataF_comp_i[0];
        rxdataF_comp128_i1 = (int16x8_t *)rxdataF_comp_i[1];

        dl_ch_mag128_i0  = (int16x8_t *)dl_ch_mag_i[0];
        dl_ch_mag128_i1  = (int16x8_t *)dl_ch_mag_i[1];
        dl_ch_mag128_i0b = (int16x8_t *)dl_ch_magb_i[0];
        dl_ch_mag128_i1b = (int16x8_t *)dl_ch_magb_i[1];

      for (i=0; i<nb_rb*3; i++) {
        rxdataF_comp128_i0[i] = vhaddq_s16(rxdataF_comp128_i0[i],rxdataF_comp128_i1[i]);
        rho128_i0[i]          = vhaddq_s16(rho128_i0[i],rho128_i1[i]);

        dl_ch_mag128_i0[i]    = vhaddq_s16(dl_ch_mag128_i0[i],dl_ch_mag128_i1[i]);
        dl_ch_mag128_i0b[i]   = vhaddq_s16(dl_ch_mag128_i0b[i],dl_ch_mag128_i1b[i]);
      }
    }
  }

#endif
}


void dlsch_detection_mrc_TM34(LTE_DL_FRAME_PARMS *frame_parms,
                              LTE_UE_PDSCH *pdsch_vars,
                              int harq_pid,
                              int round,
                              unsigned char symbol,
                              unsigned short nb_rb,
                              unsigned char dual_stream_UE) {

    int i;
    __m128i *rxdataF_comp128_0,*rxdataF_comp128_1,*rxdataF_comp128_i0,*rxdataF_comp128_i1,*dl_ch_mag128_0,*dl_ch_mag128_1,*dl_ch_mag128_0b,*dl_ch_mag128_1b,*rho128_0,*rho128_1,*rho128_i0,*rho128_i1,*dl_ch_mag128_i0,*dl_ch_mag128_i1,*dl_ch_mag128_i0b,*dl_ch_mag128_i1b;

    int **rxdataF_comp0  = pdsch_vars->rxdataF_comp0;
    int **rxdataF_comp1  = pdsch_vars->rxdataF_comp1[harq_pid][round];
    int **dl_ch_rho_ext  = pdsch_vars->dl_ch_rho_ext[harq_pid][round]; //for second stream
    int **dl_ch_rho2_ext = pdsch_vars->dl_ch_rho2_ext;
    int **dl_ch_mag0     = pdsch_vars->dl_ch_mag0;
    int **dl_ch_mag1     = pdsch_vars->dl_ch_mag1[harq_pid][round];
    int **dl_ch_magb0    = pdsch_vars->dl_ch_magb0;
    int **dl_ch_magb1    = pdsch_vars->dl_ch_magb1[harq_pid][round];

    if (frame_parms->nb_antennas_rx>1)
    {
        rxdataF_comp128_0 = (__m128i *)&rxdataF_comp0[0];
        rxdataF_comp128_1 = (__m128i *)&rxdataF_comp0[1];
        dl_ch_mag128_0    = (__m128i *)&dl_ch_mag0[0];
        dl_ch_mag128_1    = (__m128i *)&dl_ch_mag0[1];
        dl_ch_mag128_0b   = (__m128i *)&dl_ch_magb0[0];
        dl_ch_mag128_1b   = (__m128i *)&dl_ch_magb0[1];

        // MRC on each re of rb, both on MF output and magnitude (for 16QAM/64QAM llr computation)
        for (i=0; i<nb_rb*3; i++)
        {
            rxdataF_comp128_0[i] = _mm_adds_epi16(_mm_srai_epi16(rxdataF_comp128_0[i],1),_mm_srai_epi16(rxdataF_comp128_1[i],1));
            dl_ch_mag128_0[i]    = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_0[i],1),_mm_srai_epi16(dl_ch_mag128_1[i],1));
            dl_ch_mag128_0b[i]   = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_0b[i],1),_mm_srai_epi16(dl_ch_mag128_1b[i],1));

            // print_shorts("mrc compens0:",&rxdataF_comp128_0[i]);
            // print_shorts("mrc mag128_0:",&dl_ch_mag128_0[i]);
            // print_shorts("mrc mag128_0b:",&dl_ch_mag128_0b[i]);
        }
    }

    rho128_0 = (__m128i *) &dl_ch_rho2_ext[0][symbol*frame_parms->N_RB_DL*12];
    rho128_1 = (__m128i *) &dl_ch_rho2_ext[1][symbol*frame_parms->N_RB_DL*12];
    for (i=0;i<nb_rb*3;i++)
    {
        // print_shorts("mrc rho0:",&rho128_0[i]);
        // print_shorts("mrc rho1:",&rho128_1[i]);
        rho128_0[i] = _mm_adds_epi16(_mm_srai_epi16(rho128_0[i],1),_mm_srai_epi16(rho128_1[i],1));
    }

    if (dual_stream_UE == 1)
    {
        rho128_i0 = (__m128i *) &dl_ch_rho_ext[0][symbol*frame_parms->N_RB_DL*12];
        rho128_i1 = (__m128i *) &dl_ch_rho_ext[1][symbol*frame_parms->N_RB_DL*12];
        rxdataF_comp128_i0   = (__m128i *)&rxdataF_comp1[0][symbol*frame_parms->N_RB_DL*12];
        rxdataF_comp128_i1   = (__m128i *)&rxdataF_comp1[1][symbol*frame_parms->N_RB_DL*12];
        dl_ch_mag128_i0      = (__m128i *)&dl_ch_mag1[0][symbol*frame_parms->N_RB_DL*12];
        dl_ch_mag128_i1      = (__m128i *)&dl_ch_mag1[1][symbol*frame_parms->N_RB_DL*12];
        dl_ch_mag128_i0b     = (__m128i *)&dl_ch_magb1[0][symbol*frame_parms->N_RB_DL*12];
        dl_ch_mag128_i1b     = (__m128i *)&dl_ch_magb1[1][symbol*frame_parms->N_RB_DL*12];

        for (i=0;i<nb_rb*3;i++)
        {
            rxdataF_comp128_i0[i] = _mm_adds_epi16(_mm_srai_epi16(rxdataF_comp128_i0[i],1),_mm_srai_epi16(rxdataF_comp128_i1[i],1));
            rho128_i0[i]          = _mm_adds_epi16(_mm_srai_epi16(rho128_i0[i],1),_mm_srai_epi16(rho128_i1[i],1));

            dl_ch_mag128_i0[i]  = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_i0[i],1),_mm_srai_epi16(dl_ch_mag128_i1[i],1));
            dl_ch_mag128_i0b[i] = _mm_adds_epi16(_mm_srai_epi16(dl_ch_mag128_i0b[i],1),_mm_srai_epi16(dl_ch_mag128_i1b[i],1));

            //print_shorts("mrc compens1:",&rxdataF_comp128_i0[i]);
            //print_shorts("mrc mag128_i0:",&dl_ch_mag128_i0[i]);
            //print_shorts("mrc mag128_i0b:",&dl_ch_mag128_i0b[i]);
        }
    }

  _mm_empty();
  _m_empty();
}



void dlsch_scale_channel(int **dl_ch_estimates_ext,
                         LTE_DL_FRAME_PARMS *frame_parms,
                         LTE_UE_DLSCH_t **dlsch_ue,
                         uint8_t symbol,
                         unsigned short nb_rb)
{

#if defined(__x86_64__)||defined(__i386__)

    short rb, ch_amp;
    unsigned char aatx,aarx,pilots=0,symbol_mod;
    __m128i *dl_ch128, ch_amp128;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
    {
        if (frame_parms->mode1_flag==1)
        {   // 10 out of 12 so don't reduce size
            nb_rb=1+(5*nb_rb/6);
        }
        else
        {
            pilots=1;
        }
    }

    // Determine scaling amplitude based the symbol
    ch_amp = ((pilots) ? (dlsch_ue[0]->sqrt_rho_b) : (dlsch_ue[0]->sqrt_rho_a));

    LOG_D(PHY,"Scaling PDSCH Chest in OFDM symbol %d by %d\n",symbol_mod,ch_amp);
    // printf("Scaling PDSCH Chest in OFDM symbol %d by %d\n",symbol_mod,ch_amp);

    ch_amp128 = _mm_set1_epi16(ch_amp); // Q3.13

    for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
    {
        for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
        {
            dl_ch128=(__m128i *)dl_ch_estimates_ext[(aatx<<1)+aarx];

            for (rb=0;rb<nb_rb;rb++)
            {
                dl_ch128[0] = _mm_mulhi_epi16(dl_ch128[0],ch_amp128);
                dl_ch128[0] = _mm_slli_epi16(dl_ch128[0],3);

                dl_ch128[1] = _mm_mulhi_epi16(dl_ch128[1],ch_amp128);
                dl_ch128[1] = _mm_slli_epi16(dl_ch128[1],3);

                if (pilots)
                {
                    dl_ch128+=2;
                }
                else
                {
                    dl_ch128[2] = _mm_mulhi_epi16(dl_ch128[2],ch_amp128);
                    dl_ch128[2] = _mm_slli_epi16(dl_ch128[2],3);
                    dl_ch128+=3;
                }
            } // rb
        } // rx antennas
    } // tx antennas

#elif defined(__arm__)

#endif
}


//compute average channel_level on each (TX,RX) antenna pair
void dlsch_channel_level(int **dl_ch_estimates_ext,
                         LTE_DL_FRAME_PARMS *frame_parms,
                         int32_t *avg,
                         uint8_t symbol,
                         unsigned short nb_rb)
{

#if defined(__x86_64__)||defined(__i386__)
    
    short rb;
    unsigned char aatx,aarx,nre=12,symbol_mod;
    __m128i *dl_ch128, avg128D, coeff128;
    
    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
    
    if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
    {
        nre=8;
    }
    else if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==1))
    {
        nre=10;
    }
    else
    {
        nre=12;
    }
    
    double one_over_nb_re = 0.0;
    one_over_nb_re = 1/((double)(nb_rb*nre));
    int16_t one_over_nb_re_q1_15 = (int16_t)(one_over_nb_re * (double)(1<<15) );
    coeff128 = _mm_set_epi16(one_over_nb_re_q1_15,one_over_nb_re_q1_15,one_over_nb_re_q1_15,one_over_nb_re_q1_15,
                             one_over_nb_re_q1_15,one_over_nb_re_q1_15,one_over_nb_re_q1_15,one_over_nb_re_q1_15);
    
    for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
    {
        for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
        {
            //clear average level
            avg128D = _mm_setzero_si128();
            // 5 is always a symbol with no pilots for both normal and extended prefix
            
            dl_ch128 = (__m128i *)dl_ch_estimates_ext[(aatx<<1)+aarx];

            for (rb=0;rb<nb_rb;rb++)
            {
                //      printf("rb %d : ",rb);
                //      print_shorts("ch",&dl_ch128[0]);
                avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[0],_mm_srai_epi16(_mm_mulhi_epi16(dl_ch128[0], coeff128),15)));
                avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[1],_mm_srai_epi16(_mm_mulhi_epi16(dl_ch128[1], coeff128),15)));
                
                if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
                {
                    dl_ch128+=2;
                }
                else
                {
                    avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[2],_mm_srai_epi16(_mm_mulhi_epi16(dl_ch128[2], coeff128),15)));
                    dl_ch128+=3;
                }
                /*
                  if (rb==0) {
                  print_shorts("dl_ch128",&dl_ch128[0]);
                  print_shorts("dl_ch128",&dl_ch128[1]);
                  print_shorts("dl_ch128",&dl_ch128[2]);
                  }
                */
            }    
            
            avg[(aatx<<1)+aarx] =(((int32_t*)&avg128D)[0] +
                                  ((int32_t*)&avg128D)[1] +
                                  ((int32_t*)&avg128D)[2] +
                                  ((int32_t*)&avg128D)[3]);
            //  printf("Channel level : %d\n",avg[(aatx<<1)+aarx]);
        } // rx
    } // tx
    
    _mm_empty();
    _m_empty();

#elif defined(__arm__)

  short rb;
  unsigned char aatx,aarx,nre=12,symbol_mod;
  int32x4_t avg128D;
  int16x4_t *dl_ch128;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {
      //clear average level
      avg128D = vdupq_n_s32(0);
      // 5 is always a symbol with no pilots for both normal and extended prefix

      dl_ch128=(int16x4_t *)dl_ch_estimates_ext[(aatx<<1)+aarx];

      for (rb=0; rb<nb_rb; rb++) {
        //  printf("rb %d : ",rb);
        //  print_shorts("ch",&dl_ch128[0]);
        avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[0],dl_ch128[0]));
        avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[1],dl_ch128[1]));
        avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[2],dl_ch128[2]));
        avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[3],dl_ch128[3]));

        if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0)) {
          dl_ch128+=4;
        } else {
          avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[4],dl_ch128[4]));
          avg128D = vqaddq_s32(avg128D,vmull_s16(dl_ch128[5],dl_ch128[5]));
          dl_ch128+=6;
        }

        /*
          if (rb==0) {
          print_shorts("dl_ch128",&dl_ch128[0]);
          print_shorts("dl_ch128",&dl_ch128[1]);
          print_shorts("dl_ch128",&dl_ch128[2]);
          }
        */
      }

      if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
        nre=8;
      else if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==1))
        nre=10;
      else
        nre=12;

      avg[(aatx<<1)+aarx] = (((int32_t*)&avg128D)[0] +
                             ((int32_t*)&avg128D)[1] +
                             ((int32_t*)&avg128D)[2] +
                             ((int32_t*)&avg128D)[3])/(nb_rb*nre);

      //            printf("Channel level : %d\n",avg[(aatx<<1)+aarx]);
    }


#endif
}

//compute average channel_level of effective (precoded) channel

//compute average channel_level of effective (precoded) channel
void dlsch_channel_level_TM34(int **dl_ch_estimates_ext,
                              LTE_DL_FRAME_PARMS *frame_parms,
                              unsigned char *pmi_ext,
                              int *avg_0,
                              int *avg_1,
                              uint8_t symbol,
                              unsigned short nb_rb,
                              MIMO_mode_t mimo_mode){

#if defined(__x86_64__)||defined(__i386__)

    short rb;
    unsigned char aarx,nre=12,symbol_mod;
    __m128i *dl_ch0_128,*dl_ch1_128, dl_ch0_128_tmp, dl_ch1_128_tmp, avg_0_128D, avg_1_128D;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    //clear average level
    // avg_0_128D = _mm_setzero_si128();
    // avg_1_128D = _mm_setzero_si128();
    avg_0[0] = 0;
    avg_0[1] = 0;
    avg_1[0] = 0;
    avg_1[1] = 0;
    // 5 is always a symbol with no pilots for both normal and extended prefix

    if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
    {
        nre=8;
    }
    else if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==1))
    {
        nre=10;
    }
    else
    {
        nre=12;
    }

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
    {
        dl_ch0_128 = (__m128i *)dl_ch_estimates_ext[aarx];
        dl_ch1_128 = (__m128i *)dl_ch_estimates_ext[2+aarx];

        avg_0_128D = _mm_setzero_si128();
        avg_1_128D = _mm_setzero_si128();

        for (rb=0; rb<nb_rb; rb++)
        {
            // printf("rb %d : \n",rb);
            // print_shorts("ch0\n",&dl_ch0_128[0]);
            //print_shorts("ch1\n",&dl_ch1_128[0]);
            dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[0]);
            dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[0]);

            if (mimo_mode==LARGE_CDD)
            {
                prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
            {
                prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
            {
                prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_PUSCH_PRECODING)
            {
                prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }

            //      mmtmpD0 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
            avg_0_128D = _mm_add_epi32(avg_0_128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

            avg_1_128D = _mm_add_epi32(avg_1_128D,_mm_madd_epi16(dl_ch1_128_tmp,dl_ch1_128_tmp));

            dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[1]);
            dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[1]);

            if (mimo_mode==LARGE_CDD)
            {
                prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
            {
                prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
            {
                prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }
            else if (mimo_mode==DUALSTREAM_PUSCH_PRECODING)
            {
                prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            }

            //      mmtmpD1 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
            avg_0_128D = _mm_add_epi32(avg_0_128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

            avg_1_128D = _mm_add_epi32(avg_1_128D,_mm_madd_epi16(dl_ch1_128_tmp,dl_ch1_128_tmp));

            if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
            {
                dl_ch0_128+=2;
                dl_ch1_128+=2;
            }
            else
            {
                dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[2]);
                dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[2]);

                if (mimo_mode==LARGE_CDD)
                {
                    prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
                }
                else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
                {
                    prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
                }
                else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
                {
                    prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
                }
                else if (mimo_mode==DUALSTREAM_PUSCH_PRECODING)
                {
                    prec2A_TM4_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
                }
                //      mmtmpD2 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);

                avg_1_128D = _mm_add_epi32(avg_1_128D,_mm_madd_epi16(dl_ch1_128_tmp,dl_ch1_128_tmp));
                avg_0_128D = _mm_add_epi32(avg_0_128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

                dl_ch0_128+=3;
                dl_ch1_128+=3;
            }
        } // rb



        avg_0[aarx] = (((int*)&avg_0_128D)[0])/(nb_rb*nre) +
            (((int*)&avg_0_128D)[1])/(nb_rb*nre) +
            (((int*)&avg_0_128D)[2])/(nb_rb*nre) +
            (((int*)&avg_0_128D)[3])/(nb_rb*nre);
        //  printf("From Chan_level aver stream 0 %d =%d\n", aarx, avg_0[aarx]);

        avg_1[aarx] = (((int*)&avg_1_128D)[0])/(nb_rb*nre) +
            (((int*)&avg_1_128D)[1])/(nb_rb*nre) +
            (((int*)&avg_1_128D)[2])/(nb_rb*nre) +
            (((int*)&avg_1_128D)[3])/(nb_rb*nre);
        //    printf("From Chan_level aver stream 1 %d =%d\n", aarx, avg_1[aarx]);
    } // rx
//avg_0[0] = max(avg_0[0],avg_0[1]);
//avg_1[0] = max(avg_1[0],avg_1[1]);
//avg_0[0]= max(avg_0[0], avg_1[0]);

    avg_0[0] = avg_0[0] + avg_0[1];
    // printf("From Chan_level aver stream 0 final =%d\n", avg_0[0]);
    avg_1[0] = avg_1[0] + avg_1[1];
    // printf("From Chan_level aver stream 1 final =%d\n", avg_1[0]);
    avg_0[0] = min (avg_0[0], avg_1[0]);
    avg_1[0] = avg_0[0];

    _mm_empty();
    _m_empty();

#elif defined(__arm__)

#endif
}



/*void dlsch_channel_level_TM34(int **dl_ch_estimates_ext,
                              LTE_DL_FRAME_PARMS *frame_parms,
                              int *avg,
                              uint8_t symbol,
                              unsigned short nb_rb,
                              MIMO_mode_t mimo_mode){

#if defined(__x86_64__)||defined(__i386__)


  short rb;
  unsigned char aarx,nre=12,symbol_mod;
  __m128i *dl_ch0_128,*dl_ch1_128, dl_ch0_128_tmp, dl_ch1_128_tmp,avg128D;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

  //clear average level
  avg128D = _mm_setzero_si128();
  avg[0] = 0;
  avg[1] = 0;
  // 5 is always a symbol with no pilots for both normal and extended prefix

  if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
    nre=8;
  else if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==1))
    nre=10;
  else
    nre=12;

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {
    dl_ch0_128 = (__m128i *)dl_ch_estimates_ext[aarx];
    dl_ch1_128 = (__m128i *)dl_ch_estimates_ext[2+aarx];

    for (rb=0; rb<nb_rb; rb++) {

      dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[0]);
      dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[0]);

      if (mimo_mode==LARGE_CDD)
        prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
        prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
        prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);

      //      mmtmpD0 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
      avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

      dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[1]);
      dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[1]);

      if (mimo_mode==LARGE_CDD)
        prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
        prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
      else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
        prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);

      //      mmtmpD1 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
      avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

      if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0)) {
        dl_ch0_128+=2;
        dl_ch1_128+=2;
      }
      else {
        dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[2]);
        dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[2]);

        if (mimo_mode==LARGE_CDD)
          prec2A_TM3_128(&dl_ch0_128_tmp,&dl_ch1_128_tmp);
        else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODING1)
          prec2A_TM4_128(0,&dl_ch0_128_tmp,&dl_ch1_128_tmp);
        else if (mimo_mode==DUALSTREAM_UNIFORM_PRECODINGj)
          prec2A_TM4_128(1,&dl_ch0_128_tmp,&dl_ch1_128_tmp);

        //      mmtmpD2 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
        avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

        dl_ch0_128+=3;
        dl_ch1_128+=3;
      }
    }

    avg[aarx] = (((int*)&avg128D)[0])/(nb_rb*nre) +
      (((int*)&avg128D)[1])/(nb_rb*nre) +
      (((int*)&avg128D)[2])/(nb_rb*nre) +
      (((int*)&avg128D)[3])/(nb_rb*nre);
  }

  // choose maximum of the 2 effective channels
  avg[0] = cmax(avg[0],avg[1]);

  _mm_empty();
  _m_empty();

#elif defined(__arm__)

#endif
}*/

//compute average channel_level of effective (precoded) channel
void dlsch_channel_level_TM56(int **dl_ch_estimates_ext,
                              LTE_DL_FRAME_PARMS *frame_parms,
                              unsigned char *pmi_ext,
                              int *avg,
                              uint8_t symbol,
                              unsigned short nb_rb)
{

#if defined(__x86_64__)||defined(__i386__)

    short rb;
    unsigned char aarx,nre=12,symbol_mod;
    __m128i *dl_ch0_128,*dl_ch1_128, dl_ch0_128_tmp, dl_ch1_128_tmp,avg128D;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    //clear average level
    avg128D = _mm_setzero_si128();
    avg[0] = 0;
    avg[1] = 0;
    // 5 is always a symbol with no pilots for both normal and extended prefix

    if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
    {
        nre=8;
    }
    else if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==1))
    {
        nre=10;
    }
    else
    {
        nre=12;
    }

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
    {
        dl_ch0_128 = (__m128i *) dl_ch_estimates_ext[aarx];
        dl_ch1_128 = (__m128i *) dl_ch_estimates_ext[2+aarx];

        for (rb=0; rb<nb_rb; rb++)
        {
            dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[0]);
            dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[0]);

            prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            //      mmtmpD0 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
            avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

            dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[1]);
            dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[1]);

            prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
            //      mmtmpD1 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
            avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

            if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
            {
                dl_ch0_128+=2;
                dl_ch1_128+=2;
            }
            else
            {
                dl_ch0_128_tmp = _mm_load_si128(&dl_ch0_128[2]);
                dl_ch1_128_tmp = _mm_load_si128(&dl_ch1_128[2]);

                prec2A_TM56_128(pmi_ext[rb],&dl_ch0_128_tmp,&dl_ch1_128_tmp);
                //      mmtmpD2 = _mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp);
                avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch0_128_tmp,dl_ch0_128_tmp));

                dl_ch0_128+=3;
                dl_ch1_128+=3;
            }
        }

        avg[aarx] = (((int*)&avg128D)[0])/(nb_rb*nre) +
            (((int*)&avg128D)[1])/(nb_rb*nre) +
            (((int*)&avg128D)[2])/(nb_rb*nre) +
            (((int*)&avg128D)[3])/(nb_rb*nre);
    }

    // choose maximum of the 2 effective channels
    avg[0] = cmax(avg[0],avg[1]);

    _mm_empty();
    _m_empty();

#elif defined(__arm__)


#endif
}

//compute average channel_level for TM7
void dlsch_channel_level_TM7(int **dl_bf_ch_estimates_ext,
                         LTE_DL_FRAME_PARMS *frame_parms,
                         int *avg,
                         uint8_t symbol,
                         unsigned short nb_rb)
{

#if defined(__x86_64__)||defined(__i386__)

    short rb;
    unsigned char aatx,aarx,nre=12,symbol_mod;
    __m128i *dl_ch128,avg128D;

    symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

    for (aatx=0; aatx<frame_parms->nb_antenna_ports_eNB; aatx++)
    {
        for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
        {
            //clear average level
            avg128D = _mm_setzero_si128();
            // 5 is always a symbol with no pilots for both normal and extended prefix

            dl_ch128=(__m128i *)dl_bf_ch_estimates_ext[(aatx<<1)+aarx];

            for (rb=0; rb<nb_rb; rb++)
            {
                //  printf("rb %d : ",rb);
                //  print_shorts("ch",&dl_ch128[0]);
                avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[0],dl_ch128[0]));
                avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[1],dl_ch128[1]));

                if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1)))&&(frame_parms->mode1_flag==0))
                {
                    dl_ch128+=2;
                }
                else
                {
                    avg128D = _mm_add_epi32(avg128D,_mm_madd_epi16(dl_ch128[2],dl_ch128[2]));
                    dl_ch128+=3;
                }

                /*
                  if (rb==0) {
                  print_shorts("dl_ch128",&dl_ch128[0]);
                  print_shorts("dl_ch128",&dl_ch128[1]);
                  print_shorts("dl_ch128",&dl_ch128[2]);
                  }
                */
            }

            if (((symbol_mod == 0) || (symbol_mod == (frame_parms->Ncp-1))))
            {
                nre=10;
            }
            else if ((frame_parms->Ncp==0) && (symbol==3 || symbol==6 || symbol==9 || symbol==12))
            {
                nre=9;
            }
            else if ((frame_parms->Ncp==1) && (symbol==4 || symbol==7 || symbol==9))
            {
                nre=8;
            }
            else
            {
                nre=12;
            }

            avg[(aatx<<1)+aarx] = (((int*)&avg128D)[0] +
                                   ((int*)&avg128D)[1] +
                                   ((int*)&avg128D)[2] +
                                   ((int*)&avg128D)[3])/(nb_rb*nre);

            //            printf("Channel level : %d\n",avg[(aatx<<1)+aarx]);
        }
    }

    _mm_empty();
    _m_empty();

#elif defined(__arm__)

#endif
}
//#define ONE_OVER_2_Q15 16384
void dlsch_alamouti(LTE_DL_FRAME_PARMS *frame_parms,
                    int **rxdataF_comp,
                    int **dl_ch_mag,
                    int **dl_ch_magb,
                    unsigned char symbol,
                    unsigned short nb_rb)
{

#if defined(__x86_64__)||defined(__i386__)

    short *rxF0,*rxF1;
    __m128i *ch_mag0,*ch_mag1,*ch_mag0b,*ch_mag1b, *rxF0_128;
    unsigned char rb,re;
    uint8_t symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
    uint8_t pilots = ((symbol_mod==0)||(symbol_mod==(4-frame_parms->Ncp))) ? 1 : 0;
    rxF0_128 = (__m128i*) rxdataF_comp[0];

    //amp = _mm_set1_epi16(ONE_OVER_2_Q15);

    //    printf("Doing alamouti!\n");
    rxF0     = (short*)rxdataF_comp[0];  //tx antenna 0  h0*y
    rxF1     = (short*)rxdataF_comp[2];  //tx antenna 1  h1*y
    ch_mag0  = (__m128i *)dl_ch_mag[0];
    ch_mag1  = (__m128i *)dl_ch_mag[2];
    ch_mag0b = (__m128i *)dl_ch_magb[0];
    ch_mag1b = (__m128i *)dl_ch_magb[2];

    for (rb=0; rb<nb_rb; rb++)
    {
        for (re=0; re<((pilots==0)?12:8); re+=2)
        {
            // Alamouti RX combining

            //      printf("Alamouti: symbol %d, rb %d, re %d: rxF0 (%d,%d,%d,%d), rxF1 (%d,%d,%d,%d)\n",symbol,rb,re,rxF0[0],rxF0[1],rxF0[2],rxF0[3],rxF1[0],rxF1[1],rxF1[2],rxF1[3]);
            rxF0[0] = rxF0[0] + rxF1[2];
            rxF0[1] = rxF0[1] - rxF1[3];

            rxF0[2] = rxF0[2] - rxF1[0];
            rxF0[3] = rxF0[3] + rxF1[1];

            //      printf("Alamouti: rxF0 after (%d,%d,%d,%d)\n",rxF0[0],rxF0[1],rxF0[2],rxF0[3]);
            rxF0+=4;
            rxF1+=4;
        }

        // compute levels for 16QAM or 64 QAM llr unit
        ch_mag0[0] = _mm_adds_epi16(ch_mag0[0],ch_mag1[0]);
        ch_mag0[1] = _mm_adds_epi16(ch_mag0[1],ch_mag1[1]);

        ch_mag0b[0] = _mm_adds_epi16(ch_mag0b[0],ch_mag1b[0]);
        ch_mag0b[1] = _mm_adds_epi16(ch_mag0b[1],ch_mag1b[1]);

        // account for 1/sqrt(2) scaling at transmission
        //ch_mag0[0] = _mm_srai_epi16(ch_mag0[0],1);
        //ch_mag0[1] = _mm_srai_epi16(ch_mag0[1],1);
        //ch_mag0b[0] = _mm_srai_epi16(ch_mag0b[0],1);
        //ch_mag0b[1] = _mm_srai_epi16(ch_mag0b[1],1);

        //rxF0_128[0] = _mm_mulhi_epi16(rxF0_128[0],amp);
        //rxF0_128[0] = _mm_slli_epi16(rxF0_128[0],1);
        //rxF0_128[1] = _mm_mulhi_epi16(rxF0_128[1],amp);
        //rxF0_128[1] = _mm_slli_epi16(rxF0_128[1],1);

        //rxF0_128[0] = _mm_srai_epi16(rxF0_128[0],1);
        //rxF0_128[1] = _mm_srai_epi16(rxF0_128[1],1);

        if (pilots==0)
        {
            ch_mag0[2] = _mm_adds_epi16(ch_mag0[2],ch_mag1[2]);
            ch_mag0b[2] = _mm_adds_epi16(ch_mag0b[2],ch_mag1b[2]);

            //ch_mag0[2] = _mm_srai_epi16(ch_mag0[2],1);
            //ch_mag0b[2] = _mm_srai_epi16(ch_mag0b[2],1);

            //rxF0_128[2] = _mm_mulhi_epi16(rxF0_128[2],amp);
            //rxF0_128[2] = _mm_slli_epi16(rxF0_128[2],1);

            //rxF0_128[2] = _mm_srai_epi16(rxF0_128[2],1);

            ch_mag0+=3;
            ch_mag1+=3;
            ch_mag0b+=3;
            ch_mag1b+=3;
            rxF0_128+=3;
        }
        else
        {
            ch_mag0+=2;
            ch_mag1+=2;
            ch_mag0b+=2;
            ch_mag1b+=2;
            rxF0_128+=2;
        }
    }

    _mm_empty();
    _m_empty();

#elif defined(__arm__)

#endif
}


//==============================================================================================
// Extraction functions
//==============================================================================================

unsigned short dlsch_extract_rbs_single(int **rxdataF,
                                        int **dl_ch_estimates,
                                        int **rxdataF_ext,
                                        int **dl_ch_estimates_ext,
                                        unsigned short pmi,
                                        unsigned char *pmi_ext,
                                        unsigned int *rb_alloc,
                                        unsigned char symbol,
                                        unsigned char subframe,
                                        uint32_t high_speed_flag,
                                        LTE_DL_FRAME_PARMS *frame_parms) {



  unsigned short rb,nb_rb=0;
  unsigned char rb_alloc_ind;
  unsigned char i,aarx,l,nsymb,skip_half=0,sss_symb,pss_symb=0;
  int *dl_ch0,*dl_ch0_ext,*rxF,*rxF_ext;



  unsigned char symbol_mod,pilots=0,j=0,poffset=0;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
  pilots = ((symbol_mod==0)||(symbol_mod==(4-frame_parms->Ncp))) ? 1 : 0;
  l=symbol;
  nsymb = (frame_parms->Ncp==NORMAL) ? 14:12;

  if (frame_parms->frame_type == TDD) {  // TDD
    sss_symb = nsymb-1;
    pss_symb = 2;
  } else {
    sss_symb = (nsymb>>1)-2;
    pss_symb = (nsymb>>1)-1;
  }

  if (symbol_mod==(4-frame_parms->Ncp))
    poffset=3;

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

    if (high_speed_flag == 1)
      dl_ch0     = &dl_ch_estimates[aarx][5+(symbol*(frame_parms->ofdm_symbol_size))];
    else
      dl_ch0     = &dl_ch_estimates[aarx][5];

    dl_ch0_ext = dl_ch_estimates_ext[aarx];

    rxF_ext   = rxdataF_ext[aarx];
    rxF       = &rxdataF[aarx][(frame_parms->first_carrier_offset + (symbol*(frame_parms->ofdm_symbol_size)))];

    if ((frame_parms->N_RB_DL&1) == 0)  // even number of RBs

      for (rb=0;rb<frame_parms->N_RB_DL;rb++) {

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

        if (rb_alloc_ind == 1)
          nb_rb++;

        // For second half of RBs skip DC carrier
        if (rb==(frame_parms->N_RB_DL>>1)) {
          rxF       = &rxdataF[aarx][(1 + (symbol*(frame_parms->ofdm_symbol_size)))];
          //dl_ch0++;
        }

        // PBCH
        if ((subframe==0) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=nsymb>>1) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }

        //SSS
        if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }


        if (frame_parms->frame_type == FDD) {
          //PSS
          if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if ((frame_parms->frame_type == TDD) &&
            (subframe==6)) { //TDD Subframe 6
          if ((rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if (rb_alloc_ind==1) {
          *pmi_ext = (pmi>>((rb>>2)<<1))&3;
          memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));

          /*
            printf("rb %d\n",rb);
            for (i=0;i<12;i++)
            printf("(%d %d)",((short *)dl_ch0)[i<<1],((short*)dl_ch0)[1+(i<<1)]);
            printf("\n");
          */
          if (pilots==0) {
            for (i=0; i<12; i++) {
              rxF_ext[i]=rxF[i];
              /*
                printf("%d : (%d,%d)\n",(rxF+i-&rxdataF[aarx][( (symbol*(frame_parms->ofdm_symbol_size)))]),
                ((short*)&rxF[i])[0],((short*)&rxF[i])[1]);*/
            }

            dl_ch0_ext+=12;
            rxF_ext+=12;
          } else {
            j=0;

            for (i=0; i<12; i++) {
              if ((i!=(frame_parms->nushift+poffset)) &&
                  (i!=((frame_parms->nushift+poffset+6)%12))) {
                rxF_ext[j]=rxF[i];
                //            printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
                dl_ch0_ext[j++]=dl_ch0[i];

              }
            }

            dl_ch0_ext+=10;
            rxF_ext+=10;
          }


        }

        dl_ch0+=12;
        rxF+=12;

      }
    else {  // Odd number of RBs
      for (rb=0; rb<frame_parms->N_RB_DL>>1; rb++) {
#ifdef DEBUG_DLSCH_DEMOD
        printf("dlch_ext %d\n",dl_ch0_ext - dl_ch_estimates_ext[aarx]);
#endif
        skip_half=0;

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

        if (rb_alloc_ind == 1)
          nb_rb++;


        // PBCH
        if ((subframe==0) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }

        //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
        if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=1;
        else if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=2;

        //SSS

        if (((subframe==0)||(subframe==5)) &&
            (rb>((frame_parms->N_RB_DL>>1)-3)) &&
            (rb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }
        //SSS
        if (((subframe==0)||(subframe==5)) &&
            (rb==((frame_parms->N_RB_DL>>1)-3)) &&
            (l==sss_symb))
          skip_half=1;
        else if (((subframe==0)||(subframe==5)) &&
                 (rb==((frame_parms->N_RB_DL>>1)+3)) &&
                 (l==sss_symb))
          skip_half=2;

        //PSS in subframe 0/5 if FDD
        if (frame_parms->frame_type == FDD) {  //FDD

          if (((subframe==0)||(subframe==5)) &&
              (rb>((frame_parms->N_RB_DL>>1)-3)) &&
              (rb<((frame_parms->N_RB_DL>>1)+3)) &&
              (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if ((frame_parms->frame_type == TDD) &&
            (subframe==6)){  //TDD Subframe 6
          if ((rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
          if ((rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if ((rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }


        if (rb_alloc_ind==1) {

#ifdef DEBUG_DLSCH_DEMOD
          printf("rb %d/symbol %d (skip_half %d)\n",rb,l,skip_half);
#endif
          if (pilots==0) {
            //      printf("Extracting w/o pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            if (skip_half==1) {
              memcpy(dl_ch0_ext,dl_ch0,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=6;
              rxF_ext+=6;
            } else if (skip_half==2) {
              memcpy(dl_ch0_ext,dl_ch0+6,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=6;
              rxF_ext+=6;
            } else {
              memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));

              for (i=0; i<12; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=12;
              rxF_ext+=12;
            }
          } else {
            //      printf("Extracting with pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            j=0;

            if (skip_half==1) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              }
              rxF_ext+=5;
              dl_ch0_ext+=5;
            } else if (skip_half==2) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i+6];
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else {
              for (i=0; i<12; i++) {
                if ((i!=(frame_parms->nushift+poffset)) &&
                    (i!=((frame_parms->nushift+poffset+6)%12))) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];

                }
              }

              dl_ch0_ext+=10;
              rxF_ext+=10;
            }
          }
        }
        dl_ch0+=12;
        rxF+=12;
      } // first half loop


      // Do middle RB (around DC)
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;


      if (rb_alloc_ind == 1)
        nb_rb++;

      // PBCH

      if ((subframe==0) &&
          (l>=(nsymb>>1)) &&
          (l<((nsymb>>1) + 4))) {
        rb_alloc_ind = 0;
      }

      //SSS
      if (((subframe==0)||(subframe==5)) && (l==sss_symb) ) {
        rb_alloc_ind = 0;
      }

      if (frame_parms->frame_type == FDD) {
        //PSS
        if (((subframe==0)||(subframe==5)) && (l==pss_symb) ) {
          rb_alloc_ind = 0;
        }
      }

      //PSS
      if ((frame_parms->frame_type == TDD) &&
          (subframe==6) &&
          (l==pss_symb) ) {
        rb_alloc_ind = 0;
      }


      //  printf("dlch_ext %d\n",dl_ch0_ext - dl_ch_estimates_ext[aarx]);
      //      printf("DC rb %d (%p)\n",rb,rxF);
      if (rb_alloc_ind==1) {
#ifdef DEBUG_DLSCH_DEMOD
        printf("rb %d/symbol %d (skip_half %d)\n",rb,l,skip_half);
#endif
        if (pilots==0) {
          for (i=0; i<6; i++) {
            dl_ch0_ext[i]=dl_ch0[i];
            rxF_ext[i]=rxF[i];
          }

          rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];

          for (; i<12; i++) {
            dl_ch0_ext[i]=dl_ch0[i];
            rxF_ext[i]=rxF[(1+i-6)];
          }

          dl_ch0_ext+=12;
          rxF_ext+=12;
        } else { // pilots==1
          j=0;

          for (i=0; i<6; i++) {
            if (i!=((frame_parms->nushift+poffset)%6)) {
              dl_ch0_ext[j]=dl_ch0[i];
              rxF_ext[j++]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("**extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j-1],*(1+(short*)&rxF_ext[j-1]));
#endif
            }
          }

          rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];

          for (; i<12; i++) {
            if (i!=((frame_parms->nushift+6+poffset)%12)) {
              dl_ch0_ext[j]=dl_ch0[i];
              rxF_ext[j++]=rxF[(1+i-6)];
#ifdef DEBUG_DLSCH_DEMOD
              printf("**extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j-1],*(1+(short*)&rxF_ext[j-1]));
#endif
            }
          }

          dl_ch0_ext+=10;
          rxF_ext+=10;
        } // symbol_mod==0
      } // rballoc==1
      else {
        rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];
      }

      dl_ch0+=12;
      rxF+=7;
      rb++;

      for (;rb<frame_parms->N_RB_DL;rb++) {
        //      printf("dlch_ext %d\n",dl_ch0_ext - dl_ch_estimates_ext[aarx]);
        //      printf("rb %d (%p)\n",rb,rxF);
        skip_half=0;

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

        if (rb_alloc_ind == 1)
          nb_rb++;

        // PBCH
        if ((subframe==0) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=nsymb>>1) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }
        //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
        if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=1;
        else if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=2;

        //SSS
        if (((subframe==0)||(subframe==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }
        //SSS
        if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==sss_symb))
          skip_half=1;
        else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb))
          skip_half=2;
        if (frame_parms->frame_type == FDD) {
          //PSS
          if (((subframe==0)||(subframe==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          //PSS

          if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if ((frame_parms->frame_type == TDD) &&

            (subframe==6)) { //TDD Subframe 6
          if ((rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if ((rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if ((rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if (rb_alloc_ind==1) {
#ifdef DEBUG_DLSCH_DEMOD
          printf("rb %d/symbol %d (skip_half %d)\n",rb,l,skip_half);
#endif
          /*
            printf("rb %d\n",rb);
            for (i=0;i<12;i++)
            printf("(%d %d)",((short *)dl_ch0)[i<<1],((short*)dl_ch0)[1+(i<<1)]);
            printf("\n");
          */
          if (pilots==0) {
            //      printf("Extracting w/o pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            if (skip_half==1) {
              memcpy(dl_ch0_ext,dl_ch0,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=6;
              rxF_ext+=6;

            } else if (skip_half==2) {
              memcpy(dl_ch0_ext,dl_ch0+6,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=6;
              rxF_ext+=6;

            } else {
              memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));

              for (i=0; i<12; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
              dl_ch0_ext+=12;
              rxF_ext+=12;
            }
          } else {
            //      printf("Extracting with pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            j=0;

            if (skip_half==1) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else if (skip_half==2) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i+6];
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else {
              for (i=0; i<12; i++) {
                if ((i!=(frame_parms->nushift+poffset)) &&
                    (i!=((frame_parms->nushift+poffset+6)%12))) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              }
              dl_ch0_ext+=10;
              rxF_ext+=10;
            }
          } // pilots=0
        }

        dl_ch0+=12;
        rxF+=12;
      }
    }
  }


  return(nb_rb/frame_parms->nb_antennas_rx);
}

unsigned short dlsch_extract_rbs_dual(int **rxdataF,
                                      int **dl_ch_estimates,
                                      int **rxdataF_ext,
                                      int **dl_ch_estimates_ext,
                                      unsigned short pmi,
                                      unsigned char *pmi_ext,
                                      unsigned int *rb_alloc,
                                      unsigned char symbol,
                                      unsigned char subframe,
                                      uint32_t high_speed_flag,
                                      LTE_DL_FRAME_PARMS *frame_parms,
                                      MIMO_mode_t mimo_mode) {

  int prb,nb_rb=0;
  int prb_off,prb_off2;
  int rb_alloc_ind,skip_half=0,sss_symb,pss_symb=0,nsymb,l;
  int i,aarx;
  int32_t *dl_ch0,*dl_ch0p,*dl_ch0_ext,*dl_ch1,*dl_ch1p,*dl_ch1_ext,*rxF,*rxF_ext;
  int symbol_mod,pilots=0,j=0;
  unsigned char *pmi_loc;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
  //  printf("extract_rbs: symbol_mod %d\n",symbol_mod);

  if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
    pilots=1;

  nsymb = (frame_parms->Ncp==NORMAL) ? 14:12;
  l=symbol;

  if (frame_parms->frame_type == TDD) {  // TDD
    sss_symb = nsymb-1;
    pss_symb = 2;
  } else {
    sss_symb = (nsymb>>1)-2;
    pss_symb = (nsymb>>1)-1;
  }

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

    if (high_speed_flag==1) {
      dl_ch0     = &dl_ch_estimates[aarx][5+(symbol*(frame_parms->ofdm_symbol_size))];
      dl_ch1     = &dl_ch_estimates[2+aarx][5+(symbol*(frame_parms->ofdm_symbol_size))];
    } else {
      dl_ch0     = &dl_ch_estimates[aarx][5];
      dl_ch1     = &dl_ch_estimates[2+aarx][5];
    }

    pmi_loc = pmi_ext;

    // pointers to extracted RX signals and channel estimates
    rxF_ext    = rxdataF_ext[aarx];
    dl_ch0_ext = dl_ch_estimates_ext[aarx];
    dl_ch1_ext = dl_ch_estimates_ext[2+aarx];

    for (prb=0; prb<frame_parms->N_RB_DL; prb++) {
      skip_half=0;

      if (prb < 32)
        rb_alloc_ind = (rb_alloc[0]>>prb) & 1;
      else if (prb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(prb-32)) & 1;
      else if (prb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(prb-64)) & 1;
      else if (prb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(prb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind == 1)
          nb_rb++;


      if ((frame_parms->N_RB_DL&1) == 0) {  // even number of RBs

        // PBCH
        if ((subframe==0) &&
            (prb>=((frame_parms->N_RB_DL>>1)-3)) &&
            (prb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l>=(nsymb>>1)) &&
            (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
          //    printf("symbol %d / rb %d: skipping PBCH REs\n",symbol,prb);
        }

        //SSS

        if (((subframe==0)||(subframe==5)) &&
            (prb>=((frame_parms->N_RB_DL>>1)-3)) &&
            (prb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l==sss_symb) ) {
          rb_alloc_ind = 0;
          //    printf("symbol %d / rb %d: skipping SSS REs\n",symbol,prb);
        }



        //PSS in subframe 0/5 if FDD
        if (frame_parms->frame_type == FDD) {  //FDD
          if (((subframe==0)||(subframe==5)) &&
              (prb>=((frame_parms->N_RB_DL>>1)-3)) &&
              (prb<((frame_parms->N_RB_DL>>1)+3)) &&
              (l==pss_symb) ) {
            rb_alloc_ind = 0;
            //    printf("symbol %d / rb %d: skipping PSS REs\n",symbol,prb);
          }
        }

        if ((frame_parms->frame_type == TDD) &&
            (subframe==6)) { //TDD Subframe 6
          if ((prb>=((frame_parms->N_RB_DL>>1)-3)) &&
              (prb<((frame_parms->N_RB_DL>>1)+3)) &&
              (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if (rb_alloc_ind==1) {              // PRB is allocated



          prb_off      = 12*prb;
          prb_off2     = 1+(12*(prb-(frame_parms->N_RB_DL>>1)));
          dl_ch0p    = dl_ch0+(12*prb);
          dl_ch1p    = dl_ch1+(12*prb);
          if (prb<(frame_parms->N_RB_DL>>1)){
            rxF      = &rxdataF[aarx][prb_off+
                                      frame_parms->first_carrier_offset +
                                      (symbol*(frame_parms->ofdm_symbol_size))];
          }
          else {
            rxF      = &rxdataF[aarx][prb_off2+
                                      (symbol*(frame_parms->ofdm_symbol_size))];
          }

         /*
         if (mimo_mode <= PUSCH_PRECODING1)
          *pmi_loc = (pmi>>((prb>>2)<<1))&3;
         else
          *pmi_loc=(pmi>>prb)&1;*/

         *pmi_loc = get_pmi(frame_parms->N_RB_DL,mimo_mode,pmi,prb);
          pmi_loc++;


          if (pilots == 0) {

            memcpy(dl_ch0_ext,dl_ch0p,12*sizeof(int));
            memcpy(dl_ch1_ext,dl_ch1p,12*sizeof(int));
            memcpy(rxF_ext,rxF,12*sizeof(int));
            dl_ch0_ext +=12;
            dl_ch1_ext +=12;
            rxF_ext    +=12;
          } else { // pilots==1
            j=0;
            for (i=0; i<12; i++) {
              if ((i!=frame_parms->nushift) &&
                  (i!=frame_parms->nushift+3) &&
                  (i!=frame_parms->nushift+6) &&
                  (i!=((frame_parms->nushift+9)%12))) {
                rxF_ext[j]=rxF[i];
                //        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
                dl_ch0_ext[j]=dl_ch0p[i];
                dl_ch1_ext[j++]=dl_ch1p[i];
              }
            }
            dl_ch0_ext+=8;
            dl_ch1_ext+=8;
            rxF_ext+=8;
          } // pilots==1

        }
      } else {  // Odd number of RBs


      // PBCH
        if ((subframe==0) &&
            (prb>((frame_parms->N_RB_DL>>1)-3)) &&
            (prb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l>=(nsymb>>1)) &&
            (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
          //    printf("symbol %d / rb %d: skipping PBCH REs\n",symbol,prb);
        }

        //SSS

        if (((subframe==0)||(subframe==5)) &&
            (prb>((frame_parms->N_RB_DL>>1)-3)) &&
            (prb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l==sss_symb) ) {
          rb_alloc_ind = 0;
          //    printf("symbol %d / rb %d: skipping SSS REs\n",symbol,prb);
        }



        //PSS in subframe 0/5 if FDD
        if (frame_parms->frame_type == FDD) {  //FDD
          if (((subframe==0)||(subframe==5)) &&
              (prb>((frame_parms->N_RB_DL>>1)-3)) &&
              (prb<((frame_parms->N_RB_DL>>1)+3)) &&
              (l==pss_symb) ) {
            rb_alloc_ind = 0;
            //    printf("symbol %d / rb %d: skipping PSS REs\n",symbol,prb);
          }
        }

        if ((frame_parms->frame_type == TDD) &&
            ((subframe==1) || (subframe==6))) { //TDD Subframe 1-6
          if ((prb>((frame_parms->N_RB_DL>>1)-3)) &&
              (prb<((frame_parms->N_RB_DL>>1)+3)) &&
              (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if (rb_alloc_ind == 1) {
          skip_half=0;

          //Check if we have to drop half a PRB due to PSS/SSS/PBCH
          // skip_half == 0 means full PRB
          // skip_half == 1 means first half is used (leftmost half-PRB from PSS/SSS/PBCH)
          // skip_half == 2 means second half is used (rightmost half-PRB from PSS/SSS/PBCH)
          //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
          if ((subframe==0) &&
              (prb==((frame_parms->N_RB_DL>>1)-3)) &&
              (l>=(nsymb>>1)) &&
              (l<((nsymb>>1) + 4)))
            skip_half=1;
          else if ((subframe==0) &&
                   (prb==((frame_parms->N_RB_DL>>1)+3)) &&
                   (l>=(nsymb>>1)) &&
                   (l<((nsymb>>1) + 4)))
            skip_half=2;

          //SSS
          if (((subframe==0)||(subframe==5)) &&
              (prb==((frame_parms->N_RB_DL>>1)-3)) &&
              (l==sss_symb))
            skip_half=1;
          else if (((subframe==0)||(subframe==5)) &&
                   (prb==((frame_parms->N_RB_DL>>1)+3)) &&
                   (l==sss_symb))
            skip_half=2;

          //PSS Subframe 0,5
          if (((frame_parms->frame_type == FDD) &&
               (((subframe==0)||(subframe==5)))) ||  //FDD Subframes 0,5
              ((frame_parms->frame_type == TDD) &&
               (((subframe==1) || (subframe==6))))) { //TDD Subframes 1,6

            if ((prb==((frame_parms->N_RB_DL>>1)-3)) &&
                (l==pss_symb))
              skip_half=1;
            else if ((prb==((frame_parms->N_RB_DL>>1)+3)) &&
                     (l==pss_symb))
              skip_half=2;
          }


          prb_off      = 12*prb;
          prb_off2     = 7+(12*(prb-(frame_parms->N_RB_DL>>1)-1));
          dl_ch0p      = dl_ch0+(12*prb);
          dl_ch1p      = dl_ch1+(12*prb);

          if (prb<=(frame_parms->N_RB_DL>>1)){
            rxF      = &rxdataF[aarx][prb_off+
                                      frame_parms->first_carrier_offset +
                                      (symbol*(frame_parms->ofdm_symbol_size))];
          }
          else {
            rxF      = &rxdataF[aarx][prb_off2+
                                      (symbol*(frame_parms->ofdm_symbol_size))];
          }
#ifdef DEBUG_DLSCH_DEMOD
          printf("symbol %d / rb %d: alloc %d skip_half %d (rxF %p, rxF_ext %p) prb_off (%d,%d)\n",symbol,prb,rb_alloc_ind,skip_half,rxF,rxF_ext,prb_off,prb_off2);
#endif
         /* if (mimo_mode <= PUSCH_PRECODING1)
           *pmi_loc = (pmi>>((prb>>2)<<1))&3;
          else
           *pmi_loc=(pmi>>prb)&1;
         // printf("symbol_mod %d (pilots %d) rb %d, sb %d, pmi %d (pmi_loc %p,rxF %p, ch00 %p, ch01 %p, rxF_ext %p dl_ch0_ext %p dl_ch1_ext %p)\n",symbol_mod,pilots,prb,prb>>2,*pmi_loc,pmi_loc,rxF,dl_ch0, dl_ch1, rxF_ext,dl_ch0_ext,dl_ch1_ext);
*/
         *pmi_loc = get_pmi(frame_parms->N_RB_DL,mimo_mode,pmi,prb);
          pmi_loc++;

          if (prb != (frame_parms->N_RB_DL>>1)) { // This PRB is not around DC
            if (pilots==0) {
              if (skip_half==1) {
                memcpy(dl_ch0_ext,dl_ch0p,6*sizeof(int32_t));
                memcpy(dl_ch1_ext,dl_ch1p,6*sizeof(int32_t));
                memcpy(rxF_ext,rxF,6*sizeof(int32_t));
#ifdef DEBUG_DLSCH_DEMOD
                for (i=0;i<6;i++)
                  printf("extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                dl_ch0_ext+=6;
                dl_ch1_ext+=6;
                rxF_ext+=6;
              } else if (skip_half==2) {
                memcpy(dl_ch0_ext,dl_ch0p+6,6*sizeof(int32_t));
                memcpy(dl_ch1_ext,dl_ch1p+6,6*sizeof(int32_t));
                memcpy(rxF_ext,rxF+6,6*sizeof(int32_t));
#ifdef DEBUG_DLSCH_DEMOD
                for (i=0;i<6;i++)
                  printf("extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                dl_ch0_ext+=6;
                dl_ch1_ext+=6;
                rxF_ext+=6;
              } else {  // skip_half==0
                memcpy(dl_ch0_ext,dl_ch0p,12*sizeof(int32_t));
                memcpy(dl_ch1_ext,dl_ch1p,12*sizeof(int32_t));
                memcpy(rxF_ext,rxF,12*sizeof(int32_t));
#ifdef DEBUG_DLSCH_DEMOD
                for (i=0;i<12;i++)
                  printf("extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                dl_ch0_ext+=12;
                dl_ch1_ext+=12;
                rxF_ext+=12;
              }
            } else { // pilots=1
              j=0;

              if (skip_half==1) {
                for (i=0; i<6; i++) {
                  if ((i!=frame_parms->nushift) &&
                      (i!=((frame_parms->nushift+3)%6))) {
                    rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                    printf("(pilots,skip1)extract rb %d, re %d (%d)=> (%d,%d)\n",prb,i,j,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                    dl_ch0_ext[j]=dl_ch0p[i];
                    dl_ch1_ext[j++]=dl_ch1p[i];
                  }
                }
                dl_ch0_ext+=4;
                dl_ch1_ext+=4;
                rxF_ext+=4;
              } else if (skip_half==2) {
                for (i=0; i<6; i++) {
                  if ((i!=frame_parms->nushift) &&
                      (i!=((frame_parms->nushift+3)%6))) {
                    rxF_ext[j]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
                    printf("(pilots,skip2)extract rb %d, re %d (%d) => (%d,%d)\n",prb,i,j,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                    dl_ch0_ext[j]=dl_ch0p[i+6];
                    dl_ch1_ext[j++]=dl_ch1p[i+6];
                  }
                }
                dl_ch0_ext+=4;
                dl_ch1_ext+=4;
                rxF_ext+=4;

              } else { //skip_half==0
                for (i=0; i<12; i++) {
                  if ((i!=frame_parms->nushift) &&
                      (i!=frame_parms->nushift+3) &&
                      (i!=frame_parms->nushift+6) &&
                      (i!=((frame_parms->nushift+9)%12))) {
                    rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                    printf("(pilots)extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                    dl_ch0_ext[j]  =dl_ch0p[i];
                    dl_ch1_ext[j++]=dl_ch1p[i];
                  }
                }
                dl_ch0_ext+=8;
                dl_ch1_ext+=8;
                rxF_ext+=8;
              } //skip_half==0
            } //pilots==1
          } else {       // Do middle RB (around DC)

            if (pilots==0) {
              memcpy(dl_ch0_ext,dl_ch0p,6*sizeof(int32_t));
              memcpy(dl_ch1_ext,dl_ch1p,6*sizeof(int32_t));
              memcpy(rxF_ext,rxF,6*sizeof(int32_t));
#ifdef DEBUG_DLSCH_DEMOD
              for (i=0; i<6; i++) {
                printf("extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
              }
#endif
              rxF_ext+=6;
              dl_ch0_ext+=6;
              dl_ch1_ext+=6;
              dl_ch0p+=6;
              dl_ch1p+=6;

              rxF       = &rxdataF[aarx][1+((symbol*(frame_parms->ofdm_symbol_size)))];

              memcpy(dl_ch0_ext,dl_ch0p,6*sizeof(int32_t));
              memcpy(dl_ch1_ext,dl_ch1p,6*sizeof(int32_t));
              memcpy(rxF_ext,rxF,6*sizeof(int32_t));
#ifdef DEBUG_DLSCH_DEMOD
              for (i=0; i<6; i++) {
                printf("extract rb %d, re %d => (%d,%d)\n",prb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
              }
#endif
              rxF_ext+=6;
              dl_ch0_ext+=6;
              dl_ch1_ext+=6;
            } else { // pilots==1
              j=0;

              for (i=0; i<6; i++) {
                if ((i!=frame_parms->nushift) &&
                    (i!=((frame_parms->nushift+3)%6))) {
                  dl_ch0_ext[j]=dl_ch0p[i];
                  dl_ch1_ext[j]=dl_ch1p[i];
                  rxF_ext[j++]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("(pilots)extract rb %d, re %d (%d) => (%d,%d)\n",prb,i,j,*(short *)&rxF[i],*(1+(short*)&rxF[i]));
#endif
                }
              }
              rxF       = &rxdataF[aarx][1+symbol*(frame_parms->ofdm_symbol_size)];

              for (; i<12; i++) {
                if ((i!=((frame_parms->nushift+6)%12)) &&
                    (i!=((frame_parms->nushift+9)%12))) {
                  dl_ch0_ext[j]=dl_ch0p[i];
                  dl_ch1_ext[j]=dl_ch1p[i];
                  rxF_ext[j++]=rxF[i-6];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("(pilots)extract rb %d, re %d (%d) => (%d,%d)\n",prb,i,j,*(short *)&rxF[1+i-6],*(1+(short*)&rxF[1+i-6]));
#endif
                }
              }

              dl_ch0_ext+=8;
              dl_ch1_ext+=8;
              rxF_ext+=8;
            } //pilots==1
          }  // if Middle PRB
        } // if odd PRB
      } // if rballoc==1
    } // for prb
  } // for aarx
  return(nb_rb/frame_parms->nb_antennas_rx);
}

unsigned short dlsch_extract_rbs_TM7(int **rxdataF,
                                     int **dl_bf_ch_estimates,
                                     int **rxdataF_ext,
                                     int **dl_bf_ch_estimates_ext,
                                     unsigned int *rb_alloc,
                                     unsigned char symbol,
                                     unsigned char subframe,
                                     uint32_t high_speed_flag,
                                     LTE_DL_FRAME_PARMS *frame_parms)
{

  unsigned short rb,nb_rb=0;
  unsigned char rb_alloc_ind;
  unsigned char i,aarx,l,nsymb,skip_half=0,sss_symb,pss_symb=0;
  int *dl_ch0,*dl_ch0_ext,*rxF,*rxF_ext;

  unsigned char symbol_mod,pilots=0,uespec_pilots=0,j=0,poffset=0,uespec_poffset=0;
  int8_t uespec_nushift = frame_parms->Nid_cell%3;

  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
  pilots = ((symbol_mod==0)||(symbol_mod==(4-frame_parms->Ncp))) ? 1 : 0;
  l=symbol;
  nsymb = (frame_parms->Ncp==NORMAL) ? 14:12;

  if (frame_parms->Ncp==0){
    if (symbol==3 || symbol==6 || symbol==9 || symbol==12)
      uespec_pilots = 1;
  } else{
    if (symbol==4 || symbol==7 || symbol==10)
      uespec_pilots = 1;
  }

  if (frame_parms->frame_type == TDD) {// TDD
    sss_symb = nsymb-1;
    pss_symb = 2;
  } else {
    sss_symb = (nsymb>>1)-2;
    pss_symb = (nsymb>>1)-1;
  }

  if (symbol_mod==(4-frame_parms->Ncp))
    poffset=3;

  if ((frame_parms->Ncp==0 && (symbol==6 ||symbol ==12)) || (frame_parms->Ncp==1 && symbol==7))
    uespec_poffset=2;

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

    if (high_speed_flag == 1)
    {
        dl_ch0 = dl_bf_ch_estimates[aarx];
    }
    else
    {
        dl_ch0 = dl_bf_ch_estimates[aarx];
    }

    dl_ch0_ext = dl_bf_ch_estimates_ext[aarx];

    rxF_ext = rxdataF_ext[aarx];
    rxF     = &rxdataF[aarx][(frame_parms->first_carrier_offset + (symbol*(frame_parms->ofdm_symbol_size)))];

    if ((frame_parms->N_RB_DL&1) == 0)  // even number of RBs
      for (rb=0; rb<frame_parms->N_RB_DL; rb++) {

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

  if (rb_alloc_ind == 1)
          nb_rb++;

        // For second half of RBs skip DC carrier
        if (rb==(frame_parms->N_RB_DL>>1)) {
          rxF       = &rxdataF[aarx][(1 + (symbol*(frame_parms->ofdm_symbol_size)))];
          //dl_ch0++;
        }

        // PBCH
        if ((subframe==0) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=nsymb>>1) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }

        //SSS
        if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }


        if (frame_parms->frame_type == FDD) {
          //PSS
          if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if ((frame_parms->frame_type == TDD) &&
            (subframe==6)) { //TDD Subframe 6
          if ((rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }
        }

        if (rb_alloc_ind==1) {

          /*
              printf("rb %d\n",rb);
              for (i=0;i<12;i++)
              printf("(%d %d)",((short *)dl_ch0)[i<<1],((short*)dl_ch0)[1+(i<<1)]);
              printf("\n");
          */
          if (pilots==0 && uespec_pilots==0) {
            memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));

            for (i=0; i<12; i++) {
              rxF_ext[i]=rxF[i];
            }

            dl_ch0_ext+=12;
            rxF_ext+=12;
          } else if(pilots==1 && uespec_pilots==0) {
            j=0;

            for (i=0; i<12; i++) {
              if ((i!=(frame_parms->nushift+poffset)) &&
                  (i!=((frame_parms->nushift+poffset+6)%12))) {
                rxF_ext[j]=rxF[i];
                dl_ch0_ext[j++]=dl_ch0[i];
              }
            }

            dl_ch0_ext+=10;
            rxF_ext+=10;

          } else if (pilots==0 && uespec_pilots==1) {
            j=0;


      for (i=0; i<12; i++){
              if (frame_parms->Ncp==0){
                if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
      rxF_ext[j] = rxF[i];
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              } else{
                if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
      rxF_ext[j] = rxF[i];
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              }

      }

            dl_ch0_ext+=9-frame_parms->Ncp;
            rxF_ext+=9-frame_parms->Ncp;

          } else {
            msg("dlsch_extract_rbs_TM7(dl_demodulation.c):pilot or ue spec pilot detection error\n");
            exit(-1);
          }

        }

        dl_ch0+=12;
        rxF+=12;

      }
    else {  // Odd number of RBs
      for (rb=0; rb<frame_parms->N_RB_DL>>1; rb++) {
        skip_half=0;

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

        if (rb_alloc_ind == 1)
          nb_rb++;

        // PBCH
        if ((subframe==0) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }

        //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
        if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=1;
        else if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=2;

        //SSS

        if (((subframe==0)||(subframe==5)) &&
            (rb>((frame_parms->N_RB_DL>>1)-3)) &&
            (rb<((frame_parms->N_RB_DL>>1)+3)) &&
            (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }

        //SSS
        if (((subframe==0)||(subframe==5)) &&
            (rb==((frame_parms->N_RB_DL>>1)-3)) &&
            (l==sss_symb))
          skip_half=1;
        else if (((subframe==0)||(subframe==5)) &&
                 (rb==((frame_parms->N_RB_DL>>1)+3)) &&
                 (l==sss_symb))
          skip_half=2;

        //PSS in subframe 0/5 if FDD
        if (frame_parms->frame_type == FDD) {  //FDD
          if (((subframe==0)||(subframe==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if ((frame_parms->frame_type == TDD) && ((subframe==1)||(subframe==6))) { //TDD Subframe 1 and 6
          if ((rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if ((rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if ((rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }


        if (rb_alloc_ind==1) {
#ifdef DEBUG_DLSCH_DEMOD
          printf("rb %d/symbol %d pilots %d, uespec_pilots %d, (skip_half %d)\n",rb,l,pilots,uespec_pilots,skip_half);
#endif

          if (pilots==0 && uespec_pilots==0) {
            //printf("Extracting w/o pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);

            if (skip_half==1) {
              memcpy(dl_ch0_ext,dl_ch0,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
    printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }

              dl_ch0_ext+=6;
              rxF_ext+=6;
            } else if (skip_half==2) {
              memcpy(dl_ch0_ext,dl_ch0+6,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[(i+6)];
#ifdef DEBUG_DLSCH_DEMOD
    printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }

              dl_ch0_ext+=6;
              rxF_ext+=6;
            } else {
              memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));

              for (i=0; i<12; i++){
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                printf("extract rb %d, re %d => (%d,%d)\n",symbol,rb,i,*(short *)&rxF[i],*(1+(short*)&rxF[i]));
#endif
              }
              dl_ch0_ext+=12;
              rxF_ext+=12;
            }
          } else if (pilots==1 && uespec_pilots==0) {
            // printf("Extracting with pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            j=0;

            if (skip_half==1) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[i];
                  dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
    printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else if (skip_half==2) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[(i+6)];
                  dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
    printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else {
              for (i=0; i<12; i++) {
                if ((i!=(frame_parms->nushift+poffset)) &&
                    (i!=((frame_parms->nushift+poffset+6)%12))) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];

                }
              }

              dl_ch0_ext+=10;
              rxF_ext+=10;
            }
          } else if(pilots==0 && uespec_pilots==1){
            //printf("Extracting with uespec pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            j=0;

            if (skip_half==1) {
              if (frame_parms->Ncp==0){
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                    rxF_ext[j]=rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=6-(uespec_nushift+uespec_poffset<6)-(uespec_nushift+uespec_poffset+4<6)-((uespec_nushift+uespec_poffset+8)%12<6);
                rxF_ext+=6-(uespec_nushift+uespec_poffset<6)-(uespec_nushift+uespec_poffset+4<6)-((uespec_nushift+uespec_poffset+8)%12<6);

              } else{
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                    rxF_ext[j]=rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=4;
                rxF_ext+=4;
              }

            } else if (skip_half==2) {
              if(frame_parms->Ncp==0){
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                    rxF_ext[j]=rxF[(i+6)];
                    dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=6-(uespec_nushift+uespec_poffset>6)-(uespec_nushift+uespec_poffset+4>6)-((uespec_nushift+uespec_poffset+8)%12>6);
                rxF_ext+=6-(uespec_nushift+uespec_poffset>6)-(uespec_nushift+uespec_poffset+4>6)-((uespec_nushift+uespec_poffset+8)%12>6);

              } else {
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                    rxF_ext[j]=rxF[(i+6)];
                    dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=4;
                rxF_ext+=4;
              }

            } else {

        for (i=0; i<12; i++){
                if (frame_parms->Ncp==0){
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
              rxF_ext[j] = rxF[i];
                    dl_ch0_ext[j++] = dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
                    printf("extract rb %d, re %d, j %d => (%d,%d)\n",symbol,rb,i,j-1,*(short *)&dl_ch0[j],*(1+(short*)&dl_ch0[i]));
#endif
                  }
                } else{
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
              rxF_ext[j] = rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }

        }

              dl_ch0_ext+=9-frame_parms->Ncp;
              rxF_ext+=9-frame_parms->Ncp;
      }

          } else {
            msg("dlsch_extract_rbs_TM7(dl_demodulation.c):pilot or ue spec pilot detection error\n");
            exit(-1);

          }
        }

        dl_ch0+=12;
        rxF+=12;
      } // first half loop


      // Do middle RB (around DC)
      if (rb < 32)
        rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
      else if (rb < 64)
        rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
      else if (rb < 96)
        rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
      else if (rb < 100)
        rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
      else
        rb_alloc_ind = 0;

      if (rb_alloc_ind == 1)
        nb_rb++;

      // PBCH
      if ((subframe==0) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4))) {
        rb_alloc_ind = 0;
      }

      //SSS
      if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb) ) {
        rb_alloc_ind = 0;
      }

      if (frame_parms->frame_type == FDD) {
        //PSS
        if (((subframe==0)||(subframe==5)) && (rb>=((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
          rb_alloc_ind = 0;
        }
      }

      if ((frame_parms->frame_type == TDD) && ((subframe==1)||(subframe==6))) {
        //PSS
        if ((rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
          rb_alloc_ind = 0;
        }
      }

      //printf("dlch_ext %d\n",dl_ch0_ext-&dl_ch_estimates_ext[aarx][0]);
      //printf("DC rb %d (%p)\n",rb,rxF);
      if (rb_alloc_ind==1) {
        //printf("rb %d/symbol %d (skip_half %d)\n",rb,l,skip_half);
        if (pilots==0 && uespec_pilots==0) {
          for (i=0; i<6; i++) {
            dl_ch0_ext[i]=dl_ch0[i];
            rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
      printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
          }

          rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];

          for (; i<12; i++) {
            dl_ch0_ext[i]=dl_ch0[i];
            rxF_ext[i]=rxF[(1+i-6)];
#ifdef DEBUG_DLSCH_DEMOD
      printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
          }

          dl_ch0_ext+=12;
          rxF_ext+=12;
        } else if(pilots==1 && uespec_pilots==0){ // pilots==1
          j=0;

          for (i=0; i<6; i++) {
            if (i!=((frame_parms->nushift+poffset)%6)) {
              dl_ch0_ext[j]=dl_ch0[i];
              rxF_ext[j++]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
            }
          }

          rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];

          for (; i<12; i++) {
            if (i!=((frame_parms->nushift+6+poffset)%12)) {
              dl_ch0_ext[j]=dl_ch0[i];
              rxF_ext[j++]=rxF[(1+i-6)];
#ifdef DEBUG_DLSCH_DEMOD
        printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
            }
          }

          dl_ch0_ext+=10;
          rxF_ext+=10;
        } else if(pilots==0 && uespec_pilots==1) {
          j=0;

    for (i=0; i<6; i++) {
            if (frame_parms->Ncp==0){
              if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                dl_ch0_ext[j]=dl_ch0[i];
          rxF_ext[j++] = rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
            } else {
              if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                dl_ch0_ext[j]=dl_ch0[i];
          rxF_ext[j++] = rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
            }
    }

          rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];

          for (; i<12; i++) {
            if (frame_parms->Ncp==0){
              if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                dl_ch0_ext[j]=dl_ch0[i];
                rxF_ext[j++]=rxF[(1+i-6)];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
            } else {
              if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                dl_ch0_ext[j]=dl_ch0[i];
          rxF_ext[j++] = rxF[(1+i-6)];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }
            }
          }

          dl_ch0_ext+=9-frame_parms->Ncp;
          rxF_ext+=9-frame_parms->Ncp;

  }// symbol_mod==0

      } // rballoc==1
      else {
        rxF       = &rxdataF[aarx][((symbol*(frame_parms->ofdm_symbol_size)))];
      }

      dl_ch0+=12;
      rxF+=7;
      rb++;

      for (; rb<frame_parms->N_RB_DL; rb++) {
        //  printf("dlch_ext %d\n",dl_ch0_ext-&dl_ch_estimates_ext[aarx][0]);
        //  printf("rb %d (%p)\n",rb,rxF);
        skip_half=0;

        if (rb < 32)
          rb_alloc_ind = (rb_alloc[0]>>rb) & 1;
        else if (rb < 64)
          rb_alloc_ind = (rb_alloc[1]>>(rb-32)) & 1;
        else if (rb < 96)
          rb_alloc_ind = (rb_alloc[2]>>(rb-64)) & 1;
        else if (rb < 100)
          rb_alloc_ind = (rb_alloc[3]>>(rb-96)) & 1;
        else
          rb_alloc_ind = 0;

        if (rb_alloc_ind==1)
          nb_rb++;

        // PBCH
        if ((subframe==0) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l>=nsymb>>1) && (l<((nsymb>>1) + 4))) {
          rb_alloc_ind = 0;
        }

        //PBCH subframe 0, symbols nsymb>>1 ... nsymb>>1 + 3
        if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=1;
        else if ((subframe==0) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l>=(nsymb>>1)) && (l<((nsymb>>1) + 4)))
          skip_half=2;

        //SSS
        if (((subframe==0)||(subframe==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb) ) {
          rb_alloc_ind = 0;
        }

        //SSS
        if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==sss_symb))
          skip_half=1;
        else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==sss_symb))
          skip_half=2;

        //PSS
        if (frame_parms->frame_type == FDD) {
          if (((subframe==0)||(subframe==5)) && (rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if (((subframe==0)||(subframe==5)) && (rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if ((frame_parms->frame_type == TDD) && ((subframe==1)||(subframe==6))) { //TDD Subframe 1 and 6
          if ((rb>((frame_parms->N_RB_DL>>1)-3)) && (rb<((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb) ) {
            rb_alloc_ind = 0;
          }

          if ((rb==((frame_parms->N_RB_DL>>1)-3)) && (l==pss_symb))
            skip_half=1;
          else if ((rb==((frame_parms->N_RB_DL>>1)+3)) && (l==pss_symb))
            skip_half=2;
        }

        if (rb_alloc_ind==1) {
#ifdef DEBUG_DLSCH_DEMOD
           printf("rb %d/symbol %d (skip_half %d)\n",rb,l,skip_half);
#endif
          /*
              printf("rb %d\n",rb);
            for (i=0;i<12;i++)
            printf("(%d %d)",((short *)dl_ch0)[i<<1],((short*)dl_ch0)[1+(i<<1)]);
            printf("\n");
          */
          if (pilots==0 && uespec_pilots==0) {
            //printf("Extracting w/o pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            if (skip_half==1) {
              memcpy(dl_ch0_ext,dl_ch0,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }

              dl_ch0_ext+=6;
              rxF_ext+=6;

            } else if (skip_half==2) {
              memcpy(dl_ch0_ext,dl_ch0+6,6*sizeof(int));

              for (i=0; i<6; i++) {
                rxF_ext[i]=rxF[i+6];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }

              dl_ch0_ext+=6;
              rxF_ext+=6;

            } else {
              memcpy(dl_ch0_ext,dl_ch0,12*sizeof(int));
              //printf("symbol %d, extract rb %d, => (%d,%d)\n",symbol,rb,*(short *)&dl_ch0[j],*(1+(short*)&dl_ch0[i]));

              for (i=0; i<12; i++) {
                rxF_ext[i]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
          printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
              }

              dl_ch0_ext+=12;
              rxF_ext+=12;
            }
          } else if (pilots==1 && uespec_pilots==0){
            //printf("Extracting with pilots (symbol %d, rb %d, skip_half %d)\n",l,rb,skip_half);
            j=0;

            if (skip_half==1) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[i];
                  dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
            printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else if (skip_half==2) {
              for (i=0; i<6; i++) {
                if (i!=((frame_parms->nushift+poffset)%6)) {
                  rxF_ext[j]=rxF[(i+6)];
                  dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
            printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                }
              }

              dl_ch0_ext+=5;
              rxF_ext+=5;
            } else {
              for (i=0; i<12; i++) {
                if ((i!=(frame_parms->nushift+poffset)) &&
                    (i!=((frame_parms->nushift+poffset+6)%12))) {
                  rxF_ext[j]=rxF[i];
#ifdef DEBUG_DLSCH_DEMOD
                  printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[j],*(1+(short*)&rxF_ext[j]));
#endif
                  dl_ch0_ext[j++]=dl_ch0[i];
                }
              }

              dl_ch0_ext+=10;
              rxF_ext+=10;
            }
          } else if(pilots==0 && uespec_pilots==1) {
            j=0;

            if (skip_half==1) {
              if (frame_parms->Ncp==0){
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                    rxF_ext[j]=rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=6-(uespec_nushift+uespec_poffset<6)-(uespec_nushift+uespec_poffset+4<6)-((uespec_nushift+uespec_poffset+8)%12<6);
                rxF_ext+=6-(uespec_nushift+uespec_poffset<6)-(uespec_nushift+uespec_poffset+4<6)-((uespec_nushift+uespec_poffset+8)%12<6);

              } else{
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                    rxF_ext[j]=rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=4;
                rxF_ext+=4;
              }

            } else if (skip_half==2) {
              if(frame_parms->Ncp==0){
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
                    rxF_ext[j]=rxF[i+6];
                    dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=6-(uespec_nushift+uespec_poffset>6)-(uespec_nushift+uespec_poffset+4>6)-((uespec_nushift+uespec_poffset+8)%12>6);
                rxF_ext+=6-(uespec_nushift+uespec_poffset>6)-(uespec_nushift+uespec_poffset+4>6)-((uespec_nushift+uespec_poffset+8)%12>6);

              } else {
                for (i=0; i<6; i++) {
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
                    rxF_ext[j]=rxF[(i+6)];
                    dl_ch0_ext[j++]=dl_ch0[i+6];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
                dl_ch0_ext+=4;
                rxF_ext+=4;
              }

            } else {
        for (i=0; i<12; i++){
                if (frame_parms->Ncp==0){
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+4 && i!=(uespec_nushift+uespec_poffset+8)%12){
              rxF_ext[j] = rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                } else{
                  if (i!=uespec_nushift+uespec_poffset && i!=uespec_nushift+uespec_poffset+3 && i!=uespec_nushift+uespec_poffset+6 && i!=(uespec_nushift+uespec_poffset+9)%12){
              rxF_ext[j] = rxF[i];
                    dl_ch0_ext[j++]=dl_ch0[i];
#ifdef DEBUG_DLSCH_DEMOD
              printf("extract rb %d, re %d => (%d,%d)\n",rb,i,*(short *)&rxF_ext[i],*(1+(short*)&rxF_ext[i]));
#endif
                  }
                }
        }

              dl_ch0_ext+=9-frame_parms->Ncp;
              rxF_ext+=9-frame_parms->Ncp;

            }

          }// pilots=0
        }

        dl_ch0+=12;
        rxF+=12;
      }
    }
  }

  _mm_empty();
  _m_empty();

  return(nb_rb/frame_parms->nb_antennas_rx);
}

//==============================================================================================

#ifdef USER_MODE


void dump_dlsch2(PHY_VARS_UE *ue,uint8_t eNB_id,uint8_t subframe,unsigned int *coded_bits_per_codeword,int round,  unsigned char harq_pid)
{
    unsigned int nsymb = (ue->frame_parms.Ncp == 0) ? 14 : 12;
    char fname[32],vname[32];
    int N_RB_DL=ue->frame_parms.N_RB_DL;
    uint32_t l;

    sprintf(fname,"dlsch%d_rxF_r%d_ext0.m",eNB_id,round);
    sprintf(vname,"dl%d_rxF_r%d_ext0",eNB_id,round);

    for (l=0; l<nsymb; l++)
    {
        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->rxdataF_ext[l][0],12*N_RB_DL,10,1);

        if (ue->frame_parms.nb_antennas_rx >1)
        {
            if (l==0)
            {
                sprintf(fname,"dlsch%d_rxF_r%d_ext1.m",eNB_id,round);
                sprintf(vname,"dl%d_rxF_r%d_ext1",eNB_id,round);
            }
            write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->rxdataF_ext[l][1],12*N_RB_DL,10,1);
        }

        if (l==0)
        {
            sprintf(fname,"dlsch%d_ch_r%d_ext00.m",eNB_id,round);
            sprintf(vname,"dl%d_ch_r%d_ext00",eNB_id,round);
        }

        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_estimates_ext[l][0],12*N_RB_DL,10,1);

        if (ue->transmission_mode[eNB_id]==7)
        {
            if (l==0)
            {
                sprintf(fname,"dlsch%d_bf_ch_r%d.m",eNB_id,round);
                sprintf(vname,"dl%d_bf_ch_r%d",eNB_id,round);
            }
            write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_bf_ch_estimates[l][0], 12*N_RB_DL,10,1);
            //write_output(fname,vname,phy_vars_ue->lte_ue_pdsch_vars[eNB_id]->dl_bf_ch_estimates[0],512,1,1);

            if (l==0)
            {
                sprintf(fname,"dlsch%d_bf_ch_r%d_ext00.m",eNB_id,round);
                sprintf(vname,"dl%d_bf_ch_r%d_ext00",eNB_id,round);
            }
            write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_bf_ch_estimates_ext[l][0],12*N_RB_DL,10,1);
        }

        if (ue->frame_parms.nb_antennas_rx == 2)
        {
            if (l==0)
            {
                sprintf(fname,"dlsch%d_ch_r%d_ext01.m",eNB_id,round);
                sprintf(vname,"dl%d_ch_r%d_ext01",eNB_id,round);
            }
            write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_estimates_ext[l][1],12*N_RB_DL,1,1);
        }

        if (ue->frame_parms.nb_antenna_ports_eNB == 2)
        {
            if (l==0)
            {
                sprintf(fname,"dlsch%d_ch_r%d_ext10.m",eNB_id,round);
                sprintf(vname,"dl%d_ch_r%d_ext10",eNB_id,round);
            }
            write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_estimates_ext[l][2],12*N_RB_DL,10,1);

            if (ue->frame_parms.nb_antennas_rx == 2)
            {
                if (l==0)
                {
                    sprintf(fname,"dlsch%d_ch_r%d_ext11.m",eNB_id,round);
                    sprintf(vname,"dl%d_ch_r%d_ext11",eNB_id,round);
                }
                write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_estimates_ext[l][3],12*N_RB_DL,10,1);
            }
        }

        if (l==0)
        {
            sprintf(fname,"dlsch%d_rxF_r%d_uespec0.m",eNB_id,round);
            sprintf(vname,"dl%d_rxF_r%d_uespec0",eNB_id,round);
        }

        /*
          write_output("dlsch%d_ch_ext01.m","dl01_ch0_ext",pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][1],12*N_RB_DL,10,1);
          write_output("dlsch%d_ch_ext10.m","dl10_ch0_ext",pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][2],12*N_RB_DL,10,1);
          write_output("dlsch%d_ch_ext11.m","dl11_ch0_ext",pdsch_vars[eNB_id]->dl_ch_estimates_ext[l][3],12*N_RB_DL,10,1);
        */

        if (l==0)
        {
            sprintf(fname,"dlsch%d_rxF_r%d_comp0.m",eNB_id,round);
            sprintf(vname,"dl%d_rxF_r%d_comp0",eNB_id,round);
        }
        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->rxdataF_comp0[l][0],12*N_RB_DL,10,1);

        if (l==0)
        {
            sprintf(fname,"dlsch%d_r%d_mag1.m",eNB_id,round);
            sprintf(vname,"dl%d_r%d_mag1",eNB_id,round);
        }
        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_mag0[l][0],12*N_RB_DL,10,1);

        if (l==0)
        {
            sprintf(fname,"dlsch%d_r%d_mag2.m",eNB_id,round);
            sprintf(vname,"dl%d_r%d_mag2",eNB_id,round);
        }
        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_magb0[l][0],12*N_RB_DL,10,1);

    } // l

    sprintf(fname,"dlsch%d_r%d_rho.m",eNB_id,round);
    sprintf(vname,"dl_rho_r%d_%d",eNB_id,round);
    write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_rho_ext[harq_pid][round][0],12*N_RB_DL*nsymb,1,1);

    sprintf(fname,"dlsch%d_r%d_rho2.m",eNB_id,round);
    sprintf(vname,"dl_rho2_r%d_%d",eNB_id,round);
    write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->dl_ch_rho2_ext[0],12*N_RB_DL*nsymb,1,1);

    if (ue->frame_parms.nb_antenna_ports_eNB == 2)
    {
        sprintf(fname,"dlsch%d_rxF_r%d_comp1.m",eNB_id,round);
        sprintf(vname,"dl%d_rxF_r%d_comp1",eNB_id,round);
        write_output(fname,vname,ue->pdsch_vars[subframe&0x1][eNB_id]->rxdataF_comp1[harq_pid][round][0],12*N_RB_DL*nsymb,1,1);
    }

  //  printf("log2_maxh = %d\n",ue->pdsch_vars[eNB_id]->log2_maxh);
}
#endif

#ifdef DEBUG_DLSCH_DEMOD
/*
void print_bytes(char *s,__m128i *x)
{

  char *tempb = (char *)x;

  printf("%s  : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",s,
         tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7],
         tempb[8],tempb[9],tempb[10],tempb[11],tempb[12],tempb[13],tempb[14],tempb[15]
         );

}

void print_shorts(char *s,__m128i *x)
{

  short *tempb = (short *)x;
  printf("%s  : %d,%d,%d,%d,%d,%d,%d,%d\n",s,
         tempb[0],tempb[1],tempb[2],tempb[3],tempb[4],tempb[5],tempb[6],tempb[7]);

}

void print_shorts2(char *s,__m64 *x)
{

  short *tempb = (short *)x;
  printf("%s  : %d,%d,%d,%d\n",s,
         tempb[0],tempb[1],tempb[2],tempb[3]);

}

void print_ints(char *s,__m128i *x)
{

  int *tempb = (int *)x;
  printf("%s  : %d,%d,%d,%d\n",s,
         tempb[0],tempb[1],tempb[2],tempb[3]);

}*/
#endif
