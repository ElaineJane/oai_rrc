/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
    included in this distribution in the file called "COPYING". If not,
    see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@lists.eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

*******************************************************************************/
/*! \file LAYER2/MAC/defs.h
* \brief MAC data structures, constant, and function prototype
* \author Navid Nikaein and Raymond Knopp
* \date 2011
* \version 0.5
* \email navid.nikaein@eurecom.fr

*/
/** @defgroup _oai2  openair2 Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/*@}*/

#ifndef __LAYER2_MAC_DEFS_H__
#define __LAYER2_MAC_DEFS_H__



#ifdef USER_MODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

//#include "COMMON/openair_defs.h"

#include "COMMON/platform_constants.h"
#include "COMMON/mac_rrc_primitives.h"
#include "PHY/defs.h"
#include "RadioResourceConfigCommon.h"
#include "RadioResourceConfigDedicated.h"
#include "MeasGapConfig.h"
#include "TDD-Config.h"
#include "RACH-ConfigCommon.h"
#include "MeasObjectToAddModList.h"
#include "MobilityControlInfo.h"
#ifdef Rel10
#include "MBSFN-AreaInfoList-r9.h"
#include "MBSFN-SubframeConfigList.h"
#include "PMCH-InfoList-r9.h"
#include "SCellToAddMod-r10.h"
#endif

//#ifdef PHY_EMUL
//#include "SIMULATION/PHY_EMULATION/impl_defs.h"
//#endif

/** @defgroup _mac  MAC
 * @ingroup _oai2
 * @{
 */

#define BCCH_PAYLOAD_SIZE_MAX 128
#define CCCH_PAYLOAD_SIZE_MAX 128

#define SCH_PAYLOAD_SIZE_MAX 4096
/// Logical channel ids from 36-311 (Note BCCH is not specified in 36-311, uses the same as first DRB)

#ifdef Rel10

// Mask for identifying subframe for MBMS
#define MBSFN_TDD_SF3 0x80// for TDD
#define MBSFN_TDD_SF4 0x40
#define MBSFN_TDD_SF7 0x20
#define MBSFN_TDD_SF8 0x10
#define MBSFN_TDD_SF9 0x08
#define MBSFN_FDD_SF1 0x80// for FDD
#define MBSFN_FDD_SF2 0x40
#define MBSFN_FDD_SF3 0x20
#define MBSFN_FDD_SF6 0x10
#define MBSFN_FDD_SF7 0x08
#define MBSFN_FDD_SF8 0x04

#define MAX_MBSFN_AREA 8
#define MAX_PMCH_perMBSFN 15
/*!\brief MAX MCCH payload size  */
#define MCCH_PAYLOAD_SIZE_MAX 128
//#define MCH_PAYLOAD_SIZE_MAX 16384// this value is using in case mcs and TBS index are high
#endif

#ifdef USER_MODE
#define printk printf
#endif //USER_MODE

/*!\brief Maximum number of logical channl group IDs */
#define MAX_NUM_LCGID 4
/*!\brief logical channl group ID 0 */
#define LCGID0 0
/*!\brief logical channl group ID 1 */
#define LCGID1 1
/*!\brief logical channl group ID 2 */
#define LCGID2 2
/*!\brief logical channl group ID 3 */
#define LCGID3 3
/*!\brief Maximum number of logical chanels */
#define MAX_NUM_LCID 11
/*!\brief Maximum number od control elemenets */
#define MAX_NUM_CE 5
/*!\brief Maximum number of random access process */
#define NB_RA_PROC_MAX 4
/*!\brief size of buffer status report table */
#define BSR_TABLE_SIZE 64
/*!\brief The power headroom reporting range is from -23 ...+40 dB and beyond, with step 1 */
#define PHR_MAPPING_OFFSET 23  // if ( x>= -23 ) val = floor (x + 23) 
/*!\brief maximum number of resource block groups */
#define N_RBG_MAX 25 // for 20MHz channel BW
/*!\brief minimum value for channel quality indicator */
#define MIN_CQI_VALUE  0
/*!\brief maximum value for channel quality indicator */
#define MAX_CQI_VALUE  15


#define LCID_EMPTY 0
#define LCID_NOT_EMPTY 1

/* 
 * eNB part 
 */ 


/* 
 * UE/ENB common part 
 */ 
/*!\brief MAC header of Random Access Response for Random access preamble identifier (RAPID) */
typedef struct {
  uint8_t RAPID:6;
  uint8_t T:1;
  uint8_t E:1;
} __attribute__((__packed__))RA_HEADER_RAPID;

