#pragma once
namespace LivePT {

	#include <string>

	class Editor {

	private:
		struct {
			const std::string userSpace = "../fx/projectFiles";
			const std::string obj = "../fx/Editor_Debug";
			const std::string shaders = "../fx/projectFiles/shaders";
		} patches;

		struct  {
			const bool WindowManagement = true;
		} settings;

	public:

		void Process()
		{

		}

	};

	Editor editor;

}
