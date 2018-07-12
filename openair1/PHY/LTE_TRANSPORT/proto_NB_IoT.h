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

/*! \file PHY/LTE_TRANSPORT/proto.h
 * \brief Function prototypes for PHY physical/transport channel processing and generation V8.6 2009-03
 * \author R. Knopp, F. Kaltenberger
 * \date 2011
 * \version 0.1
 * \company Eurecom
 * \email: knopp@eurecom.fr
 * \last changes: M. Kanj, V. Savaux
 * \date: 2018
 * \company: b<>com
 * \email: matthieu.kanj@b-com.com, vincent.savaux@b-com.com
 * \note
 * \warning
 */
#ifndef __LTE_TRANSPORT_PROTO_NB_IOT__H__
#define __LTE_TRANSPORT_PROTO_NB_IOT__H__
#include "PHY/defs_NB_IoT.h"
#include "PHY/impl_defs_lte.h"
#include "PHY/defs.h"
//#include <math.h>

//NPSS
void init_unscrambling_lut_NB_IoT(void);

int generate_npss_NB_IoT(int32_t                **txdataF,
                         short                  amp,
                         LTE_DL_FRAME_PARMS  *frame_parms,
                         unsigned short         symbol_offset,          // symbol_offset should equal to 3 for NB-IoT 
                         unsigned short         slot_offset,
                         unsigned short         RB_IoT_ID);             // new attribute (values are between 0.. Max_RB_number-1), it does not exist for LTE

//NSSS

int generate_sss_NB_IoT(int32_t                **txdataF,
                        int16_t                amp,
                        LTE_DL_FRAME_PARMS  *frame_parms, 
                        uint16_t               symbol_offset,             // symbol_offset = 3 for NB-IoT 
                        uint16_t               slot_offset, 
                        unsigned short         frame_number,        // new attribute (Get value from higher layer), it does not exist for LTE
                        unsigned short         RB_IoT_ID);          // new attribute (values are between 0.. Max_RB_number-1), it does not exist for LTE 

//*****************Vincent part for Cell ID estimation from NSSS ******************// 

int rx_nsss_NB_IoT(PHY_VARS_UE_NB_IoT *ue,int32_t *tot_metric); 

int nsss_extract_NB_IoT(PHY_VARS_UE_NB_IoT *ue,
            NB_IoT_DL_FRAME_PARMS *frame_parms,
            int32_t **nsss_ext,
            int l);

//NRS

void generate_pilots_NB_IoT(PHY_VARS_eNB  *phy_vars_eNB,
                            int32_t              **txdataF,
                            int16_t              amp,
                            uint16_t             Ntti,                // Ntti = 10
                            unsigned short       RB_IoT_ID,       // RB reserved for NB-IoT
                            unsigned short       With_NSSS);      // With_NSSS = 1; if the frame include a sub-Frame with NSSS signal


//NPBCH

int allocate_npbch_REs_in_RB(LTE_DL_FRAME_PARMS  *frame_parms,
                             int32_t                **txdataF,
                             uint32_t               *jj,
                             uint32_t               symbol_offset,
                             uint8_t                *x0,
                             uint8_t                pilots,
                             int16_t                amp,
                             unsigned short         id_offset,
                             uint32_t               *re_allocated);

// NPDSCH
int allocate_REs_in_RB_NB_IoT(LTE_DL_FRAME_PARMS    *frame_parms,
                              int32_t               **txdataF,
                              uint32_t              *jj,
                              uint32_t              symbol_offset,
                              uint8_t               *x0,
                              uint8_t               pilots,
                              int16_t               amp,
                              unsigned short        id_offset,
                              uint8_t               pilot_shift,
                              uint32_t              *re_allocated); 


int generate_npbch(NB_IoT_eNB_NPBCH_t     *eNB_npbch,
                   int32_t                **txdataF,
                   int                    amp,
                   LTE_DL_FRAME_PARMS  *frame_parms,
                   uint8_t                *npbch_pdu,
                   uint8_t                frame_mod64,
                   unsigned short         NB_IoT_RB_ID);


