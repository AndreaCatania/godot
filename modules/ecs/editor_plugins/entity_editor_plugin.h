/* Author: AndreaCatania */

#ifndef ENTITY_EDITOR_PLUGIN_H
#define ENTITY_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

class EntityEditorPlugin;
class EditorInspectorPluginEntity;
class Entity;

class EntityEditor : public VBoxContainer {
	GDCLASS(EntityEditor, VBoxContainer);

	EditorNode *editor;
	EditorInspectorPluginEntity *editor_plugin;

	Entity *entity;

	// Add new component HUD objects.
	MenuButton *add_component_menu = nullptr;
	EditorInspectorSection *components_section = nullptr;

public:
	EntityEditor(EditorInspectorPluginEntity *p_plugin, EditorNode *p_editor, Entity *p_entity);
	~EntityEditor();

	void _notification(int p_what);

	void create_editors();
	void update_editors();
	void create_component_inspector(StringName p_component_name, VBoxContainer *p_container);
	void _add_component_pressed(uint32_t p_component_id);
	void _remove_component_pressed(StringName p_component_name);
};

class EditorInspectorPluginEntity : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginEntity, EditorInspectorPlugin);

	friend class EntityEditorPlugin;

	EditorNode *editor;

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
};

class EntityEditorPlugin : public EditorPlugin {
	GDCLASS(EntityEditorPlugin, EditorPlugin);

	EditorNode *editor;

public:
	EntityEditorPlugin(EditorNode *p_editor);

	virtual String get_name() const override { return "Entity"; }
};

#endif // ENTITY_EDITOR_PLUGIN_H
