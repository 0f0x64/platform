namespace tracker
{

	

#if EditMode

#include <iostream>
#include <fstream>

	void saveTrackToText()
	{
		std::string str;
		str += "track = {\n";
		str += "\t.masterBPM = "; str += std::to_string(track.masterBPM); str += ",\n";
		str += "\t.volume = "; str += std::to_string(track.volume); str += ",\n";
		str += "\t.channelsCount = "; str += std::to_string(track.channelsCount); str += ",\n";
		str += "\t.channels = {\n";
		for (int i = 0;i < track.channelsCount;i++)
		{
			str += "\t\tchannel {\n";

			str += "\t\t\t.vol = "; str += std::to_string(track.channels[i].vol); str += ",\n";
			str += "\t\t\t.pan = "; str += std::to_string(track.channels[i].pan); str += ",\n";
			str += "\t\t\t.mute = "; str += track.channels[i].mute == switcher::off ? "switcher::off" : "switcher::on"; str += ",\n";
			str += "\t\t\t.solo = "; str += track.channels[i].solo == switcher::off ? "switcher::off" : "switcher::on"; str += ", \n";
			str += "\t\t\t.clipsCount = "; str += std::to_string(track.channels[i].clipsCount); str += ",\n";
			
			str += "\t\t\t.clips {\n";
				for (int j = 0; j < track.channels[i].clipsCount; j++)
				{
					str += "\t\t\t\tclip {\n";
					str += "\t\t\t\t\t.pos = ";		str += std::to_string(track.channels[i].clips[j].pos); str += ",\n";
					str += "\t\t\t\t\t.len = ";		str += std::to_string(track.channels[i].clips[j].len); str += ",\n";
					str += "\t\t\t\t\t.repeat = ";	str += std::to_string(track.channels[i].clips[j].repeat); str += ",\n";
					str += "\t\t\t\t\t.bpmScale = "; str += std::to_string(track.channels[i].clips[j].bpmScale); str += ",\n";
					str += "\t\t\t\t\t.overDub = ";	str += std::to_string(track.channels[i].clips[j].overDub); str += ",\n";
					str += "\t\t\t\t\t.swing = ";	str += std::to_string(track.channels[i].clips[j].swing); str += ",\n";

					str += "\t\t\t\t\t.pitch = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].pitch[0]);
						if (track.channels[i].clips[j].pitch[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].pitch[0]+1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].pitch[k]); 
							if (k < track.channels[i].clips[j].pitch[0])
							{
								str += ",";
							}
						}
						str += " },\n";
				
					str += "\t\t\t\t\t.vol = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].vol[0]);
						if (track.channels[i].clips[j].vol[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].vol[0] + 1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].vol[k]);
							if (k < track.channels[i].clips[j].vol[0])
							{
								str += ",";
							}
						}
						str += " },\n";	
					
					str += "\t\t\t\t\t.variation = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].variation[0]);
						if (track.channels[i].clips[j].variation[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].variation[0] + 1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].variation[k]);
							if (k < track.channels[i].clips[j].variation[0])
							{
								str += ",";
							}
						}
						str += " },\n";

					str += "\t\t\t\t\t.slide = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].slide[0]);
						if (track.channels[i].clips[j].slide[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].slide[0] + 1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].slide[k]);
							if (k < track.channels[i].clips[j].slide[0])
							{
								str += ",";
							}
						}
						str += " },\n";

					str += "\t\t\t\t\t.retrigger = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].retrigger[0]);
						if (track.channels[i].clips[j].retrigger[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].retrigger[0] + 1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].retrigger[k]);
							if (k < track.channels[i].clips[j].retrigger[0])
							{
								str += ",";
							}
						}
						str += " },\n";

					str += "\t\t\t\t\t.send = ";
						str += "{ ";
						str += std::to_string(track.channels[i].clips[j].send[0]);
						if (track.channels[i].clips[j].send[0] > 0) str += ",";
						for (int k = 1; k < track.channels[i].clips[j].send[0] + 1; k++)
						{
							str += std::to_string(track.channels[i].clips[j].send[k]);
							if (k < track.channels[i].clips[j].send[0])
							{
								str += ",";
							}
						}
						str += " }\n";

						if (j < track.channels[i].clipsCount - 1)
						{
							str += "\t\t\t\t},\n";
						}
						else
						{
							str += "\t\t\t\t}\n";
						}
				}
				str += "\t\t\t}\n";
			if (i < track.channelsCount - 1)
			{
				str += "\t\t},\n";
			}
			else
			{
				str += "\t\t}\n";
			}

		}


		str += "\t}\n};";

		std::string trackFile = "..\\fx\\projectFiles\\sound\\trackData.h2";
		remove(trackFile.c_str());
		std::ofstream ofile(trackFile);

		ofile << str;

		ofile.close();

	}
#endif

	bool tInit = false;

	cmd(Track)
	{
		reflect;

		if (!tInit)
		{
			#include "trackData.h"
			tInit = true;
		}

		#if EditMode
			editor::TimeLine::bpm = track.masterBPM;
		#endif



		reflect_close;
	}
	

}
