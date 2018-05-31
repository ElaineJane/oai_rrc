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

	flexran_agent_set_slice_id();

	flexran_agent_set_thr_SLA();	


}

void flexran_agent_set_slice_id(){

	slice_ctx->slice_id = 0; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void flexran_agent_set_thr_SLA(){

	slice_ctx->thr_SLA = 10; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void flexran_agent_set_rb_SLA(){

 	/*TBD*/
}


void flexran_agent_set_UE_list(){

	/*TBD*/
}

void flexran_agent_get_slice_id(){


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
