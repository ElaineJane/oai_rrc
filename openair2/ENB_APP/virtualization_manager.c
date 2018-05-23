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

/*! \file virtualization_manager.c
 * \brief Virtualization Manager for RAN Slicing, mapping/demapping physical to/from virtual resources 
 * \author  Shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "virtualization_manager.h"

void slice_manager (){



}

/*Slice Scheduling over window*/

void slice_scheduling(){

	/*Needs to be added to flexRAN*/
	SliceScheduler_algo Vir_algo = SLA_BASED;
	int window = 10; /*Needs to be added to flexRAN*/

	Update_SliceTransmissionRate();


    slice_scheduling_algorithm(Vir_algo);

    Create_Resource_Partitioning_Grid(window);

}

void Update_Slice_TransmissionRate(){

	// slice_context_manager * slice_ctx = GetSliceCtxt();
	// int sliceId;

	for (sliceId = 0; sliceId < 1; sliceId++){ /*loop over Admitted Slice*/

			/*TBD*/

	}



}

void virtualizaion_manager(){

/*TB Implemeted*/

}


/* 
	Slice Scheduling Algorithm 

*/


void slice_scheduling_algorithm(SliceScheduler_algo Vir_algo){

   switch (Vir_algo){
     
   	 /*TBD*/
     // case PROPORTIONAL_BASED:

     // slice_scheduling_algorithm_proportioanl_based();     

     // break;

     case SLA_BASED:

     slice_scheduling_algorithm_sla_based();

     break;

     case METRIC:

     slice_scheduling_algorithm_metric_based();
     
     break;

   }



}

void slice_scheduling_algorithm_proportioanl_based(){

	slice_context_manager * slice_ctx = GetSliceCtxt();


}

void slice_scheduling_algorithm_sla_based(){

	slice_context_manager * slice_ctx = GetSliceCtxt();
	int sliceId;
	int sum = 0;
	int SLICE_NUM = 1;
	int average;
	int slice_th[10]; /*This should be indicated from context manager with admitted slices*/
	int slice_pct[10];

	/*Slice throuput based on SLA from context manager*/

	for (sliceId = 0;sliceId < SLICE_NUM;sliceId++){ /*The maximum needs to be modifed*/

		slice_th[sliceId] = slice_ctx[sliceId]->thr_SLA;


	}

	for (sliceId = 0; sliceId < SLICE_NUM; sliceId++){

		sum = sum + slice_th[sliceId];	

	}

	/*Distribute the Percentage Resources*/
	for (sliceId = 0; sliceId < SLICE_NUM; sliceId++){

		slice_pct[sliceId] = slice_ctx[sliceId]->thr_SLA/sum;
	}


}

void slice_scheduling_algorithm_metric_based(){

	slice_context_manager * slice_ctx = GetSliceCtxt();

}

 void Create_Resource_Partitioning_Grid(int window){

 	int N_RB_DL = flexran_get_N_RB_DL(0, 0); /*Needs to be handled in a better way, TBD*/



 }


/* Extend for APIs for UE association*/



