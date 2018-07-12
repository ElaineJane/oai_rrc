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

/*! \file PHY/LTE_TRANSPORT/ulsch_demodulation.c
* \brief Top-level routines for demodulating the PUSCH physical channel from 36.211 V8.6 2009-03
* \authors V. Savaux, M. Kanj
* \date 2018
* \version 0.1
* \company b<>com
* \email: vincent.savaux@b-com.com , matthieu.kanj@b-com.com
* \note
* \warning
*/

#include "PHY/defs_NB_IoT.h"
#include "PHY/extern_NB_IoT.h"
#include "defs_NB_IoT.h"
#include "extern_NB_IoT.h"
//#define DEBUG_ULSCH
//#include "PHY/sse_intrin.h"
#include "PHY/LTE_ESTIMATION/defs_NB_IoT.h"
#include "T.h"

//extern char* namepointer_chMag ;
//eren
//extern int **ulchmag_eren;
//eren

static short jitter[8]  __attribute__ ((aligned(16))) = {1,0,0,1,0,1,1,0};
static short jitterc[8] __attribute__ ((aligned(16))) = {0,1,1,0,1,0,0,1};

#ifndef OFDMA_ULSCH
void lte_idft_NB_IoT(LTE_DL_FRAME_PARMS *frame_parms,uint32_t *z, uint16_t Msc_PUSCH)
{
#if defined(__x86_64__) || defined(__i386__)
  __m128i idft_in128[3][1200],idft_out128[3][1200];
  __m128i norm128;
#elif defined(__arm__)
  int16x8_t idft_in128[3][1200],idft_out128[3][1200];
  int16x8_t norm128;
#endif
  int16_t *idft_in0=(int16_t*)idft_in128[0],*idft_out0=(int16_t*)idft_out128[0];
  int16_t *idft_in1=(int16_t*)idft_in128[1],*idft_out1=(int16_t*)idft_out128[1];
  int16_t *idft_in2=(int16_t*)idft_in128[2],*idft_out2=(int16_t*)idft_out128[2];

  uint32_t *z0,*z1,*z2,*z3,*z4,*z5,*z6,*z7,*z8,*z9,*z10=NULL,*z11=NULL;
  int i,ip;

  //  printf("Doing lte_idft for Msc_PUSCH %d\n",Msc_PUSCH);

   // Normal prefix
    z0 = z;
    z1 = z0+(frame_parms->N_RB_DL*12);
    z2 = z1+(frame_parms->N_RB_DL*12);
    //pilot
    z3 = z2+(2*frame_parms->N_RB_DL*12);
    z4 = z3+(frame_parms->N_RB_DL*12);
    z5 = z4+(frame_parms->N_RB_DL*12);

    z6 = z5+(frame_parms->N_RB_DL*12);
    z7 = z6+(frame_parms->N_RB_DL*12);
    z8 = z7+(frame_parms->N_RB_DL*12);
    //pilot
    z9 = z8+(2*frame_parms->N_RB_DL*12);
    z10 = z9+(frame_parms->N_RB_DL*12);
    // srs
    z11 = z10+(frame_parms->N_RB_DL*12);
  
  // conjugate input
  for (i=0; i<(Msc_PUSCH>>2); i++) {
#if defined(__x86_64__)||defined(__i386__)
    *&(((__m128i*)z0)[i])=_mm_sign_epi16(*&(((__m128i*)z0)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z1)[i])=_mm_sign_epi16(*&(((__m128i*)z1)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z2)[i])=_mm_sign_epi16(*&(((__m128i*)z2)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z3)[i])=_mm_sign_epi16(*&(((__m128i*)z3)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z4)[i])=_mm_sign_epi16(*&(((__m128i*)z4)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z5)[i])=_mm_sign_epi16(*&(((__m128i*)z5)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z6)[i])=_mm_sign_epi16(*&(((__m128i*)z6)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z7)[i])=_mm_sign_epi16(*&(((__m128i*)z7)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z8)[i])=_mm_sign_epi16(*&(((__m128i*)z8)[i]),*(__m128i*)&conjugate2[0]);
    *&(((__m128i*)z9)[i])=_mm_sign_epi16(*&(((__m128i*)z9)[i]),*(__m128i*)&conjugate2[0]);

    // if (frame_parms->Ncp==NORMAL_NB_IoT) {
      *&(((__m128i*)z10)[i])=_mm_sign_epi16(*&(((__m128i*)z10)[i]),*(__m128i*)&conjugate2[0]);
      *&(((__m128i*)z11)[i])=_mm_sign_epi16(*&(((__m128i*)z11)[i]),*(__m128i*)&conjugate2[0]);
    // }
#elif defined(__arm__)
    *&(((int16x8_t*)z0)[i])=vmulq_s16(*&(((int16x8_t*)z0)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z1)[i])=vmulq_s16(*&(((int16x8_t*)z1)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z2)[i])=vmulq_s16(*&(((int16x8_t*)z2)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z3)[i])=vmulq_s16(*&(((int16x8_t*)z3)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z4)[i])=vmulq_s16(*&(((int16x8_t*)z4)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z5)[i])=vmulq_s16(*&(((int16x8_t*)z5)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z6)[i])=vmulq_s16(*&(((int16x8_t*)z6)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z7)[i])=vmulq_s16(*&(((int16x8_t*)z7)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z8)[i])=vmulq_s16(*&(((int16x8_t*)z8)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z9)[i])=vmulq_s16(*&(((int16x8_t*)z9)[i]),*(int16x8_t*)&conjugate2[0]);


    // if (frame_parms->Ncp==NORMAL_NB_IoT) {
      *&(((int16x8_t*)z10)[i])=vmulq_s16(*&(((int16x8_t*)z10)[i]),*(int16x8_t*)&conjugate2[0]);
      *&(((int16x8_t*)z11)[i])=vmulq_s16(*&(((int16x8_t*)z11)[i]),*(int16x8_t*)&conjugate2[0]);
    // }

#endif
  }

  for (i=0,ip=0; i<Msc_PUSCH; i++,ip+=4) {
    ((uint32_t*)idft_in0)[ip+0] =  z0[i];
    ((uint32_t*)idft_in0)[ip+1] =  z1[i];
    ((uint32_t*)idft_in0)[ip+2] =  z2[i];
    ((uint32_t*)idft_in0)[ip+3] =  z3[i];
    ((uint32_t*)idft_in1)[ip+0] =  z4[i];
    ((uint32_t*)idft_in1)[ip+1] =  z5[i];
    ((uint32_t*)idft_in1)[ip+2] =  z6[i];
    ((uint32_t*)idft_in1)[ip+3] =  z7[i];
    ((uint32_t*)idft_in2)[ip+0] =  z8[i];
    ((uint32_t*)idft_in2)[ip+1] =  z9[i];

    // if (frame_parms->Ncp==0) {
      ((uint32_t*)idft_in2)[ip+2] =  z10[i];
      ((uint32_t*)idft_in2)[ip+3] =  z11[i];
    // }
  }


  switch (Msc_PUSCH) {
  case 12:
    dft12((int16_t *)idft_in0,(int16_t *)idft_out0);
    dft12((int16_t *)idft_in1,(int16_t *)idft_out1);
    dft12((int16_t *)idft_in2,(int16_t *)idft_out2);

#if defined(__x86_64__)||defined(__i386__)
    norm128 = _mm_set1_epi16(9459);
#elif defined(__arm__)
    norm128 = vdupq_n_s16(9459);
#endif
    for (i=0; i<12; i++) {
#if defined(__x86_64__)||defined(__i386__)
      ((__m128i*)idft_out0)[i] = _mm_slli_epi16(_mm_mulhi_epi16(((__m128i*)idft_out0)[i],norm128),1);
      ((__m128i*)idft_out1)[i] = _mm_slli_epi16(_mm_mulhi_epi16(((__m128i*)idft_out1)[i],norm128),1);
      ((__m128i*)idft_out2)[i] = _mm_slli_epi16(_mm_mulhi_epi16(((__m128i*)idft_out2)[i],norm128),1);
#elif defined(__arm__)
      ((int16x8_t*)idft_out0)[i] = vqdmulhq_s16(((int16x8_t*)idft_out0)[i],norm128);
      ((int16x8_t*)idft_out1)[i] = vqdmulhq_s16(((int16x8_t*)idft_out1)[i],norm128);
      ((int16x8_t*)idft_out2)[i] = vqdmulhq_s16(((int16x8_t*)idft_out2)[i],norm128);
#endif
    }

    break;

  // case 24:
  //   dft24(idft_in0,idft_out0,1);
  //   dft24(idft_in1,idft_out1,1);
  //   dft24(idft_in2,idft_out2,1);
  //   break;

  // case 36:
  //   dft36(idft_in0,idft_out0,1);
  //   dft36(idft_in1,idft_out1,1);
  //   dft36(idft_in2,idft_out2,1);
  //   break;

  // case 48:
  //   dft48(idft_in0,idft_out0,1);
  //   dft48(idft_in1,idft_out1,1);
  //   dft48(idft_in2,idft_out2,1);
  //   break;

  // case 60:
  //   dft60(idft_in0,idft_out0,1);
  //   dft60(idft_in1,idft_out1,1);
  //   dft60(idft_in2,idft_out2,1);
  //   break;

  // case 72:
  //   dft72(idft_in0,idft_out0,1);
  //   dft72(idft_in1,idft_out1,1);
  //   dft72(idft_in2,idft_out2,1);
  //   break;

  // case 96:
  //   dft96(idft_in0,idft_out0,1);
  //   dft96(idft_in1,idft_out1,1);
  //   dft96(idft_in2,idft_out2,1);
  //   break;

  // case 108:
  //   dft108(idft_in0,idft_out0,1);
  //   dft108(idft_in1,idft_out1,1);
  //   dft108(idft_in2,idft_out2,1);
  //   break;

  // case 120:
  //   dft120(idft_in0,idft_out0,1);
  //   dft120(idft_in1,idft_out1,1);
  //   dft120(idft_in2,idft_out2,1);
  //   break;

  // case 144:
  //   dft144(idft_in0,idft_out0,1);
  //   dft144(idft_in1,idft_out1,1);
  //   dft144(idft_in2,idft_out2,1);
  //   break;

  // case 180:
  //   dft180(idft_in0,idft_out0,1);
  //   dft180(idft_in1,idft_out1,1);
  //   dft180(idft_in2,idft_out2,1);
  //   break;

  // case 192:
  //   dft192(idft_in0,idft_out0,1);
  //   dft192(idft_in1,idft_out1,1);
  //   dft192(idft_in2,idft_out2,1);
  //   break;

  // case 216:
  //   dft216(idft_in0,idft_out0,1);
  //   dft216(idft_in1,idft_out1,1);
  //   dft216(idft_in2,idft_out2,1);
  //   break;

  // case 240:
  //   dft240(idft_in0,idft_out0,1);
  //   dft240(idft_in1,idft_out1,1);
  //   dft240(idft_in2,idft_out2,1);
  //   break;

  // case 288:
  //   dft288(idft_in0,idft_out0,1);
  //   dft288(idft_in1,idft_out1,1);
  //   dft288(idft_in2,idft_out2,1);
  //   break;

  // case 300:
  //   dft300(idft_in0,idft_out0,1);
  //   dft300(idft_in1,idft_out1,1);
  //   dft300(idft_in2,idft_out2,1);
  //   break;

  // case 324:
  //   dft324((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft324((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft324((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 360:
  //   dft360((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft360((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft360((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 384:
  //   dft384((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft384((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft384((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 432:
  //   dft432((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft432((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft432((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 480:
  //   dft480((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft480((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft480((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 540:
  //   dft540((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft540((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft540((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 576:
  //   dft576((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft576((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft576((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 600:
  //   dft600((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft600((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft600((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 648:
  //   dft648((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft648((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft648((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 720:
  //   dft720((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft720((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft720((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 864:
  //   dft864((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft864((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft864((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 900:
  //   dft900((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft900((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft900((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 960:
  //   dft960((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft960((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft960((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 972:
  //   dft972((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft972((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft972((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 1080:
  //   dft1080((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft1080((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft1080((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 1152:
  //   dft1152((int16_t*)idft_in0,(int16_t*)idft_out0,1);
  //   dft1152((int16_t*)idft_in1,(int16_t*)idft_out1,1);
  //   dft1152((int16_t*)idft_in2,(int16_t*)idft_out2,1);
  //   break;

  // case 1200:
  //   dft1200(idft_in0,idft_out0,1);
  //   dft1200(idft_in1,idft_out1,1);
  //   dft1200(idft_in2,idft_out2,1);
  //   break;

  default:
    // should not be reached
    LOG_E( PHY, "Unsupported Msc_PUSCH value of %"PRIu16"\n", Msc_PUSCH );
    return;
  }



  for (i=0,ip=0; i<Msc_PUSCH; i++,ip+=4) {
    z0[i]     = ((uint32_t*)idft_out0)[ip];
    /*
      printf("out0 (%d,%d),(%d,%d),(%d,%d),(%d,%d)\n",
      ((int16_t*)&idft_out0[ip])[0],((int16_t*)&idft_out0[ip])[1],
      ((int16_t*)&idft_out0[ip+1])[0],((int16_t*)&idft_out0[ip+1])[1],
      ((int16_t*)&idft_out0[ip+2])[0],((int16_t*)&idft_out0[ip+2])[1],
      ((int16_t*)&idft_out0[ip+3])[0],((int16_t*)&idft_out0[ip+3])[1]);
    */
    z1[i]     = ((uint32_t*)idft_out0)[ip+1];
    z2[i]     = ((uint32_t*)idft_out0)[ip+2];
    z3[i]     = ((uint32_t*)idft_out0)[ip+3];
    z4[i]     = ((uint32_t*)idft_out1)[ip+0];
    z5[i]     = ((uint32_t*)idft_out1)[ip+1];
    z6[i]     = ((uint32_t*)idft_out1)[ip+2];
    z7[i]     = ((uint32_t*)idft_out1)[ip+3];
    z8[i]     = ((uint32_t*)idft_out2)[ip];
    z9[i]     = ((uint32_t*)idft_out2)[ip+1];

    // if (frame_parms->Ncp==0) {
      z10[i]    = ((uint32_t*)idft_out2)[ip+2];
      z11[i]    = ((uint32_t*)idft_out2)[ip+3];
    // }
  }

  // conjugate output
  for (i=0; i<(Msc_PUSCH>>2); i++) {
#if defined(__x86_64__) || defined(__i386__)
    ((__m128i*)z0)[i]=_mm_sign_epi16(((__m128i*)z0)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z1)[i]=_mm_sign_epi16(((__m128i*)z1)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z2)[i]=_mm_sign_epi16(((__m128i*)z2)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z3)[i]=_mm_sign_epi16(((__m128i*)z3)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z4)[i]=_mm_sign_epi16(((__m128i*)z4)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z5)[i]=_mm_sign_epi16(((__m128i*)z5)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z6)[i]=_mm_sign_epi16(((__m128i*)z6)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z7)[i]=_mm_sign_epi16(((__m128i*)z7)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z8)[i]=_mm_sign_epi16(((__m128i*)z8)[i],*(__m128i*)&conjugate2[0]);
    ((__m128i*)z9)[i]=_mm_sign_epi16(((__m128i*)z9)[i],*(__m128i*)&conjugate2[0]);

    // if (frame_parms->Ncp==NORMAL_NB_IoT) {
      ((__m128i*)z10)[i]=_mm_sign_epi16(((__m128i*)z10)[i],*(__m128i*)&conjugate2[0]);
      ((__m128i*)z11)[i]=_mm_sign_epi16(((__m128i*)z11)[i],*(__m128i*)&conjugate2[0]);
    // }
#elif defined(__arm__)
    *&(((int16x8_t*)z0)[i])=vmulq_s16(*&(((int16x8_t*)z0)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z1)[i])=vmulq_s16(*&(((int16x8_t*)z1)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z2)[i])=vmulq_s16(*&(((int16x8_t*)z2)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z3)[i])=vmulq_s16(*&(((int16x8_t*)z3)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z4)[i])=vmulq_s16(*&(((int16x8_t*)z4)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z5)[i])=vmulq_s16(*&(((int16x8_t*)z5)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z6)[i])=vmulq_s16(*&(((int16x8_t*)z6)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z7)[i])=vmulq_s16(*&(((int16x8_t*)z7)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z8)[i])=vmulq_s16(*&(((int16x8_t*)z8)[i]),*(int16x8_t*)&conjugate2[0]);
    *&(((int16x8_t*)z9)[i])=vmulq_s16(*&(((int16x8_t*)z9)[i]),*(int16x8_t*)&conjugate2[0]);


    // if (frame_parms->Ncp==NORMAL_NB_IoT) {
      *&(((int16x8_t*)z10)[i])=vmulq_s16(*&(((int16x8_t*)z10)[i]),*(int16x8_t*)&conjugate2[0]);
      *&(((int16x8_t*)z11)[i])=vmulq_s16(*&(((int16x8_t*)z11)[i]),*(int16x8_t*)&conjugate2[0]);
    // }

#endif
  }

#if defined(__x86_64__) || defined(__i386__)
  _mm_empty();
  _m_empty();
#endif

}
#endif


