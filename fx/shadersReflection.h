	#define shader(VariableName) VariableName
	enum vertex {
		#include "projectFiles\vsList.h"
	};
	enum pixel {
		#include "projectFiles\psList.h"
	};


	#if EditMode

		#undef shader
		#define shader(VariableName) # VariableName
		const char* vsList[] = {
			#include "projectFiles\vsList.h"
		};
		const char* psList[] = {
			#include "projectFiles\psList.h"
		};

	#endif