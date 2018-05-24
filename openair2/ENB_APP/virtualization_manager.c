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


/*Slice Scheduling over window*/

void slice_scheduling(){

	/*Needs to be added to flexRAN*/
	/* virtualizer params*/
	virtualizer_manager_t * virt_mgr_t = malloc(sizeof(virtualizer_manager_t));
	virt_mgr_t->window = 10;
	virt_mgr_t->aloc_or = SEQUENTIAL;
	virt_mgr_t->scheduler_algo = SLA_BASED;
	virt_mgr_t->num_admitted_slices = 1;

	/*Algorithm context state*/
	slice_current_state  slice_state;

	/*For metric based algorithms*/
	// update_slice_transmissionrate();


    slice_scheduling_algorithm(virt_mgr_t, &slice_state);

    create_resource_partitioning_grid(virt_mgr_t, &slice_state);

}

void update_slice_transmissionrate(){

	// slice_context_manager * slice_ctx = GetSliceCtxt();
	int sliceId;

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


void slice_scheduling_algorithm(virtualizer_manager_t * virt_mgr_t, slice_current_state * slice_state){

   switch (virt_mgr_t->scheduler_algo){
     
   	 /*TBD*/
     // case PROPORTIONAL_BASED:

     // slice_scheduling_algorithm_proportioanl_based();     

     // break;

     case SLA_BASED:

     slice_scheduling_algorithm_sla_based(virt_mgr_t, slice_state);

     break;

     case METRIC:

     slice_scheduling_algorithm_metric_based(virt_mgr_t, slice_state);
     
     break;

   }



}

void slice_scheduling_algorithm_proportioanl_based(){

	slice_context_manager * slice_ctx = GetSliceCtxt();


}

void slice_scheduling_algorithm_sla_based(virtualizer_manager_t * virt_mgr_t, slice_current_state * slice_state){

	slice_context_manager * slice_ctx = GetSliceCtxt(); /*Should be handled with context manager, connected with common agent */
	int sliceId;
	int sum = 0;
	int SLICE_NUM = virt_mgr_t->num_admitted_slices;/*Should be handled with slice context manager*/
	int average;
	int slice_th[SLICE_NUM]; /*This should be indicated from context manager with admitted slices*/
	int slice_pct[SLICE_NUM];
	

	/*Slice throuput based on SLA from context manager*/

	for (sliceId = 0;sliceId < SLICE_NUM;sliceId++){ /*The maximum needs to be modifed*/

		slice_th[sliceId] = slice_ctx[sliceId].thr_SLA;


	}

	for (sliceId = 0; sliceId < SLICE_NUM; sliceId++){

		sum = sum + slice_th[sliceId];	

	}
	/*calculate the percentage*/
	for (sliceId = 0; sliceId < SLICE_NUM; sliceId++){

		slice_pct[sliceId] = slice_ctx[sliceId].thr_SLA/sum;
	}

	/*Distribute the Percentage Resources*/
	for (sliceId = 0; sliceId < SLICE_NUM; sliceId++){

		slice_state[sliceId].pct =  10;// slice_pct[sliceId]; /*Shoud be improved later*/
	}


}

void slice_scheduling_algorithm_metric_based(){

	slice_context_manager * slice_ctx = GetSliceCtxt();

}

 void create_resource_partitioning_grid(virtualizer_manager_t * virt_mgr_t, slice_current_state * slice_state){

 	/*Temprorys*/
 	int cc_id = 0;
 	int mod_id = 0;
 	int i;


 	int N_RBG_DL = flexran_get_N_RBG(mod_id, cc_id); /*Needs to be handled in a better way, TBD*/
 	int window = virt_mgr_t->window;
 	int tot_rb = window * N_RBG_DL;
 	int end_rb;

 	/*Create and init the matrix*/
 	int mat[N_RBG_DL][window];

 	memset(mat,0, N_RBG_DL * window);

 	/*Decide based on distribution policy*/
 	if (virt_mgr_t->aloc_or == SEQUENTIAL){


 		for (i = 0;i < virt_mgr_t->num_admitted_slices;i++){

 			end_rb = ceil(slice_state[i].pct * tot_rb);

 		}


		mat[end_rb % N_RBG_DL][end_rb / N_RBG_DL] = 1;




 	}
 	 else if (virt_mgr_t->aloc_or == PARALLEL){

 	 	/*Can be directly connected to RAN API!*/


 	 }




 }


/* Extend for APIs for UE association*/