int32_t ulsch_bpsk_llr_NB_IoT(PHY_VARS_eNB *eNB, 
                              LTE_DL_FRAME_PARMS *frame_parms,
                              int32_t **rxdataF_comp,
                              int16_t *ulsch_llr, 
                              uint8_t symbol, 
                              uint8_t UE_id, 
                              int16_t **llrp)
{

  int16_t *rxF; 
  uint32_t I_sc = 11;//eNB->ulsch_NB_IoT[UE_id]->harq_process->I_sc;  // NB_IoT: subcarrier indication field: must be defined in higher layer
  uint16_t ul_sc_start; // subcarrier start index into UL RB 
  // int i; 

  ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); // NB-IoT: get the used subcarrier in RB
  rxF = (int16_t *)&rxdataF_comp[0][(symbol*frame_parms->N_RB_DL*12) + ul_sc_start]; 

  //  printf("qpsk llr for symbol %d (pos %d), llr offset %d\n",symbol,(symbol*frame_parms->N_RB_DL*12),llr128U-(__m128i*)ulsch_llr);

    //printf("%d,%d,%d,%d,%d,%d,%d,%d\n",((int16_t *)rxF)[0],((int16_t *)rxF)[1],((int16_t *)rxF)[2],((int16_t *)rxF)[3],((int16_t *)rxF)[4],((int16_t *)rxF)[5],((int16_t *)rxF)[6],((int16_t *)rxF)[7]);
    *(*llrp) = *rxF;
    //rxF++;
    (*llrp)++;

  return(0);

}


// int32_t ulsch_qpsk_llr_NB_IoT(NB_IoT_DL_FRAME_PARMS *frame_parms,
//                               int32_t **rxdataF_comp,
//                               int16_t *ulsch_llr,
//                               uint8_t symbol,
//                               uint16_t nb_rb,
//                               int16_t **llrp)
// {
// #if defined(__x86_64__) || defined(__i386__)
//   __m128i *rxF=(__m128i*)&rxdataF_comp[0][(symbol*frame_parms->N_RB_DL*12)];
//   __m128i **llrp128 = (__m128i **)llrp;
// #elif defined(__arm__)
//   int16x8_t *rxF= (int16x8_t*)&rxdataF_comp[0][(symbol*frame_parms->N_RB_DL*12)];
//   int16x8_t **llrp128 = (int16x8_t **)llrp;
// #endif

//   int i;

//   //  printf("qpsk llr for symbol %d (pos %d), llr offset %d\n",symbol,(symbol*frame_parms->N_RB_DL*12),llr128U-(__m128i*)ulsch_llr);

//   for (i=0; i<(nb_rb*3); i++) {
//     //printf("%d,%d,%d,%d,%d,%d,%d,%d\n",((int16_t *)rxF)[0],((int16_t *)rxF)[1],((int16_t *)rxF)[2],((int16_t *)rxF)[3],((int16_t *)rxF)[4],((int16_t *)rxF)[5],((int16_t *)rxF)[6],((int16_t *)rxF)[7]);
//     *(*llrp128) = *rxF;
//     rxF++;
//     (*llrp128)++;
//   }

// #if defined(__x86_64__) || defined(__i386__)
//   _mm_empty();
//   _m_empty();
// #endif

//   return(0);

// }

int32_t ulsch_qpsk_llr_NB_IoT(PHY_VARS_eNB *eNB, 
                              LTE_DL_FRAME_PARMS *frame_parms,
                              int32_t **rxdataF_comp,
                              int16_t *ulsch_llr, 
                              uint8_t symbol, 
                              uint8_t UE_id, 
                              int16_t *llrp)
{

  int32_t *rxF; 
  int32_t *llrp32; // = (int32_t *)llrp; 
  uint32_t I_sc = 11;//eNB->ulsch_NB_IoT[UE_id]->harq_process->I_sc;  // NB_IoT: subcarrier indication field: must be defined in higher layer
  uint16_t ul_sc_start; // subcarrier start index into UL RB 
  uint8_t Nsc_RU = 1;//eNB->ulsch_NB_IoT[UE_id]->harq_process->N_sc_RU; // Vincent: number of sc 1,3,6,12 
  int i; 
  
  llrp32 = (int32_t *)&llrp[0];
  ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); // NB-IoT: get the used subcarrier in RB
  rxF = (int32_t *)&rxdataF_comp[0][(symbol*frame_parms->N_RB_DL*12) + ul_sc_start]; 

  //  printf("qpsk llr for symbol %d (pos %d), llr offset %d\n",symbol,(symbol*frame_parms->N_RB_DL*12),llr128U-(__m128i*)ulsch_llr);

  for (i=0; i<Nsc_RU; i++) {
    //printf("%d,%d,%d,%d,%d,%d,%d,%d\n",((int16_t *)rxF)[0],((int16_t *)rxF)[1],((int16_t *)rxF)[2],((int16_t *)rxF)[3],((int16_t *)rxF)[4],((int16_t *)rxF)[5],((int16_t *)rxF)[6],((int16_t *)rxF)[7]);
    /**(*llrp32) = *rxF;
    rxF++;
    (*llrp32)++;*/
    llrp32[i] = rxF[i]; 
  /*printf("\nin llr_%d === %d",ul_sc_start,(int32_t)llrp[i]); 
  printf("\n  in llr_%d === %d",ul_sc_start,llrp32[i]);*/
  }

  return(0);

}


