/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ModelTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the component model: service, worker thread,
 *              dependency, component and thread entries and lists, and the Model.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/Model.hpp"

#include <any>
#include <string>
#include <utility>

namespace
{
    using namespace areg;

    FuncCreateComponent createFunc()
    {
        return [](const ComponentEntry &, ComponentThread &) -> Component * { return nullptr; };
    }

    FuncDeleteComponent deleteFunc()
    {
        return [](Component &, const ComponentEntry &) -> void { };
    }

    ComponentEntry makeComponent(const char * thread, const char * role)
    {
        return ComponentEntry(String(thread), String(role), createFunc(), deleteFunc());
    }

    ComponentThreadEntry makeThread(const char * thread, const char * role)
    {
        ComponentThreadEntry result{String(thread)};
        result.add_component(makeComponent(thread, role));
        return result;
    }
}

/************************************************************************
 * Names of the enumerations.
 ************************************************************************/

TEST(ModelTest, enum_names)
{
    EXPECT_STREQ("areg::FailurePolicy::Exit", as_string(FailurePolicy::Exit));
    EXPECT_STREQ("areg::FailurePolicy::Restart", as_string(FailurePolicy::Restart));
    EXPECT_STREQ("areg::FailurePolicy::Ignore", as_string(FailurePolicy::Ignore));
    EXPECT_STREQ("areg::ModelState::Unloaded", as_string(ModelState::Unloaded));
    EXPECT_STREQ("areg::ModelState::Loading", as_string(ModelState::Loading));
    EXPECT_STREQ("areg::ModelState::Loaded", as_string(ModelState::Loaded));
    EXPECT_STREQ("areg::ModelState::Unloading", as_string(ModelState::Unloading));
    EXPECT_STREQ("areg::ThreadState::Unknown", as_string(ThreadState::Unknown));
    EXPECT_STREQ("areg::ThreadState::Starting", as_string(ThreadState::Starting));
    EXPECT_STREQ("areg::ThreadState::Running", as_string(ThreadState::Running));
    EXPECT_STREQ("areg::ThreadState::Stopping", as_string(ThreadState::Stopping));
    EXPECT_STREQ("areg::ThreadState::Stopped", as_string(ThreadState::Stopped));
    EXPECT_STREQ("areg::ThreadState::Abandoned", as_string(ThreadState::Abandoned));
}

TEST(ModelTest, enum_names_of_invalid_values)
{
    EXPECT_NE(std::string::npos, std::string(as_string(static_cast<FailurePolicy>(200))).find("ERR"));
    EXPECT_NE(std::string::npos, std::string(as_string(static_cast<ModelState>(200))).find("ERR"));
    EXPECT_NE(std::string::npos, std::string(as_string(static_cast<ThreadState>(200))).find("ERR"));
}

/************************************************************************
 * ServiceEntry
 ************************************************************************/

TEST(ModelTest, service_entry_default_is_invalid)
{
    ServiceEntry entry;
    EXPECT_TRUE(entry.mName.is_empty());
    EXPECT_FALSE(entry.mVersion.is_valid());
    EXPECT_FALSE(entry.is_valid());
}

TEST(ModelTest, service_entry_construct_from_parts)
{
    ServiceEntry byParts(String("Svc"), 1u, 2u, 3u);
    EXPECT_TRUE(byParts.is_valid());
    EXPECT_TRUE(byParts.mName == String("Svc"));
    EXPECT_EQ(1u, byParts.mVersion.major());
    EXPECT_EQ(2u, byParts.mVersion.minor());
    EXPECT_EQ(3u, byParts.mVersion.patch());

    ServiceEntry byVersion(String("Svc"), Version(1, 2, 3));
    EXPECT_TRUE(byVersion.is_valid());
    EXPECT_TRUE(byParts == byVersion);
}

TEST(ModelTest, service_entry_invalid_version_is_invalid)
{
    ServiceEntry entry(String("Svc"), Version(0, 0, 5));
    EXPECT_FALSE(entry.mVersion.is_valid());
    EXPECT_FALSE(entry.is_valid());
}

TEST(ModelTest, service_entry_copy_move_and_assign)
{
    ServiceEntry src(String("Svc"), Version(2, 1, 0));

    ServiceEntry copied(src);
    EXPECT_TRUE(copied == src);

    ServiceEntry moved(std::move(copied));
    EXPECT_TRUE(moved == src);

    ServiceEntry assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);

    ServiceEntry moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);

    ServiceEntry & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);
}

TEST(ModelTest, service_entry_compare_each_field)
{
    ServiceEntry base(String("Svc"), Version(1, 2, 3));
    ServiceEntry same(String("Svc"), Version(1, 2, 3));
    ServiceEntry otherName(String("Other"), Version(1, 2, 3));
    ServiceEntry otherMajor(String("Svc"), Version(2, 2, 3));
    ServiceEntry otherMinor(String("Svc"), Version(1, 3, 3));
    ServiceEntry otherPatch(String("Svc"), Version(1, 2, 4));

    EXPECT_TRUE(base == same);
    EXPECT_FALSE(base != same);
    EXPECT_TRUE(base != otherName);
    EXPECT_TRUE(base != otherMajor);
    EXPECT_TRUE(base != otherMinor);
    EXPECT_TRUE(base != otherPatch);
    EXPECT_FALSE(base == otherName);
    EXPECT_FALSE(base == otherPatch);
    EXPECT_TRUE(base == base);
    EXPECT_FALSE(base != base);
}

/************************************************************************
 * ServiceList
 ************************************************************************/