void npbch_scrambling(LTE_DL_FRAME_PARMS  *frame_parms,
                      uint8_t                *npbch_e,
                      uint32_t               length);

void dlsch_sib_scrambling_NB_IoT(LTE_DL_FRAME_PARMS     *frame_parms,
                                  NB_IoT_DL_eNB_SIB_t    *dlsch,  //NB_IoT_eNB_NDLSCH_t
                                  int                    tot_bits,                
                                  uint16_t                Nf,
                                  uint8_t                Ns); 

void dlsch_sib_scrambling_rar_NB_IoT(LTE_DL_FRAME_PARMS     *frame_parms,
                                  NB_IoT_DL_eNB_RAR_t    *dlsch,
                                  int                    tot_bits,                // total number of bits to transmit
                                  uint16_t                Nf,              // Nf is the frame number (0..9)
                                  uint8_t                Ns,
                                  uint32_t               rnti);  
/*
int scrambling_npbch_REs_rel_14(LTE_DL_FRAME_PARMS      *frame_parms,
                                int32_t                 **txdataF,
                                uint32_t                *jj,
                                int                     l,
                                uint32_t                symbol_offset,
                                uint8_t                 pilots,
                                unsigned short          id_offset,
                                uint8_t                 *reset,
                                uint32_t                *x1,
                                uint32_t                *x2,
                                uint32_t                *s);
*/
// Functions below implement 36-211 and 36-212

/*Function to pack the DCI*/ 
// newly added function for NB-IoT , does not exist for LTE
void add_dci_NB_IoT(DCI_PDU_NB_IoT    *DCI_pdu,
                    void              *pdu,
                    rnti_t            rnti,
                    unsigned char     dci_size_bytes,
                    unsigned char     aggregation, 
                    unsigned char     dci_size_bits,
                    unsigned char     dci_fmt,
                    uint8_t           npdcch_start_symbol);


/*Use the UL DCI Information to configure PHY and also Pack the DCI*/
int generate_eNB_ulsch_params_from_dci_NB_IoT(PHY_VARS_eNB_NB_IoT     *eNB,
                                              eNB_rxtx_proc_NB_IoT_t  *proc,
                                              DCI_CONTENT             *DCI_Content,
                                              uint16_t                rnti,
                                              DCI_format_NB_IoT_t     dci_format,
                                              uint8_t                 UE_id,
                                              uint8_t                 aggregation,
									                            uint8_t                 npdcch_start_symbol);


/*Use the DL DCI Information to configure PHY and also Pack the DCI*/
int generate_eNB_dlsch_params_from_dci_NB_IoT(PHY_VARS_eNB_NB_IoT    *eNB,
                                              int                    frame,
                                              uint8_t                subframe,
                                              DCI_CONTENT            *DCI_Content,
                                              uint16_t               rnti,
                                              DCI_format_NB_IoT_t    dci_format,
                                              NB_IoT_eNB_NDLSCH_t    *ndlsch,
                                              NB_IoT_DL_FRAME_PARMS  *frame_parms,
                                              uint8_t                aggregation,
									                            uint8_t                npdcch_start_symbol);


/*Function for DCI encoding, scrambling, modulation*/
uint8_t generate_dci_top_NB_IoT(NB_IoT_eNB_NPDCCH_t     *npdcch,
						                    uint8_t                 Num_dci,
                                DCI_ALLOC_NB_IoT_t      *dci_alloc,
                                int16_t                 amp,
                                NB_IoT_DL_FRAME_PARMS   *fp,
                                int32_t                 **txdataF,
                                uint32_t                subframe,
						                    uint8_t                 npdcch_start_symbol);

