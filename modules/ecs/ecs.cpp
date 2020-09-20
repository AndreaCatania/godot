
#include "ecs.h"

#include "core/message_queue.h"
#include "scene/main/scene_tree.h"

ECS *ECS::singleton = nullptr;
LocalVector<StringName> ECS::components;

ECS::ECS() :
		Object() {
	MessageQueue::get_singleton()->push_callable(callable_mp(this, &ECS::ecs_init));
}

ECS::~ECS() {
}

void ECS::add_system(SystemMethod *p_system) {
	systems.push_back(p_system);
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

void ECS::_bind_methods() {
}

void ECS::ecs_init() {
}