TEST(ModelTest, service_list_default_and_single_entry)
{
    ServiceList empty;
    EXPECT_FALSE(empty.is_valid());
    EXPECT_EQ(0u, empty.mListServices.size());

    ServiceList single(ServiceEntry(String("Svc"), Version(1, 0, 0)));
    EXPECT_TRUE(single.is_valid());
    EXPECT_EQ(1u, single.mListServices.size());

    ServiceList fromInvalid{ServiceEntry()};
    EXPECT_FALSE(fromInvalid.is_valid());
}

TEST(ModelTest, service_list_find_by_name_and_entry)
{
    ServiceList list;
    list.mListServices.add(ServiceEntry(String("A"), Version(1, 0, 0)));
    list.mListServices.add(ServiceEntry(String("B"), Version(2, 0, 0)));

    EXPECT_EQ(0, list.find_service(String("A")));
    EXPECT_EQ(1, list.find_service(String("B")));
    EXPECT_EQ(INVALID_INDEX, list.find_service(String("C")));
    EXPECT_EQ(1, list.find_service(ServiceEntry(String("B"), Version(9, 9, 9))));

    EXPECT_TRUE(list.service(String("B")).mVersion == Version(2, 0, 0));
    EXPECT_FALSE(list.service(String("C")).is_valid());
}

TEST(ModelTest, service_list_index_operator)
{
    ServiceList list(ServiceEntry(String("A"), Version(1, 0, 0)));
    EXPECT_TRUE(list[0].mName == String("A"));
    EXPECT_FALSE(list[1].is_valid());
    EXPECT_FALSE(list[0xFFFFu].is_valid());
}

TEST(ModelTest, service_list_copy_move_assign)
{
    ServiceList src(ServiceEntry(String("A"), Version(1, 0, 0)));
    ServiceList copied(src);
    EXPECT_EQ(0, copied.find_service(String("A")));

    ServiceList moved(std::move(copied));
    EXPECT_EQ(0, moved.find_service(String("A")));

    ServiceList assigned;
    assigned = src;
    EXPECT_EQ(0, assigned.find_service(String("A")));

    ServiceList moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(0, moveAssigned.find_service(String("A")));

    ServiceList & self = moveAssigned;
    moveAssigned = self;
    EXPECT_EQ(1u, moveAssigned.mListServices.size());
}

/************************************************************************
 * WorkerThreadEntry and WorkerThreadList
 ************************************************************************/

TEST(ModelTest, worker_entry_default_values)
{
    WorkerThreadEntry entry;
    EXPECT_FALSE(entry.is_valid());
    EXPECT_TRUE(entry.mThreadName.is_empty());
    EXPECT_TRUE(entry.mConsumerName.is_empty());
    EXPECT_EQ(WATCHDOG_IGNORE, entry.mWatchdogTimeout);
    EXPECT_EQ(DEFAULT_STACK_SIZE, entry.mStackSizeKb);
    EXPECT_EQ(IGNORE_VALUE, entry.mMaxQueue);
    EXPECT_TRUE(entry.mDropOnFull == Bool::Undefined);
    EXPECT_EQ(WAIT_INFINITE, entry.mQueueTimeout);
}

TEST(ModelTest, worker_entry_names_are_composed)
{
    WorkerThreadEntry entry(String("master"), String("worker"), String("role"), String("consumer"), 500u, 64u, 10u, Bool::True, 20u);
    EXPECT_TRUE(entry.is_valid());
    EXPECT_TRUE(entry.mThreadName == create_component_item_name(String("master"), String("worker")));
    EXPECT_TRUE(entry.mConsumerName == create_component_item_name(String("role"), String("consumer")));
    EXPECT_EQ(500u, entry.mWatchdogTimeout);
    EXPECT_EQ(64u, entry.mStackSizeKb);
    EXPECT_EQ(10u, entry.mMaxQueue);
    EXPECT_TRUE(entry.mDropOnFull == Bool::True);
    EXPECT_EQ(20u, entry.mQueueTimeout);
}

TEST(ModelTest, worker_entry_empty_part_makes_it_invalid)
{
    EXPECT_FALSE(WorkerThreadEntry(String(), String("w"), String("r"), String("c")).is_valid());
    EXPECT_FALSE(WorkerThreadEntry(String("m"), String(), String("r"), String("c")).is_valid());
    EXPECT_FALSE(WorkerThreadEntry(String("m"), String("w"), String(), String("c")).is_valid());
    EXPECT_FALSE(WorkerThreadEntry(String("m"), String("w"), String("r"), String()).is_valid());
}

TEST(ModelTest, worker_entry_copy_move_assign_compare)
{
    WorkerThreadEntry src(String("m"), String("w"), String("r"), String("c"), 100u);

    WorkerThreadEntry copied(src);
    EXPECT_TRUE(copied == src);
    EXPECT_EQ(100u, copied.mWatchdogTimeout);

    WorkerThreadEntry moved(std::move(copied));
    EXPECT_TRUE(moved == src);

    WorkerThreadEntry assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);

    WorkerThreadEntry moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);

    WorkerThreadEntry & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);
    EXPECT_FALSE(moveAssigned != src);
}

TEST(ModelTest, worker_entry_compare_each_field)
{
    WorkerThreadEntry base(String("m"), String("w"), String("r"), String("c"));
    WorkerThreadEntry otherThread(String("m"), String("w2"), String("r"), String("c"));
    WorkerThreadEntry otherConsumer(String("m"), String("w"), String("r"), String("c2"));

    EXPECT_TRUE(base != otherThread);
    EXPECT_FALSE(base == otherThread);
    EXPECT_TRUE(base != otherConsumer);
    EXPECT_FALSE(base == otherConsumer);
    EXPECT_TRUE(base == base);
    EXPECT_FALSE(base != base);
}