void ulsch_detection_mrc_NB_IoT(LTE_DL_FRAME_PARMS *frame_parms,
                         int32_t **rxdataF_comp,
                         int32_t **ul_ch_mag,
                         int32_t **ul_ch_magb,
                         uint8_t symbol,
                         uint16_t nb_rb)
{

#if defined(__x86_64__) || defined(__i386__)

  __m128i *rxdataF_comp128_0,*ul_ch_mag128_0,*ul_ch_mag128_0b;
  __m128i *rxdataF_comp128_1,*ul_ch_mag128_1,*ul_ch_mag128_1b;
#elif defined(__arm__)

  int16x8_t *rxdataF_comp128_0,*ul_ch_mag128_0,*ul_ch_mag128_0b;
  int16x8_t *rxdataF_comp128_1,*ul_ch_mag128_1,*ul_ch_mag128_1b;

#endif
  int32_t i;

  if (frame_parms->nb_antennas_rx>1) {
#if defined(__x86_64__) || defined(__i386__)
    rxdataF_comp128_0   = (__m128i *)&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12];
    rxdataF_comp128_1   = (__m128i *)&rxdataF_comp[1][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_0      = (__m128i *)&ul_ch_mag[0][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_1      = (__m128i *)&ul_ch_mag[1][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_0b     = (__m128i *)&ul_ch_magb[0][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_1b     = (__m128i *)&ul_ch_magb[1][symbol*frame_parms->N_RB_DL*12];

    // MRC on each re of rb, both on MF output and magnitude (for 16QAM/64QAM llr computation)
    for (i=0; i<nb_rb*3; i++) {
      rxdataF_comp128_0[i] = _mm_adds_epi16(_mm_srai_epi16(rxdataF_comp128_0[i],1),_mm_srai_epi16(rxdataF_comp128_1[i],1));
      ul_ch_mag128_0[i]    = _mm_adds_epi16(_mm_srai_epi16(ul_ch_mag128_0[i],1),_mm_srai_epi16(ul_ch_mag128_1[i],1));
      ul_ch_mag128_0b[i]   = _mm_adds_epi16(_mm_srai_epi16(ul_ch_mag128_0b[i],1),_mm_srai_epi16(ul_ch_mag128_1b[i],1));
      rxdataF_comp128_0[i] = _mm_add_epi16(rxdataF_comp128_0[i],(*(__m128i*)&jitterc[0]));
    }

#elif defined(__arm__)
    rxdataF_comp128_0   = (int16x8_t *)&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12];
    rxdataF_comp128_1   = (int16x8_t *)&rxdataF_comp[1][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_0      = (int16x8_t *)&ul_ch_mag[0][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_1      = (int16x8_t *)&ul_ch_mag[1][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_0b     = (int16x8_t *)&ul_ch_magb[0][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128_1b     = (int16x8_t *)&ul_ch_magb[1][symbol*frame_parms->N_RB_DL*12];

    // MRC on each re of rb, both on MF output and magnitude (for 16QAM/64QAM llr computation)
    for (i=0; i<nb_rb*3; i++) {
      rxdataF_comp128_0[i] = vhaddq_s16(rxdataF_comp128_0[i],rxdataF_comp128_1[i]);
      ul_ch_mag128_0[i]    = vhaddq_s16(ul_ch_mag128_0[i],ul_ch_mag128_1[i]);
      ul_ch_mag128_0b[i]   = vhaddq_s16(ul_ch_mag128_0b[i],ul_ch_mag128_1b[i]);
      rxdataF_comp128_0[i] = vqaddq_s16(rxdataF_comp128_0[i],(*(int16x8_t*)&jitterc[0]));
    }


#endif
    
  }

#if defined(__x86_64__) || defined(__i386__)
  _mm_empty();
  _m_empty();
#endif
}

void ulsch_extract_rbs_single_NB_IoT(int32_t **rxdataF,
                                     int32_t **rxdataF_ext,
                                     // uint32_t first_rb, 
                                     uint16_t UL_RB_ID_NB_IoT, // index of UL NB_IoT resource block !!! may be defined twice : in frame_parms and in NB_IoT_UL_eNB_HARQ_t
                                     uint8_t N_sc_RU, // number of subcarriers in UL 
                                     uint8_t subframe,// uint32_t I_sc, // NB_IoT: subcarrier indication field: must be defined in higher layer
                                     uint32_t nb_rb,
                                     uint8_t l,
                                     uint8_t Ns,
                                     LTE_DL_FRAME_PARMS *frame_parms)
{
  uint16_t  nb_rb1; 
  // uint16_t  nb_rb2; 
  uint8_t   aarx,n;
  // int32_t   *rxF,*rxF_ext;
  //uint8_t symbol = l+Ns*frame_parms->symbols_per_tti/2;
  uint8_t   symbol = l+(7*(Ns&1)); ///symbol within sub-frame 
  // uint16_t ul_sc_start; // subcarrier start index into UL RB 
  //unsigned short UL_RB_ID_NB_IoT; 

  // ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); 
  //UL_RB_ID_NB_IoT = frame_parms->NB_IoT_RB_ID; 

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

    nb_rb1 = cmin(cmax((int)(frame_parms->N_RB_UL) - (int)(2*UL_RB_ID_NB_IoT),(int)0),(int)(2));    // 2 times no. RBs before the DC
                                 // 2 times no. RBs after the DC

    // rxF_ext   = &rxdataF_ext[aarx][(symbol*frame_parms->N_RB_UL*12)];

    if (nb_rb1) { // RB NB-IoT is in the first half

      for (n=0;n<12;n++){ // extract whole RB of 12 subcarriers
        // Note that FFT splits the RBs 
        // !!! Note that frame_parms->N_RB_UL is the number of RB in LTE
        // rxdataF_ext[aarx][symbol*frame_parms->N_RB_UL*12 + n] = rxdataF[aarx][UL_RB_ID_NB_IoT*12 + ul_sc_start + frame_parms->first_carrier_offset + symbol*frame_parms->ofdm_symbol_size + n];
        rxdataF_ext[aarx][symbol*frame_parms->N_RB_UL*12 + n] = rxdataF[aarx][UL_RB_ID_NB_IoT*12 + frame_parms->first_carrier_offset + (symbol)*frame_parms->ofdm_symbol_size + n]; 
        //rxdataF_ext[aarx][symbol*12 + n] = rxdataF[aarx][UL_RB_ID_NB_IoT*12 + frame_parms->first_carrier_offset + symbol*frame_parms->ofdm_symbol_size + n];
      
      }

      // rxF = &rxdataF[aarx][(first_rb*12 + frame_parms->first_carrier_offset + symbol*frame_parms->ofdm_symbol_size)];
      // memcpy(rxF_ext, rxF, nb_rb1*6*sizeof(int));
      // rxF_ext += nb_rb1*6;

      // if (nb_rb2)  {
      //   //#ifdef OFDMA_ULSCH
      //   //  rxF = &rxdataF[aarx][(1 + symbol*frame_parms->ofdm_symbol_size)*2];
      //   //#else
      //   rxF = &rxdataF[aarx][(symbol*frame_parms->ofdm_symbol_size)];
      //   //#endif
      //   memcpy(rxF_ext, rxF, nb_rb2*6*sizeof(int));
      //   rxF_ext += nb_rb2*6;
      // }
    } else { // RB NB-IoT is in the second half 

     
      for (n=0;n<12;n++){ // extract whole RB of 12 subcarriers
        // Note that FFT splits the RBs 
        // rxdataF_ext[aarx][symbol*frame_parms->N_RB_UL*12 + n] = rxdataF[aarx][6*(2*UL_RB_ID_NB_IoT - frame_parms->N_RB_UL) +  ul_sc_start + symbol*frame_parms->ofdm_symbol_size + n]; 
        rxdataF_ext[aarx][symbol*frame_parms->N_RB_UL*12 + n] = rxdataF[aarx][6*(2*UL_RB_ID_NB_IoT - frame_parms->N_RB_UL) + (symbol)*frame_parms->ofdm_symbol_size + n]; 
        //printf("   rx_22_%d = %d   ",n,rxdataF[aarx][6*(2*UL_RB_ID_NB_IoT - frame_parms->N_RB_UL) + (subframe*14+symbol)*frame_parms->ofdm_symbol_size + n]); 
        //printf("   rx_20_%d = %d   ",n,rxdataF[aarx][6*(2*(UL_RB_ID_NB_IoT-7) - frame_parms->N_RB_UL) + (subframe*14+symbol)*frame_parms->ofdm_symbol_size + n]);
        //rxdataF_ext[aarx][symbol*12 + n] = rxdataF[aarx][6*(2*UL_RB_ID_NB_IoT - frame_parms->N_RB_UL) + symbol*frame_parms->ofdm_symbol_size + n];
      }
      //#ifdef OFDMA_ULSCH
      //      rxF = &rxdataF[aarx][(1 + 6*(2*first_rb - frame_parms->N_RB_UL) + symbol*frame_parms->ofdm_symbol_size)*2];
      //#else
      // rxF = &rxdataF[aarx][(6*(2*first_rb - frame_parms->N_RB_UL) + symbol*frame_parms->ofdm_symbol_size)]; 
      // //#endif
      // memcpy(rxF_ext, rxF, nb_rb2*6*sizeof(int));
      // rxF_ext += nb_rb2*6;
    }
  }

}


void ulsch_channel_compensation_NB_IoT(int32_t **rxdataF_ext,
                                int32_t **ul_ch_estimates_ext,
                                int32_t **ul_ch_mag,
                                int32_t **ul_ch_magb,
                                int32_t **rxdataF_comp,
                                LTE_DL_FRAME_PARMS *frame_parms,
                                uint8_t symbol,
                                uint8_t Qm,
                                uint16_t nb_rb,
                                uint8_t output_shift)
{

  // uint16_t rb;

#if defined(__x86_64__) || defined(__i386__)

  __m128i *ul_ch128,*ul_ch_mag128,*ul_ch_mag128b,*rxdataF128,*rxdataF_comp128;
  uint8_t aarx;//,symbol_mod;
  __m128i mmtmpU0,mmtmpU1,mmtmpU2,mmtmpU3;
#ifdef OFDMA_ULSCH
  __m128i QAM_amp128U,QAM_amp128bU;
#endif

#elif defined(__arm__)

  int16x4_t *ul_ch128,*rxdataF128;
  int16x8_t *ul_ch_mag128,*ul_ch_mag128b,*rxdataF_comp128;

  uint8_t aarx;//,symbol_mod;
  int32x4_t mmtmpU0,mmtmpU1,mmtmpU0b,mmtmpU1b;
#ifdef OFDMA_ULSCH
  int16x8_t mmtmpU2,mmtmpU3;
  int16x8_t QAM_amp128U,QAM_amp128bU;
#endif
  int16_t conj[4]__attribute__((aligned(16))) = {1,-1,1,-1};
  int32x4_t output_shift128 = vmovq_n_s32(-(int32_t)output_shift);



#endif

// #ifdef OFDMA_ULSCH

// #if defined(__x86_64__) || defined(__i386__)
//   if (Qm == 4)
//     QAM_amp128U = _mm_set1_epi16(QAM16_n1);
//   else if (Qm == 6) {
//     QAM_amp128U  = _mm_set1_epi16(QAM64_n1);
//     QAM_amp128bU = _mm_set1_epi16(QAM64_n2);
//   }
// #elif defined(__arm__)
//   if (Qm == 4)
//     QAM_amp128U = vdupq_n_s16(QAM16_n1);
//   else if (Qm == 6) {
//     QAM_amp128U  = vdupq_n_s16(QAM64_n1);
//     QAM_amp128bU = vdupq_n_s16(QAM64_n2);
//   }

// #endif
// #endif

  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

#if defined(__x86_64__) || defined(__i386__)

    ul_ch128          = (__m128i *)&ul_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128      = (__m128i *)&ul_ch_mag[aarx][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128b     = (__m128i *)&ul_ch_magb[aarx][symbol*frame_parms->N_RB_DL*12];
    rxdataF128        = (__m128i *)&rxdataF_ext[aarx][symbol*frame_parms->N_RB_DL*12];
    rxdataF_comp128   = (__m128i *)&rxdataF_comp[aarx][symbol*frame_parms->N_RB_DL*12];

#elif defined(__arm__)


    ul_ch128          = (int16x4_t *)&ul_ch_estimates_ext[aarx][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128      = (int16x8_t *)&ul_ch_mag[aarx][symbol*frame_parms->N_RB_DL*12];
    ul_ch_mag128b     = (int16x8_t *)&ul_ch_magb[aarx][symbol*frame_parms->N_RB_DL*12];
    rxdataF128        = (int16x4_t *)&rxdataF_ext[aarx][symbol*frame_parms->N_RB_DL*12];
    rxdataF_comp128   = (int16x8_t *)&rxdataF_comp[aarx][symbol*frame_parms->N_RB_DL*12];

#endif
    // for (rb=0; rb<nb_rb; rb++) {
      //            printf("comp: symbol %d rb %d\n",symbol,rb);
// #ifdef OFDMA_ULSCH
//       if (Qm>2) {
//         // get channel amplitude if not QPSK

// #if defined(__x86_64__) || defined(__i386__)
//         mmtmpU0 = _mm_madd_epi16(ul_ch128[0],ul_ch128[0]);

//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);

//         mmtmpU1 = _mm_madd_epi16(ul_ch128[1],ul_ch128[1]);
//         mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//         mmtmpU0 = _mm_packs_epi32(mmtmpU0,mmtmpU1);

//         ul_ch_mag128[0] = _mm_unpacklo_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b[0] = ul_ch_mag128[0];
//         ul_ch_mag128[0] = _mm_mulhi_epi16(ul_ch_mag128[0],QAM_amp128U);
//         ul_ch_mag128[0] = _mm_slli_epi16(ul_ch_mag128[0],2);  // 2 to compensate the scale channel estimate
//         ul_ch_mag128[1] = _mm_unpackhi_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b[1] = ul_ch_mag128[1];
//         ul_ch_mag128[1] = _mm_mulhi_epi16(ul_ch_mag128[1],QAM_amp128U);
//         ul_ch_mag128[1] = _mm_slli_epi16(ul_ch_mag128[1],2);  // 2 to compensate the scale channel estimate

//         mmtmpU0 = _mm_madd_epi16(ul_ch128[2],ul_ch128[2]);
//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//         mmtmpU1 = _mm_packs_epi32(mmtmpU0,mmtmpU0);

//         ul_ch_mag128[2] = _mm_unpacklo_epi16(mmtmpU1,mmtmpU1);
//         ul_ch_mag128b[2] = ul_ch_mag128[2];

//         ul_ch_mag128[2] = _mm_mulhi_epi16(ul_ch_mag128[2],QAM_amp128U);
//         ul_ch_mag128[2] = _mm_slli_epi16(ul_ch_mag128[2],2); // 2 to compensate the scale channel estimate


//         ul_ch_mag128b[0] = _mm_mulhi_epi16(ul_ch_mag128b[0],QAM_amp128bU);
//         ul_ch_mag128b[0] = _mm_slli_epi16(ul_ch_mag128b[0],2); // 2 to compensate the scale channel estimate


//         ul_ch_mag128b[1] = _mm_mulhi_epi16(ul_ch_mag128b[1],QAM_amp128bU);
//         ul_ch_mag128b[1] = _mm_slli_epi16(ul_ch_mag128b[1],2); // 2 to compensate the scale channel estimate

//         ul_ch_mag128b[2] = _mm_mulhi_epi16(ul_ch_mag128b[2],QAM_amp128bU);
//         ul_ch_mag128b[2] = _mm_slli_epi16(ul_ch_mag128b[2],2);// 2 to compensate the scale channel estimate

// #elif defined(__arm__)
//           mmtmpU0 = vmull_s16(ul_ch128[0], ul_ch128[0]);
//           mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
//           mmtmpU1 = vmull_s16(ul_ch128[1], ul_ch128[1]);
//           mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
//           mmtmpU2 = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
//           mmtmpU0 = vmull_s16(ul_ch128[2], ul_ch128[2]);
//           mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
//           mmtmpU1 = vmull_s16(ul_ch128[3], ul_ch128[3]);
//           mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
//           mmtmpU3 = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
//           mmtmpU0 = vmull_s16(ul_ch128[4], ul_ch128[4]);
//           mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
//           mmtmpU1 = vmull_s16(ul_ch128[5], ul_ch128[5]);
//           mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
//           mmtmpU4 = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));

//           ul_ch_mag128b[0] = vqdmulhq_s16(mmtmpU2,QAM_amp128b);
//           ul_ch_mag128b[1] = vqdmulhq_s16(mmtmpU3,QAM_amp128b);
//           ul_ch_mag128[0] = vqdmulhq_s16(mmtmpU2,QAM_amp128);
//           ul_ch_mag128[1] = vqdmulhq_s16(mmtmpU3,QAM_amp128);
//           ul_ch_mag128b[2] = vqdmulhq_s16(mmtmpU4,QAM_amp128b);
//           ul_ch_mag128[2]  = vqdmulhq_s16(mmtmpU4,QAM_amp128);
// #endif
//       }

// #else // SC-FDMA
// just compute channel magnitude without scaling, this is done after equalization for SC-FDMA

#if defined(__x86_64__) || defined(__i386__)
      mmtmpU0 = _mm_madd_epi16(ul_ch128[0],ul_ch128[0]);

      mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
      mmtmpU1 = _mm_madd_epi16(ul_ch128[1],ul_ch128[1]);

      mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);

      mmtmpU0 = _mm_packs_epi32(mmtmpU0,mmtmpU1);

      ul_ch_mag128[0] = _mm_unpacklo_epi16(mmtmpU0,mmtmpU0);
      ul_ch_mag128[1] = _mm_unpackhi_epi16(mmtmpU0,mmtmpU0);

      mmtmpU0 = _mm_madd_epi16(ul_ch128[2],ul_ch128[2]);

      mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
      mmtmpU1 = _mm_packs_epi32(mmtmpU0,mmtmpU0);
      ul_ch_mag128[2] = _mm_unpacklo_epi16(mmtmpU1,mmtmpU1);

      // printf("comp: symbol %d rb %d => %d,%d,%d (output_shift %d)\n",symbol,rb,*((int16_t*)&ul_ch_mag128[0]),*((int16_t*)&ul_ch_mag128[1]),*((int16_t*)&ul_ch_mag128[2]),output_shift);


#elif defined(__arm__)
          mmtmpU0 = vmull_s16(ul_ch128[0], ul_ch128[0]);
          mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
          mmtmpU1 = vmull_s16(ul_ch128[1], ul_ch128[1]);
          mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
          ul_ch_mag128[0] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
          mmtmpU0 = vmull_s16(ul_ch128[2], ul_ch128[2]);
          mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
          mmtmpU1 = vmull_s16(ul_ch128[3], ul_ch128[3]);
          mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
          ul_ch_mag128[1] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
          mmtmpU0 = vmull_s16(ul_ch128[4], ul_ch128[4]);
          mmtmpU0 = vqshlq_s32(vqaddq_s32(mmtmpU0,vrev64q_s32(mmtmpU0)),-output_shift128);
          mmtmpU1 = vmull_s16(ul_ch128[5], ul_ch128[5]);
          mmtmpU1 = vqshlq_s32(vqaddq_s32(mmtmpU1,vrev64q_s32(mmtmpU1)),-output_shift128);
          ul_ch_mag128[2] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));

#endif
// #endif

#if defined(__x86_64__) || defined(__i386__)
      // multiply by conjugated channel
      mmtmpU0 = _mm_madd_epi16(ul_ch128[0],rxdataF128[0]);
      //        print_ints("re",&mmtmpU0);

      // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpU1 = _mm_shufflelo_epi16(ul_ch128[0],_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)&conjugate[0]);

      mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[0]);
      //      print_ints("im",&mmtmpU1);
      // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
      //  print_ints("re(shift)",&mmtmpU0);
      mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
      //  print_ints("im(shift)",&mmtmpU1);
      mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
      mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);
      //        print_ints("c0",&mmtmpU2);
      //  print_ints("c1",&mmtmpU3);
      rxdataF_comp128[0] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
      /*
              print_shorts("rx:",&rxdataF128[0]);
              print_shorts("ch:",&ul_ch128[0]);
              print_shorts("pack:",&rxdataF_comp128[0]);
      */
      // multiply by conjugated channel
      mmtmpU0 = _mm_madd_epi16(ul_ch128[1],rxdataF128[1]);
      // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpU1 = _mm_shufflelo_epi16(ul_ch128[1],_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
      mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[1]);
      // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
      mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
      mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
      mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

      rxdataF_comp128[1] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
      //        print_shorts("rx:",rxdataF128[1]);
      //        print_shorts("ch:",ul_ch128[1]);
      //        print_shorts("pack:",rxdataF_comp128[1]);
      //       multiply by conjugated channel
      mmtmpU0 = _mm_madd_epi16(ul_ch128[2],rxdataF128[2]);
      // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
      mmtmpU1 = _mm_shufflelo_epi16(ul_ch128[2],_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
      mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
      mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[2]);
      // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
      mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
      mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
      mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
      mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

      rxdataF_comp128[2] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
      //        print_shorts("rx:",rxdataF128[2]);
      //        print_shorts("ch:",ul_ch128[2]);
      //        print_shorts("pack:",rxdataF_comp128[2]);

      // Add a jitter to compensate for the saturation in "packs" resulting in a bias on the DC after IDFT
      rxdataF_comp128[0] = _mm_add_epi16(rxdataF_comp128[0],(*(__m128i*)&jitter[0]));
      rxdataF_comp128[1] = _mm_add_epi16(rxdataF_comp128[1],(*(__m128i*)&jitter[0]));
      rxdataF_comp128[2] = _mm_add_epi16(rxdataF_comp128[2],(*(__m128i*)&jitter[0]));

      ul_ch128+=3;
      ul_ch_mag128+=3;
      ul_ch_mag128b+=3;
      rxdataF128+=3;
      rxdataF_comp128+=3;
#elif defined(__arm__)
        mmtmpU0 = vmull_s16(ul_ch128[0], rxdataF128[0]);
        //mmtmpU0 = [Re(ch[0])Re(rx[0]) Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1]) Im(ch[1])Im(ch[1])] 
        mmtmpU1 = vmull_s16(ul_ch128[1], rxdataF128[1]);
        //mmtmpU1 = [Re(ch[2])Re(rx[2]) Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3]) Im(ch[3])Im(ch[3])] 
        mmtmpU0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0),vget_high_s32(mmtmpU0)),
                               vpadd_s32(vget_low_s32(mmtmpU1),vget_high_s32(mmtmpU1)));
        //mmtmpU0 = [Re(ch[0])Re(rx[0])+Im(ch[0])Im(ch[0]) Re(ch[1])Re(rx[1])+Im(ch[1])Im(ch[1]) Re(ch[2])Re(rx[2])+Im(ch[2])Im(ch[2]) Re(ch[3])Re(rx[3])+Im(ch[3])Im(ch[3])] 

        mmtmpU0b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[0],*(int16x4_t*)conj)), rxdataF128[0]);
        //mmtmpU0 = [-Im(ch[0])Re(rx[0]) Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1]) Re(ch[1])Im(rx[1])]
        mmtmpU1b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[1],*(int16x4_t*)conj)), rxdataF128[1]);
        //mmtmpU0 = [-Im(ch[2])Re(rx[2]) Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3]) Re(ch[3])Im(rx[3])]
        mmtmpU1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0b),vget_high_s32(mmtmpU0b)),
                               vpadd_s32(vget_low_s32(mmtmpU1b),vget_high_s32(mmtmpU1b)));
        //mmtmpU1 = [-Im(ch[0])Re(rx[0])+Re(ch[0])Im(rx[0]) -Im(ch[1])Re(rx[1])+Re(ch[1])Im(rx[1]) -Im(ch[2])Re(rx[2])+Re(ch[2])Im(rx[2]) -Im(ch[3])Re(rx[3])+Re(ch[3])Im(rx[3])]

        mmtmpU0 = vqshlq_s32(mmtmpU0,-output_shift128);
        mmtmpU1 = vqshlq_s32(mmtmpU1,-output_shift128);
        rxdataF_comp128[0] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
        mmtmpU0 = vmull_s16(ul_ch128[2], rxdataF128[2]);
        mmtmpU1 = vmull_s16(ul_ch128[3], rxdataF128[3]);
        mmtmpU0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0),vget_high_s32(mmtmpU0)),
                               vpadd_s32(vget_low_s32(mmtmpU1),vget_high_s32(mmtmpU1)));
        mmtmpU0b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[2],*(int16x4_t*)conj)), rxdataF128[2]);
        mmtmpU1b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[3],*(int16x4_t*)conj)), rxdataF128[3]);
        mmtmpU1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0b),vget_high_s32(mmtmpU0b)),
                               vpadd_s32(vget_low_s32(mmtmpU1b),vget_high_s32(mmtmpU1b)));
        mmtmpU0 = vqshlq_s32(mmtmpU0,-output_shift128);
        mmtmpU1 = vqshlq_s32(mmtmpU1,-output_shift128);
        rxdataF_comp128[1] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));

        mmtmpU0 = vmull_s16(ul_ch128[4], rxdataF128[4]);
        mmtmpU1 = vmull_s16(ul_ch128[5], rxdataF128[5]);
        mmtmpU0 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0),vget_high_s32(mmtmpU0)),
                               vpadd_s32(vget_low_s32(mmtmpU1),vget_high_s32(mmtmpU1)));

        mmtmpU0b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[4],*(int16x4_t*)conj)), rxdataF128[4]);
        mmtmpU1b = vmull_s16(vrev32_s16(vmul_s16(ul_ch128[5],*(int16x4_t*)conj)), rxdataF128[5]);
        mmtmpU1 = vcombine_s32(vpadd_s32(vget_low_s32(mmtmpU0b),vget_high_s32(mmtmpU0b)),
                               vpadd_s32(vget_low_s32(mmtmpU1b),vget_high_s32(mmtmpU1b)));

              
        mmtmpU0 = vqshlq_s32(mmtmpU0,-output_shift128);
        mmtmpU1 = vqshlq_s32(mmtmpU1,-output_shift128);
        rxdataF_comp128[2] = vcombine_s16(vmovn_s32(mmtmpU0),vmovn_s32(mmtmpU1));
              
              // Add a jitter to compensate for the saturation in "packs" resulting in a bias on the DC after IDFT
        rxdataF_comp128[0] = vqaddq_s16(rxdataF_comp128[0],(*(int16x8_t*)&jitter[0]));
        rxdataF_comp128[1] = vqaddq_s16(rxdataF_comp128[1],(*(int16x8_t*)&jitter[0]));
        rxdataF_comp128[2] = vqaddq_s16(rxdataF_comp128[2],(*(int16x8_t*)&jitter[0]));

      
        ul_ch128+=6;
        ul_ch_mag128+=3;
        ul_ch_mag128b+=3;
        rxdataF128+=6;
        rxdataF_comp128+=3;
              
