
#include <libCore/libCore.h>
#include <libHarnessed/libHarnessed.h>

#include <libExistence/libExistence.h>
#include <libExistence/ObjectPool.h>
#include <libExistence/Engine.h>
#include <libExistence/System.h>
#include <libExistence/Component.h>
#include <libExistence/Entity.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>

using namespace Firestorm;

using std::cout;
using std::cin;
using std::endl;

class PooledObj
{
public:
	PooledObj() {
		i = 0;
		j = 1;
		k = 2;
	}

	void Delete() {}
	void Recycle() {}
	void Init() {}

	char i;
	char j;
	char k;
};

#define HANDLE_ERROR( RES, TAG ) \
	if(!RES.has_value()) \
		t.Assert(RES.has_value(), "operation tagged '" TAG "'returned an unexpected error : "+RES.error().Message)





class TestComponent_Plural : public Component
{
	FIRE_MIRROR_DECLARE(TestComponent_Plural, Component);
public:
	TestComponent_Plural() {}
	virtual ~TestComponent_Plural() {}
private:
	virtual void* DoInspect(Mirror::Type type) override
	{
		if (type == TestComponent_Plural::MyType())
		{
			return this;
		}
		return Component::DoInspect(type);
	}
};

class TestComponent_Singleton : public Component
{
	FIRE_MIRROR_DECLARE(TestComponent_Singleton, Component);
public:
	TestComponent_Singleton() {}
	virtual ~TestComponent_Singleton() {}
private:
	virtual void* DoInspect(Mirror::Type type) override
	{
		if(type == TestComponent_Singleton::MyType())
		{
			return this;
		}
		return Component::DoInspect(type);
	}
};


class TestSystem_1 : public System
{
	FIRE_MIRROR_DECLARE(TestSystem_1, System);
private:
	virtual void OnModified(double deltaT) override;
	virtual void OnEntityAdded(const WeakPtr<Entity>& entity) override;
	virtual void OnEntityRemoved(const WeakPtr<Entity>& entity) override;
	virtual void OnBeforeAddToEngine() override;
	virtual void OnAddToEngine() override;
	virtual void OnRemoveFromEngine() override;
	virtual void OnTick(double deltaT, const Vector<WeakPtr<Entity>>& entities) override;
	virtual bool OnEntityFilter(const WeakPtr<Entity>& entity) const override;

	virtual void* DoInspect(Mirror::Type type) override
	{
		if(type == TestSystem_1::MyType())
		{
			return this;
		}
		return System::DoInspect(type);
	}
};

void TestSystem_1::OnModified(double deltaT)
{
}

void TestSystem_1::OnEntityAdded(const WeakPtr<Entity>& entity)
{
}

void TestSystem_1::OnEntityRemoved(const WeakPtr<Entity>& entity)
{
}

void TestSystem_1::OnBeforeAddToEngine()
{
}

void TestSystem_1::OnAddToEngine()
{
}

void TestSystem_1::OnRemoveFromEngine()
{
}

void TestSystem_1::OnTick(double deltaT, const Vector<WeakPtr<Entity>>& entities)
{
}

bool TestSystem_1::OnEntityFilter(const WeakPtr<Entity>& entity) const
{
	return entity.Lock()->Inspect<TestComponent_Plural>() != nullptr;
}

FIRE_MIRROR_DEFINE(TestSystem_1)
{
	Class.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	);
}

FIRE_MIRROR_DEFINE(TestComponent_Plural)
{
	Class
	(
		Mirror::Meta(ComponentMetadata::kPlural, true)
	)
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	);
}

FIRE_MIRROR_DEFINE(TestComponent_Singleton)
{
	Class
	(
	)
	.constructor<>()
	(
		rttr::policy::ctor::as_raw_ptr
	);
}