TEST(ModelTest, worker_list_add_find_and_index)
{
    WorkerThreadList empty;
    EXPECT_FALSE(empty.is_valid());

    WorkerThreadEntry first(String("m"), String("w1"), String("r"), String("c"));
    WorkerThreadEntry second(String("m"), String("w2"), String("r"), String("c"));
    WorkerThreadList list(first);
    list.mListWorkers.add(second);

    EXPECT_TRUE(list.is_valid());
    EXPECT_EQ(0, list.find_thread(first));
    EXPECT_EQ(1, list.find_thread(second));
    EXPECT_EQ(1, list.find_thread(second.mThreadName));
    EXPECT_EQ(INVALID_INDEX, list.find_thread(String("none")));
    EXPECT_TRUE(list.worker_thread(second.mThreadName) == second);
    EXPECT_FALSE(list.worker_thread(String("none")).is_valid());
    EXPECT_TRUE(list[0] == first);
    EXPECT_FALSE(list[2].is_valid());

    WorkerThreadList fromInvalid{WorkerThreadEntry()};
    EXPECT_FALSE(fromInvalid.is_valid());
}

TEST(ModelTest, worker_list_copy_move_assign)
{
    WorkerThreadList src(WorkerThreadEntry(String("m"), String("w"), String("r"), String("c")));
    WorkerThreadList copied(src);
    EXPECT_EQ(1u, copied.mListWorkers.size());
    WorkerThreadList moved(std::move(copied));
    EXPECT_EQ(1u, moved.mListWorkers.size());
    WorkerThreadList assigned;
    assigned = src;
    EXPECT_EQ(1u, assigned.mListWorkers.size());
    WorkerThreadList moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(1u, moveAssigned.mListWorkers.size());
    WorkerThreadList & self = moveAssigned;
    moveAssigned = self;
    EXPECT_EQ(1u, moveAssigned.mListWorkers.size());
}

/************************************************************************
 * DependencyEntry and DependencyList
 ************************************************************************/

TEST(ModelTest, dependency_entry_default_value_and_validity)
{
    DependencyEntry empty;
    EXPECT_FALSE(empty.is_valid());

    DependencyEntry entry(String("Server"));
    EXPECT_TRUE(entry.is_valid());
    EXPECT_TRUE(entry.depdendent_service() == String("Server"));
    EXPECT_FALSE(DependencyEntry(String()).is_valid());
}

TEST(ModelTest, dependency_entry_set_dependent_service)
{
    DependencyEntry entry;
    entry.set_dependent_service(String("Server"));
    EXPECT_TRUE(entry.is_valid());
    EXPECT_TRUE(entry.mRoleName == String("Server"));
    entry.set_dependent_service(String());
    EXPECT_FALSE(entry.is_valid());
}

TEST(ModelTest, dependency_entry_copy_move_assign_compare)
{
    DependencyEntry src(String("Server"));
    DependencyEntry copied(src);
    EXPECT_TRUE(copied == src);
    DependencyEntry moved(std::move(copied));
    EXPECT_TRUE(moved == src);
    DependencyEntry assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);
    DependencyEntry moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);
    DependencyEntry & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);

    DependencyEntry other(String("Other"));
    EXPECT_TRUE(src != other);
    EXPECT_FALSE(src == other);
    EXPECT_FALSE(src != moved);
}

TEST(ModelTest, dependency_list_find_and_index)
{
    DependencyList empty;
    EXPECT_FALSE(empty.is_valid());

    DependencyList list(DependencyEntry(String("A")));
    list.mListDependencies.add(DependencyEntry(String("B")));
    EXPECT_TRUE(list.is_valid());
    EXPECT_EQ(0, list.find_dependency(String("A")));
    EXPECT_EQ(1, list.find_dependency(DependencyEntry(String("B"))));
    EXPECT_EQ(INVALID_INDEX, list.find_dependency(String("C")));
    EXPECT_TRUE(list.dependency(String("B")).is_valid());
    EXPECT_FALSE(list.dependency(String("C")).is_valid());
    EXPECT_TRUE(list[1].mRoleName == String("B"));
    EXPECT_FALSE(list[2].is_valid());

    DependencyList fromInvalid{DependencyEntry()};
    EXPECT_FALSE(fromInvalid.is_valid());
}

TEST(ModelTest, dependency_list_copy_move_assign)
{
    DependencyList src(DependencyEntry(String("A")));
    DependencyList copied(src);
    EXPECT_EQ(0, copied.find_dependency(String("A")));
    DependencyList moved(std::move(copied));
    EXPECT_EQ(0, moved.find_dependency(String("A")));
    DependencyList assigned;
    assigned = src;
    EXPECT_EQ(0, assigned.find_dependency(String("A")));
    DependencyList moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(0, moveAssigned.find_dependency(String("A")));
    DependencyList & self = moveAssigned;
    moveAssigned = self;
    EXPECT_EQ(1u, moveAssigned.mListDependencies.size());
}

/************************************************************************
 * ComponentEntry
 ************************************************************************/

TEST(ModelTest, component_entry_default_is_invalid)
{
    ComponentEntry entry;
    EXPECT_FALSE(entry.is_valid());
    EXPECT_FALSE(entry.has_data());
    EXPECT_FALSE(entry.supported_services().is_valid());
    EXPECT_FALSE(entry.worker_threads().is_valid());
    EXPECT_FALSE(entry.dependency_services().is_valid());
}