#endif
    // }
  }

#if defined(__x86_64__) || defined(__i386__)
  _mm_empty();
  _m_empty();
#endif
}




// #if defined(__x86_64__) || defined(__i386__)
// __m128i QAM_amp128U_0,QAM_amp128bU_0,QAM_amp128U_1,QAM_amp128bU_1;
// #endif

// void ulsch_channel_compensation_alamouti_NB_IoT(int32_t **rxdataF_ext,                 // For Distributed Alamouti Combining
//     int32_t **ul_ch_estimates_ext_0,
//     int32_t **ul_ch_estimates_ext_1,
//     int32_t **ul_ch_mag_0,
//     int32_t **ul_ch_magb_0,
//     int32_t **ul_ch_mag_1,
//     int32_t **ul_ch_magb_1,
//     int32_t **rxdataF_comp_0,
//     int32_t **rxdataF_comp_1,
//     NB_IoT_DL_FRAME_PARMS *frame_parms,
//     uint8_t symbol,
//     uint8_t Qm,
//     uint16_t nb_rb,
//     uint8_t output_shift)
// {
// #if defined(__x86_64__) || defined(__i386__)
//   uint16_t rb;
//   __m128i *ul_ch128_0,*ul_ch128_1,*ul_ch_mag128_0,*ul_ch_mag128_1,*ul_ch_mag128b_0,*ul_ch_mag128b_1,*rxdataF128,*rxdataF_comp128_0,*rxdataF_comp128_1;
//   uint8_t aarx;//,symbol_mod;
//   __m128i mmtmpU0,mmtmpU1,mmtmpU2,mmtmpU3;

