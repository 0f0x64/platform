	#define shader(VariableName) VariableName,
	enum vertex {
		#include "generated\vsList.h"
	};
	enum pixel {
		#include "generated\psList.h"
	};


	#if EditMode

		#undef shader
		#define shader(VariableName) #VariableName ,
		const char* vsList[] = {
			#include "generated\vsList.h"
		};
		const char* psList[] = {
			#include "generated\psList.h"
		};

	#endif