/*!\brief  MAC header of Random Access Response for backoff indicator (BI)*/
typedef struct {
  uint8_t BI:4;
  uint8_t R:2;
  uint8_t T:1;
  uint8_t E:1;
} __attribute__((__packed__))RA_HEADER_BI;
/*
typedef struct {
  uint64_t padding:16;
  uint64_t t_crnti:16;
  uint64_t hopping_flag:1;
  uint64_t rb_alloc:10;
  uint64_t mcs:4;
  uint64_t TPC:3;
  uint64_t UL_delay:1;
  uint64_t cqi_req:1;
  uint64_t Timing_Advance_Command:11;  // first/2nd octet LSB
  uint64_t R:1;                        // octet MSB
  } __attribute__((__packed__))RAR_PDU;

typedef struct {
  uint64_t padding:16;
  uint64_t R:1;                        // octet MSB
  uint64_t Timing_Advance_Command:11;  // first/2nd octet LSB
  uint64_t cqi_req:1;
  uint64_t UL_delay:1;
  uint64_t TPC:3;
  uint64_t mcs:4;
  uint64_t rb_alloc:10;
  uint64_t hopping_flag:1;
  uint64_t t_crnti:16;
  } __attribute__((__packed__))RAR_PDU;

#define sizeof_RAR_PDU 6
*/
/*!\brief  MAC subheader short with 7bit Length field */
typedef struct {
  uint8_t LCID:5;  // octet 1 LSB
  uint8_t E:1;
  uint8_t R:2;     // octet 1 MSB
  uint8_t L:7;     // octet 2 LSB
  uint8_t F:1;     // octet 2 MSB
} __attribute__((__packed__))SCH_SUBHEADER_SHORT;
/*!\brief  MAC subheader long  with 15bit Length field */
typedef struct {
  uint8_t LCID:5;   // octet 1 LSB
  uint8_t E:1;
  uint8_t R:2;      // octet 1 MSB
  uint8_t L_MSB:7;
  uint8_t F:1;      // octet 2 MSB
  uint8_t L_LSB:8;
  uint8_t padding;
} __attribute__((__packed__))SCH_SUBHEADER_LONG;
/*!\brief MAC subheader short without length field */
typedef struct {
  uint8_t LCID:5;
  uint8_t E:1;
  uint8_t R:2;
} __attribute__((__packed__))SCH_SUBHEADER_FIXED;

/*!\brief  mac control element: short buffer status report for a specific logical channel group ID*/
typedef struct {
  uint8_t Buffer_size:6;  // octet 1 LSB
  uint8_t LCGID:2;        // octet 1 MSB
} __attribute__((__packed__))BSR_SHORT;

typedef BSR_SHORT BSR_TRUNCATED;
/*!\brief  mac control element: long buffer status report for all logical channel group ID*/
typedef struct {
  uint32_t Buffer_size3:6;
  uint32_t Buffer_size2:6;
  uint32_t Buffer_size1:6;
  uint32_t Buffer_size0:6;
  uint32_t padding:8;
} __attribute__((__packed__))BSR_LONG;

#define BSR_LONG_SIZE  (sizeof(BSR_LONG))
/*!\brief  mac control element: timing advance  */
typedef struct {
  uint8_t TA:6;
  uint8_t R:2;
} __attribute__((__packed__))TIMING_ADVANCE_CMD;
/*!\brief  mac control element: power headroom report  */
typedef struct {
  uint8_t PH:6;
  uint8_t R:2;
} __attribute__((__packed__))POWER_HEADROOM_CMD;

/*!\brief  DCI PDU filled by MAC for the PHY  */
typedef struct {
  uint8_t Num_ue_spec_dci ;
  uint8_t Num_common_dci  ;
  unsigned int nCCE;
  DCI_ALLOC_t dci_alloc[NUM_DCI_MAX] ;
} DCI_PDU;
/*! \brief CCCH payload */
typedef struct {
  uint8_t payload[CCCH_PAYLOAD_SIZE_MAX] ;
} __attribute__((__packed__))CCCH_PDU;
/*! \brief BCCH payload */
typedef struct {
  uint8_t payload[BCCH_PAYLOAD_SIZE_MAX] ;
} __attribute__((__packed__))BCCH_PDU;

#ifdef Rel10
/*! \brief MCCH payload */
typedef struct {
  uint8_t payload[MCCH_PAYLOAD_SIZE_MAX] ;
} __attribute__((__packed__))MCCH_PDU;
/*!< \brief MAC control element for activation and deactivation of component carriers */
typedef struct {
  uint8_t C7:1;/*!< \brief Component carrier 7 */
  uint8_t C6:1;/*!< \brief Component carrier 6 */
  uint8_t C5:1;/*!< \brief Component carrier 5 */
  uint8_t C4:1;/*!< \brief Component carrier 4 */
  uint8_t C3:1;/*!< \brief Component carrier 3 */
  uint8_t C2:1;/*!< \brief Component carrier 2 */
  uint8_t C1:1;/*!< \brief Component carrier 1 */
  uint8_t R:1;/*!< \brief Reserved  */
} __attribute__((__packed__))CC_ELEMENT;
/*! \brief MAC control element: MCH Scheduling Information */
typedef struct {
  uint8_t stop_sf_MSB:3; // octet 1 LSB
  uint8_t lcid:5;        // octet 2 MSB
  uint8_t stop_sf_LSB:8;
} __attribute__((__packed__))MSI_ELEMENT;
#endif
/*! \brief Values of CCCH LCID for DLSCH */ 
#define CCCH_LCHANID 0
/*!\brief Values of BCCH logical channel */
#define BCCH 3  // SI 
/*!\brief Value of CCCH / SRB0 logical channel */
#define CCCH 0  // srb0
/*!\brief DCCH / SRB1 logical channel */
#define DCCH 1  // srb1
/*!\brief DCCH1 / SRB2  logical channel */
#define DCCH1 2 // srb2
/*!\brief DTCH DRB1  logical channel */
#define DTCH 3 // LCID
/*!\brief MCCH logical channel */
#define MCCH 4 
/*!\brief MTCH logical channel */
#define MTCH 1 
// DLSCH LCHAN ID
/*!\brief LCID of UE contention resolution identity for DLSCH*/
#define UE_CONT_RES 28
/*!\brief LCID of timing advance for DLSCH */
#define TIMING_ADV_CMD 29
/*!\brief LCID of discontinous reception mode for DLSCH */
#define DRX_CMD 30
/*!\brief LCID of padding LCID for DLSCH */
#define SHORT_PADDING 31

