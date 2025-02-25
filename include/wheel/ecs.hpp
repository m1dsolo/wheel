// TODO: destroy
#pragma once

#include <any>
#include <vector>
#include <cstdint>
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
using EntityTemplate = std::unordered_map<ComponentID, std::any>;

inline constexpr Entity EntityNone = -1;

class EntityGenerator {
public:
    EntityGenerator() = default;

    static Entity generate();
    static Entity next_entity() { return next_entity_; }
    static void set_next_entity(Entity entity) { next_entity_ = entity; }

private:
    static inline Entity next_entity_ = 0;
};

struct ComponentContainer {
public:
    explicit ComponentContainer(std::type_index component_id);

    template <typename ComponentType>
    void add(ComponentType&& component, Entity entity);

    bool del(size_t idx);

    auto begin();
    auto end();
    size_t size() const;

    std::vector<std::any> components;
    std::vector<Entity> invert_index;

    std::type_index component_id;
};

class ECS : public Singleton<ECS> {
    friend class Singleton<ECS>;
    friend class ComponentContainer;

public:
    void startup() const;
    void update();
    void shutdown();

    Entity add_entity(EntityTemplate& component_map);

    template <typename... ComponentTypes>
    Entity add_entity(ComponentTypes&&... components);

    template <typename... ComponentTypes>
    Entity add_entity(Entity entity, ComponentTypes&&... components);

    void del_entity(Entity entity);
    bool has_entity(Entity entity) const;

    template <typename... ComponentTypes>
    std::vector<Entity> get_entities() const;

    template <typename... ComponentTypes>
    Entity get_entity() const;

    size_t count_entities() const;

    template <typename ComponentType>
    void add_component(Entity entity, ComponentType&& component);

    template <typename... ComponentTypes>
    void add_components(Entity entity, ComponentTypes&&... components);

    template <typename ComponentType>
    void del_component();

    template <typename ComponentType>
    void del_component(Entity entity);

    void del_component(Entity entity, ComponentID component_id);

    template <typename... ComponentTypes>
    void del_components();

    template <typename... ComponentTypes>
    void del_components(Entity entity);

    template <typename... ComponentIDs> requires (std::is_convertible_v<ComponentIDs, ComponentID> && ...)
    void del_components(Entity entity, ComponentIDs&&... component_ids);

    template <typename ComponentType>
    bool has_component(Entity entity) const;

    template <typename ComponentType>
    bool has_component() const;

    template <typename... ComponentTypes>
    bool has_components(Entity entity) const;

    template <typename... ComponentTypes>
    bool has_components() const;

    template <typename ComponentType>
    ComponentType& get_component();

    template <typename ComponentType>
    ComponentType& get_component(Entity entity);

    template <typename... ComponentTypes>
    auto get_components();

    template <typename... ComponentTypes>
    auto get_components(Entity entity);

    template <typename... ComponentTypes>
    auto get_entity_and_components();

    void add_startup_system(const System& system);
    void add_system(const System& system);
    void add_shutdown_system(const System& system);
    
    void clear_entities();
    void clear_systems();

private:
    template <typename ComponentType>
    void add_component(Entity entity, const std::type_index& component_id, ComponentType&& component);

    ECS() = default;
    ~ECS() = default;
    ECS(const ECS&) = delete;

    std::unordered_map<Entity, std::unordered_map<ComponentID, size_t>> entity2components_;
    std::unordered_map<ComponentID, SparseSet<Entity>> component2entities_;
    std::unordered_map<ComponentID, ComponentContainer> component2containers_;

    std::vector<System> startup_systems_;
    std::vector<System> systems_;
    std::vector<System> shutdown_systems_;
};

inline Entity EntityGenerator::generate() {
    return next_entity_++;
}

inline ComponentContainer::ComponentContainer(std::type_index component_id) : component_id(component_id) {}

template <typename ComponentType>
inline void ComponentContainer::add(ComponentType&& component, Entity entity) {
    components.emplace_back(std::forward<ComponentType>(component));
    invert_index.emplace_back(std::move(entity));
}

inline auto ComponentContainer::begin() { return components.begin(); }
inline auto ComponentContainer::end() { return components.end(); }
inline size_t ComponentContainer::size() const { return components.size(); }

inline void ECS::startup() const {
    for (auto& system : startup_systems_) {
        system();
    }
}

inline void ECS::update() {
    for (auto& system : systems_) {
        system();
    }
}

inline void ECS::shutdown() {
    for (auto& system : shutdown_systems_) {
        system();
    }
    clear_systems();
    clear_entities();
}

inline Entity ECS::add_entity(EntityTemplate& component_map) {
    Entity entity = EntityGenerator::generate();
    for (auto& [component_id, component] : component_map) {
        add_component(entity, component_id, component);
    }
    return entity;
}

template <typename... ComponentTypes>
inline Entity ECS::add_entity(ComponentTypes&&... components) {
    return add_entity(EntityGenerator::generate(), std::forward<ComponentTypes>(components)...);
}

template <typename... ComponentTypes>
Entity ECS::add_entity(Entity entity, ComponentTypes&&... components) {
    EntityGenerator::set_next_entity(std::max(EntityGenerator::next_entity(), entity + 1));
    if constexpr (sizeof...(ComponentTypes) > 0) {
        add_components(entity, std::forward<ComponentTypes>(components)...);
    }
    return entity;
}

inline void ECS::del_entity(Entity entity) {
    if (!has_entity(entity)) return;

    for (auto& [component_id, idx] : entity2components_.at(entity)) {
        component2entities_.at(component_id).del(entity);
        component2containers_.at(component_id).del(idx);
    }
    entity2components_.erase(entity);
}

