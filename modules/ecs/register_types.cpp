
#include "./register_types.h"

#include "core/engine.h"
#include "core/message_queue.h"
#include "ecs.h"
#include "entity.h"

#include "components/mesh_component.h"
#include "components/transform_component.h"

#include "editor_plugins/entity_editor_plugin.h"

// TODO remove this
#include "iterators/query.h"
#include "pipeline.h"
#include "storages/storage_io.h"

// TODO improve this workflow once the new pipeline is integrated.
class REP : public Object {
public:
	void register_editor_plugins() {
		EditorNode::get_singleton()->add_editor_plugin(memnew(EntityEditorPlugin(EditorNode::get_singleton())));
	}
} rep;

void register_ecs_types() {
	ClassDB::register_class<ECS>();
	ClassDB::register_class<Entity>();

	// Create and register singleton
	ECS *ecs = memnew(ECS);
	ECS::__set_singleton(ecs);
	Engine::get_singleton()->add_singleton(Engine::Singleton("ECS", ecs));

	// Register components
	ECS::register_component<MeshComponent>();
	ECS::register_component<TransformComponent>();

	// Register editor plugins
	MessageQueue::get_singleton()->push_callable(callable_mp(&rep, &REP::register_editor_plugins));

	// TODO test ~~~~~~~~~~~~~~~~~
	Pipeline pipeline;

	// Create entity 1
	pipeline.create_entity()
			.with(TransformComponent())
			.with(MeshComponent());

	// Create entity 2
	pipeline.create_entity()
			.with(TransformComponent());

	for (Query query = Query<TransformComponent, const MeshComponent>(&pipeline);
			query.is_done() == false;
			query += 1) {
		auto [transform, mesh] = query.get();

		transform.set_transform(Transform(Basis(), Vector3(100.0, 0, 0)));
		print_line(String() + transform.get_transform());
		break;
	}
}

void unregister_ecs_types() {
	ECS *ecs = ECS::get_singleton();
	ECS::__set_singleton(nullptr);
	memdelete(ecs);
}
