#include "Std_Types.h"


Csm_MacGenerate(); //浣跨敤缁欏畾鐨勬暟鎹潵鎵цMAC鐢熸垚锛屽苟灏哅AC瀛樺偍鍦∕AC鎸囬拡鎵�鎸囧悜鐨勫唴瀛樹綅缃腑銆�

Std_ReturnType Csm_MacVerify(){
	while(verify_fails){
		if(attempt < SecOCFreshnessValueSyncAttempts){
			attempts+=1;
			most_fvVerigyValue+=1;
		}else{
			drop(msg);
			return E_NOT_OK;
		}
	}

}

Csm_SignatureGenerate();

Csm_SignatureVerify();