/*!
  \brief Decoding of PUSCH/ACK/RI/ACK from 36-212.
  @param phy_vars_eNB Pointer to eNB top-level descriptor
  @param proc Pointer to RXTX proc variables
  @param UE_id ID of UE transmitting this PUSCH
  @param subframe Index of subframe for PUSCH
  @param control_only_flag Receive PUSCH with control information only
  @param Nbundled Nbundled parameter for ACK/NAK scrambling from 36-212/36-213
  @param llr8_flag If 1, indicate that the 8-bit turbo decoder should be used
  @returns 0 on success
*/
unsigned int  ulsch_decoding_NB_IoT(PHY_VARS_eNB     *phy_vars_eNB,
                                    eNB_rxtx_proc_t  *proc,
                                    uint8_t                 UE_id,
                                    uint8_t                 control_only_flag,
                                    uint8_t                 Nbundled,
                                    uint8_t                 llr8_flag);

//NB-IoT version
NB_IoT_eNB_NDLSCH_t *new_eNB_dlsch_NB_IoT(//unsigned char Kmimo,
                                          //unsigned char Mdlharq,
                                          uint32_t Nsoft,
                                          //unsigned char N_RB_DL,
                                          uint8_t abstraction_flag,
                                          NB_IoT_DL_FRAME_PARMS* frame_parms);


//  NB_IoT_eNB_NULSCH_t *new_eNB_ulsch_NB_IoT(uint8_t abstraction_flag);


uint8_t subframe2harq_pid_NB_IoT(LTE_DL_FRAME_PARMS *frame_parms,uint32_t frame,uint8_t subframe);


/** \brief Compute Q (modulation order) based on I_MCS for PUSCH.  Implements table 8.6.1-1 from 36.213.
    @param I_MCS */

//uint8_t get_Qm_ul_NB_IoT(uint8_t I_MCS);
unsigned char get_Qm_ul_NB_IoT(unsigned char I_MCS, uint8_t N_sc_RU);

/** \fn dlsch_encoding(PHY_VARS_eNB *eNB,
    uint8_t *input_buffer,
    LTE_DL_FRAME_PARMS *frame_parms,
    uint8_t num_pdcch_symbols,
    LTE_eNB_DLSCH_t *dlsch,
    int frame,
    uint8_t subframe)
    \brief This function performs a subset of the bit-coding functions for LTE as described in 36-212, Release 8.Support is limited to turbo-coded channels (DLSCH/ULSCH). The implemented functions are:
    - CRC computation and addition
    - Code block segmentation and sub-block CRC addition
    - Channel coding (Turbo coding)
    - Rate matching (sub-block interleaving, bit collection, selection and transmission
    - Code block concatenation
    @param eNB Pointer to eNB PHY context
    @param input_buffer Pointer to input buffer for sub-frame
    @param frame_parms Pointer to frame descriptor structure
    @param num_pdcch_symbols Number of PDCCH symbols in this subframe
    @param dlsch Pointer to dlsch to be encoded
    @param frame Frame number
    @param subframe Subframe number
    @param rm_stats Time statistics for rate-matching
    @param te_stats Time statistics for turbo-encoding
    @param i_stats Time statistics for interleaving
    @returns status
*/

int dci_modulation_NB_IoT(int32_t           **txdataF,
              int16_t           amp,
              LTE_DL_FRAME_PARMS   *frame_parms,
              uint8_t           control_region_size,    //XXX we pass the npdcch_start_symbol // control region size for LTE , values between 0..3, (0 for stand-alone / 1, 2 or 3 for in-band)
              uint8_t           *e,          // Input data
              uint8_t           dci_number,       // This variable should takes the 1 or 2 (1 for in case of one DCI, 2 in case of two DCI)
              uint8_t           agr_level,
              int               RB_index,
              int               subframe);   

