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

/*! \file slice_manager.c
 * \brief Slice Cxt Mgm, Admission Control, CRUD operations from orchestrator 
 * \author  shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "flexran_agent_virtualization_manager.h"


slice_context_manager * slice_ctx;

/*Needs to be with Slice Creation*/ 
void flexran_agent_slice_context_setup(){

	slice_ctx = malloc(sizeof(slice_context_manager) * MAX_NUM_SLICE);
	if (slice_ctx == NULL)
		goto error;

	// flexran_agent_set_slice_id(); TBD

	// flexran_agent_set_thr_SLA();	 TBD


    error:

    free(slice_ctx);


}
/*APIs used by the slice creation*/
void flexran_agent_set_slice_id(int slice_id, int index){

	slice_ctx[index].slice_id = slice_id; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void flexran_agent_set_slice_label(int index, int label){

	slice_ctx[index].label = label; /*Needs to be filled with Slice Creation in Slice Manager*/
}


void flexran_agent_set_thr_SLA(int index, int thr_SLA){

	slice_ctx[index].thr_SLA = thr_SLA; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void flexran_agent_set_rb_SLA(){

 	/*TBD*/
}


void flexran_agent_set_UE_list(){

	/*TBD*/
}

void flexran_agent_get_slice_id(){


}

void flexran_agent_get_slice_label(int index, int label){

	return slice_ctx[index].label; /*Needs to be filled with Slice Creation in Slice Manager*/
}


void flexran_agent_get_thr_SLA(){


}

void flexran_agent_get_rb_SLA(){

 	/*TBD*/
}

void flexran_agent_get_UE_list(){

	/*TBD*/
}

slice_context_manager * flexran_agent_getslicectxt(){

  return slice_ctx;
}

/*Slice Creation/Delete/Update */

int flexran_agent_create_slice(){


	// flexran_create_dl_slice(mod_id, sc_update[mod_id]->dl[i]->id); 
      /*flexran_create_ul_slice(mod_id, sc_update[mod_id]->ul[i]->id);*/







}

int flexran_agent_delete_slice(){


	// flexran_remove_dl_slice(mod_id, i);
	 /*flexran_remove_ul_slice(mod_id, i) < 1)*/

	
}


