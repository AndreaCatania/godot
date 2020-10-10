/* Author: AndreaCatania */

#include "entity_editor_plugin.h"
#include "modules/ecs/nodes/entity.h"

EntityEditor::EntityEditor(
		EditorInspectorPluginEntity *p_plugin,
		EditorNode *p_editor,
		Entity *p_entity) :
		editor(p_editor),
		editor_plugin(p_plugin),
		entity(p_entity) {
}

EntityEditor::~EntityEditor() {
}

void EntityEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			create_editors();
			update_editors();
			break;
		}
	}
}

void EntityEditor::create_editors() {
	const Color section_color = get_theme_color("prop_subsection", "Editor");

	add_component_menu = memnew(MenuButton);
	add_component_menu->set_text("Add new component");
	add_component_menu->get_popup()->connect("id_pressed", callable_mp(this, &EntityEditor::_add_component_pressed));
	add_child(add_component_menu);

	components_section = memnew(EditorInspectorSection);
	components_section->setup("components", "Components", entity, section_color, true);
	add_child(components_section);
	components_section->unfold();
}

void EntityEditor::update_editors() {
	const Color section_color = get_theme_color("prop_subsection", "Editor");

	if (add_component_menu) {
		// Remove all old components.
		add_component_menu->get_popup()->clear();

		const LocalVector<StringName> &components = ECS::get_registered_components();
		for (uint32_t i = 0; i < components.size(); i += 1) {
			add_component_menu->get_popup()->add_item(components[i], i);
		}
	}

	if (components_section) {
		// Remove old childs.
		for (int i = components_section->get_vbox()->get_child_count() - 1; i >= 0; i -= 1) {
			Node *n = components_section->get_vbox()->get_child(i);
			components_section->get_vbox()->remove_child(n);
			memdelete(n);
		}

		const Dictionary &components = entity->get_components_data();
		for (const Variant *key = components.next(nullptr); key != nullptr; key = components.next(key)) {
			const Variant *data = components.getptr(*key);
			// Add the components of this Entity
			EditorInspectorSection *component_section = memnew(EditorInspectorSection);
			component_section->setup("component_" + String(*key), String(*key), entity, section_color, true);
			component_section->unfold();

			Button *del_btn = memnew(Button);
			del_btn->set_text("Remove component");
			del_btn->set_flat(true);
			del_btn->connect("pressed", callable_mp(this, &EntityEditor::_remove_component_pressed), varray(key->operator StringName()));
			component_section->get_vbox()->add_child(del_btn);

			// TODO here the component properties.
			Label *lbl = memnew(Label);
			lbl->set_text("Test component");
			component_section->get_vbox()->add_child(lbl);

			components_section->get_vbox()->add_child(component_section);
		}
	}
}

void EntityEditor::_add_component_pressed(uint32_t p_component_id) {
	entity->add_component_data(ECS::get_registered_components()[p_component_id]);
	update_editors();
}

void EntityEditor::_remove_component_pressed(StringName p_component_name) {
	entity->remove_component_data(p_component_name);
	update_editors();
}

bool EditorInspectorPluginEntity::can_handle(Object *p_object) {
	return Object::cast_to<Entity>(p_object) != nullptr;
}

void EditorInspectorPluginEntity::parse_begin(Object *p_object) {
	Entity *entity = Object::cast_to<Entity>(p_object);
	ERR_FAIL_COND(!entity);

	EntityEditor *entity_editor = memnew(EntityEditor(this, editor, entity));
	add_custom_control(entity_editor);
}

EntityEditorPlugin::EntityEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginEntity> entity_plugin;
	entity_plugin.instance();
	entity_plugin->editor = p_node;

	EditorInspector::add_inspector_plugin(entity_plugin);
}