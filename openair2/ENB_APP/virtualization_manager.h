/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
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

/*! \file eNB_virtualizer.h
 * \brief Virtualization Manager for RAN Slicing 
 * \author  Shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "assertions.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/extern.h"

/* Virtualizer Parameters */

typedef enum {

   SEQUENTIAL,
   PARALLEL 

} aloc_order;

typedef enum {

	SLA_BASED,
	METRIC

} sched_algo_type;

typedef struct {

   int slice_id;
   int thr_SLA ;
   int rb_SLA;
   int num_active_slices;
   int num_slice_creation;
    /*Should be considered for slice ctx management TBD*/
   // UE_list_t ue_list; 

    
} slice_context_manager;


typedef struct {
    
    int window;
    int num_admitted_slices;
    aloc_order aloc_or;
    sched_algo_type scheduler_algo;


} virtualizer_manager_t;

/*Slice Scheduler Algo*/

typedef struct {
	int slice_id;
	int curr_thr;
	double pct;

} slice_current_state;


/*Slice Manager Protos*/

void set_slice_id(void);

void set_thr_SLA(void);

void set_UE_list(void);

void slice_context_setup(void);

void set_rb_SLA(void);

void get_slice_id(void);

void get_thr_SLA(void);

void get_rb_SLA(void);

void get_UE_list(void);

slice_context_manager * getslicectxt(void);

/*Virtualization Manager Protos*/

void slice_scheduling(void);

void virtualizaion_manager(void);

/*Slice Scheduling protos*/

void resource_distribute_algorithm(virtualizer_manager_t * virt_mgr_t, slice_current_state * slice_state);

void create_resource_partitioning_grid(virtualizer_manager_t * virt_mgr_t, slice_current_state *  slice_state);

void resource_distribute_algorithm_sla_based(virtualizer_manager_t * virt_mgr_t, slice_current_state *  slice_state);

void resource_distribute_algorithm_metric_based(virtualizer_manager_t * virt_mgr_t, slice_current_state *  slice_state);