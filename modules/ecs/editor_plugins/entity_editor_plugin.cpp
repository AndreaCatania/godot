/* Author: AndreaCatania */

#include "entity_editor_plugin.h"
#include "modules/ecs/entity.h"

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
		for (int i = 0; i < add_component_menu->get_popup()->get_item_count(); i += 1) {
			add_component_menu->get_popup()->remove_item(i);
		}

		//LocalVector<StringName> &components = ECS::get_components();
		add_component_menu->get_popup()->add_item(TTR("Element 1"), 0);
		add_component_menu->get_popup()->add_item(TTR("Element 2"), 1);
		add_component_menu->get_popup()->add_item(TTR("Element 3"), 2);
	}

	if (components_section) {
		// Remove old childs.
		for (int i = 0; i < components_section->get_vbox()->get_child_count(); i += 1) {
			components_section->get_vbox()->remove_child(components_section->get_vbox()->get_child(i));
		}

		EditorInspectorSection *component_section = memnew(EditorInspectorSection);
		component_section->setup("component_##name", "##name", entity, section_color, true);
		component_section->unfold();
		Label *lbl = memnew(Label);
		lbl->set_text("Test component");
		component_section->get_vbox()->add_child(lbl);

		components_section->get_vbox()->add_child(component_section);
	}
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