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

/*! \file PHY/LTE_TRANSPORT/dlsch_scrambling_NB_IoT.c
* \brief Routines for the scrambling procedure of the NPDSCH physical channel for NB_IoT,	 TS 36-211, V13.4.0 2017-02
* \author M. KANJ
* \date 2017
* \version 0.0
* \company bcom
* \email: matthieu.kanj@b-com.com
* \note
* \warning
*/

//#define DEBUG_SCRAMBLING 1

//#include "PHY/defs.h"
//#include "PHY/defs_NB_IoT.h"
//#include "PHY/CODING/extern.h"
//#include "PHY/CODING/lte_interleaver_inline.h"
//#include "defs.h"
//#include "extern_NB_IoT.h"
//#include "PHY/extern_NB_IoT.h"
//#include "UTIL/LOG/vcd_signal_dumper.h"

#include "PHY/LTE_TRANSPORT/defs_NB_IoT.h"
#include "PHY/impl_defs_lte_NB_IoT.h"
#include "PHY/impl_defs_lte.h"
#include "PHY/LTE_REFSIG/defs_NB_IoT.h"
#include "openair1/PHY/extern_NB_IoT.h"

void dlsch_sib_scrambling_NB_IoT(LTE_DL_FRAME_PARMS     *frame_parms,
							                    NB_IoT_DL_eNB_SIB_t    *dlsch,
                                  int                    tot_bits,                // total number of bits to transmit
                                  uint16_t                Nf,              // Nf is the frame number (0..9)
                                  uint8_t                Ns)  
{
  int         i,j,k=0;
  uint32_t    x1,x2, s=0;
  uint8_t     *e = dlsch->e; 															//uint8_t *e=dlsch->harq_processes[dlsch->current_harq_pid]->e;

   //x2 = (dlsch->si_rnti<<15) + (frame_parms->Nid_cell + 1) * ( (Nf % 61) + 1 ) ;
  x2 = (dlsch->si_rnti<<14) + ((Nf%2)<<13) + ((Ns>>1)<<9) + frame_parms->Nid_cell;
  // for NPDSCH not carriying SIBs
  //x2 = (dlsch->harq_process_sib1.rnti<<14) + ((Nf%2)<<13) + ((Ns>>1)<<9) + frame_parms->Nid_cell;   //this is c_init in 36.211 Sec 10.2.3.1
  
  s = lte_gold_generic_NB_IoT(&x1, &x2, 1);

  for (i=0; i<(1+(tot_bits>>5)); i++) {

    for (j=0; j<32; j++,k++) {

      dlsch->s_e[k] = (e[k]&1) ^ ((s>>j)&1);

    }
    s = lte_gold_generic_NB_IoT(&x1, &x2, 0);
  }

}

void dlsch_sib_scrambling_rar_NB_IoT(LTE_DL_FRAME_PARMS     *frame_parms,
                                  NB_IoT_DL_eNB_RAR_t    *dlsch,
                                  int                    tot_bits,                // total number of bits to transmit
                                  uint16_t                Nf,              // Nf is the frame number (0..9)
                                  uint8_t                Ns,
                                  uint32_t               rnti)  
{
  int         i,j,k=0;
  uint32_t    x1,x2, s=0;
  uint8_t     *e = dlsch->e;                              //uint8_t *e=dlsch->harq_processes[dlsch->current_harq_pid]->e;

   //x2 = (dlsch->si_rnti<<15) + (frame_parms->Nid_cell + 1) * ( (Nf % 61) + 1 ) ;
  x2 = (rnti<<14) + ((Nf%2)<<13) + ((Ns>>1)<<9) + frame_parms->Nid_cell;
  // for NPDSCH not carriying SIBs
  //x2 = (dlsch->harq_process_sib1.rnti<<14) + ((Nf%2)<<13) + ((Ns>>1)<<9) + frame_parms->Nid_cell;   //this is c_init in 36.211 Sec 10.2.3.1
  
  s = lte_gold_generic_NB_IoT(&x1, &x2, 1);

  for (i=0; i<(1+(tot_bits>>5)); i++) {

    for (j=0; j<32; j++,k++) {

      dlsch->s_e[k] = (e[k]&1) ^ ((s>>j)&1);

    }
    s = lte_gold_generic_NB_IoT(&x1, &x2, 0);
  }

}


void init_unscrambling_lut_NB_IoT() {

  uint32_t s;
  int i=0,j;

  for (s=0;s<=65535;s++) {
    for (j=0;j<16;j++) {
      unscrambling_lut_NB_IoT[i++] = (int16_t)((((s>>j)&1)<<1)-1);
    }
  }
}
