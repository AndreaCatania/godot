
#include "ecs.h"

#include "core/message_queue.h"
#include "scene/main/scene_tree.h"

ECS *ECS::singleton = nullptr;
LocalVector<StringName> ECS::components;
LocalVector<StringName> ECS::resources;

void ECS::_bind_methods() {
}

ECS::ECS() :
		Object() {
	// TODO Do I need this?
	MessageQueue::get_singleton()->push_callable(callable_mp(this, &ECS::ecs_init));
}

ECS::~ECS() {
}

const LocalVector<StringName> &ECS::get_registered_components() {
	return components;
}

const LocalVector<StringName> &ECS::get_registered_resources() {
	return resources;
}

ECS *ECS::get_singleton() {
	return singleton;
}

void ECS::__set_singleton(ECS *p_singleton) {
	if (p_singleton == nullptr) {
		ERR_FAIL_COND(singleton == nullptr);
		singleton = nullptr;
	} else {
		ERR_FAIL_COND_MSG(singleton != nullptr, "There is already a singleton, make sure to remove that first.");
		singleton = p_singleton;
	}
}

void ECS::ecs_init() {
}
