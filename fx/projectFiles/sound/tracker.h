namespace tracker
{
	int curChannel;
	int curClip;
	int curLayer;
			
	void Music()
	{
		
		#include "trackData.h"

#if EditMode
		
		editor::TimeLine::bpm = Track.masterBPM;
		editor::TimeLine::bpmMaj = Track.timeNumerator;
		editor::TimeLine::bpmMin = Track.timeDenominator;
#endif // EditMode

	}

}