TEST(ModelTest, component_entry_validity_needs_role_and_functions)
{
    EXPECT_TRUE(makeComponent("thread", "role").is_valid());
    EXPECT_FALSE(ComponentEntry(String("thread"), String(), createFunc(), deleteFunc()).is_valid());
    EXPECT_FALSE(ComponentEntry(String("thread"), String("role"), nullptr, deleteFunc()).is_valid());
    EXPECT_FALSE(ComponentEntry(String("thread"), String("role"), createFunc(), nullptr).is_valid());

    ComponentEntry entry(String("thread"), String("role"), nullptr, nullptr);
    EXPECT_FALSE(entry.is_valid());
    entry.set_instance_methods(createFunc(), deleteFunc());
    EXPECT_TRUE(entry.is_valid());
    entry.set_instance_methods(nullptr, nullptr);
    EXPECT_FALSE(entry.is_valid());
}

TEST(ModelTest, component_entry_construct_with_lists)
{
    ServiceList services(ServiceEntry(String("Svc"), Version(1, 0, 0)));
    DependencyList dependencies(DependencyEntry(String("Server")));
    WorkerThreadList workers(WorkerThreadEntry(String("thread"), String("w"), String("role"), String("c")));

    ComponentEntry entry(String("thread"), String("role"), createFunc(), deleteFunc(), services, dependencies, workers);
    EXPECT_TRUE(entry.is_valid());
    EXPECT_TRUE(entry.mRoleName == String("role"));
    EXPECT_TRUE(entry.mThreadName == String("thread"));
    EXPECT_EQ(0, entry.find_supported_service(String("Svc")));
    EXPECT_EQ(0, entry.find_dependency_service(String("Server")));
    EXPECT_EQ(1u, entry.worker_threads().mListWorkers.size());
}

TEST(ModelTest, component_entry_construct_with_single_entries)
{
    ServiceEntry service(String("Svc"), Version(1, 0, 0));
    DependencyEntry dependency(String("Server"));
    WorkerThreadEntry worker(String("thread"), String("w"), String("role"), String("c"));

    ComponentEntry entry(String("thread"), String("role"), createFunc(), deleteFunc(), service, dependency, worker);
    EXPECT_EQ(0, entry.find_supported_service(service));
    EXPECT_EQ(0, entry.find_dependency_service(dependency));
    EXPECT_EQ(0, entry.find_worker_thread(worker));
}

TEST(ModelTest, component_entry_copy_move_assign)
{
    ComponentEntry src = makeComponent("thread", "role");
    src.add_supported_service(String("Svc"), Version(1, 0, 0));
    src.set_data(std::any(7));

    ComponentEntry copied(src);
    EXPECT_TRUE(copied == src);
    EXPECT_TRUE(copied.is_valid());
    EXPECT_EQ(0, copied.find_supported_service(String("Svc")));
    EXPECT_EQ(7, std::any_cast<int>(copied.data()));

    ComponentEntry moved(std::move(copied));
    EXPECT_TRUE(moved == src);
    EXPECT_TRUE(moved.is_valid());
    EXPECT_EQ(0, moved.find_supported_service(String("Svc")));
    EXPECT_EQ(7, std::any_cast<int>(moved.data()));

    ComponentEntry assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);
    EXPECT_TRUE(assigned.is_valid());
    EXPECT_EQ(7, std::any_cast<int>(assigned.data()));

    ComponentEntry moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);
    EXPECT_TRUE(moveAssigned.is_valid());
    EXPECT_EQ(7, std::any_cast<int>(moveAssigned.data()));

    ComponentEntry & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);
    EXPECT_EQ(0, moveAssigned.find_supported_service(String("Svc")));
    EXPECT_EQ(7, std::any_cast<int>(moveAssigned.data()));
}

TEST(ModelTest, component_entry_compare_role_and_thread)
{
    ComponentEntry base = makeComponent("thread", "role");
    EXPECT_TRUE(base == makeComponent("thread", "role"));
    EXPECT_FALSE(base == makeComponent("thread", "other"));
    EXPECT_FALSE(base == makeComponent("other", "role"));
    EXPECT_TRUE(base == base);
}

TEST(ModelTest, component_entry_add_supported_service_entry_rejects_duplicate)
{
    ComponentEntry entry = makeComponent("thread", "role");
    ServiceEntry service(String("Svc"), Version(1, 0, 0));

    entry.add_supported_service(service);
    entry.add_supported_service(service);
    entry.add_supported_service(ServiceEntry(String("Svc"), Version(2, 0, 0)));
    EXPECT_EQ(1u, entry.supported_services().mListServices.size());
    EXPECT_TRUE(entry.supported_services().service(String("Svc")).mVersion == Version(1, 0, 0));
}

TEST(ModelTest, component_entry_add_supported_service_list)
{
    ServiceList list(ServiceEntry(String("A"), Version(1, 0, 0)));
    list.mListServices.add(ServiceEntry(String("B"), Version(1, 0, 0)));

    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_supported_service(ServiceEntry(String("A"), Version(1, 0, 0)));
    entry.add_supported_service(list);
    EXPECT_EQ(2u, entry.supported_services().mListServices.size());
    EXPECT_EQ(0, entry.find_supported_service(String("A")));
    EXPECT_EQ(1, entry.find_supported_service(String("B")));
    EXPECT_EQ(1, entry.find_supported_service(ServiceEntry(String("B"), Version(1, 0, 0))));
}

TEST(ModelTest, component_entry_add_supported_service_by_name_returns_existing)
{
    ComponentEntry entry = makeComponent("thread", "role");
    ServiceEntry & first = entry.add_supported_service(String("Svc"), Version(1, 2, 3));
    EXPECT_TRUE(first.mName == String("Svc"));
    EXPECT_TRUE(first.mVersion == Version(1, 2, 3));

    ServiceEntry & again = entry.add_supported_service(String("Svc"), Version(9, 9, 9));
    EXPECT_EQ(&first, &again);
    EXPECT_TRUE(again.mVersion == Version(1, 2, 3));
    EXPECT_EQ(1u, entry.supported_services().mListServices.size());
}

