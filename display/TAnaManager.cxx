#include "TAnaManager.hxx"



TAnaManager::TAnaManager(){

	fKTMHistogram = 0;
#ifdef USE_KTM
	fKTMHistogram = new KTMHistograms();
	fKTMHistogram->DisableAutoUpdate();  // disable auto-update.  Update histo in AnaManager.
#endif

};



int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){


	if(fKTMHistogram) fKTMHistogram->UpdateHistograms(dataContainer); 


        return 1;
}



bool TAnaManager::HaveKTMHistograms(){
	if(fKTMHistogram) return true; 
	return false;
}


KTMHistograms* TAnaManager::GetKTMHistograms() {return fKTMHistogram;}



