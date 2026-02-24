track = {
	.masterBPM = 120,
	.volume = 10,
	.channelsCount = 2,
	.channels = {
		channel {
			.vol = 0,
			.pan = 0,
			.mute = switcher::off,
			.solo = switcher::off,
			.clipsCount = 2,
			.clips {
				//drums
				clip {
					.pos = 11,
					.len = 10,
					.repeat = 3,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,
									  // 0               16              32              48              64
									  // 1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
									  // |...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|
					.pitch =			"dd c d e00000000",
					.vol =				"0000000000000000",
					.variation =		"0000000000000000",
					.slide =			"0000000000000000",
					.retrigger =		"00",
					.send =				"1"
				},
				clip {
					.pos = 0,
					.len = 10,
					.repeat = 4,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,

				}
			}
		},
		channel {
			.vol = 0,
			.pan = 0,
			.mute = switcher::off,
			.solo = switcher::off,
			.clipsCount = 1,
			.clips {
				clip {
					.pos = 120,
					.len = 10,
					.repeat = 1,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,

				}
			}
		}
	}
};