TEST(ModelTest, component_entry_remove_supported_service)
{
    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_supported_service(String("A"), Version(1, 0, 0));
    entry.add_supported_service(String("B"), Version(1, 0, 0));

    EXPECT_FALSE(entry.remove_supported_service(String()));
    EXPECT_FALSE(entry.remove_supported_service(String("C")));
    EXPECT_TRUE(entry.remove_supported_service(String("A")));
    EXPECT_EQ(INVALID_INDEX, entry.find_supported_service(String("A")));
    EXPECT_EQ(0, entry.find_supported_service(String("B")));
    EXPECT_FALSE(entry.remove_supported_service(String("A")));
}

TEST(ModelTest, component_entry_add_worker_thread_rejects_duplicate)
{
    WorkerThreadEntry worker(String("thread"), String("w1"), String("role"), String("c"));
    WorkerThreadEntry other(String("thread"), String("w2"), String("role"), String("c"));

    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_worker_thread(worker);
    entry.add_worker_thread(worker);
    EXPECT_EQ(1u, entry.worker_threads().mListWorkers.size());
    EXPECT_EQ(0, entry.find_worker_thread(worker));
    EXPECT_EQ(INVALID_INDEX, entry.find_worker_thread(other));

    WorkerThreadList list(worker);
    list.mListWorkers.add(other);
    entry.add_worker_thread(list);
    EXPECT_EQ(2u, entry.worker_threads().mListWorkers.size());
    EXPECT_EQ(1, entry.find_worker_thread(other.mThreadName));
}

TEST(ModelTest, component_entry_remove_worker_thread)
{
    WorkerThreadEntry worker(String("thread"), String("w1"), String("role"), String("c"));
    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_worker_thread(worker);

    EXPECT_FALSE(entry.remove_worker_thread(String()));
    EXPECT_FALSE(entry.remove_worker_thread(String("none")));
    EXPECT_TRUE(entry.remove_worker_thread(worker.mThreadName));
    EXPECT_FALSE(entry.worker_threads().is_valid());
    EXPECT_FALSE(entry.remove_worker_thread(worker.mThreadName));
}

TEST(ModelTest, component_entry_add_dependency_service_by_entry_and_list)
{
    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_dependency_service(DependencyEntry(String("A")));
    EXPECT_EQ(0, entry.find_dependency_service(String("A")));
    EXPECT_EQ(0, entry.find_dependency_service(DependencyEntry(String("A"))));
    EXPECT_EQ(INVALID_INDEX, entry.find_dependency_service(String("B")));

    DependencyList list(DependencyEntry(String("B")));
    list.mListDependencies.add(DependencyEntry(String("C")));
    entry.add_dependency_service(list);
    EXPECT_EQ(3u, entry.dependency_services().mListDependencies.size());
    EXPECT_EQ(2, entry.find_dependency_service(String("C")));
}

TEST(ModelTest, component_entry_add_dependency_service_by_role_returns_entry)
{
    ComponentEntry entry = makeComponent("thread", "role");
    DependencyEntry & added = entry.add_dependency_service(String("Server"));
    EXPECT_TRUE(added.mRoleName == String("Server"));
    EXPECT_EQ(0, entry.find_dependency_service(String("Server")));
}

TEST(ModelTest, component_entry_add_dependency_service_by_role_returns_existing)
{
    ComponentEntry entry = makeComponent("thread", "role");
    DependencyEntry & first = entry.add_dependency_service(String("Server"));
    DependencyEntry & again = entry.add_dependency_service(String("Server"));
    EXPECT_EQ(1u, entry.dependency_services().mListDependencies.size());
    EXPECT_EQ(&first, &again);
}

TEST(ModelTest, component_entry_add_dependency_service_skips_listed_role)
{
    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_dependency_service(DependencyEntry(String("A")));
    entry.add_dependency_service(DependencyEntry(String("A")));

    DependencyList list(DependencyEntry(String("A")));
    list.mListDependencies.add(DependencyEntry(String("B")));
    entry.add_dependency_service(list);

    EXPECT_EQ(2u, entry.dependency_services().mListDependencies.size());
    EXPECT_EQ(0, entry.find_dependency_service(String("A")));
    EXPECT_EQ(1, entry.find_dependency_service(String("B")));
}

TEST(ModelTest, component_entry_remove_dependency_service)
{
    ComponentEntry entry = makeComponent("thread", "role");
    entry.add_dependency_service(String("A"));
    entry.add_dependency_service(String("B"));

    EXPECT_FALSE(entry.remove_dependency_service(String()));
    EXPECT_FALSE(entry.remove_dependency_service(String("C")));
    EXPECT_TRUE(entry.remove_dependency_service(String("A")));
    EXPECT_EQ(INVALID_INDEX, entry.find_dependency_service(String("A")));
    EXPECT_EQ(0, entry.find_dependency_service(String("B")));
}

TEST(ModelTest, component_entry_data_pointer)
{
    ComponentEntry entry = makeComponent("thread", "role");
    EXPECT_FALSE(entry.has_data());
    EXPECT_FALSE(entry.data().has_value());

    int value = 5;
    entry.set_data(std::any(&value));
    EXPECT_TRUE(entry.has_data());
    EXPECT_EQ(&value, std::any_cast<int *>(entry.data()));

    entry.set_data(std::any(std::string("text")));
    EXPECT_EQ("text", std::any_cast<std::string>(entry.data()));

    entry.reset();
    EXPECT_FALSE(entry.has_data());
    EXPECT_FALSE(entry.data().has_value());
}