//   //  symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;

//   //    printf("comp: symbol %d\n",symbol);


//   if (Qm == 4) {
//     QAM_amp128U_0 = _mm_set1_epi16(QAM16_n1);
//     QAM_amp128U_1 = _mm_set1_epi16(QAM16_n1);
//   } else if (Qm == 6) {
//     QAM_amp128U_0  = _mm_set1_epi16(QAM64_n1);
//     QAM_amp128bU_0 = _mm_set1_epi16(QAM64_n2);

//     QAM_amp128U_1  = _mm_set1_epi16(QAM64_n1);
//     QAM_amp128bU_1 = _mm_set1_epi16(QAM64_n2);
//   }

//   for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

//     ul_ch128_0          = (__m128i *)&ul_ch_estimates_ext_0[aarx][symbol*frame_parms->N_RB_DL*12];
//     ul_ch_mag128_0      = (__m128i *)&ul_ch_mag_0[aarx][symbol*frame_parms->N_RB_DL*12];
//     ul_ch_mag128b_0     = (__m128i *)&ul_ch_magb_0[aarx][symbol*frame_parms->N_RB_DL*12];
//     ul_ch128_1          = (__m128i *)&ul_ch_estimates_ext_1[aarx][symbol*frame_parms->N_RB_DL*12];
//     ul_ch_mag128_1      = (__m128i *)&ul_ch_mag_1[aarx][symbol*frame_parms->N_RB_DL*12];
//     ul_ch_mag128b_1     = (__m128i *)&ul_ch_magb_1[aarx][symbol*frame_parms->N_RB_DL*12];
//     rxdataF128        = (__m128i *)&rxdataF_ext[aarx][symbol*frame_parms->N_RB_DL*12];
//     rxdataF_comp128_0   = (__m128i *)&rxdataF_comp_0[aarx][symbol*frame_parms->N_RB_DL*12];
//     rxdataF_comp128_1   = (__m128i *)&rxdataF_comp_1[aarx][symbol*frame_parms->N_RB_DL*12];


//     for (rb=0; rb<nb_rb; rb++) {
//       //      printf("comp: symbol %d rb %d\n",symbol,rb);
//       if (Qm>2) {
//         // get channel amplitude if not QPSK

//         mmtmpU0 = _mm_madd_epi16(ul_ch128_0[0],ul_ch128_0[0]);

//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);

//         mmtmpU1 = _mm_madd_epi16(ul_ch128_0[1],ul_ch128_0[1]);
//         mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//         mmtmpU0 = _mm_packs_epi32(mmtmpU0,mmtmpU1);

//         ul_ch_mag128_0[0] = _mm_unpacklo_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b_0[0] = ul_ch_mag128_0[0];
//         ul_ch_mag128_0[0] = _mm_mulhi_epi16(ul_ch_mag128_0[0],QAM_amp128U_0);
//         ul_ch_mag128_0[0] = _mm_slli_epi16(ul_ch_mag128_0[0],2); // 2 to compensate the scale channel estimate

//         ul_ch_mag128_0[1] = _mm_unpackhi_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b_0[1] = ul_ch_mag128_0[1];
//         ul_ch_mag128_0[1] = _mm_mulhi_epi16(ul_ch_mag128_0[1],QAM_amp128U_0);
//         ul_ch_mag128_0[1] = _mm_slli_epi16(ul_ch_mag128_0[1],2); // 2 to scale compensate the scale channel estimate

//         mmtmpU0 = _mm_madd_epi16(ul_ch128_0[2],ul_ch128_0[2]);
//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//         mmtmpU1 = _mm_packs_epi32(mmtmpU0,mmtmpU0);

//         ul_ch_mag128_0[2] = _mm_unpacklo_epi16(mmtmpU1,mmtmpU1);
//         ul_ch_mag128b_0[2] = ul_ch_mag128_0[2];

//         ul_ch_mag128_0[2] = _mm_mulhi_epi16(ul_ch_mag128_0[2],QAM_amp128U_0);
//         ul_ch_mag128_0[2] = _mm_slli_epi16(ul_ch_mag128_0[2],2);  //  2 to scale compensate the scale channel estimat


//         ul_ch_mag128b_0[0] = _mm_mulhi_epi16(ul_ch_mag128b_0[0],QAM_amp128bU_0);
//         ul_ch_mag128b_0[0] = _mm_slli_epi16(ul_ch_mag128b_0[0],2);  //  2 to scale compensate the scale channel estima


//         ul_ch_mag128b_0[1] = _mm_mulhi_epi16(ul_ch_mag128b_0[1],QAM_amp128bU_0);
//         ul_ch_mag128b_0[1] = _mm_slli_epi16(ul_ch_mag128b_0[1],2);   //  2 to scale compensate the scale channel estima

//         ul_ch_mag128b_0[2] = _mm_mulhi_epi16(ul_ch_mag128b_0[2],QAM_amp128bU_0);
//         ul_ch_mag128b_0[2] = _mm_slli_epi16(ul_ch_mag128b_0[2],2);   //  2 to scale compensate the scale channel estima




//         mmtmpU0 = _mm_madd_epi16(ul_ch128_1[0],ul_ch128_1[0]);

//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);

//         mmtmpU1 = _mm_madd_epi16(ul_ch128_1[1],ul_ch128_1[1]);
//         mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//         mmtmpU0 = _mm_packs_epi32(mmtmpU0,mmtmpU1);

//         ul_ch_mag128_1[0] = _mm_unpacklo_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b_1[0] = ul_ch_mag128_1[0];
//         ul_ch_mag128_1[0] = _mm_mulhi_epi16(ul_ch_mag128_1[0],QAM_amp128U_1);
//         ul_ch_mag128_1[0] = _mm_slli_epi16(ul_ch_mag128_1[0],2); // 2 to compensate the scale channel estimate

//         ul_ch_mag128_1[1] = _mm_unpackhi_epi16(mmtmpU0,mmtmpU0);
//         ul_ch_mag128b_1[1] = ul_ch_mag128_1[1];
//         ul_ch_mag128_1[1] = _mm_mulhi_epi16(ul_ch_mag128_1[1],QAM_amp128U_1);
//         ul_ch_mag128_1[1] = _mm_slli_epi16(ul_ch_mag128_1[1],2); // 2 to scale compensate the scale channel estimate

//         mmtmpU0 = _mm_madd_epi16(ul_ch128_1[2],ul_ch128_1[2]);
//         mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//         mmtmpU1 = _mm_packs_epi32(mmtmpU0,mmtmpU0);

//         ul_ch_mag128_1[2] = _mm_unpacklo_epi16(mmtmpU1,mmtmpU1);
//         ul_ch_mag128b_1[2] = ul_ch_mag128_1[2];

//         ul_ch_mag128_1[2] = _mm_mulhi_epi16(ul_ch_mag128_1[2],QAM_amp128U_0);
//         ul_ch_mag128_1[2] = _mm_slli_epi16(ul_ch_mag128_1[2],2);  //  2 to scale compensate the scale channel estimat


//         ul_ch_mag128b_1[0] = _mm_mulhi_epi16(ul_ch_mag128b_1[0],QAM_amp128bU_1);
//         ul_ch_mag128b_1[0] = _mm_slli_epi16(ul_ch_mag128b_1[0],2);  //  2 to scale compensate the scale channel estima


//         ul_ch_mag128b_1[1] = _mm_mulhi_epi16(ul_ch_mag128b_1[1],QAM_amp128bU_1);
//         ul_ch_mag128b_1[1] = _mm_slli_epi16(ul_ch_mag128b_1[1],2);   //  2 to scale compensate the scale channel estima

//         ul_ch_mag128b_1[2] = _mm_mulhi_epi16(ul_ch_mag128b_1[2],QAM_amp128bU_1);
//         ul_ch_mag128b_1[2] = _mm_slli_epi16(ul_ch_mag128b_1[2],2);   //  2 to scale compensate the scale channel estima
//       }


//       /************************For Computing (y)*(h0*)********************************************/

//       // multiply by conjugated channel
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_0[0],rxdataF128[0]);
//       //  print_ints("re",&mmtmpU0);

