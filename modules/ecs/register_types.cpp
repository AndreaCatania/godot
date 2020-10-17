
#include "./register_types.h"

#include "components/mesh_component.h"
#include "components/transform_component.h"
#include "core/engine.h"
#include "core/message_queue.h"
#include "ecs.h"
#include "nodes/ecs_world.h"
#include "nodes/entity.h"

#include "editor_plugins/entity_editor_plugin.h"

// TODO remove this
#include "iterators/query.h"
#include "nodes/test_node.h"
#include "pipeline.h"
#include "resources/ecs_resource.h"
#include "resources/test_res.h"
#include "storages/storage_io.h"
#include "systems/system_builder.h"

// TODO improve this workflow once the new pipeline is integrated.
class REP : public Object {
public:
	void register_editor_plugins() {
		if (EditorNode::get_singleton() != nullptr) {
			EditorNode::get_singleton()->add_editor_plugin(memnew(EntityEditorPlugin(EditorNode::get_singleton())));
		}
	}
} rep;

void test_system(TestResource &p_res, Query<TransformComponent, const MeshComponent &> &p_query) {
	print_line("System is executed: " + itos(p_res.a));
	p_res.a += 1;

	for (; p_query.has_next(); p_query += 1) {
		auto [transform, mesh] = p_query.get();

		transform.set_transform(Transform(Basis(), transform.get_transform().origin + Vector3(10.0, 0, 0)));
		print_line(String() + transform.get_transform());
	}
}

void register_ecs_types() {
	ClassDB::register_class<ECS>();
	ClassDB::register_class<WorldECS>();
	ClassDB::register_class<Entity>();
	ClassDB::register_class<TestNode>(); // TODO Just test

	// Create and register singleton
	ECS *ecs = memnew(ECS);
	ECS::__set_singleton(ecs);
	Engine::get_singleton()->add_singleton(Engine::Singleton("ECS", ecs));

	// Register components
	ECS::register_component<MeshComponent>();
	ECS::register_component<TransformComponent>();
	ECS::register_resource<TestResource>(); // TODO Just test

	// Register editor plugins
	if (Engine::get_singleton()->is_editor_hint()) {
		MessageQueue::get_singleton()->push_callable(callable_mp(&rep, &REP::register_editor_plugins));
	}

	// TODO test ~~~~~~~~~~~~~~~~~
	Pipeline pipeline;

	// Create entity 1
	pipeline.create_entity()
			.with(TransformComponent())
			.with(MeshComponent());

	// Create entity 2
	pipeline.create_entity()
			.with(TransformComponent());

	pipeline.add_resource(TestResource());

	pipeline.add_system(test_system);
}

void unregister_ecs_types() {
	ECS *ecs = ECS::get_singleton();
	ECS::__set_singleton(nullptr);
	memdelete(ecs);
}
