
#include <libCore/libCore.h>
#include <libHarnessed/libHarnessed.h>

using std::cout;
using std::cin;
using std::endl;

int main(int argc, char** argv)
{
	Elf::TestHarness harness("Test");

	uint32_t errorCount = harness.Run();
	cout << "Tests Completed with " << errorCount << " errors." << endl;

	cout << "Press any key to close..." << endl;
	cin.get();

	return errorCount;
}