inline bool ECS::has_entity(Entity entity) const {
    return entity2components_.count(entity);
}

template <typename... ComponentTypes>
inline std::vector<Entity> ECS::get_entities() const {
    std::vector<Entity> res;
    if constexpr (sizeof...(ComponentTypes) == 0) {
        for (const auto& [entity, _] : entity2components_) {
            res.emplace_back(entity);
        }
    } else {
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
    }

    return res;
}

template <typename... ComponentTypes>
inline Entity ECS::get_entity() const {
    auto entities = get_entities<ComponentTypes...>();
    if (entities.size() > 0) {
        return entities[0];
    }
    return EntityNone;
}

inline size_t ECS::count_entities() const {
    return entity2components_.size();
}

template <typename ComponentType>
inline void ECS::add_component(Entity entity, ComponentType&& component) {
    add_component(entity, typeid(ComponentType), std::forward<ComponentType>(component));
}

template <typename... ComponentTypes>
inline void ECS::add_components(Entity entity, ComponentTypes&&... components) {
    auto components_tuple = std::make_tuple(std::forward<ComponentTypes>(components)...);
    std::apply([this, entity](auto&&... component) {
        (add_component(entity, std::forward<ComponentTypes>(component)), ...);
    }, components_tuple);
}

template <typename ComponentType>
inline void ECS::del_component() {
    for (auto& entity : get_entities<ComponentType>()) {
        del_component<ComponentType>(entity);
    }
}

template <typename ComponentType>
inline void ECS::del_component(Entity entity) {
    return del_component(entity, typeid(ComponentType));
}

inline void ECS::del_component(Entity entity, ComponentID component_id) {
    size_t idx = entity2components_.at(entity).at(component_id);
    component2entities_.at(component_id).del(entity);
    component2containers_.at(component_id).del(idx);
    entity2components_.at(entity).erase(component_id);
}

template <typename... ComponentTypes>
inline void ECS::del_components() {
    (del_component<ComponentTypes>(), ...);
}

template <typename... ComponentTypes>
inline void ECS::del_components(Entity entity) {
    (del_component<ComponentTypes>(entity), ...);
}

template <typename... ComponentIDs> requires (std::is_convertible_v<ComponentIDs, ComponentID> && ...)
inline void ECS::del_components(Entity entity, ComponentIDs&&... component_ids) {
    (del_component(entity, std::forward<ComponentIDs>(component_ids)), ...);
}

template <typename ComponentType>
inline bool ECS::has_component(Entity entity) const {
    return has_components<ComponentType>(entity);
}

template <typename ComponentType>
inline bool ECS::has_component() const {
    return has_components<ComponentType>();
}

template <typename... ComponentTypes>
inline bool ECS::has_components(Entity entity) const {
    return entity2components_.count(entity) && ((entity2components_.at(entity).contains(typeid(ComponentTypes)) && ...));
}

template <typename... ComponentTypes>
inline bool ECS::has_components() const {
    return ((component2containers_.count(typeid(ComponentTypes)) && component2containers_.at(typeid(ComponentTypes)).size() > 0) && ...);
}

template <typename ComponentType>
inline ComponentType& ECS::get_component() {
    return std::any_cast<ComponentType&>(component2containers_.at(typeid(ComponentType)).components.at(0));
}

template <typename ComponentType>
inline ComponentType& ECS::get_component(Entity entity) {
    size_t idx = entity2components_.at(entity).at(typeid(ComponentType));
    return std::any_cast<ComponentType&>(component2containers_.at(typeid(ComponentType)).components.at(idx));
}

template <typename... ComponentTypes>
inline auto ECS::get_components() {
    auto entities = get_entities<ComponentTypes...>();
    return std::views::zip(
        std::vector<Entity>(entities) |
        std::views::transform( [&](Entity entity) -> std::any& {
            size_t idx = entity2components_.at(entity).at(typeid(ComponentTypes));
            return component2containers_.at(typeid(ComponentTypes)).components.at(idx);
        }) |
        std::views::transform( [](std::any& component) -> ComponentTypes& { return std::any_cast<ComponentTypes&>(component) ; })
    ...);
}

template <typename... ComponentTypes>
inline auto ECS::get_components(Entity entity) {
    return std::make_tuple(std::ref(get_component<ComponentTypes>(entity))...);
}

template <typename... ComponentTypes>
inline auto ECS::get_entity_and_components() {
    auto entities = get_entities<ComponentTypes...>();
    return std::views::zip(
        std::vector<Entity>(entities),
        std::vector<Entity>(entities) |
        std::views::transform( [&](Entity entity) -> std::any& {
            size_t idx = entity2components_.at(entity).at(typeid(ComponentTypes));
            return component2containers_.at(typeid(ComponentTypes)).components.at(idx);
        }) |
        std::views::transform( [](std::any& component) -> ComponentTypes& { return std::any_cast<ComponentTypes&>(component) ; })
    ...);
}

inline void ECS::add_startup_system(const System& system) {
    startup_systems_.emplace_back(system);
}

inline void ECS::add_system(const System& system) {
    systems_.emplace_back(system);
}

inline void ECS::add_shutdown_system(const System& system) {
    shutdown_systems_.emplace_back(system);
}

inline void ECS::clear_entities() {
    entity2components_.clear();
    component2entities_.clear();
    component2containers_.clear();
    EntityGenerator::set_next_entity(0);
}

inline void ECS::clear_systems() {
    startup_systems_.clear();
    systems_.clear();
    shutdown_systems_.clear();
}

template <typename ComponentType>
inline void ECS::add_component(Entity entity, const std::type_index& component_id, ComponentType&& component) {
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
