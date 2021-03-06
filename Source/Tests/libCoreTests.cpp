
#include <libCore/libCore.h>
#include <libCore/RefPtr.h>
#include <libCore/Assert.h>
#include <libCore/UUIDMgr.h>
#include <libHarnessed/libHarnessed.h>
#include <functional>

#include <libMath/Vector.h>
#include <libCore/SOA.h>

#include <EASTL/bonus/tuple_vector.h>

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
		_s.reserve(allocs);
	}

	void MakeInstance()
	{
		_s.push_back_uninitialized();
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
	SOA<string, double, Vector3> _s;
	TupleBasedComplexComponentMgr(size_t allocs = 5)
	{
		_s.reserve(allocs);
	}

	~TupleBasedComplexComponentMgr()
	{
	}

	void MakeInstance()
	{
		_s.push_back("", 9001.0, Vector3{ 0.0f,0.0f,0.0f });
	}
	void SetName(const std::size_t& entity, const char* value)
	{
		_s[0_soa][entity] = value;
	}

	const string& GetName(const std::size_t& entity) const
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
		components.push_back(NaieveSimpleComponent{ 0,0,0 });
	}

	void SetFlag1(const std::size_t& e, const uint8_t& value)
	{
		components[e].flag1 = value;
	}

	const uint8_t& GetFlag1(const std::size_t& e) const
	{
		return components[e].flag1;
	}

	void SetFlag2(const std::size_t& e, const uint8_t& value)
	{
		components[e].flag2 = value;
	}

	const uint8_t& GetFlag2(const std::size_t& e) const
	{
		return components[e].flag2;
	}

	vector<NaieveSimpleComponent> components;
};


struct NaieveComplexComponent
{
	string name;
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
		components.push_back(NaieveComplexComponent{ "",0.0,0.0f });
	}

	void SetName(const std::size_t& e, const string& value)
	{
		components[e].name = value;
	}

	const string& GetName(const std::size_t& e) const
	{
		return components[e].name;
	}

	void SetVelocity(const std::size_t& e, const double& value)
	{
		components[e].velocity = value;
	}

	const double& GetVelocity(const std::size_t& e) const
	{
		return components[e].velocity;
	}

	vector<NaieveComplexComponent> components;
};

#define NUM_OBJECTS 5000
#define NUM_RUNS 100


