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
					.pitch =	"cdefgab",
					.octave =	"1111111",
					.velocity = "9888988",
					.positon =	"0000000",
					.variation ="0000000",
					.offset =	"0000000",
					.slide =	"0000000",
					.retrigger ="0000000"
					},
				},
		},
	
	},

};