#ifdef Rel10
// MCH LCHAN IDs (table6.2.1-4 TS36.321)
/*!\brief LCID of MCCH for DL */
#define MCCH_LCHANID 0
/*!\brief LCID of MCH scheduling info for DL */
#define MCH_SCHDL_INFO 3
/*!\brief LCID of Carrier component activation/deactivation */
#define CC_ACT_DEACT 27
#endif

// ULSCH LCHAN IDs
/*!\brief LCID of extended power headroom for ULSCH */
#define EXTENDED_POWER_HEADROOM 25
/*!\brief LCID of power headroom for ULSCH */
#define POWER_HEADROOM 26
/*!\brief LCID of CRNTI for ULSCH */
#define CRNTI 27
/*!\brief LCID of truncated BSR for ULSCH */
#define TRUNCATED_BSR 28
/*!\brief LCID of short BSR for ULSCH */
#define SHORT_BSR 29
/*!\brief LCID of long BSR for ULSCH */
#define LONG_BSR 30

/*! \brief Downlink SCH PDU Structure */
typedef struct {
  int8_t payload[8][SCH_PAYLOAD_SIZE_MAX];
  uint16_t Pdu_size[8];
} __attribute__ ((__packed__)) DLSCH_PDU;

/*! \brief MCH PDU Structure */
typedef struct {
  int8_t payload[SCH_PAYLOAD_SIZE_MAX];
  uint16_t Pdu_size;
  uint8_t mcs;
  uint8_t sync_area;
  uint8_t msi_active;
  uint8_t mcch_active;
  uint8_t mtch_active;
} __attribute__ ((__packed__)) MCH_PDU;

/*! \brief Uplink SCH PDU Structure */
typedef struct {
  int8_t payload[SCH_PAYLOAD_SIZE_MAX];         /*!< \brief SACH payload */
  uint16_t Pdu_size;
} __attribute__ ((__packed__)) ULSCH_PDU;

#include "PHY/impl_defs_top.h"

/*!\brief  UE ULSCH scheduling states*/
typedef enum {
  S_UL_NONE =0,
  S_UL_WAITING,
  S_UL_SCHEDULED,
  S_UL_BUFFERED,
  S_UL_NUM_STATUS
} UE_ULSCH_STATUS;

/*!\brief  UE DLSCH scheduling states*/
typedef enum {
  S_DL_NONE =0,
  S_DL_WAITING,
  S_DL_SCHEDULED,
  S_DL_BUFFERED,
  S_DL_NUM_STATUS
} UE_DLSCH_STATUS;

/*!\brief  scheduling policy for the contention-based access */
typedef enum {
  CBA_ES=0, /// equal share of RB among groups w
  CBA_ES_S,  /// equal share of RB among groups with small allocation
  CBA_PF, /// proportional fair (kind of)
  CBA_PF_S,  /// proportional fair (kind of) with small RB allocation
  CBA_RS /// random allocation
} CBA_POLICY;


