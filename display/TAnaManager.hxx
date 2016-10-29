#ifndef TAnaManager_h
#define TAnaManager_h

// Use this list here to decide which type of equipment to use.

#define USE_KTM

#include "KTMHistogram.h"

/// This is an example of how to organize a set of different histograms
/// so that we can access the same information in a display or a batch
/// analyzer.
/// Change the set of ifdef's above to define which equipment to use.
class TAnaManager  {
public:
  TAnaManager();
  virtual ~TAnaManager(){};

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);

	/// Methods for determining if we have a particular set of histograms.
	bool HaveKTMHistograms();

	/// Methods for getting particular set of histograms.
	KTMHistograms* GetKTMHistograms();


private:

	KTMHistograms *fKTMHistogram;

};



#endif