/************************************************************************
 * ComponentList
 ************************************************************************/

TEST(ModelTest, component_list_construct_and_find)
{
    ComponentList empty;
    EXPECT_FALSE(empty.is_valid());

    ComponentList fromInvalid{ComponentEntry()};
    EXPECT_FALSE(fromInvalid.is_valid());

    ComponentList list(makeComponent("thread", "A"));
    list.mListComponents.add(makeComponent("thread", "B"));
    EXPECT_TRUE(list.is_valid());
    EXPECT_EQ(0, list.find_component(String("A")));
    EXPECT_EQ(1, list.find_component(makeComponent("thread", "B")));
    EXPECT_EQ(INVALID_INDEX, list.find_component(String("C")));
    EXPECT_TRUE(list.component(String("B")).is_valid());
    EXPECT_FALSE(list.component(String("C")).is_valid());
    EXPECT_TRUE(list[1].mRoleName == String("B"));
    EXPECT_FALSE(list[2].is_valid());
}

TEST(ModelTest, component_list_data_set_and_reset)
{
    ComponentList list(makeComponent("thread", "A"));
    EXPECT_FALSE(list.set_component_data(String("none"), std::any(1)));
    EXPECT_FALSE(list.reset(String("none")));

    EXPECT_TRUE(list.set_component_data(String("A"), std::any(42)));
    EXPECT_TRUE(list.component(String("A")).has_data());
    EXPECT_EQ(42, std::any_cast<int>(list.component(String("A")).data()));

    EXPECT_TRUE(list.reset(String("A")));
    EXPECT_FALSE(list.component(String("A")).has_data());
}

TEST(ModelTest, component_list_copy_move_assign)
{
    ComponentList src(makeComponent("thread", "A"));
    ComponentList copied(src);
    EXPECT_EQ(0, copied.find_component(String("A")));
    ComponentList moved(std::move(copied));
    EXPECT_EQ(0, moved.find_component(String("A")));
    ComponentList assigned;
    assigned = src;
    EXPECT_EQ(0, assigned.find_component(String("A")));
    ComponentList moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(0, moveAssigned.find_component(String("A")));
    ComponentList & self = moveAssigned;
    moveAssigned = self;
    EXPECT_EQ(1u, moveAssigned.mListComponents.size());
}

/************************************************************************
 * ComponentThreadEntry
 ************************************************************************/

TEST(ModelTest, thread_entry_default_values)
{
    ComponentThreadEntry entry;
    EXPECT_FALSE(entry.is_valid());
    EXPECT_TRUE(entry.mThreadName.is_empty());
    EXPECT_EQ(WATCHDOG_IGNORE, entry.mWatchdogTimeout);
    EXPECT_EQ(DEFAULT_STACK_SIZE, entry.mStackSizeKB);
    EXPECT_EQ(IGNORE_VALUE, entry.mMaxQueue);
    EXPECT_TRUE(entry.mDropOnFull == Bool::Undefined);
    EXPECT_EQ(WAIT_INFINITE, entry.mQueueTimeout);
    EXPECT_EQ(LIFECYCLE_TIMEOUT_DEFAULT, entry.mLifecycleTimeout);
    EXPECT_TRUE(entry.mFailurePolicy == FailurePolicy::Exit);
}

TEST(ModelTest, thread_entry_construct_with_parameters)
{
    ComponentThreadEntry byName(String("thread"), 100u, 32u, 8u, Bool::False, 50u);
    EXPECT_TRUE(byName.mThreadName == String("thread"));
    EXPECT_EQ(100u, byName.mWatchdogTimeout);
    EXPECT_EQ(32u, byName.mStackSizeKB);
    EXPECT_EQ(8u, byName.mMaxQueue);
    EXPECT_TRUE(byName.mDropOnFull == Bool::False);
    EXPECT_EQ(50u, byName.mQueueTimeout);
    EXPECT_FALSE(byName.is_valid());

    ComponentThreadEntry byList(String("thread"), ComponentList(makeComponent("thread", "A")), 100u, 32u, 8u, Bool::True, 50u);
    EXPECT_TRUE(byList.is_valid());
    EXPECT_EQ(0, byList.find_component_entry(String("A")));
    EXPECT_EQ(100u, byList.mWatchdogTimeout);
    EXPECT_TRUE(byList.mDropOnFull == Bool::True);
}

TEST(ModelTest, thread_entry_validity_needs_name_and_component)
{
    ComponentThreadEntry noName;
    noName.add_component(makeComponent("", "A"));
    EXPECT_FALSE(noName.is_valid());

    ComponentThreadEntry noComponent{String("thread")};
    EXPECT_FALSE(noComponent.is_valid());

    EXPECT_TRUE(makeThread("thread", "A").is_valid());
}

TEST(ModelTest, thread_entry_copy_move_assign_compare)
{
    ComponentThreadEntry src = makeThread("thread", "A");
    ComponentThreadEntry copied(src);
    EXPECT_TRUE(copied == src);
    EXPECT_EQ(0, copied.find_component_entry(String("A")));
    ComponentThreadEntry moved(std::move(copied));
    EXPECT_TRUE(moved == src);
    EXPECT_EQ(0, moved.find_component_entry(String("A")));
    ComponentThreadEntry assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);
    ComponentThreadEntry moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);
    EXPECT_EQ(0, moveAssigned.find_component_entry(String("A")));
    ComponentThreadEntry & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);

    ComponentThreadEntry other = makeThread("other", "A");
    EXPECT_TRUE(src != other);
    EXPECT_FALSE(src == other);
    EXPECT_FALSE(src != src);
}