/*! \brief temporary struct for ULSCH sched */
typedef struct {
  rnti_t rnti;
  uint16_t subframe;
  uint16_t serving_num;
  UE_ULSCH_STATUS status;
} eNB_ULSCH_INFO;
/*! \brief temp struct for DLSCH sched */
typedef struct {
  rnti_t rnti;
  uint16_t weight;
  uint16_t subframe;
  uint16_t serving_num;
  UE_DLSCH_STATUS status;
} eNB_DLSCH_INFO;
/*! \brief eNB overall statistics */
typedef struct {
  /// num BCCH PDU per CC 
  uint32_t total_num_bcch_pdu;
  /// BCCH buffer size  
  uint32_t bcch_buffer;
  /// total BCCH buffer size  
  uint32_t total_bcch_buffer;
  /// BCCH MCS
  uint32_t bcch_mcs;

  /// num CCCH PDU per CC 
  uint32_t total_num_ccch_pdu;
  /// BCCH buffer size  
  uint32_t ccch_buffer;
  /// total BCCH buffer size  
  uint32_t total_ccch_buffer;
  /// BCCH MCS
  uint32_t ccch_mcs;

/// num active users
  uint16_t num_dlactive_UEs;
  ///  available number of PRBs for a give SF
  uint16_t available_prbs;
  /// total number of PRB available for the user plane
  uint32_t total_available_prbs;
  /// aggregation
  /// total avilable nccc : num control channel element
  uint16_t available_ncces;
  // only for a new transmission, should be extended for retransmission
  // current dlsch  bit rate for all transport channels
  uint32_t dlsch_bitrate;
  //
  uint32_t dlsch_bytes_tx;
  //
  uint32_t dlsch_pdus_tx;
  //
  uint32_t total_dlsch_bitrate;
  //
  uint32_t total_dlsch_bytes_tx;
  //
  uint32_t total_dlsch_pdus_tx;
  
  // here for RX
  //
  uint32_t ulsch_bitrate;
  //
  uint32_t ulsch_bytes_rx;
  //
  uint64_t ulsch_pdus_rx; 

  uint32_t total_ulsch_bitrate;
  //
  uint32_t total_ulsch_bytes_rx;
  //
  uint32_t total_ulsch_pdus_rx;
  
} eNB_STATS;
/*! \brief eNB statistics for the connected UEs*/
typedef struct {

  /// CRNTI of UE
  rnti_t crnti; ///user id (rnti) of connected UEs
  // rrc status
  uint8_t rrc_status;
  /// harq pid
  uint8_t harq_pid;
  /// harq rounf
  uint8_t harq_round;
  /// DL Wideband CQI index (2 TBs)
  uint8_t dl_cqi;
  /// total available number of PRBs for a new transmission
  uint16_t rbs_used;
  /// total available number of PRBs for a retransmission
  uint16_t rbs_used_retx;
  /// total nccc used for a new transmission: num control channel element
  uint16_t ncce_used;
  /// total avilable nccc for a retransmission: num control channel element
  uint16_t ncce_used_retx;

  // mcs1 before the rate adaptaion
  uint8_t dlsch_mcs1;
  /// Target mcs2 after rate-adaptation
  uint8_t dlsch_mcs2;
  //  current TBS with mcs2
  uint32_t TBS;
  //  total TBS with mcs2
  //  uint32_t total_TBS;
  //  total rb used for a new transmission
  uint32_t total_rbs_used;
  //  total rb used for retransmission
  uint32_t total_rbs_used_retx;

   /// TX
  /// Num pkt
  uint32_t num_pdu_tx[NB_RB_MAX];
  /// num bytes
  uint32_t num_bytes_tx[NB_RB_MAX];
  /// num retransmission / harq
  uint32_t num_retransmission;
  /// instantaneous tx throughput for each TTI
  //  uint32_t tti_throughput[NB_RB_MAX];

  /// overall
  //
  uint32_t  dlsch_bitrate;
  //total
  uint32_t  total_dlsch_bitrate;
  /// headers+ CE +  padding bytes for a MAC PDU
  uint64_t overhead_bytes;
  /// headers+ CE +  padding bytes for a MAC PDU
  uint64_t total_overhead_bytes;
  /// headers+ CE +  padding bytes for a MAC PDU
  uint64_t avg_overhead_bytes;
  // MAC multiplexed payload
  uint64_t total_sdu_bytes;
  // total MAC pdu bytes
  uint64_t total_pdu_bytes;

  // total num pdu
  uint32_t total_num_pdus;
  //
  //  uint32_t avg_pdu_size;

  /// RX

  /// preassigned mcs after rate adaptation
  uint8_t ulsch_mcs1;
  /// adjusted mcs
  uint8_t ulsch_mcs2;

  /// estimated average pdu inter-departure time
  uint32_t avg_pdu_idt;
  /// estimated average pdu size
  uint32_t avg_pdu_ps;
  ///
  uint32_t aggregated_pdu_size;
  uint32_t aggregated_pdu_arrival;

  ///  uplink transport block size
  uint32_t ulsch_TBS;

  ///  total rb used for a new uplink transmission
  uint32_t num_retransmission_rx;
  ///  total rb used for a new uplink transmission
  uint32_t rbs_used_rx;
   ///  total rb used for a new uplink retransmission
  uint32_t rbs_used_retx_rx;
  ///  total rb used for a new uplink transmission
  uint32_t total_rbs_used_rx;
  /// normalized rx power 
  int32_t      normalized_rx_power;
   /// target rx power 
  int32_t    target_rx_power;

  /// num rx pdu
  uint32_t num_pdu_rx[NB_RB_MAX];
  /// num bytes rx
  uint32_t num_bytes_rx[NB_RB_MAX];
  /// instantaneous rx throughput for each TTI
  //  uint32_t tti_goodput[NB_RB_MAX];
  /// errors
  uint32_t num_errors_rx;
  
  uint64_t overhead_bytes_rx;
  /// headers+ CE +  padding bytes for a MAC PDU
  uint64_t total_overhead_bytes_rx;
  /// headers+ CE +  padding bytes for a MAC PDU
  uint64_t avg_overhead_bytes_rx;
 //
  uint32_t  ulsch_bitrate;
  //total
  uint32_t  total_ulsch_bitrate;
  /// overall
  ///  MAC pdu bytes
  uint64_t pdu_bytes_rx;
  /// total MAC pdu bytes
  uint64_t total_pdu_bytes_rx;
  /// total num pdu
  uint32_t total_num_pdus_rx;
  /// num of error pdus
  uint32_t total_num_errors_rx;

} eNB_UE_STATS;
/*! \brief eNB template for UE context information  */
typedef struct {
  /// C-RNTI of UE
  rnti_t rnti;
  /// NDI from last scheduling
  uint8_t oldNDI[8];
  /// NDI from last UL scheduling
  uint8_t oldNDI_UL[8];
  /// Flag to indicate UL has been scheduled at least once
  boolean_t ul_active;
  /// Flag to indicate UE has been configured (ACK from RRCConnectionSetup received)
  boolean_t configured;

  // PHY interface info

  /// DCI format for DLSCH
  uint16_t DLSCH_dci_fmt;

  /// Current Aggregation Level for DCI
  uint8_t DCI_aggregation_min;

  /// size of DLSCH size in bit 
  uint8_t DLSCH_dci_size_bits;

  /// DCI buffer for DLSCH
  uint8_t DLSCH_DCI[8][(MAX_DCI_SIZE_BITS>>3)+1];

  /// Number of Allocated RBs for DL after scheduling (prior to frequency allocation)
  uint16_t nb_rb[8]; // num_max_harq

  /// Number of Allocated RBs for UL after scheduling (prior to frequency allocation)
  uint16_t nb_rb_ul[8]; // num_max_harq

  /// Number of Allocated RBs by the ulsch preprocessor
  uint8_t pre_allocated_nb_rb_ul;

  /// index of Allocated RBs by the ulsch preprocessor
  int8_t pre_allocated_rb_table_index_ul;

  /// total allocated RBs
  int8_t total_allocated_rbs;

  /// pre-assigned MCS by the ulsch preprocessor
  uint8_t pre_assigned_mcs_ul;

  /// assigned MCS by the ulsch scheduler
  uint8_t assigned_mcs_ul;

  /// DCI buffer for ULSCH
  uint8_t ULSCH_DCI[8][(MAX_DCI_SIZE_BITS>>3)+1];

  /// DL DAI
  uint8_t DAI;

  /// UL DAI
  uint8_t DAI_ul[10];

  /// UL Scheduling Request Received
  uint8_t ul_SR;

  ///Resource Block indication for each sub-band in MU-MIMO
  uint8_t rballoc_subband[8][50];

  // Logical channel info for link with RLC

  /// UE BSR info for each logical channel group id
  uint8_t bsr_info[MAX_NUM_LCGID];

  /// phr information
  int8_t phr_info;

  /// phr information
  int8_t phr_info_configured;

  ///dl buffer info
  uint32_t dl_buffer_info[MAX_NUM_LCID];
  /// total downlink buffer info
  uint32_t dl_buffer_total;
  /// total downlink pdus
  uint32_t dl_pdus_total;
  /// downlink pdus for each LCID
  uint32_t dl_pdus_in_buffer[MAX_NUM_LCID];
  /// creation time of the downlink buffer head for each LCID
  uint32_t dl_buffer_head_sdu_creation_time[MAX_NUM_LCID];
  /// maximum creation time of the downlink buffer head across all LCID
  uint32_t  dl_buffer_head_sdu_creation_time_max;
  /// a flag indicating that the downlink head SDU is segmented  
  uint8_t    dl_buffer_head_sdu_is_segmented[MAX_NUM_LCID];
  /// size of remaining size to send for the downlink head SDU
  uint32_t dl_buffer_head_sdu_remaining_size_to_send[MAX_NUM_LCID];

  /// total uplink buffer size 
  uint32_t ul_total_buffer;
  /// uplink buffer creation time for each LCID
  uint32_t ul_buffer_creation_time[MAX_NUM_LCGID];
  /// maximum uplink buffer creation time across all the LCIDs
  uint32_t ul_buffer_creation_time_max;
  /// uplink buffer size per LCID
  uint32_t ul_buffer_info[MAX_NUM_LCGID];

  /// UE tx power
  int32_t ue_tx_power;

  /// stores the frame where the last TPC was transmitted
  uint32_t pusch_tpc_tx_frame;
  uint32_t pusch_tpc_tx_subframe;
  uint32_t pucch_tpc_tx_frame;
  uint32_t pucch_tpc_tx_subframe;

#ifdef LOCALIZATION
  eNB_UE_estimated_distances distance;
#endif
} UE_TEMPLATE;

