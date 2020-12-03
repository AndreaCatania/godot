/* Author: AndreaCatania */

#include "editor_world_ecs.h"

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "modules/ecs/nodes/ecs_world.h"

EditorWorldECS::EditorWorldECS() {
	Ref<StyleBoxEmpty> border;
	border.instance();
	border->set_default_margin(MARGIN_LEFT, 5 * EDSCALE);
	border->set_default_margin(MARGIN_RIGHT, 5 * EDSCALE);
	border->set_default_margin(MARGIN_BOTTOM, 5 * EDSCALE);
	border->set_default_margin(MARGIN_TOP, 5 * EDSCALE);
	add_theme_style_override("panel", border);

	HBoxContainer *main_hb = memnew(HBoxContainer);
	main_hb->set_anchor(Margin::MARGIN_LEFT, 0.0);
	main_hb->set_anchor(Margin::MARGIN_TOP, 0.0);
	main_hb->set_anchor(Margin::MARGIN_RIGHT, 1.0);
	main_hb->set_anchor(Margin::MARGIN_BOTTOM, 1.0);
	main_hb->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	main_hb->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	main_hb->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	add_child(main_hb);

	Label *lbl = memnew(Label);
	lbl->set_text("aa");
	add_child(lbl);

	lbl = memnew(Label);
	lbl->set_text("bb");
	add_child(lbl);
}

WorldECSEditorPlugin::WorldECSEditorPlugin(EditorNode *p_node) :
		editor(p_node) {
	ecs_editor = memnew(EditorWorldECS);
	ecs_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	editor->get_viewport()->add_child(ecs_editor);
	ecs_editor->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	ecs_editor->hide();
}

WorldECSEditorPlugin::~WorldECSEditorPlugin() {
	editor->get_viewport()->remove_child(ecs_editor);
	memdelete(ecs_editor);
	ecs_editor = nullptr;
}

void WorldECSEditorPlugin::edit(Object *p_object) {
	world_ecs = Object::cast_to<WorldECS>(p_object);
	ERR_FAIL_COND_MSG(world_ecs == nullptr, "The object should be of type WorldECS [BUG].");
}

bool WorldECSEditorPlugin::handles(Object *p_object) const {
	return Object::cast_to<WorldECS>(p_object) != nullptr;
}

void WorldECSEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		ecs_editor->show();
	} else {
		ecs_editor->hide();
	}
}
