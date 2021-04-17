/*
 * SecOC_helper.h
 *
 *  Created on: 2021Äê3ÔÂ23ÈÕ
 *      Author: 83926
 */

#ifndef SECOC_SECOC_HELPER_H_
#define SECOC_SECOC_HELPER_H_


#define GET_IPdu(IPduId) \
	(&(SecOCIPdu[IPduId]))


#define GET_AsuIPdu(IPduId) \
	(&(SecOC_Asu_Config->SecOCAsuIPdu[IPduId]))



#endif /* SECOC_SECOC_HELPER_H_ */
