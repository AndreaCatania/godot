/* Author: AndreaCatania */

#ifndef EDITORWORLDECS_H
#define EDITORWORLDECS_H

#include "editor/editor_plugin.h"

class EditorNode;
class WorldECS;

class EditorWorldECS : public PanelContainer {
	GDCLASS(EditorWorldECS, PanelContainer);

public:
	EditorWorldECS();
};

class WorldECSEditorPlugin : public EditorPlugin {
	GDCLASS(WorldECSEditorPlugin, EditorPlugin);

	EditorNode *editor = nullptr;
	EditorWorldECS *ecs_editor = nullptr;
	WorldECS *world_ecs;

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