//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(ul_ch128_0[0],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)&conjugate[0]);
//       //  print_ints("im",&mmtmpU1);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[0]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       //  print_ints("re(shift)",&mmtmpU0);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       //  print_ints("im(shift)",&mmtmpU1);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);
//       //        print_ints("c0",&mmtmpU2);
//       //  print_ints("c1",&mmtmpU3);
//       rxdataF_comp128_0[0] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[0]);
//       //        print_shorts("ch:",ul_ch128_0[0]);
//       //        print_shorts("pack:",rxdataF_comp128_0[0]);

//       // multiply by conjugated channel
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_0[1],rxdataF128[1]);
//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(ul_ch128_0[1],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[1]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

//       rxdataF_comp128_0[1] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[1]);
//       //        print_shorts("ch:",ul_ch128_0[1]);
//       //        print_shorts("pack:",rxdataF_comp128_0[1]);
//       //       multiply by conjugated channel
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_0[2],rxdataF128[2]);
//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(ul_ch128_0[2],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,rxdataF128[2]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

//       rxdataF_comp128_0[2] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[2]);
//       //        print_shorts("ch:",ul_ch128_0[2]);
//       //        print_shorts("pack:",rxdataF_comp128_0[2]);




//       /*************************For Computing (y*)*(h1)************************************/
//       // multiply by conjugated signal
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_1[0],rxdataF128[0]);
//       //  print_ints("re",&mmtmpU0);

//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(rxdataF128[0],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)&conjugate[0]);
//       //  print_ints("im",&mmtmpU1);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,ul_ch128_1[0]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       //  print_ints("re(shift)",&mmtmpU0);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       //  print_ints("im(shift)",&mmtmpU1);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);
//       //        print_ints("c0",&mmtmpU2);
//       //  print_ints("c1",&mmtmpU3);
//       rxdataF_comp128_1[0] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[0]);
//       //        print_shorts("ch_conjugate:",ul_ch128_1[0]);
//       //        print_shorts("pack:",rxdataF_comp128_1[0]);


//       // multiply by conjugated signal
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_1[1],rxdataF128[1]);
//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(rxdataF128[1],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,ul_ch128_1[1]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

//       rxdataF_comp128_1[1] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[1]);
//       //        print_shorts("ch_conjugate:",ul_ch128_1[1]);
//       //        print_shorts("pack:",rxdataF_comp128_1[1]);


//       //       multiply by conjugated signal
//       mmtmpU0 = _mm_madd_epi16(ul_ch128_1[2],rxdataF128[2]);
//       // mmtmpU0 contains real part of 4 consecutive outputs (32-bit)
//       mmtmpU1 = _mm_shufflelo_epi16(rxdataF128[2],_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_shufflehi_epi16(mmtmpU1,_MM_SHUFFLE(2,3,0,1));
//       mmtmpU1 = _mm_sign_epi16(mmtmpU1,*(__m128i*)conjugate);
//       mmtmpU1 = _mm_madd_epi16(mmtmpU1,ul_ch128_1[2]);
//       // mmtmpU1 contains imag part of 4 consecutive outputs (32-bit)
//       mmtmpU0 = _mm_srai_epi32(mmtmpU0,output_shift);
//       mmtmpU1 = _mm_srai_epi32(mmtmpU1,output_shift);
//       mmtmpU2 = _mm_unpacklo_epi32(mmtmpU0,mmtmpU1);
//       mmtmpU3 = _mm_unpackhi_epi32(mmtmpU0,mmtmpU1);

//       rxdataF_comp128_1[2] = _mm_packs_epi32(mmtmpU2,mmtmpU3);
//       //        print_shorts("rx:",rxdataF128[2]);
//       //        print_shorts("ch_conjugate:",ul_ch128_0[2]);
//       //        print_shorts("pack:",rxdataF_comp128_1[2]);



//       ul_ch128_0+=3;
//       ul_ch_mag128_0+=3;
//       ul_ch_mag128b_0+=3;
//       ul_ch128_1+=3;
//       ul_ch_mag128_1+=3;
//       ul_ch_mag128b_1+=3;
//       rxdataF128+=3;
//       rxdataF_comp128_0+=3;
//       rxdataF_comp128_1+=3;

//     }
//   }


//   _mm_empty();
//   _m_empty();
// #endif
// }

void fill_rbs_zeros_NB_IoT(PHY_VARS_eNB *eNB, 
                            LTE_DL_FRAME_PARMS *frame_parms,
                            int32_t **rxdataF_comp, 
                            uint8_t UE_id,
                            uint8_t symbol)
{

  uint32_t I_sc = 11;//eNB->ulsch[UE_id]->harq_process->I_sc;  // NB_IoT: subcarrier indication field: must be defined in higher layer
  uint8_t Nsc_RU = 1;//eNB->ulsch[UE_id]->harq_process->N_sc_RU; // Vincent: number of sc 1,3,6,12 
  uint16_t ul_sc_start; // subcarrier start index into UL RB 
  int32_t *rxdataF_comp32;   
  uint8_t m; // index of subcarrier

  ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); // NB-IoT: get the used subcarrier in RB 
  rxdataF_comp32   = (int32_t *)&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12]; 
  if (Nsc_RU != 12){
    for (m=0;m<12;m++)
    { // 12 is the number of subcarriers per RB
        if (m == ul_sc_start)
        {
            m = m + Nsc_RU; // skip non-zeros subcarriers
        }

        if(m<12)
        {
            rxdataF_comp32[m] = 0; 
        }   
    }  
  }


}
//for (m=0;m<12;m++)
    //{ // 12 is the number of subcarriers per RB
 
  //printf("  rxdataF_comp32_%d = %d",m,rxdataF_comp32[m]); 
      
  //}
void rotate_single_carrier_NB_IoT(PHY_VARS_eNB *eNB, 
                                  LTE_DL_FRAME_PARMS *frame_parms,
                                  int32_t **rxdataF_comp, 
                                  uint8_t UE_id,
                                  uint8_t symbol, //symbol within subframe
          uint8_t counter_msg3,
                                  uint8_t Qm)
{

  uint32_t I_sc = 11;//eNB->ulsch_NB_IoT[UE_id]->harq_process->I_sc;  // NB_IoT: subcarrier indication field: must be defined in higher layer
  uint16_t ul_sc_start; // subcarrier start index into UL RB 
  int16_t pi_2_re[2] = {32767 , 0}; 
  int16_t pi_2_im[2] = {0 , 32768}; 
  //int16_t pi_4_re[2] = {32767 , 25735}; 
  //int16_t pi_4_im[2] = {0 , 25736}; 
  int16_t pi_4_re[2] = {32767 , 23170}; 
  int16_t pi_4_im[2] = {0 , 23170}; 
  int16_t e_phi_re[120] = {32767, 24811, 4807, -17531, -31357, -29956, -14010, 0, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, 0, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, 0, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, -1, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, 0, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, 0, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, -1, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, 0, -21403, -32413, -27684, -9512, 13278, 29621}; 
  int16_t e_phi_im[120] = {0, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, -1, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, 0, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, 0, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, -1, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, 0, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009, 0, -21403, -32413, -27684, -9512, 13278, 29621, 32767, 24811, 4807, -17531, -31357, -29956, -14010, -1, 21402, 32412, 27683, 9511, -13279, -29622, -32767, -24812, -4808, 17530, 31356, 29955, 14009}; 
  int16_t *rxdataF_comp16; 
  int16_t rxdataF_comp16_re, rxdataF_comp16_im,rxdataF_comp16_re_2,rxdataF_comp16_im_2;    

  ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); // NB-IoT: get the used subcarrier in RB
  rxdataF_comp16   = (int16_t *)&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12 + ul_sc_start]; 
  rxdataF_comp16_re = rxdataF_comp16[0]; 
  rxdataF_comp16_im = rxdataF_comp16[1]; 
  rxdataF_comp16_re_2 = rxdataF_comp16_re; 
  rxdataF_comp16_im_2 = rxdataF_comp16_re;

  if (Qm == 1){
    rxdataF_comp16_re_2 = (int16_t)(((int32_t)pi_2_re[symbol%2] * (int32_t)rxdataF_comp16_re + 
                        (int32_t)pi_2_im[symbol%2] * (int32_t)rxdataF_comp16_im)>>15); 
    rxdataF_comp16_im_2 = (int16_t)(((int32_t)pi_2_re[symbol%2] * (int32_t)rxdataF_comp16_im - 
                        (int32_t)pi_2_im[symbol%2] * (int32_t)rxdataF_comp16_re)>>15); 
  }
  if(Qm == 2){
    rxdataF_comp16_re_2 = (int16_t)(((int32_t)pi_4_re[symbol%2] * (int32_t)rxdataF_comp16_re + 
                        (int32_t)pi_4_im[symbol%2] * (int32_t)rxdataF_comp16_im)>>15); 
    rxdataF_comp16_im_2 = (int16_t)(((int32_t)pi_4_re[symbol%2] * (int32_t)rxdataF_comp16_im - 
                        (int32_t)pi_4_im[symbol%2] * (int32_t)rxdataF_comp16_re)>>15); 
  }

  rxdataF_comp16[0] = (int16_t)(((int32_t)e_phi_re[14*(8-counter_msg3) + symbol] * (int32_t)rxdataF_comp16_re_2 + 
                        (int32_t)e_phi_im[14*(8-counter_msg3) + symbol] * (int32_t)rxdataF_comp16_im_2)>>15); 
  rxdataF_comp16[1] = (int16_t)(((int32_t)e_phi_re[14*(8-counter_msg3) + symbol] * (int32_t)rxdataF_comp16_im_2 - 
                        (int32_t)e_phi_im[14*(8-counter_msg3) + symbol] * (int32_t)rxdataF_comp16_re_2)>>15); 
  /*rxdataF_comp16[0] = (int16_t)(((int32_t)e_phi_re[0] * (int32_t)rxdataF_comp16_re_2 + 
                        (int32_t)e_phi_im[0] * (int32_t)rxdataF_comp16_im_2)>>15); 
  rxdataF_comp16[1] = (int16_t)(((int32_t)e_phi_re[0] * (int32_t)rxdataF_comp16_im_2 - 
                        (int32_t)e_phi_im[0] * (int32_t)rxdataF_comp16_re_2)>>15); */
  /*printf("\n");
  printf("  re_eq_data = %d  im_eq_data = %d   ",rxdataF_comp16[0],rxdataF_comp16[1]);
  printf("\n");*/

}

/*int ooo; 
  for (ooo=0;ooo<12;ooo++){
  printf("   rx_data_%d = %d    ",ooo,rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12 + ooo]);
  }*/

void rotate_bpsk_NB_IoT(PHY_VARS_eNB *eNB, 
                        LTE_DL_FRAME_PARMS *frame_parms,
                        int32_t **rxdataF_comp, 
                        uint8_t UE_id,
                        uint8_t symbol)
{

  uint32_t I_sc = eNB->ulsch_NB_IoT[UE_id]->harq_process->I_sc;  // NB_IoT: subcarrier indication field: must be defined in higher layer
  uint16_t ul_sc_start; // subcarrier start index into UL RB 
  int16_t m_pi_4_re = 25735; // cos(pi/4) 
  int16_t m_pi_4_im = 25736; // sin(pi/4) 
  int16_t *rxdataF_comp16; 
  int16_t rxdataF_comp16_re, rxdataF_comp16_im; 

  ul_sc_start = get_UL_sc_start_NB_IoT(I_sc); // NB-IoT: get the used subcarrier in RB
  rxdataF_comp16   = (int16_t *)&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12 + ul_sc_start]; 
  rxdataF_comp16_re = rxdataF_comp16[0]; 
  rxdataF_comp16_im = rxdataF_comp16[1]; 

  rxdataF_comp16[0] = (int16_t)(((int32_t)m_pi_4_re * (int32_t)rxdataF_comp16_re + 
                        (int32_t)m_pi_4_im * (int32_t)rxdataF_comp16_im)>>15); 
  rxdataF_comp16[1] = (int16_t)(((int32_t)m_pi_4_re * (int32_t)rxdataF_comp16_im -  
                        (int32_t)m_pi_4_im * (int32_t)rxdataF_comp16_re)>>15); 

} 

