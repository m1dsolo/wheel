// TODO: destroy
#pragma once

#include <any>
#include <vector>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <ranges>

#include <wheel/singleton.hpp>
#include <wheel/sparse_set.hpp>
#include <wheel/utils.hpp>

namespace wheel {

using Entity = uint32_t;
using ComponentID = std::type_index;
using System = std::function<void()>;
using ResourceID = std::type_index;
using EventID = std::type_index;
using EntityTemplate = std::unordered_map<ComponentID, std::any>;

inline constexpr Entity EntityNone = -1;

class EntityGenerator {
public:
    EntityGenerator() = default;
    static Entity generate() {
        return next_entity_++;
    }

private:
    static inline Entity next_entity_ = 0;
};

struct ComponentContainer {
public:
    explicit ComponentContainer(std::type_index component_id) : component_id(component_id) {}

    template <typename ComponentType>
    void add(ComponentType&& component, Entity entity) {
        components.emplace_back(std::forward<ComponentType>(component));
        invert_index.emplace_back(std::move(entity));
    }

    bool del(size_t idx);

    auto begin() { return components.begin(); }
    auto end() { return components.end(); }
    size_t size() const { return components.size(); }

    std::vector<std::any> components;
    std::vector<Entity> invert_index;

    std::type_index component_id;
};


class ECS : public Singleton<ECS> {
    friend class Singleton<ECS>;
    friend class ComponentContainer;

public:
    void startup() const {
        for (auto& system : startup_systems_) {
            system();
        }
    }

    void update() {
        for (auto& system : systems_) {
            system();
        }
        cur_event_map_ = std::move(next_event_map_);
    }

    // TODO: check
    void shutdown() {
        for (auto& system : shutdown_systems_) {
            system();
        }

        entity2components_.clear();
        component2entities_.clear();
        component2containers_.clear();
        cur_event_map_.clear();
        next_event_map_.clear();

        startup_systems_.clear();
        systems_.clear();
        shutdown_systems_.clear();

        resource_map_.clear();
    }

    Entity add_entity(EntityTemplate& component_map) {
        Entity entity = EntityGenerator::generate();
        for (auto& [component_id, component] : component_map) {
            add_component(entity, component_id, component);
        }
        return entity;
    }

    template <typename... ComponentTypes>
    Entity add_entity(ComponentTypes&&... components) {
        Entity entity = EntityGenerator::generate();
        add_components(entity, std::forward<ComponentTypes>(components)...);

        return entity;
    }

    void del_entity(Entity entity) {
        if (!has_entity(entity)) return;

        for (auto& [component_id, idx] : entity2components_.at(entity)) {
            component2entities_.at(component_id).del(entity);
            component2containers_.at(component_id).del(idx);
        }
        entity2components_.erase(entity);
    }

    bool has_entity(Entity entity) const {
        return entity2components_.count(entity);
    }

    template <typename... ComponentTypes>
    std::vector<Entity> get_entities() const {
        std::vector<Entity> res;
        bool first = true;
        for (const ComponentID& component_id : {static_cast<ComponentID>(typeid(ComponentTypes))...}) {
            std::vector<Entity> null_entities;
            std::vector<Entity>& entities = null_entities;
            if (component2entities_.count(component_id)) {
                entities = component2entities_.at(component_id).entities();
            }
            if (first) {
                res = entities;
                first = false;
            } else {
                res = Utils::intersection(res, entities);
            }
        }

        return res;
    }

    size_t count_entities() const {
        return entity2components_.size();
    }

    template <typename... ComponentTypes>
    void add_components(Entity entity, ComponentTypes&&... components) {
        auto components_tuple = std::make_tuple(std::forward<ComponentTypes>(components)...);
        std::apply([this, entity](auto&&... component) {
            (add_component(entity, std::forward<ComponentTypes>(component)), ...);
        }, components_tuple);
    }

    template <typename... ComponentTypes>
    void del_components() {
        (del_component<ComponentTypes>(), ...);
    }

    template <typename... ComponentTypes>
    void del_components(Entity entity) {
        (del_component<ComponentTypes>(entity), ...);
    }

    template <typename... ComponentIDs> requires (std::is_convertible_v<ComponentIDs, ComponentID> && ...)
    void del_components(Entity entity, ComponentIDs&&... component_ids) {
        (del_component(entity, std::forward<ComponentIDs>(component_ids)), ...);
    }

    template <typename... ComponentTypes>
    bool has_components(Entity entity) const {
        return entity2components_.count(entity) && ((entity2components_.at(entity).contains(typeid(ComponentTypes)) && ...));
    }

    template <typename... ComponentTypes>
    bool has_components() const {
        return ((component2containers_.count(typeid(ComponentTypes)) && component2containers_.at(typeid(ComponentTypes)).size() > 0) && ...);
    }

    // return the first component
    template <typename ComponentType>
    ComponentType& get_component() {
        return std::any_cast<ComponentType&>(component2containers_.at(typeid(ComponentType)).components.at(0));
    }

    template <typename ComponentType>
    ComponentType& get_component(Entity entity) {
        size_t idx = entity2components_.at(entity).at(typeid(ComponentType));
        return std::any_cast<ComponentType&>(component2containers_.at(typeid(ComponentType)).components.at(idx));
    }