int dci_allocate_REs_in_RB_NB_IoT(LTE_DL_FRAME_PARMS  *frame_parms,
                                  int32_t           **txdataF,
                                  uint32_t          *jj,
                                  uint32_t          symbol_offset,
                                  uint8_t           *x0[2],
                                  uint8_t           pilots,
                                  int16_t           amp,
                                  unsigned short      id_offset,
                                  uint32_t          *re_allocated,  //  not used variable ??!!
                                  uint8_t           dci_number,   // This variable should takes the 1 or 2 (1 for in case of one DCI, 2 in case of two DCI)
                                  uint8_t           agr_level);

void npdcch_scrambling_NB_IoT(LTE_DL_FRAME_PARMS     *frame_parms,
                uint8_t                  *e,     
                int                      length,          
                uint8_t            Ns,
                uint8_t            dci_number,    
                uint8_t            agr_level);   


int dlsch_modulation_NB_IoT(int32_t               **txdataF,
                            int16_t               amp,
                            LTE_DL_FRAME_PARMS      *frame_parms,
                            uint8_t               control_region_size,      // control region size for LTE , values between 0..3, (0 for stand-alone / 1, 2 or 3 for in-band)
                            NB_IoT_DL_eNB_SIB_t    *dlsch0,  //NB_IoT_eNB_NDLSCH_t
                            int                   G,              // number of bits per subframe
                            unsigned int            npdsch_data_subframe,     // subframe index of the data table of npdsch channel (G*Nsf)  , values are between 0..Nsf        
                            unsigned int            subframe,
                            unsigned short        NB_IoT_RB_ID);

int dlsch_modulation_rar_NB_IoT(int32_t         **txdataF,
              int16_t         amp,
              LTE_DL_FRAME_PARMS      *frame_parms,
              uint8_t         control_region_size,      // control region size for LTE , values between 0..3, (0 for stand-alone / 1, 2 or 3 for in-band)
              NB_IoT_DL_eNB_RAR_t      *dlsch0, //NB_IoT_eNB_NDLSCH_t
              int           G,              // number of bits per subframe
              unsigned int        npdsch_data_subframe,     // subframe index of the data table of npdsch channel (G*Nsf)  , values are between 0..Nsf        
              unsigned int        subframe,
              unsigned short      NB_IoT_RB_ID,
              uint8_t             option);

int32_t dlsch_encoding_NB_IoT(unsigned char              *a,
                              NB_IoT_DL_eNB_SIB_t        *dlsch, // NB_IoT_eNB_NDLSCH_t
                              uint8_t                    Nsf,        // number of subframes required for npdsch pdu transmission calculated from Isf (3GPP spec table)
                              unsigned int               G,
                              uint8_t                    option);         // G (number of available RE) is implicitly multiplied by 2 (since only QPSK modulation)
 
///////////////////////temp function ///////////////////////////////
int dlsch_encoding_rar_NB_IoT(unsigned char           *a,
                    NB_IoT_DL_eNB_RAR_t       *dlsch, //NB_IoT_eNB_NDLSCH_t
                    uint8_t         Nsf,       // number of subframes required for npdsch pdu transmission calculated from Isf (3GPP spec table)
                    unsigned int        G,
                    uint8_t option);
/////////////////////////////////////////////////////////////////
void rx_ulsch_NB_IoT(PHY_VARS_eNB      *phy_vars_eNB,
                     eNB_rxtx_proc_t   *proc,
                     uint8_t                  eNB_id,               // this is the effective sector id
                     uint8_t                  UE_id,
                     NB_IoT_eNB_NULSCH_t      **ulsch,
                     uint8_t                  cooperation_flag);

void ulsch_extract_rbs_single_NB_IoT(int32_t **rxdataF,
                                     int32_t **rxdataF_ext,
                                     // uint32_t first_rb, 
                                     uint16_t UL_RB_ID_NB_IoT, // index of UL NB_IoT resource block !!! may be defined twice : in frame_parms and in NB_IoT_UL_eNB_HARQ_t
                                     uint8_t N_sc_RU, // number of subcarriers in UL 
                                     uint8_t subframe,// uint32_t I_sc, // NB_IoT: subcarrier indication field: must be defined in higher layer
                                     uint32_t nb_rb,
                                     uint8_t l,
                                     uint8_t Ns,
                                     LTE_DL_FRAME_PARMS *frame_parms);

