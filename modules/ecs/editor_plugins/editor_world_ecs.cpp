/* Author: AndreaCatania */

#include "editor_world_ecs.h"

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "modules/ecs/ecs.h"
#include "modules/ecs/nodes/ecs_world.h"
#include "scene/gui/color_rect.h"

SystemInfoBox::SystemInfoBox(EditorNode *p_editor) :
		editor(p_editor) {
	add_theme_constant_override("margin_right", 2);
	add_theme_constant_override("margin_top", 2);
	add_theme_constant_override("margin_left", 2);
	add_theme_constant_override("margin_bottom", 2);

	ColorRect *bg = memnew(ColorRect);
	bg->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	bg->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	bg->set_color(Color(0.0, 0.0, 0.0, 0.2));
	add_child(bg);

	MarginContainer *inner_container = memnew(MarginContainer);
	inner_container->add_theme_constant_override("margin_right", 2);
	inner_container->add_theme_constant_override("margin_top", 2);
	inner_container->add_theme_constant_override("margin_left", 20);
	inner_container->add_theme_constant_override("margin_bottom", 2);
	add_child(inner_container);

	HBoxContainer *box = memnew(HBoxContainer);
	box->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	box->set_v_size_flags(0);
	inner_container->add_child(box);

	system_name = memnew(Label);
	system_name->set_h_size_flags(0);
	system_name->set_v_size_flags(0);
	box->add_child(system_name);

	system_data_list = memnew(ItemList);
	system_data_list->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	system_data_list->set_v_size_flags(SizeFlags::SIZE_EXPAND);
	system_data_list->set_auto_height(true);
	system_data_list->set_max_columns(0);
	system_data_list->set_fixed_icon_size(Size2(13.0, 13.0));
	system_data_list->add_theme_constant_override("hseparation", 7.0);
	box->add_child(system_data_list);
}

SystemInfoBox::SystemInfoBox(EditorNode *p_editor, const String &p_system_name) :
		SystemInfoBox(p_editor) {
	set_system_name(p_system_name);
}

void SystemInfoBox::set_system_name(const String &p_name) {
	system_name->set_text(p_name);
}

void SystemInfoBox::add_component(const String &p_name, bool is_write) {
	Ref<Texture2D> icon;
	if (is_write) {
		icon = editor->get_theme_base()->get_theme_icon("Edit", "EditorIcons");
	} else {
		icon = editor->get_theme_base()->get_theme_icon("GuiVisibilityVisible", "EditorIcons");
	}
	system_data_list->add_item(p_name, icon, false);
}

Point2 SystemInfoBox::name_global_transform() const {
	Control *nc = static_cast<Control *>(system_name->get_parent());
	return nc->get_global_position() + Vector2(0.0, nc->get_size().y / 2.0);
}

DrawLayer::DrawLayer() {
	// This is just a draw layer, we don't need input.
	set_mouse_filter(MOUSE_FILTER_IGNORE);
}

void DrawLayer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (editor->is_ui_dirty) {
				editor->is_ui_dirty = false;
				update();
			}
		} break;
		case NOTIFICATION_DRAW: {
			editor->draw(this);
		} break;
		case NOTIFICATION_READY: {
			set_process_internal(true);
		} break;
	}
}

