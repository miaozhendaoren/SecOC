#include "Std_Types.h"

uint8 GetTxFreshness(){ //【0042】
	if(SecOCRxPduProcessing.SecOCFreshnessValueLength == SecOCRxPduProcessing.SecOCFreshnessValueTruncLength){
		return parsefv;
	}else{
		attempts =0; //此处attempt是哪个
		//根据SecOCfreshnessValueID判断
		if (parsefv > least_significant bits of_fv_SecOCfreshnessValueID){
			fv = mostfv (SecOCfreshnessValueID)|parsefv;
		}else{ //重构直接拼接
			fv = mostfv (SecOCfreshnessValueID+1)|parsefv;
		}
		return fv


	}
}