    template <typename... ComponentTypes>
    auto get_components() {
        auto entities = get_entities<ComponentTypes...>();
        return std::views::zip(
            // copy constructor(not good)
            std::vector<Entity>(entities) |
            std::views::transform([&](Entity entity) -> std::any& {
                size_t idx = entity2components_.at(entity).at(typeid(ComponentTypes));
                return component2containers_.at(typeid(ComponentTypes)).components.at(idx);
            }) |
            std::views::transform([](std::any& component) -> ComponentTypes& { return std::any_cast<ComponentTypes&>(component); })
        ...);
    }

    template <typename... ComponentTypes>
    auto get_entity_and_components() {
        auto entities = get_entities<ComponentTypes...>();
        return std::views::zip(
            // copy constructor(not good)
            std::vector<Entity>(entities),
            std::vector<Entity>(entities) |
            std::views::transform([&](Entity entity) -> std::any& {
                size_t idx = entity2components_.at(entity).at(typeid(ComponentTypes));
                return component2containers_.at(typeid(ComponentTypes)).components.at(idx);
            }) |
            std::views::transform([](std::any& component) -> ComponentTypes& { return std::any_cast<ComponentTypes&>(component); })
        ...);
    }

    void add_startup_system(const System& system) {
        startup_systems_.emplace_back(system);
    }

    void add_system(const System& system) {
        systems_.emplace_back(system);
    }

    void add_shutdown_system(const System& system) {
        shutdown_systems_.emplace_back(system);
    }

    template <typename ResourceType, typename... Args>
    void add_resource(Args&&... args) {
        resource_map_[typeid(ResourceType)] = std::make_shared<ResourceType>(std::forward<Args>(args)...);
    }

    template <typename ResourceType>
    void del_resource() {
        resource_map_.erase(typeid(ResourceType));
    }

    template <typename ResourceType>
    bool has_resource() const {
        return resource_map_.count(typeid(ResourceType));
    }

    template <typename ResourceType>
    ResourceType& get_resource() {
        return *std::any_cast<std::shared_ptr<ResourceType>>(resource_map_.at(typeid(ResourceType)));
    }

    template <typename EventType>
    void add_event(EventType&& event) {
        cur_event_map_[typeid(EventType)].emplace_back(std::forward<EventType>(event));
    }

    template <typename EventType, typename... Args>
    void emplace_event(Args&&... args) {
        cur_event_map_[typeid(EventType)].emplace_back(EventType(std::forward<Args>(args)...));
    }

    // event will be processed in next frame
    template <typename EventType, typename... Args>
    void emplace_next_event(Args&&... args) {
        next_event_map_[typeid(EventType)].emplace_back(EventType(std::forward<Args>(args)...));
    }

    template <typename EventType>
    bool has_event() const {
        return cur_event_map_.count(typeid(EventType));
    }

    template <typename EventType>
    auto get_events() {
        // copy now(to prevent modify event to cause problem)
        return std::vector<std::any>(cur_event_map_[typeid(EventType)]) |
        // return cur_event_map_[typeid(EventType)] |
            std::views::transform([](std::any& event) -> EventType& { return std::any_cast<EventType&>(event); });
    }

private:
    template <typename ComponentType>
    void add_component(Entity entity, ComponentType&& component) {
        return add_component(entity, typeid(ComponentType), std::forward<ComponentType>(component));
    }

    template <typename ComponentType>
    void add_component(Entity entity, const std::type_index& component_id, ComponentType&& component) {
        if (entity2components_[entity].count(component_id)) {
            return;
        }

        component2entities_[component_id].add(entity);
        if (!component2containers_.count(component_id)) {
            component2containers_.emplace(component_id, ComponentContainer(component_id));
        }
        auto& component_container = component2containers_.at(component_id);
        component_container.add(std::forward<ComponentType>(component), entity);
        entity2components_[entity][component_id] = component_container.size() - 1;
    }

    template <typename ComponentType>
    void del_component() {
        for (auto& entity : get_entities<ComponentType>()) {
            del_component<ComponentType>(entity);
        }
    }

    template <typename ComponentType>
    void del_component(Entity entity) {
        return del_component(entity, typeid(ComponentType));
    }

    void del_component(Entity entity, ComponentID component_id) {
        size_t idx = entity2components_.at(entity).at(component_id);
        component2entities_.at(component_id).del(entity);
        component2containers_.at(component_id).del(idx);
        entity2components_.at(entity).erase(component_id);
    }

    ECS() = default;
    ~ECS() = default;
    ECS(const ECS&) = delete;

    // std::unordered_map<Entity, std::unordered_map<ComponentID, std::any>> entity2components_;
    // std::unordered_map<ComponentID, SparseSet<Entity>> component2entities_;
    // std::unordered_map<ComponentID, std::unordered_set<const void*>> component_map_;
    std::unordered_map<Entity, std::unordered_map<ComponentID, size_t>> entity2components_;
    std::unordered_map<ComponentID, SparseSet<Entity>> component2entities_;
    std::unordered_map<ComponentID, ComponentContainer> component2containers_;
    std::unordered_map<EventID, std::vector<std::any>> cur_event_map_, next_event_map_;

    std::vector<System> startup_systems_;
    std::vector<System> systems_;
    std::vector<System> shutdown_systems_;

    std::unordered_map<ResourceID, std::any> resource_map_;
};

inline bool ComponentContainer::del(size_t idx) {
    if (idx < components.size()) {
        auto& entity2components = ECS::instance().entity2components_;
        if (idx < components.size() - 1) {
            components[idx] = std::move(components.back());
            invert_index[idx] = std::move(invert_index.back());

            entity2components.at(invert_index[idx]).at(component_id) = idx;
        }
        components.pop_back();
        invert_index.pop_back();

        return true;
    }
    return false;
}

}  // namespace wheel
