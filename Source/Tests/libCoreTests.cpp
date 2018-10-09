
#include <libCore/libCore.h>
#include <libCore/IRefCounted.h>
#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>
#include <libCore/Assert.h>
#include <libHarnessed/libHarnessed.h>
#include <functional>

#include <libMath/Vector.h>
#include <libCore/SOA.h>

using std::cout;
using std::cin;
using std::endl;

using namespace Firestorm;


class TupleBasedSimpleComponentMgr
{
public:
	SOA<uint8_t, uint8_t, uint8_t> _s;
	TupleBasedSimpleComponentMgr(size_t allocs = 5)
	{
		_s.Reserve(allocs);
	}

	void MakeInstance()
	{
		_s.PushBack(0, 0, 0);
	}

	void SetFlag1(const std::size_t& entity, uint8_t value)
	{
		_s[0_soa][entity] = value;
	}

	void SetFlag2(const std::size_t& entity, uint8_t value)
	{
		_s[1_soa][entity] = value;
	}

	const uint8_t& GetFlag1(const std::size_t& entity) const
	{
		return _s[0_soa][entity];
	}

	const uint8_t& GetFlag2(const std::size_t& entity) const
	{
		return _s[1_soa][entity];
	}
};



class TupleBasedComplexComponentMgr
{
public:
	SOA<String, double, float> _s;
	TupleBasedComplexComponentMgr(size_t allocs = 5)
	{
		_s.Reserve(allocs);
	}

	~TupleBasedComplexComponentMgr()
	{
	}

	void MakeInstance()
	{
		_s.PushBack("", 9001.0, 9001.0f);
	}
	void SetName(const std::size_t& entity, const char* value)
	{
		_s[0_soa][entity] = value;
	}

	const String& GetName(const std::size_t& entity) const
	{
		return _s[0_soa][entity];
	}

	void SetVelocity(const std::size_t& entity, double value)
	{
		_s[1_soa][entity] = value;
	}

	const double& GetVelocity(const std::size_t& entity) const
	{
		return _s[1_soa][entity];
	}
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// naieve OOP implementations
struct NaieveSimpleComponent
{
	uint8_t flag1;
	uint8_t flag2;
	uint8_t flag3;
};
struct NaieveSimpleComponentManager
{
	NaieveSimpleComponentManager(size_t allocations = 5)
	{
		components.reserve(allocations);
	}
	~NaieveSimpleComponentManager()
	{
	}

	void MakeInstance()
	{
		components.push_back(std::make_unique<NaieveSimpleComponent>());
	}

	void SetFlag1(const std::size_t& e, const uint8_t& value)
	{
		components[e]->flag1 = value;
	}

	const uint8_t& GetFlag1(const std::size_t& e) const
	{
		return components[e]->flag1;
	}

	void SetFlag2(const std::size_t& e, const uint8_t& value)
	{
		components[e]->flag2 = value;
	}

	const uint8_t& GetFlag2(const std::size_t& e) const
	{
		return components[e]->flag2;
	}

	Vector<std::unique_ptr<NaieveSimpleComponent>> components;
};


struct NaieveComplexComponent
{
	String name;
	double velocity;
	float forTheLulz;
};
struct NaieveComplexComponentManager
{
	NaieveComplexComponentManager(size_t allocations = 5)
	{
		components.reserve(allocations);
	}
	~NaieveComplexComponentManager()
	{
	}

	void MakeInstance(size_t numItems = 1)
	{
		components.push_back(std::make_unique<NaieveComplexComponent>());
	}

	void SetName(const std::size_t& e, const String& value)
	{
		components[e]->name = value;
	}

	const String& GetName(const std::size_t& e) const
	{
		return components[e]->name;
	}

	void SetVelocity(const std::size_t& e, const double& value)
	{
		components[e]->velocity = value;
	}

	const double& GetVelocity(const std::size_t& e) const
	{
		return components[e]->velocity;
	}

	Vector<std::unique_ptr<NaieveComplexComponent>> components;
};

#define NUM_OBJECTS 50000
#define NUM_RUNS 50


RefPtr<TestHarness> libCorePrepareHarness(int argc, char** argv)
{
	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<std::size_t> ents;
	ents.reserve(NUM_OBJECTS);
	for(size_t i = 0; i < NUM_OBJECTS; ++i)
	{
		ents.push_back(i);
	}

	FIRE_LOG_DEBUG("Sizeof String: %d", sizeof(String));

	RefPtr<TestHarness> h(new TestHarness("libCore Tests"));

	h->Profile(Format("Tuple/Vector Simple ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedSimpleComponentMgr* testMgr = new TupleBasedSimpleComponentMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetFlag1(ents[i], 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetFlag2(ents[i], 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetFlag1(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetFlag2(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
			delete testMgr;
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Tuple/Vector Complex ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedComplexComponentMgr* testMgr = new TupleBasedComplexComponentMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetName(ents[i], "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->SetVelocity(ents[i], 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetName(ents[i]) == "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr->GetVelocity(ents[i]) == 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
			delete testMgr;
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Naieve Simple ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			NaieveSimpleComponentManager testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag1(ents[i], 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetFlag2(ents[i], 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting flags 1");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag1(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting flags 2");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetFlag2(ents[i]) == 101);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});

	h->Profile(Format("Naieve Complex ComponentManager [%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			NaieveComplexComponentManager testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("component additions");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("setting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetName(ents[i], "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("setting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.SetVelocity(ents[i], 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("getting names");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetName(ents[i]) == "Slim Shady");
			}
			bm.StopSegment(ssh);

			ssh = bm.StartSegment("getting velocities");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				FIRE_ASSERT(testMgr.GetVelocity(ents[i]) == 9001.0);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
		}
		bm.StopSegment(ssh);
	});
	
	return h;
}