EditorWorldECS::EditorWorldECS(EditorNode *p_editor) :
		editor(p_editor) {
	//set_anchors_and_margins_preset(Control::PRESET_WIDE);
	set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	set_anchor(MARGIN_LEFT, 0.0);
	set_anchor(MARGIN_TOP, 0.0);
	set_anchor(MARGIN_RIGHT, 1.0);
	set_anchor(MARGIN_BOTTOM, 1.0);
	set_margin(MARGIN_LEFT, 0.0);
	set_margin(MARGIN_TOP, 0.0);
	set_margin(MARGIN_RIGHT, 0.0);
	set_margin(MARGIN_BOTTOM, 0.0);

	HBoxContainer *main_hb = memnew(HBoxContainer);
	main_hb->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	main_hb->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	main_hb->set_anchor(MARGIN_LEFT, 0.0);
	main_hb->set_anchor(MARGIN_TOP, 0.0);
	main_hb->set_anchor(MARGIN_RIGHT, 1.0);
	main_hb->set_anchor(MARGIN_BOTTOM, 1.0);
	add_child(main_hb);

	DrawLayer *draw_layer = memnew(DrawLayer);
	draw_layer->editor = this;
	draw_layer->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	draw_layer->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
	draw_layer->set_anchor(MARGIN_LEFT, 0.0);
	draw_layer->set_anchor(MARGIN_TOP, 0.0);
	draw_layer->set_anchor(MARGIN_RIGHT, 1.0);
	draw_layer->set_anchor(MARGIN_BOTTOM, 1.0);
	draw_layer->set_margin(MARGIN_LEFT, 0.0);
	draw_layer->set_margin(MARGIN_TOP, 0.0);
	draw_layer->set_margin(MARGIN_RIGHT, 0.0);
	draw_layer->set_margin(MARGIN_BOTTOM, 0.0);
	add_child(draw_layer);

	// ~~ Left Panel ~~
	{
		VBoxContainer *main_container = memnew(VBoxContainer);
		main_container->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		main_container->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		main_hb->add_child(main_container);

		// ~~ Info box ~~
		{
			HBoxContainer *title_box = memnew(HBoxContainer);
			title_box->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
			title_box->set_v_size_flags(0);
			main_container->add_child(title_box);

			Label *title = memnew(Label);
			title->set_text(TTR("ECS Node:"));
			title->set_valign(Label::VALIGN_CENTER);
			title->set_h_size_flags(0);
			title->set_v_size_flags(0);
			title_box->add_child(title);

			node_name_lbl = memnew(Label);
			node_name_lbl->set_valign(Label::VALIGN_CENTER);
			node_name_lbl->set_h_size_flags(0);
			node_name_lbl->set_v_size_flags(0);
			node_name_lbl->add_theme_color_override("font_color", Color(0.0, 0.5, 1.0));
			title_box->add_child(node_name_lbl);

			// TODO populate with other info here.
		}

		// ~~ Resources box ~~
		{
			Label *title = memnew(Label);
			title->set_text(TTR("Resources"));
			title->set_valign(Label::VALIGN_CENTER);
			title->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
			title->set_v_size_flags(0);
			main_container->add_child(title);
		}
	}

	VSeparator *separator = memnew(VSeparator);
	main_hb->add_child(separator);

	// ~~ Right Panel ~~
	{
		VBoxContainer *main_container = memnew(VBoxContainer);
		main_container->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		main_container->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		main_hb->add_child(main_container);

		Label *title = memnew(Label);
		title->set_text(TTR("Pipeline"));
		title->set_valign(Label::VALIGN_CENTER);
		title->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		title->set_v_size_flags(0);
		main_container->add_child(title);

		Panel *panel = memnew(Panel);
		panel->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		panel->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		panel->set_anchor(MARGIN_LEFT, 0.0);
		panel->set_anchor(MARGIN_TOP, 0.0);
		panel->set_anchor(MARGIN_RIGHT, 1.0);
		panel->set_anchor(MARGIN_BOTTOM, 1.0);
		main_container->add_child(panel);

		ScrollContainer *wrapper = memnew(ScrollContainer);
		wrapper->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		wrapper->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		wrapper->set_anchor(MARGIN_LEFT, 0.0);
		wrapper->set_anchor(MARGIN_TOP, 0.0);
		wrapper->set_anchor(MARGIN_RIGHT, 1.0);
		wrapper->set_anchor(MARGIN_BOTTOM, 1.0);
		wrapper->set_enable_h_scroll(true);
		wrapper->set_enable_v_scroll(false);
		panel->add_child(wrapper);

		pipeline_panel = memnew(VBoxContainer);
		pipeline_panel->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		pipeline_panel->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		wrapper->add_child(pipeline_panel);

		HBoxContainer *button_container = memnew(HBoxContainer);
		button_container->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		button_container->set_v_size_flags(0);
		main_container->add_child(button_container);

		Button *show_btn_add_sys = memnew(Button);
		show_btn_add_sys->set_text(TTR("Add System"));
		show_btn_add_sys->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		show_btn_add_sys->set_v_size_flags(0);
		show_btn_add_sys->connect("pressed", callable_mp(this, &EditorWorldECS::add_sys_show));
		button_container->add_child(show_btn_add_sys);

		Button *create_sys_btn = memnew(Button);
		create_sys_btn->set_icon(editor->get_theme_base()->get_theme_icon("New", "EditorIcons"));
		create_sys_btn->set_h_size_flags(0.0);
		create_sys_btn->set_v_size_flags(0.0);
		create_sys_btn->connect("pressed", callable_mp(this, &EditorWorldECS::create_sys_show));
		button_container->add_child(create_sys_btn);
	}

	// ~~ Add system window ~~
	{
		add_sys_window = memnew(ConfirmationDialog);
		add_sys_window->set_min_size(Size2i(500, 500));
		add_sys_window->set_title(TTR("Add System"));
		add_sys_window->get_ok()->set_text(TTR("Add"));
		add_sys_window->connect("confirmed", callable_mp(this, &EditorWorldECS::add_sys_add));
		add_child(add_sys_window);

		VBoxContainer *vert_container = memnew(VBoxContainer);
		vert_container->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		vert_container->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		add_sys_window->add_child(vert_container);

		add_sys_search = memnew(LineEdit);
		add_sys_search->set_placeholder(TTR("Search"));
		add_sys_search->connect("text_changed", callable_mp(this, &EditorWorldECS::add_sys_update));
		add_sys_search->connect("text_changed", callable_mp(this, &EditorWorldECS::add_sys_update));
		vert_container->add_child(add_sys_search);

		add_sys_tree = memnew(Tree);
		add_sys_tree->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		add_sys_tree->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		add_sys_tree->set_hide_root(true);
		add_sys_tree->connect("item_selected", callable_mp(this, &EditorWorldECS::add_sys_update_desc));
		vert_container->add_child(add_sys_tree);

		add_sys_desc = memnew(TextEdit);
		add_sys_desc->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		add_sys_desc->set_v_size_flags(0);
		add_sys_desc->set_custom_minimum_size(Size2i(0, 50));
		add_sys_desc->set_h_scroll(true);
		add_sys_desc->set_v_scroll(false);
		add_sys_desc->set_context_menu_enabled(false);
		add_sys_desc->set_shortcut_keys_enabled(false);
		add_sys_desc->set_virtual_keyboard_enabled(false);
		add_sys_desc->set_focus_mode(FOCUS_NONE);
		add_sys_desc->set_readonly(true);
		add_sys_desc->add_theme_color_override("font_color_readonly", Color(1.0, 1.0, 1.0));
		vert_container->add_child(add_sys_desc);
	}

	// ~~ Create script system window ~~
	{
		create_script_sys_window = memnew(ConfirmationDialog);
		create_script_sys_window->set_min_size(Size2i(500, 200));
		create_script_sys_window->set_title(TTR("Create system"));
		create_script_sys_window->set_hide_on_ok(false);
		create_script_sys_window->get_ok()->set_text(TTR("Create"));
		create_script_sys_window->connect("confirmed", callable_mp(this, &EditorWorldECS::create_sys_do));
		add_child(create_script_sys_window);

		VBoxContainer *vert_container = memnew(VBoxContainer);
		vert_container->set_h_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		vert_container->set_v_size_flags(SizeFlags::SIZE_FILL | SizeFlags::SIZE_EXPAND);
		create_script_sys_window->add_child(vert_container);

		Label *lbl = memnew(Label);
		lbl->set_text("Script path");
		vert_container->add_child(lbl);

		create_sys_path = memnew(LineEdit);
		create_sys_path->set_placeholder(TTR("Script path"));
		vert_container->add_child(create_sys_path);

		lbl = memnew(Label);
		lbl->set_text("Function name");
		vert_container->add_child(lbl);

		create_sys_func = memnew(LineEdit);
		create_sys_func->set_placeholder(TTR("function name"));
		vert_container->add_child(create_sys_func);

		create_sys_error_lbl = memnew(Label);
		create_sys_error_lbl->hide();
		create_sys_error_lbl->add_theme_color_override("font_color", Color(1.0, 0.0, 0.0));
		vert_container->add_child(create_sys_error_lbl);
	}
}

