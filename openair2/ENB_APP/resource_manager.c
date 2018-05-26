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

/*! \file eNB_virtualizer_slice_resouce_manager.c
 * \brief Resource Manager for RAN Slicing 
 * \author  Shahab SHARIAT BAGHERI
 * \date 2018
 * \email: 
 * \version 1.0
 * @ingroup _mac

 */


int average_transmissionrate = 100000;
int tranmisttedbytes;



typedef struct {
	int slice_id;
	int curr_thr;
	
} slice_current_state;


void setupdate(){

		/*TBD*/

}

void get_transmittedbytes(){


}


void reset_transmittedbytes(){

	tranmisttedbytes = 0;
	setupdate();

}

/*For weight based*/
void updateAverage_slicetransmissionRate(){

	int beta = 0.2;
	int throughput_current = 1000; /*Needs to be changed*/

	average_transmissionrate = (1 - beta) * average_transmissionrate + (beta * throughput_current);


}