void ulsch_channel_level_NB_IoT(int32_t **drs_ch_estimates_ext,
                                LTE_DL_FRAME_PARMS *frame_parms,
                                int32_t *avg,
                                uint16_t nb_rb);

void ulsch_channel_compensation_NB_IoT(int32_t **rxdataF_ext,
                                int32_t **ul_ch_estimates_ext,
                                int32_t **ul_ch_mag,
                                int32_t **ul_ch_magb,
                                int32_t **rxdataF_comp,
                                LTE_DL_FRAME_PARMS *frame_parms,
                                uint8_t symbol,
                                uint8_t Qm,
                                uint16_t nb_rb,
                                uint8_t output_shift);

void lte_idft_NB_IoT(LTE_DL_FRAME_PARMS *frame_parms,uint32_t *z, uint16_t Msc_PUSCH);

void extract_CQI_NB_IoT(void *o,UCI_format_NB_IoT_t uci_format,NB_IoT_eNB_UE_stats *stats,uint8_t N_RB_DL, uint16_t * crnti, uint8_t * access_mode);

//*****************Vincent part for nprach ******************//
uint32_t RX_NPRACH_NB_IoT(PHY_VARS_eNB *eNB, int frame); 

uint32_t TA_estimation_NB_IoT(PHY_VARS_eNB *eNB, 
                              int16_t *Rx_sub_sampled_buffer, 
                              uint16_t sub_sampling_rate, 
                              uint16_t FRAME_LENGTH_COMPLEX_SUB_SAMPLES, 
                              uint32_t estimated_TA_coarse, 
                              uint8_t coarse); 

uint8_t NPRACH_detection_NB_IoT(int16_t *input_buffer,uint32_t FRAME_LENGTH_COMPLEX_SAMPLESx); 

int16_t* sub_sampling_NB_IoT(int16_t *input_buffer, uint32_t length_input, uint32_t *length_ouput, uint16_t sub_sampling_rate); 

void filtering_signal(int16_t *input_buffer, int16_t *filtered_buffer, uint32_t FRAME_LENGTH_COMPLEX_SAMPLESx); 
//************************************************************//
//*****************Vincent part for ULSCH demodulation ******************//
uint16_t get_UL_sc_start_NB_IoT(uint16_t I_sc); 

void generate_grouphop_NB_IoT(LTE_DL_FRAME_PARMS *frame_parms); 

void init_ul_hopping_NB_IoT(NB_IoT_DL_FRAME_PARMS *frame_parms); 

void rotate_single_carrier_NB_IoT(PHY_VARS_eNB *eNB, 
                                  LTE_DL_FRAME_PARMS *frame_parms,
                                  int32_t **rxdataF_comp, 
                                  uint8_t UE_id,
                                  uint8_t symbol, 
                                  uint8_t counter_msg3,
                                  uint8_t Qm); 

void fill_rbs_zeros_NB_IoT(PHY_VARS_eNB *eNB, 
                                  LTE_DL_FRAME_PARMS *frame_parms,
                                  int32_t **rxdataF_comp, 
                                  uint8_t UE_id,
                                  uint8_t symbol); 

int32_t ulsch_bpsk_llr_NB_IoT(PHY_VARS_eNB *eNB, 
                              LTE_DL_FRAME_PARMS *frame_parms,
                              int32_t **rxdataF_comp,
                              int16_t *ulsch_llr,
                              uint8_t symbol, 
                              uint8_t uint8_t, 
                              int16_t **llrp); 

int32_t ulsch_qpsk_llr_NB_IoT(PHY_VARS_eNB *eNB, 
                              LTE_DL_FRAME_PARMS *frame_parms,
                              int32_t **rxdataF_comp,
                              int16_t *ulsch_llr, 
                              uint8_t symbol, 
                              uint8_t UE_id, 
                              int16_t *llrp);

