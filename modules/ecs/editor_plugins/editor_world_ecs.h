/* Author: AndreaCatania */

#ifndef EDITORWORLDECS_H
#define EDITORWORLDECS_H

#include "editor/editor_plugin.h"

class EditorNode;
class WorldECS;
class EditorWorldECS;

class SystemInfoBox : public MarginContainer {
	GDCLASS(SystemInfoBox, MarginContainer);

	EditorNode *editor = nullptr;
	Label *system_name = nullptr;
	ItemList *system_data_list = nullptr;

public:
	SystemInfoBox(EditorNode *p_editor);
	SystemInfoBox(EditorNode *p_ditor, const String &p_system_name);

	void set_system_name(const String &p_name);
	void add_component(const String &p_name, bool is_write);

	Point2 name_global_transform() const;
};

class DrawLayer : public Control {
	GDCLASS(DrawLayer, Control);

public:
	EditorWorldECS *editor = nullptr;

public:
	DrawLayer();

	void _notification(int p_what);
};

class EditorWorldECS : public PanelContainer {
	GDCLASS(EditorWorldECS, PanelContainer);

	friend class DrawLayer;
	friend class WorldECSEditorPlugin;

	EditorNode *editor = nullptr;
	WorldECS *world_ecs = nullptr;

	DrawLayer *draw_layer = nullptr;
	Label *node_name_lbl = nullptr;
	VBoxContainer *pipeline_panel = nullptr;

	// Add system window.
	ConfirmationDialog *add_sys_window = nullptr;
	LineEdit *add_sys_search = nullptr;
	class Tree *add_sys_tree = nullptr;

	// Create script system window.
	ConfirmationDialog *create_script_sys_window = nullptr;
	LineEdit *create_sys_path = nullptr;
	LineEdit *create_sys_func = nullptr;
	Label *create_sys_error_lbl = nullptr;

	LocalVector<SystemInfoBox *> pipeline_systems;

	bool is_ui_dirty = false;

public:
	EditorWorldECS(EditorNode *p_editor);

	void _notification(int p_what);

	void show_editor();
	void hide_editor();

	void set_world_ecs(WorldECS *p_world);

	void draw(DrawLayer *p_draw_layer);

	void pipeline_add_system(SystemInfoBox *p_system);
	void pipeline_clear();

	void pipeline_draw_batch(uint32_t p_start_system, uint32_t p_end_system);

	void add_sys_show();
	void add_sys_hide();
	void add_sys_update(const String &p_search = String());
	void add_sys_add();

	void create_sys_show();
	void create_sys_hide();
	void create_sys_do();
};

class WorldECSEditorPlugin : public EditorPlugin {
	GDCLASS(WorldECSEditorPlugin, EditorPlugin);

	friend class SystemInfoBox;
	friend class DrawLayer;
	friend class EditorWorldECS;

	EditorNode *editor = nullptr;
	EditorWorldECS *ecs_editor = nullptr;
	WorldECS *world_ecs = nullptr;

public:
	WorldECSEditorPlugin(EditorNode *p_node);
	~WorldECSEditorPlugin();

	virtual String get_name() const override { return "WorldECS"; }
	virtual bool has_main_screen() const override { return true; }
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;
};

#endif // EDITORWORLDECS_H
