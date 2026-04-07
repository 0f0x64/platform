static track Track =
{
	.channelsCount = 1,
	.masterBPM = 120,
	.timeNumerator = 4,
	.timeDenominator = 4,
	.volume = 100,

	.channel =
	{
		/*kick*/ {
		.clipsCount = 1,
		.vol = 100,
		.pan = 0,
		.mute = switcher::off,
		.solo = switcher::off,
		.clip = {
					/*intro*/ {
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

	.pattern = 
	{
		//								 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
		//				                 •               •               •               •               •               •               •               •               •
		/*drumloop*/ { .note =         " d-1 ... d#3 ... d#2 ... d-1 ... ",		                                                  
			.params = { .velocity =	   " +10 ... +30",
						.position =	   " .00 ... .00 ... .00 ... .00 "	} },
		/*drumloop2*/ {	.note =			"        d#3             d-1     ",
			.params = {	.velocity =		"        +10             +30      ",
						.position =		"        .00             .00      "	}},
		/*solo*/ { 		.note =	       " c-1 d-2 e-2 ... ... d-1 d-2 ... c-1 ... e-2 ... d-1 d-1 d-2 ... c-1 ... e-2 ... d-1 ... d-2 ... c-1 ... e-2 ... d-1 d-1 d-2 d-3 ",
			.params = {	.velocity =	    "+10 ... +30 ...",
						.position =     ".00 ... .00 ..." } },
	}

};

//