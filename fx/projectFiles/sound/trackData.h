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
				clip {
					.pos = 0,
					.len = 10,
					.repeat = 1,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,
					.pitch = { 4,3,1,2,1 },
					.vol = { 2,1,0 },
					.variation = { 0 },
					.slide = { 0 },
					.retrigger = { 0 },
					.send = { 3,2,1,1 }
				},
				clip {
					.pos = 0,
					.len = 10,
					.repeat = 1,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,
					.pitch = { 4,111,1,112,1 },
					.vol = { 3,1,1,0 },
					.variation = { 2,1,1 },
					.slide = { 0 },
					.retrigger = { 0 },
					.send = { 3,2,1,1 }
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
					.pos = 0,
					.len = 10,
					.repeat = 1,
					.bpmScale = 1,
					.overDub = 0,
					.swing = 0,
					.pitch = { 3,1,2,1 },
					.vol = { 2,1,0 },
					.variation = { 2,1,1 },
					.slide = { 0 },
					.retrigger = { 0 },
					.send = { 3,2,1,1 }
				}
			}
		}
	}
};