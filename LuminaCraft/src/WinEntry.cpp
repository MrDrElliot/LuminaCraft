#include "Application.h"
#include <memory>


int main()
{
	FApplicationParams AppParams = {};
	AppParams.ApplicationName = "LuminaCraft";
	AppParams.WindowHeight = 1080;
	AppParams.WindowWidth = 1920;

	std::shared_ptr<Application> App = Application::Create(AppParams);

	App->Init();
	App->Run();
	App->Terminate();

	return 0;
	
}