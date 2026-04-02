static track Track =
{
	.channelsCount = 1,
	.masterBPM = 120,
	.timeNumerator = 4,
	.timeDenominator = 4,
	.volume = 100,

	.channel =
	{
		//kick
		{
		.clipsCount = 1,
		.vol = 100,
		.pan = 0,
		.mute = switcher::off,
		.solo = switcher::off,
		.clip = {
					{
					.pos = 0,
					.len = 4,
					.repeat = 1,
					.bpmScaleNumerator = 1,
					.bpmScaleDenominator = 8,
					.overDub = switcher::off,
					.pattern = "drumloop"
					},
				},
		},
	
	},

};
