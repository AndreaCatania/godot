/* Author: AndreaCatania */

#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_storages/dense_vector.h"
#include "core/oa_hash_map.h"
#include "core/object.h"
#include "ecs.h"

#define COMPONENT(m_class, m_storage_class)                                                    \
	ECSCLASS(m_class)                                                                          \
	friend class Component;                                                                    \
                                                                                               \
private:                                                                                       \
	static inline m_storage_class<m_class> *__storage = nullptr;                               \
	static _FORCE_INLINE_ void init_storage() {                                                \
		CRASH_COND(__storage != nullptr);                                                      \
		__storage = memnew(m_storage_class<m_class>);                                          \
	}                                                                                          \
	static _FORCE_INLINE_ void destroy_storage() {                                             \
		CRASH_COND(__storage == nullptr);                                                      \
		memdelete(__storage);                                                                  \
		__storage = nullptr;                                                                   \
	}                                                                                          \
                                                                                               \
	static inline OAHashMap<StringName, PropertyInfo> property_map;                            \
	static void add_property(const PropertyInfo &p_info, StringName p_set, StringName p_get) { \
		print_line("TODO integrate set and get.");                                             \
		property_map.insert(p_info.name, p_info);                                              \
	}                                                                                          \
	static OAHashMap<StringName, PropertyInfo> *get_properties_static() {                      \
		return &property_map;                                                                  \
	}                                                                                          \
	static void clear_properties_static() {                                                    \
		property_map.clear();                                                                  \
	}                                                                                          \
	virtual OAHashMap<StringName, PropertyInfo> *get_properties() const override {             \
		return get_properties_static();                                                        \
	}                                                                                          \
                                                                                               \
public:                                                                                        \
	static _FORCE_INLINE_ m_storage_class<m_class> *get_storage() {                            \
		return __storage;                                                                      \
	}                                                                                          \
                                                                                               \
private:

class Component : public ECSClass {
	ECSCLASS(Component);

public:
	Component();

public:
	static void _bind_properties();
	virtual OAHashMap<StringName, PropertyInfo> *get_properties() const;
};

#endif