/*! \brief scheduling control information set through an API (not used)*/
typedef struct {
  ///UL transmission bandwidth in RBs
  uint8_t ul_bandwidth[MAX_NUM_LCID];
  ///DL transmission bandwidth in RBs
  uint8_t dl_bandwidth[MAX_NUM_LCID];

  //To do GBR bearer
  uint8_t min_ul_bandwidth[MAX_NUM_LCID];

  uint8_t min_dl_bandwidth[MAX_NUM_LCID];

  ///aggregated bit rate of non-gbr bearer per UE
  uint64_t  ue_AggregatedMaximumBitrateDL;
  ///aggregated bit rate of non-gbr bearer per UE
  uint64_t  ue_AggregatedMaximumBitrateUL;
  ///CQI scheduling interval in subframes.
  uint16_t cqiSchedInterval;
  ///Contention resolution timer used during random access
  uint8_t mac_ContentionResolutionTimer;

  uint16_t max_allowed_rbs[MAX_NUM_LCID];

  uint8_t max_mcs[MAX_NUM_LCID];

  uint16_t priority[MAX_NUM_LCID];

  // resource scheduling information
  uint8_t       harq_pid[MAX_NUM_CCs];
  uint8_t       round[MAX_NUM_CCs];
  uint8_t       dl_pow_off[MAX_NUM_CCs];
  uint16_t      pre_nb_available_rbs[MAX_NUM_CCs];
  unsigned char rballoc_sub_UE[MAX_NUM_CCs][N_RBG_MAX];

} UE_sched_ctrl;
/*! \brief eNB template for the Random access information */
typedef struct {
  /// Flag to indicate this process is active
  boolean_t RA_active;
  /// Size of DCI for RA-Response (bytes)
  uint8_t RA_dci_size_bytes1;
  /// Size of DCI for RA-Response (bits)
  uint8_t RA_dci_size_bits1;
  /// Actual DCI to transmit for RA-Response
  uint8_t RA_alloc_pdu1[(MAX_DCI_SIZE_BITS>>3)+1];
  /// DCI format for RA-Response (should be 1A)
  uint8_t RA_dci_fmt1;
  /// Size of DCI for Msg4/ContRes (bytes)
  uint8_t RA_dci_size_bytes2;
  /// Size of DCI for Msg4/ContRes (bits)
  uint8_t RA_dci_size_bits2;
  /// Actual DCI to transmit for Msg4/ContRes
  uint8_t RA_alloc_pdu2[(MAX_DCI_SIZE_BITS>>3)+1];
  /// DCI format for Msg4/ContRes (should be 1A)
  uint8_t RA_dci_fmt2;
  /// Flag to indicate the eNB should generate RAR.  This is triggered by detection of PRACH
  uint8_t generate_rar;
  /// Subframe where preamble was received
  uint8_t preamble_subframe;
  /// Subframe where Msg3 is to be sent
  uint8_t Msg3_subframe;
  /// Flag to indicate the eNB should generate Msg4 upon reception of SDU from RRC.  This is triggered by first ULSCH reception at eNB for new user.
  uint8_t generate_Msg4;
  /// Flag to indicate the eNB should generate the DCI for Msg4, after getting the SDU from RRC.
  uint8_t generate_Msg4_dci;
  /// Flag to indicate that eNB is waiting for ACK that UE has received Msg3.
  uint8_t wait_ack_Msg4;
  /// UE RNTI allocated during RAR
  rnti_t rnti;
  /// RA RNTI allocated from received PRACH
  uint16_t RA_rnti;
  /// Received preamble_index
  uint8_t preamble_index;
  /// Received UE Contention Resolution Identifier
  uint8_t cont_res_id[6];
  /// Timing offset indicated by PHY
  int16_t timing_offset;
  /// Timeout for RRC connection
  int16_t RRC_timer;
} RA_TEMPLATE;


