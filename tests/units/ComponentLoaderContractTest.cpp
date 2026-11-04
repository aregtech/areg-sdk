/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include <any>
#include <string>
#include <vector>

namespace
{
    using namespace areg;

    Component* create_registry_component(const ComponentEntry& entry, ComponentThread& owner)
    {
        return new Component(entry, owner);
    }

    void delete_registry_component(Component& component, const ComponentEntry&)
    {
        delete &component;
    }

    Model registry_model(const String& name, const String& thread, const String& role)
    {
        ComponentThreadEntry threadEntry(thread);
        threadEntry.add_component(ComponentEntry(thread, role, create_registry_component, delete_registry_component));
        Model model(name);
        model.add_thread(threadEntry);
        return model;
    }

    Model create_registry_model(const char* name)
    {
        return registry_model(String(name), String("LoaderCreatorThread"), String("LoaderCreatorRole"));
    }

    class ComponentLoaderContractTest : public testing::Test
    {
    protected:
        ComponentLoaderContractTest()
            : mModels()
        {
        }

        ~ComponentLoaderContractTest() override
        {
            for (const String& name : mModels)
                ComponentLoader::remove_component_model(name);
        }

        bool add(const Model& model)
        {
            const bool added = ComponentLoader::add_model_unique(model);
            if (added)
                mModels.push_back(model.model_name());
            return added;
        }

        std::vector<String> mModels;
    };
}

TEST_F(ComponentLoaderContractTest, unknown_and_empty_names_have_no_registry_or_lifecycle_state)
{
    for (const String& name : { String(), String("LoaderContractUnknown") })
    {
        EXPECT_FALSE(ComponentLoader::exist(name));
        EXPECT_FALSE(ComponentLoader::is_model_loaded(name));
        EXPECT_EQ(ModelState::Unloaded, ComponentLoader::model_state(name));
        EXPECT_FALSE(ComponentLoader::find_model(name).is_valid());
        EXPECT_FALSE(ComponentLoader::find_thread_entry(name).is_valid());
        EXPECT_FALSE(ComponentLoader::find_component_entry(name).is_valid());
        EXPECT_FALSE(ComponentLoader::find_component_list(name).is_valid());
        EXPECT_FALSE(ComponentLoader::find_component_entry(name, name).is_valid());
        EXPECT_FALSE(ComponentLoader::set_component_data(name, 19));
        EXPECT_FALSE(ComponentLoader::reset(name));
    }
    ComponentLoader::remove_component_model(String("LoaderContractUnknown"));
    EXPECT_FALSE(ComponentLoader::exist(String("LoaderContractUnknown")));
}

TEST_F(ComponentLoaderContractTest, registered_models_are_copied_and_named_lookups_find_later_entries)
{
    Model first = registry_model(String("LoaderContractFirst"), String("LoaderContractThreadA"), String("LoaderContractRoleA"));
    Model second = registry_model(String("LoaderContractSecond"), String("LoaderContractThreadB"), String("LoaderContractRoleB"));
    ASSERT_TRUE(add(first));
    ASSERT_TRUE(add(second));
    ASSERT_TRUE(second.set_component_data(String("LoaderContractRoleB"), 99));
    EXPECT_TRUE(ComponentLoader::exist(first.model_name()));
    EXPECT_TRUE(ComponentLoader::exist(second.model_name()));
    EXPECT_FALSE(ComponentLoader::is_model_loaded(first.model_name()));
    EXPECT_EQ(ModelState::Unloaded, ComponentLoader::model_state(second.model_name()));
    const Model& stored = ComponentLoader::find_model(second.model_name());
    EXPECT_TRUE(stored.is_valid());
    EXPECT_EQ(second.model_name(), stored.model_name());
    EXPECT_EQ(0, stored.find_thread(String("LoaderContractThreadB")));
    const String threadName("LoaderContractThreadB");
    const ComponentThreadEntry& thread = ComponentLoader::find_thread_entry(threadName);
    ASSERT_TRUE(thread.is_valid());
    EXPECT_EQ(String("LoaderContractThreadB"), thread.mThreadName);
    const String roleName("LoaderContractRoleB");
    const ComponentEntry& component = ComponentLoader::find_component_entry(roleName);
    ASSERT_TRUE(component.is_valid());
    EXPECT_EQ(String("LoaderContractThreadB"), component.mThreadName);
    EXPECT_FALSE(component.has_data());
    EXPECT_FALSE(ComponentLoader::find_component_entry(String("LoaderContractMissingRole")).is_valid());
    EXPECT_FALSE(ComponentLoader::find_thread_entry(String("LoaderContractMissingThread")).is_valid());
}

