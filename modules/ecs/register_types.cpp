
#include "./register_types.h"

#include "components/mesh_component.h"
#include "components/transform_component.h"
#include "core/engine.h"
#include "core/message_queue.h"
#include "ecs.h"
#include "nodes/ecs_world.h"
#include "nodes/entity.h"

#include "editor_plugins/entity_editor_plugin.h"

// TODO improve this workflow once the new pipeline is integrated.
class REP : public Object {
public:
	void register_editor_plugins() {
		if (EditorNode::get_singleton() != nullptr) {
			EditorNode::get_singleton()->add_editor_plugin(memnew(EntityEditorPlugin(EditorNode::get_singleton())));
		}
	}
} rep;

void register_ecs_types() {
	ClassDB::register_class<ECS>();
	ClassDB::register_class<WorldECS>();
	ClassDB::register_class<Entity>();

	// Create and register singleton
	ECS *ecs = memnew(ECS);
	ECS::__set_singleton(ecs);
	Engine::get_singleton()->add_singleton(Engine::Singleton("ECS", ecs));

	// Register components
	ECS::register_component<MeshComponent>();
	ECS::register_component<TransformComponent>();

	// Register editor plugins
	if (Engine::get_singleton()->is_editor_hint()) {
		MessageQueue::get_singleton()->push_callable(callable_mp(&rep, &REP::register_editor_plugins));
	}
}

void unregister_ecs_types() {
	ECS *ecs = ECS::get_singleton();
	ECS::__set_singleton(nullptr);
	memdelete(ecs);
}