/*! \brief subband bitmap confguration (for ALU icic algo purpose), in test phase */
typedef struct {
  uint8_t sbmap[NUMBER_OF_SUBBANDS_MAX]; //13 = number of SB MAX for 100 PRB
  uint8_t periodicity;
  uint8_t first_subframe;
  uint8_t sb_size;
  uint8_t nb_active_sb;
} SBMAP_CONF;
/*! \brief UE list used by eNB to order UEs/CC for scheduling*/ 
typedef struct {
  /// DLSCH pdu 
  DLSCH_PDU DLSCH_pdu[MAX_NUM_CCs][2][NUMBER_OF_UE_MAX];
  /// DCI template and MAC connection parameters for UEs
  UE_TEMPLATE UE_template[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  /// DCI template and MAC connection for RA processes
  int pCC_id[NUMBER_OF_UE_MAX];
  /// sorted downlink component carrier for the scheduler 
  int ordered_CCids[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  /// number of downlink active component carrier 
  int numactiveCCs[NUMBER_OF_UE_MAX];
  /// sorted uplink component carrier for the scheduler 
  int ordered_ULCCids[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  /// number of uplink active component carrier 
  int numactiveULCCs[NUMBER_OF_UE_MAX];
  /// number of downlink active component carrier 
  uint8_t dl_CC_bitmap[NUMBER_OF_UE_MAX];
  /// eNB to UE statistics
  eNB_UE_STATS eNB_UE_stats[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  /// scheduling control info
  UE_sched_ctrl UE_sched_ctrl[NUMBER_OF_UE_MAX];

  int next[NUMBER_OF_UE_MAX];
  int head;
  int next_ul[NUMBER_OF_UE_MAX];
  int head_ul;
  int avail;
  int num_UEs;
  boolean_t active[NUMBER_OF_UE_MAX];
} UE_list_t;

typedef struct {
long hys;
long ttt_ms;
}Handover_eNB_info;

/*! \brief eNB common channels */ 
typedef struct {
  /// Outgoing DCI for PHY generated by eNB scheduler
  DCI_PDU DCI_pdu;
  /// Outgoing BCCH pdu for PHY
  BCCH_PDU BCCH_pdu;
  /// Outgoing BCCH DCI allocation
  uint32_t BCCH_alloc_pdu;
  /// Outgoing CCCH pdu for PHY
  CCCH_PDU CCCH_pdu;
  RA_TEMPLATE RA_template[NB_RA_PROC_MAX];
  /// BCCH active flag
  uint8_t bcch_active;
  /// MBSFN SubframeConfig
  struct MBSFN_SubframeConfig *mbsfn_SubframeConfig[8];
  /// number of subframe allocation pattern available for MBSFN sync area
  uint8_t num_sf_allocation_pattern;
#ifdef Rel10
  /// MBMS Flag
  uint8_t MBMS_flag;
  /// Outgoing MCCH pdu for PHY
  MCCH_PDU MCCH_pdu;
  /// MCCH active flag
  uint8_t msi_active;
  /// MCCH active flag
  uint8_t mcch_active;
  /// MTCH active flag
  uint8_t mtch_active;
  /// number of active MBSFN area
  uint8_t num_active_mbsfn_area;
  /// MBSFN Area Info
  struct  MBSFN_AreaInfo_r9 *mbsfn_AreaInfo[MAX_MBSFN_AREA];
  /// PMCH Config
  struct PMCH_Config_r9 *pmch_Config[MAX_PMCH_perMBSFN];
  /// MBMS session info list
  struct MBMS_SessionInfoList_r9 *mbms_SessionList[MAX_PMCH_perMBSFN];
  /// Outgoing MCH pdu for PHY
  MCH_PDU MCH_pdu;
#endif
#ifdef CBA
  /// number of CBA groups 
  uint8_t num_active_cba_groups;
  /// RNTI for each CBA group 
  uint16_t cba_rnti[NUM_MAX_CBA_GROUP];
  /// MCS for each CBA group 
  uint8_t group_mcs[NUM_MAX_CBA_GROUP];
#endif
} COMMON_channels_t;
/*! \brief top level eNB MAC structure */ 
typedef struct {
  ///
  uint16_t Node_id;
  /// frame counter
  frame_t frame;
  /// subframe counter
  sub_frame_t subframe;
  /// Common cell resources
  COMMON_channels_t common_channels[MAX_NUM_CCs];
  UE_list_t UE_list;
  ///subband bitmap configuration
  SBMAP_CONF sbmap_conf;

  ///  active flag for Other lcid
  //  uint8_t lcid_active[NB_RB_MAX];
  /// eNB stats
  eNB_STATS eNB_stats[MAX_NUM_CCs];
  // MAC function execution peformance profiler
  /// processing time of eNB scheduler 
  time_stats_t eNB_scheduler;
  /// processing time of eNB scheduler for SI 
  time_stats_t schedule_si;
  /// processing time of eNB scheduler for Random access
  time_stats_t schedule_ra;
  /// processing time of eNB ULSCH scheduler 
  time_stats_t schedule_ulsch;
  /// processing time of eNB DCI generation
  time_stats_t fill_DLSCH_dci;
  /// processing time of eNB MAC preprocessor
  time_stats_t schedule_dlsch_preprocessor;
  /// processing time of eNB DLSCH scheduler 
  time_stats_t schedule_dlsch; // include rlc_data_req + MAC header + preprocessor
  /// processing time of eNB MCH scheduler 
  time_stats_t schedule_mch;
  /// processing time of eNB ULSCH reception
  time_stats_t rx_ulsch_sdu; // include rlc_data_ind
  Handover_eNB_info ho_info;
} eNB_MAC_INST;

/* 
 * UE part 
 */ 

/*!\brief UE layer 2 status */
typedef enum {
  CONNECTION_OK=0,
  CONNECTION_LOST,
  PHY_RESYNCH,
  PHY_HO_PRACH
} UE_L2_STATE_t;

/*!\brief UE scheduling info */
typedef struct {
  /// buffer status for each lcgid
  uint8_t  BSR[MAX_NUM_LCGID]; // should be more for mesh topology
  /// keep the number of bytes in rlc buffer for each lcid
  uint16_t  BSR_bytes[MAX_NUM_LCGID];
  /// buffer status for each lcid
  uint8_t  LCID_status[MAX_NUM_LCID];
  /// SR pending as defined in 36.321
  uint8_t  SR_pending;
  /// SR_COUNTER as defined in 36.321
  uint16_t SR_COUNTER;
  /// logical channel group ide for each LCID
  uint8_t  LCGID[MAX_NUM_LCID];
  /// retxBSR-Timer, default value is sf2560
  uint16_t retxBSR_Timer;
  /// retxBSR_SF, number of subframe before triggering a regular BSR
  int16_t retxBSR_SF;
  /// periodicBSR-Timer, default to infinity
  uint16_t periodicBSR_Timer;
  /// periodicBSR_SF, number of subframe before triggering a periodic BSR
  int16_t periodicBSR_SF;
  /// default value is 0: not configured
  uint16_t sr_ProhibitTimer;
  /// sr ProhibitTime running
  uint8_t sr_ProhibitTimer_Running;
  ///  default value to n5
  uint16_t maxHARQ_Tx;
  /// default value is false
  uint16_t ttiBundling;
  /// default value is release
  struct DRX_Config *drx_config;
  /// default value is release
  struct MAC_MainConfig__phr_Config *phr_config;
  ///timer before triggering a periodic PHR
  uint16_t periodicPHR_Timer;
  ///timer before triggering a prohibit PHR
  uint16_t prohibitPHR_Timer;
  ///DL Pathloss change value
  uint16_t PathlossChange;
  ///number of subframe before triggering a periodic PHR
  int16_t periodicPHR_SF;
  ///number of subframe before triggering a prohibit PHR
  int16_t prohibitPHR_SF;
  ///DL Pathloss Change in db
  uint16_t PathlossChange_db;
  //Bj bucket usage per  lcid
  int16_t Bj[MAX_NUM_LCID];
  // Bucket size per lcid
  int16_t bucket_size[MAX_NUM_LCID];
} UE_SCHEDULING_INFO;
/*!\brief Top level UE MAC structure */
typedef struct {
  uint16_t Node_id;
  /// frame counter
  frame_t     frame;
  /// subframe counter
  sub_frame_t subframe;
  /// C-RNTI of UE
  uint16_t crnti;
  /// C-RNTI of UE before HO
  rnti_t crnti_before_ho; ///user id (rnti) of connected UEs
  /// uplink active flag
  uint8_t ul_active;
  /// pointer to RRC PHY configuration
  RadioResourceConfigCommonSIB_t *radioResourceConfigCommon;
  /// pointer to RACH_ConfigDedicated (NULL when not active, i.e. upon HO completion or T304 expiry)
  struct RACH_ConfigDedicated *rach_ConfigDedicated;
  /// pointer to RRC PHY configuration
  struct PhysicalConfigDedicated *physicalConfigDedicated;
#ifdef Rel10
  /// pointer to RRC PHY configuration SCEll
  struct PhysicalConfigDedicatedSCell_r10 *physicalConfigDedicatedSCell_r10;
#endif
  /// pointer to TDD Configuration (NULL for FDD)
  TDD_Config_t *tdd_Config;
  /// Number of adjacent cells to measure
  uint8_t  n_adj_cells;
  /// Array of adjacent physical cell ids
  uint32_t adj_cell_id[6];
  /// Pointer to RRC MAC configuration
  MAC_MainConfig_t *macConfig;
  /// Pointer to RRC Measurement gap configuration
  MeasGapConfig_t  *measGapConfig;
  /// Pointers to LogicalChannelConfig indexed by LogicalChannelIdentity. Note NULL means LCHAN is inactive.
  LogicalChannelConfig_t *logicalChannelConfig[MAX_NUM_LCID];
  /// Scheduling Information
  UE_SCHEDULING_INFO scheduling_info;
  /// Outgoing CCCH pdu for PHY
  CCCH_PDU CCCH_pdu;
  /// Incoming DLSCH pdu for PHY
  //DLSCH_PDU DLSCH_pdu[NUMBER_OF_UE_MAX][2];
  /// number of attempt for rach
  uint8_t RA_attempt_number;
  /// Random-access procedure flag
  uint8_t RA_active;
  /// Random-access window counter
  int8_t RA_window_cnt;
  /// Random-access Msg3 size in bytes
  uint8_t RA_Msg3_size;
  /// Random-access prachMaskIndex
  uint8_t RA_prachMaskIndex;
  /// Flag indicating Preamble set (A,B) used for first Msg3 transmission
  uint8_t RA_usedGroupA;
  /// Random-access Resources
  PRACH_RESOURCES_t RA_prach_resources;
  /// Random-access PREAMBLE_TRANSMISSION_COUNTER
  uint8_t RA_PREAMBLE_TRANSMISSION_COUNTER;
  /// Random-access backoff counter
  int16_t RA_backoff_cnt;
  /// Random-access variable for window calculation (frame of last change in window counter)
  uint32_t RA_tx_frame;
  /// Random-access variable for window calculation (subframe of last change in window counter)
  uint8_t RA_tx_subframe;
  /// Random-access Group B maximum path-loss
  /// Random-access variable for backoff (frame of last change in backoff counter)
  uint32_t RA_backoff_frame;
  /// Random-access variable for backoff (subframe of last change in backoff counter)
  uint8_t RA_backoff_subframe;
  /// Random-access Group B maximum path-loss
  uint16_t RA_maxPL;
  /// Random-access Contention Resolution Timer active flag
  uint8_t RA_contention_resolution_timer_active;
  /// Random-access Contention Resolution Timer count value
  uint8_t RA_contention_resolution_cnt;
  /// power headroom reporitng reconfigured
  uint8_t PHR_reconfigured;
  /// power headroom state as configured by the higher layers
  uint8_t PHR_state;
  /// power backoff due to power management (as allowed by P-MPRc) for this cell
  uint8_t PHR_reporting_active;
  /// power backoff due to power management (as allowed by P-MPRc) for this cell
  uint8_t power_backoff_db[NUMBER_OF_eNB_MAX];
  /// MBSFN_Subframe Configuration
  struct MBSFN_SubframeConfig *mbsfn_SubframeConfig[8]; // FIXME replace 8 by MAX_MBSFN_AREA?
  /// number of subframe allocation pattern available for MBSFN sync area
  uint8_t num_sf_allocation_pattern;
#ifdef Rel10
  /// number of active MBSFN area
  uint8_t num_active_mbsfn_area;
  /// MBSFN Area Info
  struct  MBSFN_AreaInfo_r9 *mbsfn_AreaInfo[MAX_MBSFN_AREA];
  /// PMCH Config
  struct PMCH_Config_r9 *pmch_Config[MAX_PMCH_perMBSFN];
  /// MCCH status
  uint8_t mcch_status;
  /// MSI status
  uint8_t msi_status;// could be an array if there are >1 MCH in one MBSFN area
#endif
  //#ifdef CBA
  /// CBA RNTI for each group 
  uint16_t cba_rnti[NUM_MAX_CBA_GROUP];
  /// last SFN for CBA channel access 
  uint8_t cba_last_access[NUM_MAX_CBA_GROUP];
  //#endif
  /// total UE scheduler processing time 
  time_stats_t ue_scheduler; // total
  /// UE ULSCH tx  processing time inlcuding RLC interface (rlc_data_req) and mac header generation 
  time_stats_t tx_ulsch_sdu;  
  /// UE DLSCH rx  processing time inlcuding RLC interface (mac_rrc_data_ind or mac_rlc_status_ind+mac_rlc_data_ind) and mac header parser
  time_stats_t rx_dlsch_sdu ; 
  /// UE query for MCH subframe processing time 
  time_stats_t ue_query_mch;
  /// UE MCH rx processing time 
  time_stats_t rx_mch_sdu;
  /// UE BCCH rx processing time including RLC interface (mac_rrc_data_ind) 
  time_stats_t rx_si; 
} UE_MAC_INST;
/*! \brief ID of the neighboring cells used for HO*/
typedef struct {
  uint16_t cell_ids[6];
  uint8_t n_adj_cells;
} neigh_cell_id_t;

#include "proto.h"
/*@}*/
#endif /*__LAYER2_MAC_DEFS_H__ */