RefPtr<TestHarness> libExistencePrepareHarness(int argc, char** argv)
{
	TestSystem_1::RegisterReflection();
	TestComponent_Plural::RegisterReflection();

	RefPtr<TestHarness> h(new TestHarness("libExistence Tests"));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Begin ObjectPool Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*h.It("object pools should instantiate with the correct number of objects", [](Firestorm::TestCase& t) {
		Firestorm::ObjectPool<PooledObj> objPool(500);
		t.Assert(objPool.GetObjectBlockSize() == 1500, "object block size was incorrect");
		t.Assert(objPool.GetFlagBlockSize() == 500, "flag block size was incorrect");
	});

	h.It("object pools should return valid pointers when Acquire is called", [](Firestorm::TestCase& t) {
		Firestorm::ObjectPool<PooledObj> objPool(500);
		Firestorm::Result<Firestorm::RefPtr<PooledObj>,Firestorm::Error> obj = objPool.Acquire();
		HANDLE_ERROR(obj, "objPool.Acquire");
		t.Assert(obj.value().Get() != nullptr, "could not acquire object from pool");
	});

	h.It("objects acquired from the pool should be marked as 'in use' when they're active", [](Firestorm::TestCase& t) {
		Firestorm::ObjectPool<PooledObj> objPool(10);
		{
			Firestorm::Result<Firestorm::RefPtr<PooledObj>,Firestorm::Error> obj = objPool.Acquire();
			HANDLE_ERROR(obj, "objPool.Acquire");

			Firestorm::Result<bool, Firestorm::Error> result = objPool.IsInUse(obj.value().Get());
			HANDLE_ERROR(result, "objPool.IsInUse");

			if(result.has_value())
			{
				t.Assert(result.value(), "object is not marked as being in use when it should be");
			}
		}
	});

	h.It("object pools should return invalid pointers when Acquire is called and there are no more objects", 
		[](Firestorm::TestCase& t) {
		Firestorm::ObjectPool<PooledObj> objPool(500);
		Firestorm::RefPtr<PooledObj> objs[500];
		for(int i = 0; i < 500; i++)
		{
			Firestorm::Result<Firestorm::RefPtr<PooledObj>, Firestorm::Error> obj = objPool.Acquire();
			t.Assert(obj.has_value(), "objPool returned an error when it shouldn't have");
			objs[i] = obj.value();
		}
		Firestorm::Result<Firestorm::RefPtr<PooledObj>, Firestorm::Error> obj = objPool.Acquire();
		t.Assert(!obj.has_value(), "object pool should have returned an error. it didn't.");
	});*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End ObjectPool Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Begin ECS Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	h->It("[ECS] Entities should be instantiable and should be able to have components added to them", [](TestCase& t) {
		Entity entity;
		entity.AddComponent<TestComponent_Singleton>();
		t.Assert(entity.GetNumComponents() == 1, "the test entity had the wrong number of components inside of it");

		TestComponent_Singleton* testComponent = entity.Inspect<TestComponent_Singleton>();
		t.Assert(testComponent != nullptr, "the test component could not be retrieved from the entity via inspection");
	});

	h->It("[ECS] Entities should not return pointers to components they do not have [nothrow]", [](TestCase& t) {
		Entity entity;
		entity.AddComponent<TestComponent_Plural>();
		t.Assert(entity.GetNumComponents() == 1, "the test entity had the wrong number of components inside of it");

		TestComponent_Singleton* testComponent = entity.Inspect<TestComponent_Singleton>();
		t.Assert(testComponent == nullptr, "the test entity returned a pointer to a component it shouldn't have");
	});

	h->It("[ECS] Entities should only be allowed a single instance of a singleton component", [](TestCase& t) {
		Entity entity;
		bool result = entity.AddComponent<TestComponent_Singleton>();
		t.AssertIsTrue(result, "the operation to add a singleton component to the entity failed");

		int numComponents = entity.GetNumComponents();
		t.Assert(numComponents == 1, "the test entity had the wrong number of components inside of it");
		t.Assert(numComponents == 2, "forcing an error...");

		TestComponent_Singleton* testComponent = entity.Inspect<TestComponent_Singleton>();
		t.AssertNotNull(testComponent, "the test entity returned a pointer to a component it shouldn't have");

		result = entity.AddComponent<TestComponent_Singleton>();
		t.AssertIsFalse(result, "adding the second singleton should have returned false [template]");

		result = entity.AddComponent(TestComponent_Singleton::MyType());
		t.AssertIsFalse(result, "adding the second singleton should have returned false [Mirror::Type]");

		numComponents = entity.GetNumComponents();
		t.Assert(numComponents == 1, "the entity reported >1 singleton component when it should only have one");
	});

	h->It("[ECS] Engines should be able to have systems added to it", [](Firestorm::TestCase& t) {
		Engine engine;
		engine.AddSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 1, "the test engine had the wrong number of systems inside of it");
	});

	h->It("[ECS] Engines should have the correct number of Systems within them after one is removed", [](Firestorm::TestCase& t) {
		Engine engine;
		engine.AddSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 1, "the test engine had the wrong number of systems inside of it before removing it");

		engine.RemoveSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 0, "the test engine had the wrong number of systems inside of it after removing it");
	});

	h->It("[ECS] Systems should have Entities that are addable to them", [](Firestorm::TestCase& t) {
		Engine engine;
		engine.AddSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 1, "the test engine had the wrong number of systems inside of it");

		RefPtr<Entity> e(new Entity);
		e->AddComponent<TestComponent_Plural>();
		t.Assert(e->GetNumComponents() == 1, "the test entity did not have the correct number of components");

		engine.AddEntity(e);
		engine.Update(0.0);
	});

	h->It("[ECS] Engines should only ever have one system of a given type registered at a time", [](Firestorm::TestCase& t) {
		Engine engine;
		engine.AddSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 1, "the test engine had the wrong number of systems inside of it");

		engine.AddSystem<TestSystem_1>();
		engine.Update(0.0);

		t.Assert(engine.GetNumSystems() == 1, "the test engine had a duplicate system of a type inside of it");
	});
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End ECS Testing
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return h;
}