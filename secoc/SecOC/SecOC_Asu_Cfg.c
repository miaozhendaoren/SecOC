/*
 * SecOC_Asu_Cfg.c
 *
 *  Created on: 2021Äê3ÔÂ23ÈÕ
 *      Author: 83926
 */

#include "Std_Types.h"
#include "SecOC_Asu_Types.h"
#include "SecOC_Cfg.h"


Com_Asu_IPdu_type Com_Asu_IPdu[COM_NUM_OF_IPDU];

Com_Asu_Config_type ComAsuConfiguration = {

		.ComIPdu = Com_Asu_IPdu,
		.ComSignal = NULL,
		.ComGroupSignal = NULL
};
