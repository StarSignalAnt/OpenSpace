#include "OpenSpaceGame.h"
#include "IntroState.h"

OpenSpaceGame::OpenSpaceGame() {

	SetApp(1024, 768, "Future Calling - Episode 1 \"The Future Calling\"");
	PushState(new IntroState);
	Run();

}