void rotate_bpsk_NB_IoT(PHY_VARS_eNB *eNB, 
                        LTE_DL_FRAME_PARMS *frame_parms,
                        int32_t **rxdataF_comp, 
                        uint8_t UE_id,
                        uint8_t symbol); 
//************************************************************// 

//************************************************************//
//*****************Vincent part for DLSCH demodulation ******************//

int rx_npdsch_NB_IoT(PHY_VARS_UE_NB_IoT *ue,
                      unsigned char eNB_id,
                      unsigned char eNB_id_i, //if this == ue->n_connected_eNB, we assume MU interference
                      uint32_t frame,
                      uint8_t subframe,
                      unsigned char symbol,
                      unsigned char first_symbol_flag,
                      unsigned char i_mod,
                      unsigned char harq_pid); 

unsigned short dlsch_extract_rbs_single_NB_IoT(int **rxdataF,
                                        int **dl_ch_estimates,
                                        int **rxdataF_ext,
                                        int **dl_ch_estimates_ext,
                                        unsigned short pmi,
                                        unsigned char *pmi_ext,
                                        unsigned int *rb_alloc,
                                        unsigned char symbol,
                                        unsigned char subframe,
                                        uint32_t frame,
                                        uint32_t high_speed_flag,
                                        NB_IoT_DL_FRAME_PARMS *frame_parms); 

void dlsch_channel_level_NB_IoT(int **dl_ch_estimates_ext,
                                NB_IoT_DL_FRAME_PARMS *frame_parms,
                                int32_t *avg,
                                uint8_t symbol,
                                unsigned short nb_rb); 

void dlsch_channel_compensation_NB_IoT(int **rxdataF_ext,
                                        int **dl_ch_estimates_ext,
                                        int **dl_ch_mag,
                                        int **dl_ch_magb,
                                        int **rxdataF_comp,
                                        int **rho,
                                        NB_IoT_DL_FRAME_PARMS *frame_parms,
                                        unsigned char symbol,
                                        uint8_t first_symbol_flag,
                                        unsigned char mod_order,
                                        unsigned short nb_rb,
                                        unsigned char output_shift,
                                        PHY_MEASUREMENTS_NB_IoT *measurements); 

int dlsch_qpsk_llr_NB_IoT(NB_IoT_DL_FRAME_PARMS *frame_parms,
                           int32_t **rxdataF_comp,
                           int16_t *dlsch_llr,
                           uint8_t symbol,
                           uint8_t first_symbol_flag,
                           uint16_t nb_rb,
                           int16_t **llr32p,
                           uint8_t beamforming_mode); 

/// Vincent: temporary functions 

int ul_chest_tmp_NB_IoT(int32_t **rxdataF_ext,
      int32_t **ul_ch_estimates,
      uint8_t l, //symbol within slot 
      uint8_t Ns,
      uint8_t counter_msg3,
      LTE_DL_FRAME_PARMS *frame_parms); 

void rotate_channel_sc_tmp_NB_IoT(int16_t *estimated_channel,
          uint8_t l, 
          uint8_t Qm, 
          uint8_t counter_msg3); 

int ul_chequal_tmp_NB_IoT(int32_t **rxdataF_ext,
      int32_t **rxdataF_comp,
      int32_t **ul_ch_estimates,
      uint8_t l, //symbol within slot 
      uint8_t Ns,
      LTE_DL_FRAME_PARMS *frame_parms);
///

////////////////////////////NB-IoT testing ///////////////////////////////
void clean_eNb_ulsch_NB_IoT(NB_IoT_eNB_NULSCH_t *ulsch);


NB_IoT_eNB_NULSCH_t *new_eNB_ulsch_NB_IoT(uint8_t max_turbo_iterations,uint8_t N_RB_UL, uint8_t abstraction_flag);
//************************************************************//


#endif
