#include "Std_Types.h"

uint8 GetTxFreshness(){ //��0042��
	if(SecOCRxPduProcessing.SecOCFreshnessValueLength == SecOCRxPduProcessing.SecOCFreshnessValueTruncLength){
		return parsefv;
	}else{
		attempts =0; //�˴�attempt���ĸ�
		//����SecOCfreshnessValueID�ж�
		if (parsefv > least_significant bits of_fv_SecOCfreshnessValueID){
			fv = mostfv (SecOCfreshnessValueID)|parsefv;
		}else{ //�ع�ֱ��ƴ��
			fv = mostfv (SecOCfreshnessValueID+1)|parsefv;
		}
		return fv


	}
}
