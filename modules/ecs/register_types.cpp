
#include "./register_types.h"

#include "core/engine.h"
#include "core/message_queue.h"
#include "ecs.h"
#include "entity.h"

#include "components/mesh_component.h"
#include "components/transform_component.h"

#include "editor_plugins/entity_editor_plugin.h"

// TODO remove this
#include "component_storages/storage_io.h"

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

	// TODO test
	Storage *transform_storage = TransformComponent::get_storage();
	Storage *mesh_storage = MeshComponent::get_storage();

	EntityIndex entity_1(0);
	StorageIO::insert(transform_storage, entity_1, TransformComponent());
	StorageIO::insert(mesh_storage, entity_1, MeshComponent());

	EntityIndex entity_2(1);
	StorageIO::insert(transform_storage, entity_2, TransformComponent());
	StorageIO::insert(mesh_storage, entity_2, MeshComponent());

	
	print_line("bb");
	transform_storage->remove(entity_2);
	transform_storage->remove(entity_1);
	print_line("aa");
}

void unregister_ecs_types() {
	ECS::unregister_component<MeshComponent>();
	ECS::unregister_component<TransformComponent>();

	ECS *ecs = ECS::get_singleton();
	ECS::__set_singleton(nullptr);
	memdelete(ecs);
}