TEST_F(ComponentLoaderContractTest, component_data_mutation_and_reset_are_isolated_by_role)
{
    ASSERT_TRUE(add(registry_model(String("LoaderDataFirst"), String("LoaderDataThreadA"), String("LoaderDataRoleA"))));
    ASSERT_TRUE(add(registry_model(String("LoaderDataSecond"), String("LoaderDataThreadB"), String("LoaderDataRoleB"))));
    ASSERT_TRUE(ComponentLoader::set_component_data(String("LoaderDataRoleA"), std::string("first")));
    ASSERT_TRUE(ComponentLoader::set_component_data(String("LoaderDataRoleB"), 29));
    const auto first = ComponentLoader::find_component_entry(String("LoaderDataRoleA")).data();
    const auto second = ComponentLoader::find_component_entry(String("LoaderDataRoleB")).data();
    const auto* text = std::any_cast<std::string>(&first);
    const auto* number = std::any_cast<int>(&second);
    ASSERT_NE(nullptr, text);
    ASSERT_NE(nullptr, number);
    EXPECT_EQ("first", *text);
    EXPECT_EQ(29, *number);
    ASSERT_TRUE(ComponentLoader::reset(String("LoaderDataRoleB")));
    EXPECT_FALSE(ComponentLoader::find_component_entry(String("LoaderDataRoleB")).has_data());
    EXPECT_TRUE(ComponentLoader::find_component_entry(String("LoaderDataRoleA")).has_data());
    EXPECT_TRUE(ComponentLoader::reset(String("LoaderDataRoleB")));
    EXPECT_FALSE(ComponentLoader::reset(String("LoaderDataMissingRole")));
}

TEST_F(ComponentLoaderContractTest, duplicate_role_in_another_model_is_rejected_without_partial_registration)
{
    ASSERT_TRUE(add(registry_model(String("LoaderDuplicateFirst"), String("LoaderDuplicateThreadA"), String("LoaderDuplicateRole"))));
    const Model duplicate = registry_model(String("LoaderDuplicateSecond"), String("LoaderDuplicateThreadB"), String("LoaderDuplicateRole"));
    EXPECT_FALSE(ComponentLoader::add_model_unique(duplicate));
    EXPECT_FALSE(ComponentLoader::exist(duplicate.model_name()));
    EXPECT_FALSE(ComponentLoader::find_thread_entry(String("LoaderDuplicateThreadB")).is_valid());
    EXPECT_EQ(String("LoaderDuplicateThreadA"), ComponentLoader::find_component_entry(String("LoaderDuplicateRole")).mThreadName);
    EXPECT_TRUE(ComponentLoader::exist(String("LoaderDuplicateFirst")));
}

TEST_F(ComponentLoaderContractTest, removing_one_model_preserves_others_and_allows_its_names_to_be_reused)
{
    const Model first = registry_model(String("LoaderRemoveFirst"), String("LoaderRemoveThreadA"), String("LoaderRemoveRoleA"));
    const Model second = registry_model(String("LoaderRemoveSecond"), String("LoaderRemoveThreadB"), String("LoaderRemoveRoleB"));
    ASSERT_TRUE(add(first));
    ASSERT_TRUE(add(second));
    ComponentLoader::remove_component_model(first.model_name());
    ComponentLoader::remove_component_model(first.model_name());
    EXPECT_FALSE(ComponentLoader::exist(first.model_name()));
    EXPECT_FALSE(ComponentLoader::find_component_entry(String("LoaderRemoveRoleA")).is_valid());
    EXPECT_FALSE(ComponentLoader::find_thread_entry(String("LoaderRemoveThreadA")).is_valid());
    EXPECT_TRUE(ComponentLoader::exist(second.model_name()));
    ASSERT_TRUE(ComponentLoader::add_model_unique(first));
    EXPECT_TRUE(ComponentLoader::exist(first.model_name()));
}

TEST_F(ComponentLoaderContractTest, model_data_creator_registers_function_and_preconstructed_models)
{
    mModels.push_back(String("LoaderCreatorFunction"));
    ModelDataCreator fromFunction(create_registry_model, "LoaderCreatorFunction");
    EXPECT_TRUE(ComponentLoader::exist(String("LoaderCreatorFunction")));
    EXPECT_TRUE(ComponentLoader::find_component_entry(String("LoaderCreatorRole")).is_valid());
    const Model prebuilt = registry_model(String("LoaderCreatorPrebuilt"), String("LoaderCreatorOtherThread"), String("LoaderCreatorOtherRole"));
    mModels.push_back(prebuilt.model_name());
    ModelDataCreator fromModel(prebuilt);
    EXPECT_TRUE(ComponentLoader::exist(prebuilt.model_name()));
    EXPECT_EQ(prebuilt.model_name(), ComponentLoader::find_model(prebuilt.model_name()).model_name());
}
