#include "ReLib.h"
#include "Test.h"
#include <iostream>

int main(int argc, char *argv[])
{
	TestModule::LoadLibrary();
	TestModule::Foo();
	std::cout << "bar == " << TestModule::GetBar() << std::endl;

	std::cout << "Make some changes, recompile, and press enter." << std::flush;
	while(std::cin.get() != '\n') { }

	TestModule::ReloadLibrary();
	TestModule::Foo();
	std::cout << "bar == " << TestModule::GetBar() << std::endl;
	TestModule::UnloadLibrary();
	return 0;
}