void EditorWorldECS::_notification(int p_what) {
}

void EditorWorldECS::show_editor() {
	show();
	add_sys_hide();
	create_sys_hide();
}

void EditorWorldECS::hide_editor() {
	hide();
	add_sys_hide();
	create_sys_hide();
}

void EditorWorldECS::set_world_ecs(WorldECS *p_world) {
	world_ecs = p_world;
	pipeline_clear();

	if (world_ecs != nullptr) {
		node_name_lbl->set_text(world_ecs->get_name());

		// TODO try SystemInfo.
		pipeline_add_system(memnew(SystemInfoBox(editor, "TransformSystem")));
		pipeline_add_system(memnew(SystemInfoBox(editor, "BulletSystem")));
		pipeline_add_system(memnew(SystemInfoBox(editor, "AASystem1")));
		pipeline_add_system(memnew(SystemInfoBox(editor, "Chasdqwe")));

		is_ui_dirty = true;

	} else {
		node_name_lbl->set_text("");
	}
}

void EditorWorldECS::draw(DrawLayer *p_draw_layer) {
	if (pipeline_systems.size() <= 0) {
		return;
	}

	// Setting this value here, so I can avoid to pass this pointer to each func.
	draw_layer = p_draw_layer;

	pipeline_draw_batch(0, 1);
	pipeline_draw_batch(2, 3);

	draw_layer = nullptr;
}