TEST(ModelTest, thread_entry_add_component_rejects_duplicate_role)
{
    ComponentThreadEntry entry{String("thread")};
    entry.add_component(makeComponent("thread", "A"));
    entry.add_component(makeComponent("thread", "A"));
    EXPECT_EQ(1u, entry.mComponents.mListComponents.size());

    ComponentList list(makeComponent("thread", "A"));
    list.mListComponents.add(makeComponent("thread", "B"));
    entry.add_component(list);
    EXPECT_EQ(2u, entry.mComponents.mListComponents.size());
    EXPECT_EQ(1, entry.find_component_entry(makeComponent("thread", "B")));
}

TEST(ModelTest, thread_entry_add_component_by_role_returns_existing)
{
    ComponentThreadEntry entry{String("thread")};
    ComponentEntry & first = entry.add_component(String("A"), createFunc(), deleteFunc());
    EXPECT_TRUE(first.is_valid());
    EXPECT_TRUE(first.mRoleName == String("A"));
    EXPECT_TRUE(first.mThreadName == String("thread"));

    ComponentEntry & again = entry.add_component(String("A"), createFunc(), deleteFunc());
    EXPECT_EQ(&first, &again);
    EXPECT_EQ(1u, entry.mComponents.mListComponents.size());
}

TEST(ModelTest, thread_entry_remove_component)
{
    ComponentThreadEntry entry = makeThread("thread", "A");
    EXPECT_FALSE(entry.remove_component(String()));
    EXPECT_FALSE(entry.remove_component(String("B")));
    EXPECT_TRUE(entry.remove_component(String("A")));
    EXPECT_EQ(INVALID_INDEX, entry.find_component_entry(String("A")));
    EXPECT_FALSE(entry.is_valid());
    EXPECT_FALSE(entry.remove_component(String("A")));
}

TEST(ModelTest, thread_entry_component_data)
{
    ComponentThreadEntry entry = makeThread("thread", "A");
    EXPECT_FALSE(entry.set_component_data(String("none"), std::any(1)));
    EXPECT_FALSE(entry.reset(String("none")));
    EXPECT_TRUE(entry.set_component_data(String("A"), std::any(11)));
    EXPECT_EQ(11, std::any_cast<int>(entry.mComponents.component(String("A")).data()));
    EXPECT_TRUE(entry.reset(String("A")));
    EXPECT_FALSE(entry.mComponents.component(String("A")).has_data());
}

/************************************************************************
 * ComponentThreadList
 ************************************************************************/

TEST(ModelTest, thread_list_construct_and_find)
{
    ComponentThreadList empty;
    EXPECT_FALSE(empty.is_valid());

    ComponentThreadList fromInvalid{ComponentThreadEntry(String("thread"))};
    EXPECT_FALSE(fromInvalid.is_valid());

    ComponentThreadList list(makeThread("T1", "A"));
    list.mListThreads.add(makeThread("T2", "B"));
    EXPECT_TRUE(list.is_valid());
    EXPECT_EQ(0, list.find_thread(String("T1")));
    EXPECT_EQ(1, list.find_thread(makeThread("T2", "X")));
    EXPECT_EQ(INVALID_INDEX, list.find_thread(String("T3")));
    EXPECT_TRUE(list.thread(String("T2")).is_valid());
    EXPECT_FALSE(list.thread(String("T3")).is_valid());
    EXPECT_TRUE(list[0].mThreadName == String("T1"));
    EXPECT_FALSE(list[2].is_valid());
}

TEST(ModelTest, thread_list_copy_move_assign)
{
    ComponentThreadList src(makeThread("T1", "A"));
    ComponentThreadList copied(src);
    EXPECT_EQ(0, copied.find_thread(String("T1")));
    ComponentThreadList moved(std::move(copied));
    EXPECT_EQ(0, moved.find_thread(String("T1")));
    ComponentThreadList assigned;
    assigned = src;
    EXPECT_EQ(0, assigned.find_thread(String("T1")));
    ComponentThreadList moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(0, moveAssigned.find_thread(String("T1")));
    ComponentThreadList & self = moveAssigned;
    moveAssigned = self;
    EXPECT_EQ(1u, moveAssigned.mListThreads.size());
}

/************************************************************************
 * Model
 ************************************************************************/

TEST(ModelTest, model_default_is_invalid_and_unloaded)
{
    Model model;
    EXPECT_FALSE(model.is_valid());
    EXPECT_TRUE(model.model_name().is_empty());
    EXPECT_TRUE(model.model_state() == ModelState::Unloaded);
    EXPECT_FALSE(model.is_model_loaded());
    EXPECT_FALSE(model.is_model_unloading());
    EXPECT_EQ(0, model.alive_duration());
    EXPECT_FALSE(model.thread_list().is_valid());
}

TEST(ModelTest, model_construct_with_name_and_threads)
{
    Model named(String("model"));
    EXPECT_TRUE(named.model_name() == String("model"));
    EXPECT_FALSE(named.is_valid());

    Model full(String("model"), ComponentThreadList(makeThread("T1", "A")));
    EXPECT_TRUE(full.is_valid());
    EXPECT_EQ(0, full.find_thread(String("T1")));

    Model noName(String(), ComponentThreadList(makeThread("T1", "A")));
    EXPECT_FALSE(noName.is_valid());
}

