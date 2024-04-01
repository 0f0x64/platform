#ifdef FINAL_RELEASE_CRINKLER
	#define EditMode false
	#define DebugMode false
	#define DirectXDebugMode false
	#define USE_SHADER_MINIFIER true
	#define SECONDARY_DISPLAY_IF_AVAILABLE false
#endif

#ifdef EDITOR
	#define EditMode true
	#define DebugMode true
	#define DirectXDebugMode false
	#define USE_SHADER_MINIFIER false
	#define SECONDARY_DISPLAY_IF_AVAILABLE true
#endif

#ifdef EDITOR_DEBUG
	#define EditMode true
	#define DebugMode true
	#define DirectXDebugMode false
	#define USE_SHADER_MINIFIER false
	#define SECONDARY_DISPLAY_IF_AVAILABLE true
#endif


#define FRAMES_PER_SECOND 60
#define FRAME_LEN 1000. / (float) FRAMES_PER_SECOND

#define DEMO_DURATION 5. //in seconds

//----------------------------------editor settings-----------------------------------------

#define shadersPath "../fx/projectFiles/shaders"
#define vShadersPath "../fx/projectFiles/shaders/vs/"
#define pShadersPath "../fx/projectFiles/shaders/ps/"

#define MAIN_DISPLAY_DENOMINATOR 3