RefPtr<TestHarness> libCorePrepareHarness(int argc, char** argv)
{
	FIRE_LOG_DEBUG("+++++ allocating entities");
	static vector<std::size_t> ents;
	ents.reserve(NUM_OBJECTS);
	for(size_t i = 0; i < NUM_OBJECTS; ++i)
	{
		ents.push_back(i);
	}

	FIRE_LOG_DEBUG("Sizeof String: %d", sizeof(string));

	RefPtr<TestHarness> h(new TestHarness("libCore Tests"));

#ifdef FIRE_DISABLED
	h->It("allocating a massive SOA", [](TestCase& t) {
		using SOAType = 
			SOA<uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;
		SOAType container;
		container.Reserve(10000000);
		container.Bytes();
	});

	h->It("SOA containers should report back the correct traits <uint8 * 4>", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		t.AssertIsTrue(SOAType::IsTrivialDest, "the soa container should report back as trivially destructible");
		t.AssertIsTrue(SOAType::IsTrivialCons, "the soa container should report back as trivially constructible");
		t.Assert(SOAType::Sizeof == 4, "the soa container reported back the wrong size");
	});
	
	h->It("SOA containers should allocate their memory without issue", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		container.Reserve(5);
		t.Assert(container.Capacity() == 5,
			Format("the container reported the wrong capacity. expected %d, got %d", 5, container.Capacity()));
		t.Assert(container.Size() == 0,
			Format("the container reported the wrong size. expected %d, got %d", 0, container.Size()));
		t.Assert(container.Bytes() == 5 * SOAType::Sizeof,
			Format("the container reported the wrong buffer size. expected %d, got %d", 5 * SOAType::Sizeof, container.Bytes()));
	});

	h->It("This is a test of the memory leak checker. The following is not an error!", [](TestCase& t) {
		libCore::ReportMemoryLeaks();
	});

	h->It("a container that has items pushed back should increase their size exponentially.", [](TestCase& t) {
		using SOAType = SOA<uint8_t, uint8_t, uint8_t, uint8_t>;
		SOAType container;
		t.Assert(container.Capacity() == 0,
			Format("[1]the container reported the wrong capacity. expected %d, got %d", 0, container.Capacity()));
		t.Assert(container.Size() == 0,
			Format("[1]the container reported the wrong size. expected %d, got %d", 0, container.Size()));
		t.Assert(container.Bytes() == 0,
			Format("[1]the container reported the wrong buffer size. expected %d, got %d", 0, container.Bytes()));

		container.PushBack(0, 0, 0, 0);

		t.Assert(container.Capacity() == 1,
			Format("[2]the container reported the wrong capacity. expected %d, got %d", 1, container.Capacity()));
		t.Assert(container.Size() == 1,
			Format("[2]the container reported the wrong size. expected %d, got %d", 1, container.Size()));
		t.Assert(container.Bytes() == 4,
			Format("[2]the container reported the wrong buffer size. expected %d, got %d", 4, container.Bytes()));

		container.PushBack(0, 0, 0, 0);
		t.Assert(container.Capacity() == 2,
			Format("[3]the container reported the wrong capacity. expected %d, got %d", 2, container.Capacity()));
		t.Assert(container.Size() == 2,
			Format("[3]the container reported the wrong size. expected %d, got %d", 2, container.Size()));
		t.Assert(container.Bytes() == 8,
			Format("[3]the container reported the wrong buffer size. expected %d, got %d", 8, container.Bytes()));

		container.PushBack(0, 0, 0, 0);
		t.Assert(container.Capacity() == 4,
			Format("[4]the container reported the wrong capacity. expected %d, got %d", 4, container.Capacity()));
		t.Assert(container.Size() == 3,
			Format("[4]the container reported the wrong size. expected %d, got %d", 3, container.Size()));
		t.Assert(container.Bytes() == 16,
			Format("[4]the container reported the wrong buffer size. expected %d, got %d", 16, container.Bytes()));
	});
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	h->It("the UUID manager should allocate and deallocate as expected", [](TestCase& t) {
		UUIDMgr uuidMgr;
	});

	h->It("the UUID manager should return unique IDs and release those IDs when done", [](TestCase& t) {
		UUIDMgr uuidMgr;
		vector<UUID> uuids;
		uuids.reserve(1000000);
		for(size_t i=0; i<1000000; i++)
		{
			uuids.push_back(uuidMgr.Get());
		}
	});

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	h->Profile("warming up the CPU for the benchmarks...", NUM_RUNS * 20, [](Benchmark& bm) {
		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("warming up 1...");
			TupleBasedSimpleComponentMgr testMgr(NUM_OBJECTS);
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("warming up 2...");
			for(size_t i = 0; i < NUM_OBJECTS; ++i)
			{
				testMgr.MakeInstance();
			}
			bm.StopSegment(ssh);
		}
	});

	h->Profile(Format("Tuple Based SOA (Simple <uint8_t,uint8_t,uint8_t>)[%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedSimpleComponentMgr testMgr(NUM_OBJECTS);
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

	h->Profile(Format("Tuple Based SOA (Complex <string,double,float>)[%d Items]",NUM_OBJECTS), NUM_RUNS, [](Benchmark& bm) {

		Benchmark::SnapshotHandle* ssh = nullptr;
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("manager constructor");
			TupleBasedComplexComponentMgr testMgr(NUM_OBJECTS);
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
				FIRE_ASSERT(testMgr.GetVelocity(ents[i]) == 9001.0f);
			}
			bm.StopSegment(ssh);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////

			ssh = bm.StartSegment("destruction");
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