TEST(ModelTest, model_copy_move_assign_compare)
{
    Model src(String("model"), ComponentThreadList(makeThread("T1", "A")));

    Model copied(src);
    EXPECT_TRUE(copied == src);
    Model moved(std::move(copied));
    EXPECT_TRUE(moved == src);
    EXPECT_TRUE(moved.is_valid());
    Model assigned;
    assigned = src;
    EXPECT_TRUE(assigned == src);
    Model moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned == src);
    Model & self = moveAssigned;
    moveAssigned = self;
    EXPECT_TRUE(moveAssigned == src);
    EXPECT_FALSE(moveAssigned != src);

    Model otherName(String("other"), ComponentThreadList(makeThread("T1", "A")));
    Model otherThreads(String("model"), ComponentThreadList(makeThread("T2", "A")));
    EXPECT_TRUE(src != otherName);
    EXPECT_FALSE(src == otherName);
    EXPECT_TRUE(src != otherThreads);
    EXPECT_FALSE(src == otherThreads);
}

TEST(ModelTest, model_add_thread_entry_rejects_duplicate_and_invalid)
{
    Model model(String("model"));
    model.add_thread(makeThread("T1", "A"));
    model.add_thread(makeThread("T1", "B"));
    model.add_thread(ComponentThreadEntry(String("T2")));
    EXPECT_EQ(1u, model.thread_list().mListThreads.size());
    EXPECT_EQ(0, model.find_thread(String("T1")));
    EXPECT_EQ(INVALID_INDEX, model.find_thread(String("T2")));
    EXPECT_EQ(0, model.find_thread(makeThread("T1", "Z")));
    EXPECT_TRUE(model.is_valid());
}

TEST(ModelTest, model_add_thread_list)
{
    ComponentThreadList list(makeThread("T1", "A"));
    list.mListThreads.add(makeThread("T2", "B"));

    Model model(String("model"));
    model.add_thread(makeThread("T1", "C"));
    model.add_thread(list);
    EXPECT_EQ(2u, model.thread_list().mListThreads.size());
    EXPECT_EQ(1, model.find_thread(String("T2")));
}

TEST(ModelTest, model_add_thread_by_name_returns_existing)
{
    Model model(String("model"));
    ComponentThreadEntry & first = model.add_thread(String("T1"));
    EXPECT_TRUE(first.mThreadName == String("T1"));
    first.add_component(makeComponent("T1", "A"));

    ComponentThreadEntry & again = model.add_thread(String("T1"));
    EXPECT_EQ(&first, &again);
    EXPECT_EQ(1u, model.thread_list().mListThreads.size());
    EXPECT_EQ(0, again.find_component_entry(String("A")));
    EXPECT_TRUE(model.is_valid());
}

TEST(ModelTest, model_remove_thread)
{
    Model model(String("model"));
    model.add_thread(makeThread("T1", "A"));
    model.add_thread(makeThread("T2", "B"));

    EXPECT_FALSE(model.remove_thread(String()));
    EXPECT_FALSE(model.remove_thread(String("T3")));
    EXPECT_TRUE(model.remove_thread(String("T1")));
    EXPECT_EQ(INVALID_INDEX, model.find_thread(String("T1")));
    EXPECT_EQ(0, model.find_thread(String("T2")));
    EXPECT_FALSE(model.remove_thread(String("T1")));
}

TEST(ModelTest, model_find_component_across_threads)
{
    Model model(String("model"));
    model.add_thread(makeThread("T1", "A"));
    model.add_thread(makeThread("T2", "B"));

    EXPECT_TRUE(model.has_registered_component(String("A")));
    EXPECT_TRUE(model.has_registered_component(String("B")));
    EXPECT_FALSE(model.has_registered_component(String("C")));
    EXPECT_TRUE(model.has_registered_component(makeComponent("T2", "B")));
    EXPECT_FALSE(model.has_registered_component(makeComponent("T2", "C")));
    EXPECT_FALSE(Model().has_registered_component(String("A")));

    EXPECT_TRUE(model.remove_thread(String("T2")));
    EXPECT_FALSE(model.has_registered_component(String("B")));
}

TEST(ModelTest, model_component_data_across_threads)
{
    Model model(String("model"));
    model.add_thread(makeThread("T1", "A"));
    model.add_thread(makeThread("T2", "B"));

    EXPECT_FALSE(model.set_component_data(String("C"), std::any(1)));
    EXPECT_FALSE(model.reset(String("C")));

    EXPECT_TRUE(model.set_component_data(String("B"), std::any(5)));
    const ComponentEntry & entry = model.thread_list().thread(String("T2")).mComponents.component(String("B"));
    EXPECT_TRUE(entry.has_data());
    EXPECT_EQ(5, std::any_cast<int>(entry.data()));
    EXPECT_FALSE(model.thread_list().thread(String("T1")).mComponents.component(String("A")).has_data());

    EXPECT_TRUE(model.reset(String("B")));
    EXPECT_FALSE(entry.has_data());
}

/************************************************************************
 * Invalid objects
 ************************************************************************/

TEST(ModelTest, invalid_objects_are_invalid_and_stable)
{
    EXPECT_FALSE(invalid_service_entry().is_valid());
    EXPECT_FALSE(invalid_service_list().is_valid());
    EXPECT_FALSE(invalid_worker_entry().is_valid());
    EXPECT_FALSE(invalid_worker_list().is_valid());
    EXPECT_FALSE(invalid_depedency_entry().is_valid());
    EXPECT_FALSE(invalid_depedency_list().is_valid());
    EXPECT_FALSE(invalid_component_entry().is_valid());
    EXPECT_FALSE(invalid_component_list().is_valid());
    EXPECT_FALSE(invalid_thread_entry().is_valid());
    EXPECT_FALSE(invalid_thread_list().is_valid());
    EXPECT_FALSE(invalid_model().is_valid());
    EXPECT_EQ(&invalid_model(), &invalid_model());
    EXPECT_EQ(&invalid_thread_entry(), &invalid_thread_entry());
}