void EditorWorldECS::pipeline_add_system(SystemInfoBox *p_system) {
	p_system->add_component("Test write", true);
	p_system->add_component("Test read", false);
	pipeline_systems.push_back(p_system);
	pipeline_panel->add_child(p_system);
}

void EditorWorldECS::pipeline_clear() {
	for (int i = pipeline_panel->get_child_count() - 1; i >= 0; i -= 1) {
		Node *n = pipeline_panel->get_child(i);
		pipeline_panel->get_child(i)->remove_and_skip();
		memdelete(n);
	}
	pipeline_systems.clear();
}

void EditorWorldECS::pipeline_draw_batch(uint32_t p_start_system, uint32_t p_end_system) {
	ERR_FAIL_COND(p_start_system > p_end_system);
	ERR_FAIL_COND(p_end_system >= pipeline_systems.size());

	const Point2 this_pos = draw_layer->get_global_position();
	const Point2 point_offset(-15.0, 0.0);
	const Point2 circle_offset(5.0, 0.0);

	Point2 prev;

	// Draw the points
	for (uint32_t i = p_start_system; i <= p_end_system; i += 1) {
		const Point2 current_point = (pipeline_systems[i]->name_global_transform() - this_pos) + point_offset;

		if (i != p_start_system) {
			draw_layer->draw_line(prev, current_point, Color(1.0, 1.0, 1.0, 0.4), 2.0);
		}

		draw_layer->draw_circle(
				current_point + circle_offset,
				4.0,
				Color(1.0, 1.0, 1.0, 0.4));

		prev = current_point;
	}
}

void EditorWorldECS::add_sys_show() {
	// Display the modal window centered.
	const Vector2i modal_pos = (Vector2i(get_viewport_rect().size) - add_sys_window->get_size()) / 2.0;
	add_sys_window->set_position(modal_pos);
	add_sys_window->set_visible(true);
	add_sys_update();
}

void EditorWorldECS::add_sys_hide() {
	add_sys_window->set_visible(false);
}

void EditorWorldECS::add_sys_update(const String &p_search) {
	String search = p_search;
	if (search.empty()) {
		search = add_sys_search->get_text();
	}

	add_sys_tree->clear();

	TreeItem *root = add_sys_tree->create_item();
	root->set_text(0, "Systems");
	root->set_selectable(0, false);

	// Native systems
	TreeItem *native_root = nullptr;

	for (uint32_t i = 0; i < ECS::get_systems_count(); i += 1) {
		const SystemInfo &info = ECS::get_system_info(i);

		const String name(info.name);
		if (p_search.empty() == false && name.find(p_search) != 0) {
			// System filtered.
			continue;
		}

		if (native_root == nullptr) {
			// Add only if needed.
			native_root = add_sys_tree->create_item(root);
			native_root->set_text(0, "Native Systems");
			native_root->set_selectable(0, false);
			native_root->set_custom_color(0, Color(0.0, 0.9, 0.3));
		}

		TreeItem *item = add_sys_tree->create_item(native_root);
		item->set_text(0, name);
		item->set_meta("system_link", info.name);
		item->set_meta("desc", info.description);
	}

	// Scripts systems
	TreeItem *script_root = nullptr;

	Array sys_scripts = ProjectSettings::get_singleton()->get_setting("ECS/system_scripts");
	for (int i = 0; i < sys_scripts.size(); i += 1) {
		String sys_link = sys_scripts[i];
		Vector<String> v = sys_link.split("::");

		// Make sure there are exactly 2 items. The link is: `/path/to/script::func_name`.
		ERR_CONTINUE(v.size() != 2);

		if (p_search.empty() == false && v[1].find(p_search) != 0) {
			// System filtered.
			continue;
		}

		if (script_root == nullptr) {
			// Add only if needed.
			script_root = add_sys_tree->create_item(root);
			script_root->set_text(0, "Script Systems");
			script_root->set_selectable(0, false);
			script_root->set_custom_color(0, Color(0.0, 0.3, 0.9));
		}

		TreeItem *item = add_sys_tree->create_item(script_root);
		item->set_text(0, v[1]);
		item->set_meta("system_link", sys_link);
		item->set_meta("desc", "GDScript: " + v[0]);
	}

	add_sys_update_desc();
}