/*
void ulsch_alamouti_NB_IoT(NB_IoT_DL_FRAME_PARMS *frame_parms,// For Distributed Alamouti Receiver Combining
                    int32_t **rxdataF_comp,
                    int32_t **rxdataF_comp_0,
                    int32_t **rxdataF_comp_1,
                    int32_t **ul_ch_mag,
                    int32_t **ul_ch_magb,
                    int32_t **ul_ch_mag_0,
                    int32_t **ul_ch_magb_0,
                    int32_t **ul_ch_mag_1,
                    int32_t **ul_ch_magb_1,
                    uint8_t symbol,
                    uint16_t nb_rb)
{

#if defined(__x86_64__) || defined(__i386__)
  int16_t *rxF,*rxF0,*rxF1;
  __m128i *ch_mag,*ch_magb,*ch_mag0,*ch_mag1,*ch_mag0b,*ch_mag1b;
  uint8_t rb,re,aarx;
  int32_t jj=(symbol*frame_parms->N_RB_DL*12);


  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {

    rxF      = (int16_t*)&rxdataF_comp[aarx][jj];
    rxF0     = (int16_t*)&rxdataF_comp_0[aarx][jj];   // Contains (y)*(h0*)
    rxF1     = (int16_t*)&rxdataF_comp_1[aarx][jj];   // Contains (y*)*(h1)
    ch_mag   = (__m128i *)&ul_ch_mag[aarx][jj];
    ch_mag0 = (__m128i *)&ul_ch_mag_0[aarx][jj];
    ch_mag1 = (__m128i *)&ul_ch_mag_1[aarx][jj];
    ch_magb = (__m128i *)&ul_ch_magb[aarx][jj];
    ch_mag0b = (__m128i *)&ul_ch_magb_0[aarx][jj];
    ch_mag1b = (__m128i *)&ul_ch_magb_1[aarx][jj];

    for (rb=0; rb<nb_rb; rb++) {

      for (re=0; re<12; re+=2) {

        // Alamouti RX combining

        rxF[0] = rxF0[0] + rxF1[2];                   // re((y0)*(h0*))+ re((y1*)*(h1)) = re(x0)
        rxF[1] = rxF0[1] + rxF1[3];                   // im((y0)*(h0*))+ im((y1*)*(h1)) = im(x0)

        rxF[2] = rxF0[2] - rxF1[0];                   // re((y1)*(h0*))- re((y0*)*(h1)) = re(x1)
        rxF[3] = rxF0[3] - rxF1[1];                   // im((y1)*(h0*))- im((y0*)*(h1)) = im(x1)

        rxF+=4;
        rxF0+=4;
        rxF1+=4;
      }

      // compute levels for 16QAM or 64 QAM llr unit
      ch_mag[0] = _mm_adds_epi16(ch_mag0[0],ch_mag1[0]);
      ch_mag[1] = _mm_adds_epi16(ch_mag0[1],ch_mag1[1]);
      ch_mag[2] = _mm_adds_epi16(ch_mag0[2],ch_mag1[2]);
      ch_magb[0] = _mm_adds_epi16(ch_mag0b[0],ch_mag1b[0]);
      ch_magb[1] = _mm_adds_epi16(ch_mag0b[1],ch_mag1b[1]);
      ch_magb[2] = _mm_adds_epi16(ch_mag0b[2],ch_mag1b[2]);

      ch_mag+=3;
      ch_mag0+=3;
      ch_mag1+=3;
      ch_magb+=3;
      ch_mag0b+=3;
      ch_mag1b+=3;
    }
  }

  _mm_empty();
  _m_empty();

#endif
}

*/



#if defined(__x86_64__) || defined(__i386__)
__m128i avg128U;
#elif defined(__arm__)
int32x4_t avg128U;
#endif

void ulsch_channel_level_NB_IoT(int32_t **drs_ch_estimates_ext,
                                LTE_DL_FRAME_PARMS *frame_parms,
                                int32_t *avg,
                                uint16_t nb_rb)
{

  // int16_t rb;
  uint8_t aarx;
#if defined(__x86_64__) || defined(__i386__)
  __m128i *ul_ch128;
#elif defined(__arm__)
  int16x4_t *ul_ch128;
#endif
  for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++) {
    //clear average level
#if defined(__x86_64__) || defined(__i386__)
    avg128U = _mm_setzero_si128();
    ul_ch128=(__m128i *)drs_ch_estimates_ext[aarx];

    // for (rb=0; rb<nb_rb; rb++) {

      avg128U = _mm_add_epi32(avg128U,_mm_madd_epi16(ul_ch128[0],ul_ch128[0]));
      avg128U = _mm_add_epi32(avg128U,_mm_madd_epi16(ul_ch128[1],ul_ch128[1]));
      avg128U = _mm_add_epi32(avg128U,_mm_madd_epi16(ul_ch128[2],ul_ch128[2]));

      ul_ch128+=3;


    // }

#elif defined(__arm__)
    avg128U = vdupq_n_s32(0);
    ul_ch128=(int16x4_t *)drs_ch_estimates_ext[aarx];

    // for (rb=0; rb<nb_rb; rb++) {

       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[0],ul_ch128[0]));
       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[1],ul_ch128[1]));
       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[2],ul_ch128[2]));
       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[3],ul_ch128[3]));
       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[4],ul_ch128[4]));
       avg128U = vqaddq_s32(avg128U,vmull_s16(ul_ch128[5],ul_ch128[5]));
       ul_ch128+=6;


    // }

#endif

    DevAssert( nb_rb );
    avg[aarx] = (((int*)&avg128U)[0] +
                 ((int*)&avg128U)[1] +
                 ((int*)&avg128U)[2] +
                 ((int*)&avg128U)[3])/(nb_rb*12);

  }

#if defined(__x86_64__) || defined(__i386__)
  _mm_empty();
  _m_empty();
#endif
}

int32_t avgU[2];
int32_t avgU_0[2],avgU_1[2]; // For the Distributed Alamouti Scheme

