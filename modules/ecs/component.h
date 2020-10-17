#pragma once

/* Author: AndreaCatania */

#include "core/oa_hash_map.h"
#include "core/object.h"
#include "ecs.h"
#include "storages/dense_vector.h"

struct SetMethodHandleBase {
	virtual ~SetMethodHandleBase() {}
	virtual void set(void *p_object, const Variant &p_data) = 0;
};

template <class C, class T>
struct SetMethodHandle : public SetMethodHandleBase {
	T method;

	virtual void set(void *p_object, const Variant &p_data) override {
		(static_cast<C *>(p_object)->*method)(p_data);
	}
};

struct GetMethodHandleBase {
	virtual ~GetMethodHandleBase() {}
	virtual Variant get(const void *p_object) = 0;
};

template <class C, class T>
struct GetMethodHandle : public GetMethodHandleBase {
	T method;

	virtual Variant get(const void *p_object) override {
		return (static_cast<const C *>(p_object)->*method)();
	}
};

#define COMPONENT(m_class, m_storage_class)                                                                            \
	ECSCLASS(m_class)                                                                                                  \
	friend class Pipeline;                                                                                             \
	friend class Component;                                                                                            \
																													   \
private:                                                                                                               \
	/* Storages */                                                                                                     \
	static _FORCE_INLINE_ m_storage_class<m_class> *create_storage() {                                                 \
		return memnew(m_storage_class<m_class>);                                                                       \
	}                                                                                                                  \
	static _FORCE_INLINE_ void destroy_storage(m_storage_class<m_class> *p_storage) {                                  \
		memdelete(p_storage);                                                                                          \
	}                                                                                                                  \
																													   \
	/* Components */                                                                                                   \
	static inline uint32_t component_id = UINT32_MAX;                                                                  \
																													   \
	static void add_component_by_name(Pipeline *p_pipeline, EntityID entity_id, const Variant &p_data) {               \
		m_class component;                                                                                             \
		Dictionary dic = p_data;                                                                                       \
		for (const Variant *key = dic.next(nullptr); key != nullptr; key = dic.next(key)) {                            \
			component.set(*key, dic.get_valid(*key));                                                                  \
		}                                                                                                              \
		p_pipeline->add_component(                                                                                     \
				entity_id,                                                                                             \
				component);                                                                                            \
	}                                                                                                                  \
																													   \
public:                                                                                                                \
	static uint32_t get_component_id() { return component_id; }                                                        \
																													   \
	/* Properties */                                                                                                   \
private:                                                                                                               \
	static inline OAHashMap<StringName, PropertyInfo> property_map;                                                    \
	static inline OAHashMap<StringName, SetMethodHandleBase *> set_map;                                                \
	static inline OAHashMap<StringName, GetMethodHandleBase *> get_map;                                                \
	template <class M1, class M2>                                                                                      \
	static void add_property(const PropertyInfo &p_info, M1 p_set, M2 p_get) {                                         \
		property_map.insert(p_info.name, p_info);                                                                      \
																													   \
		SetMethodHandle<m_class, M1> *handle_set = new SetMethodHandle<m_class, M1>;                                   \
		handle_set->method = p_set;                                                                                    \
		set_map.insert(p_info.name, handle_set);                                                                       \
																													   \
		GetMethodHandle<m_class, M2> *handle_get = new GetMethodHandle<m_class, M2>;                                   \
		handle_get->method = p_get;                                                                                    \
		get_map.insert(p_info.name, handle_get);                                                                       \
	}                                                                                                                  \
	static OAHashMap<StringName, PropertyInfo> *get_properties_static() {                                              \
		return &property_map;                                                                                          \
	}                                                                                                                  \
	static void clear_properties_static() {                                                                            \
		property_map.clear();                                                                                          \
	}                                                                                                                  \
	virtual OAHashMap<StringName, PropertyInfo> *get_properties() const override {                                     \
		return get_properties_static();                                                                                \
	}                                                                                                                  \
	virtual void set(StringName p_name, Variant p_data) override {                                                     \
		SetMethodHandleBase **b = set_map.lookup_ptr(p_name);                                                          \
		ERR_FAIL_COND_MSG(b == nullptr, "The parameter " + p_name + " doesn't exist in this component.");              \
		(*b)->set(this, p_data);                                                                                       \
	}                                                                                                                  \
	virtual Variant get(StringName p_name) const override {                                                            \
		GetMethodHandleBase **b = get_map.lookup_ptr(p_name);                                                          \
		ERR_FAIL_COND_V_MSG(b == nullptr, Variant(), "The parameter " + p_name + " doesn't exist in this component."); \
		return (*b)->get(this);                                                                                        \
	}                                                                                                                  \
																													   \
private:

class Component : public ECSClass {
	ECSCLASS(Component)

public:
	Component();

public:
	static void _bind_properties();
	virtual OAHashMap<StringName, PropertyInfo> *get_properties() const;
	virtual void set(StringName p_name, Variant p_data);
	virtual Variant get(StringName p_name) const;
};
