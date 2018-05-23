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

/*! \file eNB_virtualizer.c
 * \brief Virtualization Manager for RAN Slicing 
 * \author  Shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "eNB_virtualizer.h"

void slice_manager (){



}


void slice_scheduling(){

	Update_SliceTransmissionRate();


    slice_scheduling_algorithm(SliceScheduler_algo Vir_algo);

}

void Update_Slice_TransmissionRate(){

	slice_context_manager * slice_ctx = GetSliceCtxt();
	int sliceId;

	for (sliceId = 0; sliceId < 1; sliceId++){ /*loop over Admitted Slice*/



	}



}

void virtualizaion_manager(){

/*TB Implemeted*/

}


/* Slice Scheduling Algorithm */

void slice_scheduling_algorithm(SliceScheduler_algo Vir_algo){

   switch (Vir_algo){
     

     case PROPORTIONAL_BASED:

     slice_scheduling_algorithm_proportioanl_based();     

     break;

     case SLA_BASED:

     slice_scheduling_algorithm_sla_based();

     break;

     case METRIC:

     slice_scheduling_algorithm_metric_based();
     
     break;

   }



}

void slice_scheduling_algorithm_proportioanl_based(){



}

void slice_scheduling_algorithm_sla_based(){

 	/*For the Downlink*/

}

void slice_scheduling_algorithm_metric_based(){


}

/* Extend for APIs for UE association*/



