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
 * \brief Slice Cxt Mgm, Admission Control, CRUD operations  
 * \author  shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */

#include "virtualization_manager.h"


slice_context_manager * slice_ctx;

/*Needs to be with Slice Creation*/ 
void slice_context_setup(){

	set_slice_id();

	set_thr_SLA();	


}

void set_slice_id(){

	slice_ctx->slice_id = 0; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void set_thr_SLA(){

	slice_ctx->thr_SLA = 10; /*Needs to be filled with Slice Creation in Slice Manager*/
}

void set_rb_SLA(){

 /*TBD*/
}


void set_UE_list(){

	/*TBD*/
}

void get_slice_id(){


}

void get_thr_SLA(){


}

void get_rb_SLA(){

 /*TBD*/
}

void get_UE_list(){

	/*TBD*/
}

slice_context_manager * GetSliceCtxt(){

  return slice_ctx;
}