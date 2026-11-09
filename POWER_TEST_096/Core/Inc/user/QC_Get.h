/*
 * QC_Get.h
 *
 *  Created on: Mar 14, 2022
 *      Author: Feng
 */

#ifndef INC_USER_QC_GET_H_
#define INC_USER_QC_GET_H_

void QC_DP_Init(void);

u8 QC_Check(void);
void QC2_adj(u8 vol);
void QC3_adj(u8 direct);
void QC_DeInit(void);

#endif /* INC_USER_QC_GET_H_ */
