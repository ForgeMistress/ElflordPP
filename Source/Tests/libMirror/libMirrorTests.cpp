////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  /!\ libMirror Tests /!\
//
//  These are the test cases for libMirror, the reflection utility library.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libCore/libCore.h>
#include <libHarnessed/libHarnessed.h>
#include <libCore/RefPtr.h>

#include <libMirror/Object.h>
#include <libMirror/SerialTraits.h>
#include <libMirror/EventDispatcher.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

using namespace Elf;

class TestBasicObject : public Mirror::Object
{
	ELF_MIRROR_DECLARE(TestBasicObject, Mirror::Object);
private:
public:
	TestBasicObject() {}
	virtual ~TestBasicObject()
	{
		std::cout << "TestBasicObject::~TestBasicObject" << std::endl;
	}
private:
};

class TestObjectWithFields : public Mirror::Object
{
	ELF_MIRROR_DECLARE(TestObjectWithFields, Mirror::Object);
public:
	TestObjectWithFields() {}
	explicit TestObjectWithFields(const std::string& str) : testString(str) {}
	virtual ~TestObjectWithFields() 
	{
		std::cout << "TestObjectWithFields::~TestObjectWithFields" << std::endl;
	}

	std::string testString;
private:
};



struct TestEvent
{
	Elf::String Message;
	static const char* EVENT_NAME;
};
const char* TestEvent::EVENT_NAME = "TestEvent";

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> Make(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
{
	return std::bind(funcPointer, instance, std::placeholders::_1);
}

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> Make(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> Make(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Arg_t>
std::function<void(const Arg_t&)> Make(void(*funcPointer)(const Arg_t&))
{
	return std::bind(funcPointer);
}

class TestEventObject
{
public:
	void DoRegistration(EventDispatcher& dispatcher)
	{
		using namespace std::placeholders;
		std::function<void(const TestEvent&)> func(std::bind(&TestEventObject::Invoke, this, _1));
		_receipt = dispatcher.Register<TestEvent>(Make(&TestEventObject::Invoke, this));
	}

	void Invoke(const TestEvent& arg) const
	{
		std::cout << "TestEventObject says '" << arg.Message << "'." << std::endl;
	}
private:
	Elf::RefPtr<Elf::EDReceipt> _receipt;
};

RTTR_REGISTRATION
{
	Mirror::Registration::class_<TestBasicObject>("TestBasicObject");

	Mirror::Registration::class_<TestObjectWithFields>("TestObjectWithFields")
		.property("testString", &TestObjectWithFields::testString)
	;
}

/*template<class T1, class T2>
void TestTypeUtils(T1 obj1, T2 obj2, TestCase& t)
{
	Mirror::Type t1 = Mirror::TypeUtils::get(obj1);
	Mirror::Type t2 = Mirror::TypeUtils::get(obj2);

	t.Assert(t1 == t2, "The types do not match.");
}*/

int main()
{
	Elf::TestHarness h("Test libMirror");

	h.It("weak pointers should have their objects deconstruct as normal and be able to detect when it's happened", [](Elf::TestCase& t) {
		Elf::WeakPtr_<TestBasicObject> weakPtr;
		{
			Elf::RefPtr<TestBasicObject> ptr(new TestBasicObject);
			weakPtr = ptr;
			t.Assert(weakPtr, "the weak pointer did not have an object when it should have");
		}
		t.Assert(!weakPtr, "the weak pointer had an object when it should not have");
	});

	h.It("weak pointers should be able to reference a base class", [](Elf::TestCase& t) {
		Elf::WeakPtr_<Mirror::Object> weakPtr;
		{
			Elf::RefPtr<TestBasicObject> ptr(new TestBasicObject);
			auto obj = ptr.Get();
			weakPtr = ptr;
			t.Assert(weakPtr, "the weak pointer did not have an object when it should have");
		}
		t.Assert(!weakPtr, "the weak pointer had an object when it should not have");
	});

	h.It("should allow instantiation of TestBasicObject", [](Elf::TestCase& t) {
		try
		{
			std::shared_ptr<TestBasicObject> test(new TestBasicObject);
		}
		catch(...)
		{
			t.Assert(false, "Allocation error or some other kind of error detected.");
		}
	});

	h.It("TestBasicObject::MyType() should return the same type as instance->GetType()", [](Elf::TestCase& t) {
		Elf::Mirror::Type tboStaticType = TestBasicObject::MyType();

		Elf::RefPtr<TestBasicObject> tbo(new TestBasicObject());
		t.Assert(tbo->GetType() == tboStaticType, "The two types were not the same.");
	});

	h.It("Mirror::TypeUtils::get should return the same type no matter what qualifiers are passed in", [](Elf::TestCase& t) {
		Elf::RefPtr<TestBasicObject> obj(new TestBasicObject());
		t.Assert(obj.Get(), "No object was constructed.");

		/*TestTypeUtils<const Mirror::Object*,            Mirror::Object*>       (obj.get(), obj.get(), t);
		TestTypeUtils<Mirror::Object*,                  const Mirror::Object*> (obj.get(), obj.get(), t);
		TestTypeUtils<shared_ptr<Mirror::Object>,       Mirror::Object*>       (obj,       obj.get(), t);
		TestTypeUtils<const shared_ptr<Mirror::Object>, const Mirror::Object*> (obj,       obj.get(), t);*/
	});

	h.It("the properties in an object instance should be retrievable when passing in a shared_ptr", [](Elf::TestCase& t) {
		using namespace rttr;

		std::string testStringValue("testing value!");
		Elf::RefPtr<Elf::Mirror::Object> test(new TestObjectWithFields(testStringValue));
		// test->testString = testStringValue;

		Elf::Result<void, Elf::Error> result;
		t.Assert(result.has_value(), "Elf::Result<void> does not contain a value...");

		instance objInstance(test);
		Elf::Mirror::Type testType = test->GetType();

		property testStringProperty = testType.get_property("testString");
		variant extractedValueVariant = testStringProperty.get_value(objInstance);

		t.Assert(extractedValueVariant.get_value<std::string>() == testStringValue, "testStringValue != extractedValueVariant");
	});


	h.It("the event dispatcher should allow the registration of events", [](Elf::TestCase& t) {
		std::unique_ptr<TestEventObject> object(std::make_unique<TestEventObject>());
		Elf::EventDispatcher dispatcher;
		//object->DoRegistration(dispatcher);
		Elf::EventDispatcher::Receipt receipt = dispatcher.Register<TestEvent>([](const TestEvent& str){
			std::cout << "TEST EVENT: " << str.Message << std::endl <<std::flush;
		});

		TestEventObject o;
		Elf::EventDispatcher::Receipt objReceipt = dispatcher.Register<TestEventObject, TestEvent>(&TestEventObject::Invoke, &o);

		t.Assert(dispatcher.GetNumRegisteredEvents() == 2, "There was an incorrect number of registered events.");

		dispatcher.Dispatch(TestEvent{ "Hello test suite!" });
	});

	h.It("the event dispatcher should unregister when receipts fall out of scope", [](Elf::TestCase& t) {
		std::unique_ptr<TestEventObject> object(std::make_unique<TestEventObject>());
		Elf::EventDispatcher dispatcher;

		{
			Elf::EventDispatcher::Receipt receipt = dispatcher.Register<TestEvent>([](const TestEvent& str) {
				std::cout << "TEST EVENT: " << str.Message << std::endl << std::flush;
			});
			t.Assert(dispatcher.GetNumRegisteredEvents() == 1, "There was an incorrect number of registered events.");
			t.Assert(dispatcher.GetNumRegisteredReceipts() == 1, "There was an incorrect number of registered receipts.");
			dispatcher.Dispatch(TestEvent{ "Hello test suite!" });
		}
		t.Assert(dispatcher.GetNumRegisteredEvents() == 0, "There was an incorrect number of registered events.");
		t.Assert(dispatcher.GetNumRegisteredReceipts() == 0, "There was an incorrect number of registered receipts.");
	});

	uint32_t results = h.Run();

	std::cout << "Completed with " << results << " errors." << std::endl << std::endl;
	std::cout << "Press 'Enter' to close..." << std::endl;
	std::cin.get();

    return results;
}