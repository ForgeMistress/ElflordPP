#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/libExistence.h>
#include <libExistence/Engine.h>
#include <libExistence/System.h>
#include <libExistence/Component.h>
#include <libExistence/Entity.h>

#include <libCore/Logger.h>

using namespace Firestorm;


struct TestComponent : public IComponent<uint8_t, uint8_t, uint8_t, uint8_t>
{
};

struct TestComponentMgr : public BasicComponentManager<TestComponent>
{
	TestComponentMgr()
	{
		Alloc(5);
	}
	void SetFlag1(const Entity& ent, uint8_t flag)
	{
		SetData<uint8_t>(ent.Index(), 0, flag);
	}

	uint8_t GetFlag1(const Entity& ent)
	{
		return GetData<uint8_t>(ent.Index(), 0);
	}
};

struct TestComponentView
{
	uint8_t v1;
	uint8_t v2;
	uint8_t v3;
	uint8_t v4;
};

struct TestComplexComponent : public IComponent<String, float, double>
{
};

struct TestComplexComponentView
{
	String name;
	float value1;
	float value2;
};

struct TestComplexComponentMgr : public BasicComponentManager<TestComplexComponent>
{
	TestComplexComponentMgr()
	{
		Alloc(5);
	}

	void SetName(const Entity& entity, const String& name)
	{
		SetData(entity.Index(), 0, name);
	}

	String GetName(const Entity& entity)
	{
		return GetData<String>(entity.Index(), 0);
	}
};

RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av)
{
	RefPtr<TestHarness> h(new TestHarness("libExistence"));

	h->It("Entities should be instantiable and contain proper ids", [](Firestorm::TestCase& t) {
		EntityMgr eMgr;

		Entity e = eMgr.Create();
		t.Assert(e.Index() == 0, "index was incorrect");
		t.Assert(e.Generation() == 0, "generation was incorrect");
	});

	h->It("components should report back their proper size", [](Firestorm::TestCase& t) {

		static size_t expectedSize = 4;

		size_t componentSize = TestComponent::SizeOf();

		FIRE_LOG_DEBUG("TestComponent (IComponent<uint8_t, uint8_t, uint8_t, uint8_t>) reported: %d", componentSize);

		t.Assert(expectedSize == componentSize, "the component reported back the incorrect size");
	});

	h->It("component field offsets should be proper", [](TestCase& t) {
		auto& memberDetailList = TestComponent::MyMembers();

		const size_t expectedMemberSize = sizeof(uint8_t);

		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.Offset);
			t.Assert(memberInfo.Size == expectedMemberSize, 
				Format("size of member [%d] was wrong. expected %d. got %d", i, expectedMemberSize, memberInfo.Size));
			t.Assert(memberInfo.Offset == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.Offset));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("complex components should have the expected offsets as well", [](TestCase& t) {
		auto& memberDetailList = TestComplexComponent::MyMembers();
		size_t expectedOffset = 0;
		for(size_t i=0; i<memberDetailList.size(); ++i)
		{
			auto& memberInfo = memberDetailList[i];
			FIRE_LOG_DEBUG("[%d] size = %d, offset = %d", i, memberInfo.Size, memberInfo.Offset);
			t.Assert(memberInfo.Offset == expectedOffset,
				Format("offset of member [%d] was wrong. expected %d. got %d.", i, expectedOffset, memberInfo.Offset));
			expectedOffset += memberInfo.Size;
		}
	});

	h->It("the component manager should allocate space properly", [](TestCase& t) {
		TestComplexComponentMgr test;
		t.Assert(test.GetCapacity() == 5,
			Format(
				"test mgr capacity was an unexpected value. expected %d. got %d.",
				5, 
				test.GetCapacity()));


	});

	h->It("components should be modifiable", [](TestCase& t) {
		TestComponentMgr test;
		EntityMgr eMgr;

		Entity e = eMgr.Create();
		test.SetFlag1(e, 1);
		t.Assert(test.GetFlag1(e) == 1,
			Format("unexpected value. expected 1. got %d", test.GetFlag1(e)));
		/*test.SetName(e, "Test Name");

		String testName = test.GetName(e);
		t.Assert(testName == "Test Name", 
			Format("unexpected name. expected 'Test Name'. got '%s'. (also this might crash...)", testName));*/
	});


	return h;
}