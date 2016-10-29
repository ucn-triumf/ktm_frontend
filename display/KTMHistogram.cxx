#include "KTMHistogram.h"


#include "TDirectory.h"

const int Nchannels = 2;
 #include <sys/time.h>

/// Reset the histograms for this canvas
KTMHistograms::KTMHistograms(){  
  
  CreateHistograms();
}


void KTMHistograms::CreateHistograms(){
  

  // Otherwise make histograms
  clear();

  int numberSamples = 22;
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"KTM_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"Digitized 1VM4 Waveform",i);	
    
    TH1D *tmp = new TH1D(name,title,numberSamples,0,numberSamples*4.4);
    tmp->SetYTitle("Beam Current (arb units)");
    tmp->SetXTitle("time near notch (us)");
    push_back(tmp);
  }

}



  
/// Update the histograms for this canvas.
void KTMHistograms::UpdateHistograms(TDataContainer& dataContainer){

  TGenericData *data = dataContainer.GetEventData<TGenericData>("KTM0");
  if(!data) return;

  GetHistogram(0)->Reset();

  //std::cout << "size " << data->GetSize() << std::endl;
  for(int i = 0; i < data->GetSize(); i++){

    uint32_t word = data->GetData32()[i];
    
    double time = (word & 0xfffff000) >> 12;
    double volt = ((double)(word & 0xfff));
    //std::cout << time << " " << volt << std::endl;
    GetHistogram(0)->SetBinContent(i+1,volt);

  }
  
  


}



/// Take actions at begin run
void KTMHistograms::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void KTMHistograms::EndRun(int transition,int run,int time){

}
