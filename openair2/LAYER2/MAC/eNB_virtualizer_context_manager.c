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

/*! \file eNB_virtualizer_API.c
 * \brief Slice Cxt Mgm Interface 
 * \author  shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "eNB_virtualizer.h"


slice_context_manager * slice_ctx;

void eNB_virtualizer_slice_context_setup(){

 
	eNB_virtualizer_set_slice_id();

	eNB_virtualizer_set_thr_SLA();	


}

void eNB_virtualizer_set_slice_id(){

	slice_ctx->slice_id = 0; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void eNB_virtualizer_set_thr_SLA(){

	slice_ctx->thr_SLA = 0; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void eNB_virtualizer_set_rb_SLA(){

 /*TBD*/
}

void eNB_virtualizer_set_UE_list(){

	/*TBD*/
}

void eNB_virtualizer_get_slice_id(){


}

void eNB_virtualizer_get_thr_SLA(){


}

void eNB_virtualizer_get_rb_SLA(){

 /*TBD*/
}

void eNB_virtualizer_get_UE_list(){

	/*TBD*/
}