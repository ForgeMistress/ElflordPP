
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


class TupleBasedSimpleComponentMgr : public SOA<uint8_t, uint8_t, uint8_t, Vector3, Vector3>
{
public:
	TupleBasedSimpleComponentMgr(size_t allocs = 5)
	{
		Alloc(allocs);
	}

	void MakeInstance()
	{
		size_t index = New();
		Set<0>(index, 0);
		Set<1>(index, 0);
		Set<2>(index, 0);
	}

	void SetFlag1(const std::size_t& entity, uint8_t value)
	{
		Set<0>(entity, value);
	}

	void SetFlag2(const std::size_t& entity, uint8_t value)
	{
		Set<1>(entity, value);
	}

	const uint8_t& GetFlag1(const std::size_t& entity) const
	{
		return Get<0>(entity);
	}

	const uint8_t& GetFlag2(const std::size_t& entity) const
	{
		return Get<1>(entity);
	}
};



class TupleBasedComplexComponentMgr : public SOA<String, double, float>
{
public:
	TupleBasedComplexComponentMgr(size_t allocs = 5)
	{
		Alloc(allocs);

	}

	virtual ~TupleBasedComplexComponentMgr()
	{
	}

	void MakeInstance(size_t numItems = 1)
	{
		Vector<size_t> indices = BatchNew(numItems);
		for(size_t i=0; i<numItems; ++i)
		{
			Set<0>(indices[i], "");
			Set<1>(indices[i], 9001.0);
			Set<2>(indices[i], 9001.0f);
		}
	}

	void SetName(const std::size_t& entity, const String& value)
	{
		Set<0>(entity, value);
	}

	const String& GetName(const std::size_t& entity) const
	{
		return Get<0>(entity);
	}

	void SetVelocity(const std::size_t& entity, double value)
	{
		Set<1>(entity, std::move(value));
	}

	const double& GetVelocity(const std::size_t& entity) const
	{
		return Get<1>(entity);
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
		for(size_t i=0; i<components.size(); ++i)
		{
			delete components[i];
		}
	}

	void MakeInstance()
	{
		components.push_back(new NaieveSimpleComponent());
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

	Vector<NaieveSimpleComponent*> components;
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
		for(size_t i=0; i<components.size(); ++i)
		{
			delete components[i];
		}
	}
	void MakeInstance(size_t numItems = 1)
	{
		components.push_back(new NaieveComplexComponent());
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

	Vector<NaieveComplexComponent*> components;
};


static size_t numRuns = 5;
static size_t numObjects = 500000;

#define NUM_OBJECTS 500000
#define NUM_RUNS 100


RefPtr<TestHarness> libCorePrepareHarness(int argc, char** argv)
{
	FIRE_LOG_DEBUG("+++++ allocating entities");
	static Vector<std::size_t> ents;
	ents.reserve(NUM_OBJECTS);
	for(size_t i = 0; i < NUM_OBJECTS; ++i)
	{
		ents.push_back(i);
	}

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
			/*for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr->MakeInstance();
			}*/
			testMgr->MakeInstance(NUM_OBJECTS);
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