void EditorWorldECS::add_sys_update_desc() {
	TreeItem *selected = add_sys_tree->get_selected();
	if (selected == nullptr) {
		// Nothing selected.
		add_sys_desc->set_text("");
	} else {
		const String desc = selected->get_meta("desc");
		add_sys_desc->set_text(desc);
	}
}

void EditorWorldECS::add_sys_add() {
	TreeItem *selected = add_sys_tree->get_selected();
	if (selected == nullptr) {
		// Nothing selected, so nothing to do.
		return;
	}

	world_ecs->insert_system(selected->get_meta("system_link"));
}

void EditorWorldECS::create_sys_show() {
	// Display the modal window centered.
	const Vector2i modal_pos = (Vector2i(get_viewport_rect().size) - create_script_sys_window->get_size()) / 2.0;
	create_script_sys_window->set_position(modal_pos);
	create_script_sys_window->set_visible(true);
	create_sys_error_lbl->hide();
}

void EditorWorldECS::create_sys_hide() {
	create_script_sys_window->set_visible(false);
}

void EditorWorldECS::create_sys_do() {
	// Test creating script system list.

	const String script_path = create_sys_path->get_text();
	const String func_name = create_sys_func->get_text();

	// Check the script path.
	if (false == false) {
		create_sys_error_lbl->set_text(TTR("The script path is not valid."));
		create_sys_error_lbl->show();
		return;
	}

	// Check if the function exists.
	if (false == false) {
		create_sys_error_lbl->set_text(TTR("The function doesn't exist."));
		create_sys_error_lbl->show();
		return;
	}

	// Check if the function is a valid system.
	if (false == false) {
		create_sys_error_lbl->set_text(TTR("The function " + func_name + " is not a valid system."));
		create_sys_error_lbl->show();
		return;
	}

	// The script and functions are ok.
	Array sys_scripts = ProjectSettings::get_singleton()->get_setting("ECS/system_scripts");

	// Check if this system already exists.
	const String script_system_link = script_path + "::" + func_name;

	if (sys_scripts.find(script_system_link) >= 0) {
		create_sys_error_lbl->set_text(TTR("The function " + func_name + " is already registered as system."));
		create_sys_error_lbl->show();
		return;
	}

	sys_scripts.push_back(script_system_link);

	ProjectSettings::get_singleton()->set_setting("ECS/system_scripts", sys_scripts);
}

WorldECSEditorPlugin::WorldECSEditorPlugin(EditorNode *p_node) :
		editor(p_node) {
	ecs_editor = memnew(EditorWorldECS(p_node));
	editor->get_viewport()->add_child(ecs_editor);
	ecs_editor->hide_editor();
}

WorldECSEditorPlugin::~WorldECSEditorPlugin() {
	editor->get_viewport()->remove_child(ecs_editor);
	memdelete(ecs_editor);
	ecs_editor = nullptr;
}

void WorldECSEditorPlugin::edit(Object *p_object) {
	world_ecs = Object::cast_to<WorldECS>(p_object);
	ERR_FAIL_COND_MSG(world_ecs == nullptr, "The object should be of type WorldECS [BUG].");
	ecs_editor->set_world_ecs(world_ecs);
}

bool WorldECSEditorPlugin::handles(Object *p_object) const {
	return Object::cast_to<WorldECS>(p_object) != nullptr;
}

void WorldECSEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		ecs_editor->show_editor();
	} else {
		ecs_editor->hide_editor();
		ecs_editor->set_world_ecs(nullptr);
	}
}