void rx_ulsch_NB_IoT(PHY_VARS_eNB     *eNB,
	                   eNB_rxtx_proc_t  *proc,
                     uint8_t                 eNB_id,  // this is the effective sector id
                     uint8_t                 UE_id,
                     NB_IoT_eNB_NULSCH_t     **ulsch,
                     uint8_t                 cooperation_flag)
{
  // flagMag = 0;
  LTE_eNB_COMMON      *common_vars  =  &eNB->common_vars;
  LTE_eNB_PUSCH       *pusch_vars   =  eNB->pusch_vars[UE_id];
  LTE_DL_FRAME_PARMS  *frame_parms  =  &eNB->frame_parms;

  uint32_t   l,i;
  int32_t    avgs;
  uint8_t    log2_maxh = 0,aarx;

 // int32_t    avgs_0,avgs_1;
 // uint32_t   log2_maxh_0 = 0,log2_maxh_1 = 0;

  uint8_t    harq_pid;
  uint8_t    Qm;
  ///uint16_t   rx_power_correction;
  int16_t    *llrp;
  int        subframe = proc->subframe_rx; 

  uint8_t npusch_format = 1; // NB-IoT: format 1 (data), or 2: ack. Should be defined in higher layer 
  uint8_t Nsc_RU = eNB->ulsch_NB_IoT[UE_id]->harq_process->N_sc_RU; // Vincent: number of sc 1,3,6,12 
  uint8_t subcarrier_spacing = frame_parms->subcarrier_spacing; // 15 kHz or 3.75 kHz 

  int pilot_pos1_15k = 3, pilot_pos2_15k = 10; // holds for npusch format 1, and 15 kHz subcarrier bandwidth
  int pilot_pos_format2_15k[2] = {2,9}; // holds for npusch format 2, and 15 kHz subcarrier bandwidth 
  int pilot_pos1_3_75k = 4, pilot_pos2_3_75k = 11; // holds for npusch format 1, and 3.75 kHz subcarrier bandwidth
  int pilot_pos_format2_3_75k[2] = {0,7}; // holds for npusch format 2, and 3.75 kHz subcarrier bandwidth 

  int pilot_pos1, pilot_pos2; // holds for npusch format 1, and 15 kHz subcarrier bandwidth
  int *pilot_pos_format2; // holds for npusch format 2, and 15 kHz subcarrier bandwidth

  harq_pid = subframe2harq_pid_NB_IoT(frame_parms,proc->frame_rx,subframe);
  Qm       = get_Qm_ul_NB_IoT(ulsch[UE_id]->harq_process->mcs,Nsc_RU);

  ///rx_power_correction = 1;

  if (ulsch[UE_id]->harq_process->nb_rb == 0) {
    LOG_E(PHY,"PUSCH (%d/%x) nb_rb=0!\n", harq_pid,ulsch[UE_id]->rnti);
    return;
  }

  if (subcarrier_spacing){
    pilot_pos_format2 = pilot_pos_format2_15k; 
    pilot_pos1 = pilot_pos1_15k; 
    pilot_pos2 = pilot_pos2_15k;
  }else{
    pilot_pos_format2 = pilot_pos_format2_3_75k; 
    pilot_pos1 = pilot_pos1_3_75k; 
    pilot_pos2 = pilot_pos2_3_75k;
  }

  for (l=0; l<frame_parms->symbols_per_tti; l++) { 

    ulsch_extract_rbs_single_NB_IoT(common_vars->rxdataF[eNB_id],
                                    pusch_vars->rxdataF_ext[eNB_id],
                                    // ulsch[UE_id]->harq_process->first_rb, 
                                    22, //ulsch[UE_id]->harq_process->UL_RB_ID_NB_IoT, // index of UL NB_IoT resource block 
                                    ulsch[UE_id]->harq_process->N_sc_RU, // number of subcarriers in UL
                                    0,// ulsch[UE_id]->harq_process->I_sc, // subcarrier indication field
                                    ulsch[UE_id]->harq_process->nb_rb,
                                    l%(frame_parms->symbols_per_tti/2),
                                    l/(frame_parms->symbols_per_tti/2),
                                    frame_parms);

    // lte_ul_channel_estimation_NB_IoT(eNB,proc,
    //                                  eNB_id,
    //                                  UE_id,
    //                                  l%(frame_parms->symbols_per_tti/2),
    //                                  l/(frame_parms->symbols_per_tti/2),
    //                                  cooperation_flag); 

    ul_channel_estimation_NB_IoT(eNB,proc,
                                     eNB_id,
                                     UE_id,
                                     l%(frame_parms->symbols_per_tti/2),
                                     l/(frame_parms->symbols_per_tti/2),
                                     1,
                                     cooperation_flag);
  }

  // if(cooperation_flag == 2) {
  //   for (i=0; i<frame_parms->nb_antennas_rx; i++) {
  //     pusch_vars->ulsch_power_0[i] = signal_energy(pusch_vars->drs_ch_estimates_0[eNB_id][i],
  //                                        ulsch[UE_id]->harq_process->nb_rb*12)*rx_power_correction;
  //     pusch_vars->ulsch_power_1[i] = signal_energy(pusch_vars->drs_ch_estimates_1[eNB_id][i],
  //                                        ulsch[UE_id]->harq_process->nb_rb*12)*rx_power_correction;
  //   }
  // } else {
    for (i=0; i<frame_parms->nb_antennas_rx; i++) {
      /*
      pusch_vars->ulsch_power[i] = signal_energy_nodc(pusch_vars->drs_ch_estimates[eNB_id][i],
                                       ulsch[UE_id]->harq_processes[harq_pid]->nb_rb*12)*rx_power_correction;

      */
      //////////////////////// NB_IoT: maybe, should be defined for NB-IoT
      // pusch_vars->ulsch_power[i] = signal_energy_nodc(pusch_vars->drs_ch_estimates[eNB_id][i],
						// 	  ulsch[UE_id]->harq_process->nb_rb*12); 
      pusch_vars->ulsch_power[i] = signal_energy_nodc(pusch_vars->drs_ch_estimates[eNB_id][i], 12);
      
// #ifdef LOCALIZATION
//       pusch_vars->subcarrier_power = (int32_t *)malloc(ulsch[UE_id]->harq_process->nb_rb*12*sizeof(int32_t));
//       pusch_vars->active_subcarrier = subcarrier_energy(pusch_vars->drs_ch_estimates[eNB_id][i],
//                                           ulsch[UE_id]->harq_process->nb_rb*12, pusch_vars->subcarrier_power, rx_power_correction);
// #endif
    }
 // }

  //write_output("rxdataF_ext.m","rxF_ext",pusch_vars->rxdataF_ext[eNB_id][0],300*(frame_parms->symbols_per_tti-ulsch[UE_id]->srs_active),1,1);
  //write_output("ulsch_chest.m","drs_est",pusch_vars->drs_ch_estimates[eNB_id][0],300*(frame_parms->symbols_per_tti-ulsch[UE_id]->srs_active),1,1);


//   if(cooperation_flag == 2) {
//     ulsch_channel_level_NB_IoT(pusch_vars->drs_ch_estimates_0[eNB_id],
//                                frame_parms,
//                                avgU_0,
//                                ulsch[UE_id]->harq_process->nb_rb);

//     //  printf("[ULSCH] avg_0[0] %d\n",avgU_0[0]);


//     avgs_0 = 0;

//     for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
//       avgs_0 = cmax(avgs_0,avgU_0[(aarx<<1)]);

//     log2_maxh_0 = (log2_approx(avgs_0)/2)+ log2_approx(frame_parms->nb_antennas_rx-1)+3;
// #ifdef DEBUG_ULSCH
//     printf("[ULSCH] log2_maxh_0 = %d (%d,%d)\n",log2_maxh_0,avgU_0[0],avgs_0);
// #endif

//     ulsch_channel_level_NB_IoT(pusch_vars->drs_ch_estimates_1[eNB_id],
//                               frame_parms,
//                               avgU_1,
//                               ulsch[UE_id]->harq_process->nb_rb);

//     //  printf("[ULSCH] avg_1[0] %d\n",avgU_1[0]);


//     avgs_1 = 0;

//     for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
//       avgs_1 = cmax(avgs_1,avgU_1[(aarx<<1)]);

//     log2_maxh_1 = (log2_approx(avgs_1)/2) + log2_approx(frame_parms->nb_antennas_rx-1)+3;
// #ifdef DEBUG_ULSCH
//     printf("[ULSCH] log2_maxh_1 = %d (%d,%d)\n",log2_maxh_1,avgU_1[0],avgs_1);
// #endif
//     log2_maxh = max(log2_maxh_0,log2_maxh_1);
//   } else {
    ulsch_channel_level_NB_IoT(pusch_vars->drs_ch_estimates[eNB_id],
                               frame_parms,
                               avgU,
                               ulsch[UE_id]->harq_process->nb_rb);

    //  printf("[ULSCH] avg[0] %d\n",avgU[0]);


    avgs = 0;

    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
      avgs = cmax(avgs,avgU[(aarx<<1)]);

    //      log2_maxh = 4+(log2_approx(avgs)/2);

    log2_maxh = (log2_approx(avgs)/2)+ log2_approx(frame_parms->nb_antennas_rx-1)+4;

#ifdef DEBUG_ULSCH
    printf("[ULSCH] log2_maxh = %d (%d,%d)\n",log2_maxh,avgU[0],avgs);
#endif
  //}

  for (l=0; l<frame_parms->symbols_per_tti; l++) {
    if (npusch_format == 1){
      if (l==pilot_pos1 || l==pilot_pos2)   // skip pilots
      {
        l++;
      }
    }
    if (npusch_format == 2){
      if (l == pilot_pos_format2[0] || l == pilot_pos_format2[1])   // skip 3 pilots
      {
        l = l + 3;
      }
    }
    // if(cooperation_flag == 2) {

    //   ulsch_channel_compensation_alamouti_NB_IoT(
    //     pusch_vars->rxdataF_ext[eNB_id],
    //     pusch_vars->drs_ch_estimates_0[eNB_id],
    //     pusch_vars->drs_ch_estimates_1[eNB_id],
    //     pusch_vars->ul_ch_mag_0[eNB_id],
    //     pusch_vars->ul_ch_magb_0[eNB_id],
    //     pusch_vars->ul_ch_mag_1[eNB_id],
    //     pusch_vars->ul_ch_magb_1[eNB_id],
    //     pusch_vars->rxdataF_comp_0[eNB_id],
    //     pusch_vars->rxdataF_comp_1[eNB_id],
    //     frame_parms,
    //     l,
    //     Qm,
    //     ulsch[UE_id]->harq_process->nb_rb,
    //     log2_maxh);

    //   ulsch_alamouti_NB_IoT(frame_parms,
    //                  pusch_vars->rxdataF_comp[eNB_id],
    //                  pusch_vars->rxdataF_comp_0[eNB_id],
    //                  pusch_vars->rxdataF_comp_1[eNB_id],
    //                  pusch_vars->ul_ch_mag[eNB_id],
    //                  pusch_vars->ul_ch_magb[eNB_id],
    //                  pusch_vars->ul_ch_mag_0[eNB_id],
    //                  pusch_vars->ul_ch_magb_0[eNB_id],
    //                  pusch_vars->ul_ch_mag_1[eNB_id],
    //                  pusch_vars->ul_ch_magb_1[eNB_id],
    //                  l,
    //                  ulsch[UE_id]->harq_process->nb_rb);
    // } else {
      ulsch_channel_compensation_NB_IoT(
        pusch_vars->rxdataF_ext[eNB_id],
        pusch_vars->drs_ch_estimates[eNB_id],
        pusch_vars->ul_ch_mag[eNB_id],
        pusch_vars->ul_ch_magb[eNB_id],
        pusch_vars->rxdataF_comp[eNB_id],
        frame_parms,
        l,
        Qm,
        ulsch[UE_id]->harq_process->nb_rb,
        log2_maxh); // log2_maxh+I0_shift

   // }


    //eren
    /* if(flagMag == 0){
    //writing for the first time
    write_output(namepointer_log2,"xxx",log2_maxh,1,1,12);

    write_output(namepointer_chMag,"xxx",pusch_vars->ul_ch_mag[eNB_id][0],300,1,11);

    //namepointer_chMag = NULL;
    flagMag=1;
    }*/

    if (frame_parms->nb_antennas_rx > 1)
      ulsch_detection_mrc_NB_IoT(frame_parms,
                          pusch_vars->rxdataF_comp[eNB_id],
                          pusch_vars->ul_ch_mag[eNB_id],
                          pusch_vars->ul_ch_magb[eNB_id],
                          l,
                          ulsch[UE_id]->harq_process->nb_rb);

// #ifndef OFDMA_ULSCH

//     if ((eNB->measurements->n0_power_dB[0]+3)<pusch_vars->ulsch_power[0]) {

      // freq_equalization_NB_IoT(frame_parms,
      //                          pusch_vars->rxdataF_comp[eNB_id],
      //                          pusch_vars->ul_ch_mag[eNB_id],
      //                          pusch_vars->ul_ch_magb[eNB_id],
      //                          l,
      //                          ulsch[UE_id]->harq_process->nb_rb*12,
      //                          Qm); 
      freq_equalization_NB_IoT(frame_parms,
                               pusch_vars->rxdataF_comp[eNB_id],
                               pusch_vars->ul_ch_mag[eNB_id],
                               pusch_vars->ul_ch_magb[eNB_id],
                               l,
                               12,
                               Qm);
//     }

// #endif
      // this function is added in to fill the RB resource elements with zeros
      // before processing the IFDT, in order to avoid absurd values
      fill_rbs_zeros_NB_IoT(eNB, 
                            frame_parms, 
                            pusch_vars->rxdataF_comp[eNB_id], 
                            UE_id,
                            l); 

  }

// #ifndef OFDMA_ULSCH

  //#ifdef DEBUG_ULSCH
  // Inverse-Transform equalized outputs
  //  printf("Doing IDFTs\n");
  // lte_idft_NB_IoT(frame_parms,
  //                 (uint32_t*)pusch_vars->rxdataF_comp[eNB_id][0],
  //                 ulsch[UE_id]->harq_process->nb_rb*12); 
    // lte_idft_NB_IoT(frame_parms,
    //               (uint32_t*)pusch_vars->rxdataF_comp[eNB_id][0],
    //               ulsch[UE_id]->harq_process->12); 
    lte_idft_NB_IoT(frame_parms,
                  (uint32_t*)pusch_vars->rxdataF_comp[eNB_id][0],
                  12); 
  //  printf("Done\n");
  //#endif //DEBUG_ULSCH

// #endif



  llrp = (int16_t*)&pusch_vars->llr[0];

  T(T_ENB_PHY_PUSCH_IQ, T_INT(eNB_id), T_INT(UE_id), T_INT(proc->frame_rx),
    T_INT(subframe), T_INT(ulsch[UE_id]->harq_process->nb_rb),
    T_INT(frame_parms->N_RB_UL), T_INT(frame_parms->symbols_per_tti),
    T_BUFFER(pusch_vars->rxdataF_comp[eNB_id][0],
             2 * /* ulsch[UE_id]->harq_processes[harq_pid]->nb_rb */ frame_parms->N_RB_UL *12*frame_parms->symbols_per_tti*2));

  for (l=0; l<frame_parms->symbols_per_tti; l++) {

    // if (((frame_parms->Ncp == 0) && ((l==3) || (l==10)))||   // skip pilots
    //     ((frame_parms->Ncp == 1) && ((l==2) || (l==8)))) {
    //   l++;
    // } 
    if (npusch_format == 1){
      if (l==pilot_pos1 || l==pilot_pos2)   // skip pilots
      {
        l++;
      }

      // In case of 1 subcarrier: BPSK and QPSK should be rotated by pi/2 and pi/4, respectively 
      if (Nsc_RU == 1){ 

          rotate_single_carrier_NB_IoT(eNB, 
                                    frame_parms, 
                                    pusch_vars->rxdataF_comp[eNB_id], 
                                    UE_id,
                                    l, 
                                    1,
                                    Qm); 

      }

    }
    if (npusch_format == 2){
      if (l == pilot_pos_format2[0] || l == pilot_pos_format2[1])   // skip 3 pilots
      {
        l = l + 3;
      }

      // In case of 1 subcarrier: BPSK and QPSK must be rotated by pi/2 and pi/4, respectively 
      rotate_single_carrier_NB_IoT(eNB, 
                                    frame_parms, 
                                    pusch_vars->rxdataF_comp[eNB_id], 
                                    UE_id,
                                    l, 
                                    1,
                                    Qm); 


      

    }

    switch (Qm) {
    case 1: 
      // In case of BPSK, apply a phase rotation of pi/4 before llr, see 36.211, Table 7.1.1-1 
      rotate_bpsk_NB_IoT(eNB, 
                          frame_parms, 
                          pusch_vars->rxdataF_comp[eNB_id], 
                          UE_id,
                          l); 

      // BPSK always corresponds to single-carrier
      ulsch_bpsk_llr_NB_IoT(eNB, 
                            frame_parms,
                            pusch_vars->rxdataF_comp[eNB_id],
                            pusch_vars->llr,
                            l, 
                            UE_id, 
                            &llrp); 

    break; 
    case 2: 
      ulsch_qpsk_llr_NB_IoT(eNB, 
                            frame_parms,
                            pusch_vars->rxdataF_comp[eNB_id],
                            pusch_vars->llr,
                            l,
                            UE_id,
                            &llrp);
      break;

    default:
#ifdef DEBUG_ULSCH
      printf("ulsch_demodulation.c (rx_ulsch): Unknown Qm!!!!\n");
#endif //DEBUG_ULSCH
      break;
    }
  }

}

