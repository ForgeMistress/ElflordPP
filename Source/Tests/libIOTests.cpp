////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  /!\ libIO Tests /!\
//
//  These are the test cases for libIO, the disk library.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libCore/libCore.h>
#include <libCore/RefPtr.h>

#include <libHarnessed/libHarnessed.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>

#include <libIO/libIO.h>
#include <libCore/Logger.h>
#include <libIO/ResourceMgr.h>

#include <libJson/libJson.h>

using namespace Firestorm;

#define FAIL_END(T, FUNC_CALL) \
{\
	Firestorm::Result<void, Firestorm::ErrorCode> err = document -> FUNC_CALL ; \
	T.Assert(err.has_value(), "error calling function "#FUNC_CALL": "+err.error().Message); \
}


class IO_TestBasicObject : public Firestorm::Mirror::Object
{
	FIRE_MIRROR_DECLARE(IO_TestBasicObject, Firestorm::Mirror::Object);
public:
	IO_TestBasicObject() {}
	virtual ~IO_TestBasicObject()
	{
		std::cout << "TestBasicObject::~TestBasicObject" << std::endl;
	}
private:
};

// FIRE_MIRROR_DEFINE(IO_TestBasicObject)
// {
// }

RefPtr<TestHarness> libIOPrepareHarness(int argc, char** argv)
{
	//IO_TestBasicObject::RegisterReflection();

	RefPtr<TestHarness> h(new TestHarness("libIOTests"));

	h->It("the FileIOMgr should spin up and shut down without issue", [](TestCase& t) {
		ResourceMgr fileMgr;
		fileMgr.Shutdown();
	});

	h->It("the FileIOMgr should be able to load a basic resource", [](TestCase& t) {
		ResourceMgr fileMgr;

		fileMgr.Shutdown();
	});

	return h;
}