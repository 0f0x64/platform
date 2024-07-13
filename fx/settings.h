#ifdef FINAL_RELEASE_CRINKLER
	#define EditMode false
	#define DebugMode false
	#define DirectXDebugMode false
	#define SECONDARY_DISPLAY_IF_AVAILABLE false
	#define USE_SHADER_MINIFIER true
#endif

#ifdef EDITOR
	#define EditMode true
	#define DebugMode true
	#define DirectXDebugMode false
	#define SECONDARY_DISPLAY_IF_AVAILABLE true
#define USE_SHADER_MINIFIER false
#endif

#ifdef EDITOR_DEBUG
	#define EditMode true
	#define DebugMode true
	#define DirectXDebugMode false
	#define SECONDARY_DISPLAY_IF_AVAILABLE false
	#define USE_SHADER_MINIFIER false
#endif

#define FRAMES_PER_SECOND 60
#define SAMPLING_FREQ 44100
#define SAMPLES_IN_FRAME (SAMPLING_FREQ/FRAMES_PER_SECOND)
#define FRAME_LEN (1000. / (float) FRAMES_PER_SECOND)

#define DEMO_DURATION 35. //in seconds

//----------------------------------editor settings-----------------------------------------

#define shadersPath "../fx/projectFiles/shaders"
#define userSpacePath "../fx/projectFiles"

#define MAIN_DISPLAY_DENOMINATOR 2

#define SRC_WATCH true
