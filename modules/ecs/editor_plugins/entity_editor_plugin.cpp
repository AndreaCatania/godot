/* Author: AndreaCatania */

#include "entity_editor_plugin.h"
#include "core/io/marshalls.h"
#include "editor/editor_properties.h"
#include "editor/editor_properties_array_dict.h"
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

	// TODO right now this is not customizable.
	//EditorInspectorCategory *category = memnew(EditorInspectorCategory);
	//add_child(category);

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
			components_section->get_vbox()->get_child(i)->queue_delete(); // TODO is this enough to also destroy the internally created things?
		}
		components_properties.clear();

		const Dictionary &components = entity->get_components_data();
		for (const Variant *key = components.next(nullptr); key != nullptr; key = components.next(key)) {
			// Add the components of this Entity
			EditorInspectorSection *component_section = memnew(EditorInspectorSection);
			component_section->setup("component_" + String(*key), String(*key), entity, section_color, true);
			component_section->unfold();

			// TODO would be nice put this into the EditorInspectorSection.
			Button *del_btn = memnew(Button);
			del_btn->set_text("Remove component");
			del_btn->set_flat(true);
			del_btn->connect("pressed", callable_mp(this, &EntityEditor::_remove_component_pressed), varray(key->operator StringName()));
			component_section->get_vbox()->add_child(del_btn);

			create_component_inspector(key->operator StringName(), component_section->get_vbox());

			components_section->get_vbox()->add_child(component_section);

			update_component_inspector(key->operator StringName());
		}
	}
}

void EntityEditor::create_component_inspector(StringName p_component_name, VBoxContainer *p_container) {
	const OAHashMap<StringName, PropertyInfo> *properties = ECS::get_component_properties(p_component_name);
	if (properties == nullptr) {
		return;
	}

	const float default_float_step = EDITOR_GET("interface/inspector/default_float_step");

	OAHashMap<StringName, EditorProperty *> editor_properties;
	for (OAHashMap<StringName, PropertyInfo>::Iterator it = properties->iter(); it.valid; it = properties->next_iter(it)) {
		EditorProperty *prop = nullptr;

		switch (it.value->type) {
			case Variant::NIL: {
				prop = memnew(EditorPropertyNil);

			} break;

			case Variant::BOOL: {
				prop = memnew(EditorPropertyCheck);

			} break;
			case Variant::INT: {
				if (it.value->hint == PROPERTY_HINT_ENUM) {
					EditorPropertyEnum *editor = memnew(EditorPropertyEnum);
					Vector<String> options = it.value->hint_string.split(",");
					editor->setup(options);
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_FLAGS) {
					EditorPropertyFlags *editor = memnew(EditorPropertyFlags);
					Vector<String> options = it.value->hint_string.split(",");
					editor->setup(options);
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_LAYERS_2D_PHYSICS || it.value->hint == PROPERTY_HINT_LAYERS_2D_RENDER || it.value->hint == PROPERTY_HINT_LAYERS_3D_PHYSICS || it.value->hint == PROPERTY_HINT_LAYERS_3D_RENDER) {
					EditorPropertyLayers::LayerType lt = EditorPropertyLayers::LAYER_RENDER_2D;
					switch (it.value->hint) {
						case PROPERTY_HINT_LAYERS_2D_RENDER:
							lt = EditorPropertyLayers::LAYER_RENDER_2D;
							break;
						case PROPERTY_HINT_LAYERS_2D_PHYSICS:
							lt = EditorPropertyLayers::LAYER_PHYSICS_2D;
							break;
						case PROPERTY_HINT_LAYERS_3D_RENDER:
							lt = EditorPropertyLayers::LAYER_RENDER_3D;
							break;
						case PROPERTY_HINT_LAYERS_3D_PHYSICS:
							lt = EditorPropertyLayers::LAYER_PHYSICS_3D;
							break;
						default: {
						}
					}
					EditorPropertyLayers *editor = memnew(EditorPropertyLayers);
					editor->setup(lt);

				} else if (it.value->hint == PROPERTY_HINT_OBJECT_ID) {
					EditorPropertyObjectID *editor = memnew(EditorPropertyObjectID);
					editor->setup("Object");
					prop = editor;

				} else {
					EditorPropertyInteger *editor = memnew(EditorPropertyInteger);
					int min = 0, max = 65535, step = 1;
					bool greater = true, lesser = true;

					if (it.value->hint == PROPERTY_HINT_RANGE && it.value->hint_string.get_slice_count(",") >= 2) {
						greater = false; //if using ranged, assume false by default
						lesser = false;
						min = it.value->hint_string.get_slice(",", 0).to_int();
						max = it.value->hint_string.get_slice(",", 1).to_int();

						if (it.value->hint_string.get_slice_count(",") >= 3) {
							step = it.value->hint_string.get_slice(",", 2).to_int();
						}

						for (int i = 2; i < it.value->hint_string.get_slice_count(","); i++) {
							const String slice = it.value->hint_string.get_slice(",", i).strip_edges();
							if (slice == "or_greater") {
								greater = true;
							}
							if (slice == "or_lesser") {
								lesser = true;
							}
						}
					}

					editor->setup(min, max, step, greater, lesser);
					prop = editor;
				}
			} break;
			case Variant::FLOAT: {
				if (it.value->hint == PROPERTY_HINT_EXP_EASING) {
					EditorPropertyEasing *editor = memnew(EditorPropertyEasing);
					bool full = true;
					bool flip = false;
					Vector<String> hints = it.value->hint_string.split(",");
					for (int i = 0; i < hints.size(); i++) {
						String h = hints[i].strip_edges();
						if (h == "attenuation") {
							flip = true;
						}
						if (h == "inout") {
							full = true;
						}
					}

					editor->setup(full, flip);
					prop = editor;

				} else {
					EditorPropertyFloat *editor = memnew(EditorPropertyFloat);
					double min = -65535, max = 65535, step = default_float_step;
					bool hide_slider = true;
					bool exp_range = false;
					bool greater = true, lesser = true;

					if ((it.value->hint == PROPERTY_HINT_RANGE || it.value->hint == PROPERTY_HINT_EXP_RANGE) && it.value->hint_string.get_slice_count(",") >= 2) {
						greater = false; //if using ranged, assume false by default
						lesser = false;
						min = it.value->hint_string.get_slice(",", 0).to_float();
						max = it.value->hint_string.get_slice(",", 1).to_float();
						if (it.value->hint_string.get_slice_count(",") >= 3) {
							step = it.value->hint_string.get_slice(",", 2).to_float();
						}
						hide_slider = false;
						exp_range = it.value->hint == PROPERTY_HINT_EXP_RANGE;
						for (int i = 2; i < it.value->hint_string.get_slice_count(","); i++) {
							const String slice = it.value->hint_string.get_slice(",", i).strip_edges();
							if (slice == "or_greater") {
								greater = true;
							}
							if (slice == "or_lesser") {
								lesser = true;
							}
						}
					}

					editor->setup(min, max, step, hide_slider, exp_range, greater, lesser);
					prop = editor;
				}

			} break;
			case Variant::STRING: {
				if (it.value->hint == PROPERTY_HINT_ENUM) {
					EditorPropertyTextEnum *editor = memnew(EditorPropertyTextEnum);
					Vector<String> options = it.value->hint_string.split(",");
					editor->setup(options);
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_MULTILINE_TEXT) {
					EditorPropertyMultilineText *editor = memnew(EditorPropertyMultilineText);
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_TYPE_STRING) {
					EditorPropertyClassName *editor = memnew(EditorPropertyClassName);
					editor->setup("Object", it.value->hint_string);
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_DIR || it.value->hint == PROPERTY_HINT_FILE || it.value->hint == PROPERTY_HINT_SAVE_FILE || it.value->hint == PROPERTY_HINT_GLOBAL_DIR || it.value->hint == PROPERTY_HINT_GLOBAL_FILE) {
					Vector<String> extensions = it.value->hint_string.split(",");
					bool global = it.value->hint == PROPERTY_HINT_GLOBAL_DIR || it.value->hint == PROPERTY_HINT_GLOBAL_FILE;
					bool folder = it.value->hint == PROPERTY_HINT_DIR || it.value->hint == PROPERTY_HINT_GLOBAL_DIR;
					bool save = it.value->hint == PROPERTY_HINT_SAVE_FILE;
					EditorPropertyPath *editor = memnew(EditorPropertyPath);
					editor->setup(extensions, folder, global);
					if (save) {
						editor->set_save_mode();
					}
					prop = editor;

				} else if (it.value->hint == PROPERTY_HINT_METHOD_OF_VARIANT_TYPE ||
						   it.value->hint == PROPERTY_HINT_METHOD_OF_BASE_TYPE ||
						   it.value->hint == PROPERTY_HINT_METHOD_OF_INSTANCE ||
						   it.value->hint == PROPERTY_HINT_METHOD_OF_SCRIPT ||
						   it.value->hint == PROPERTY_HINT_PROPERTY_OF_VARIANT_TYPE ||
						   it.value->hint == PROPERTY_HINT_PROPERTY_OF_BASE_TYPE ||
						   it.value->hint == PROPERTY_HINT_PROPERTY_OF_INSTANCE ||
						   it.value->hint == PROPERTY_HINT_PROPERTY_OF_SCRIPT) {
					EditorPropertyMember *editor = memnew(EditorPropertyMember);

					EditorPropertyMember::Type type = EditorPropertyMember::MEMBER_METHOD_OF_BASE_TYPE;
					switch (it.value->hint) {
						case PROPERTY_HINT_METHOD_OF_BASE_TYPE:
							type = EditorPropertyMember::MEMBER_METHOD_OF_BASE_TYPE;
							break;
						case PROPERTY_HINT_METHOD_OF_INSTANCE:
							type = EditorPropertyMember::MEMBER_METHOD_OF_INSTANCE;
							break;
						case PROPERTY_HINT_METHOD_OF_SCRIPT:
							type = EditorPropertyMember::MEMBER_METHOD_OF_SCRIPT;
							break;
						case PROPERTY_HINT_PROPERTY_OF_VARIANT_TYPE:
							type = EditorPropertyMember::MEMBER_PROPERTY_OF_VARIANT_TYPE;
							break;
						case PROPERTY_HINT_PROPERTY_OF_BASE_TYPE:
							type = EditorPropertyMember::MEMBER_PROPERTY_OF_BASE_TYPE;
							break;
						case PROPERTY_HINT_PROPERTY_OF_INSTANCE:
							type = EditorPropertyMember::MEMBER_PROPERTY_OF_INSTANCE;
							break;
						case PROPERTY_HINT_PROPERTY_OF_SCRIPT:
							type = EditorPropertyMember::MEMBER_PROPERTY_OF_SCRIPT;
							break;
						default: {
						}
					}
					editor->setup(type, it.value->hint_string);
					prop = editor;

				} else {
					EditorPropertyText *editor = memnew(EditorPropertyText);
					if (it.value->hint == PROPERTY_HINT_PLACEHOLDER_TEXT) {
						editor->set_placeholder(it.value->hint_string);
					}
					prop = editor;
				}
			} break;

#define SETUP_MATH_RANGE(editor, prop_info, type)                                                     \
	type min = -65535, max = 65535;                                                                   \
	bool hide_slider = true;                                                                          \
                                                                                                      \
	if (prop_info->hint == PROPERTY_HINT_RANGE && prop_info->hint_string.get_slice_count(",") >= 2) { \
		min = it.value->hint_string.get_slice(",", 0).to_float();                                     \
		max = it.value->hint_string.get_slice(",", 1).to_float();                                     \
		hide_slider = false;                                                                          \
	}                                                                                                 \
                                                                                                      \
	editor->setup(min, max, hide_slider);

#define SETUP_MATH_RANGE_WITH_STEP(editor, prop_info, type)                                           \
	type min = -65535, max = 65535, step = default_float_step;                                        \
	bool hide_slider = true;                                                                          \
                                                                                                      \
	if (prop_info->hint == PROPERTY_HINT_RANGE && prop_info->hint_string.get_slice_count(",") >= 2) { \
		min = prop_info->hint_string.get_slice(",", 0).to_float();                                    \
		max = prop_info->hint_string.get_slice(",", 1).to_float();                                    \
		if (prop_info->hint_string.get_slice_count(",") >= 3) {                                       \
			step = prop_info->hint_string.get_slice(",", 2).to_float();                               \
		}                                                                                             \
		hide_slider = false;                                                                          \
	}                                                                                                 \
                                                                                                      \
	editor->setup(min, max, step, hide_slider);
			// math types
			case Variant::VECTOR2: {
				EditorPropertyVector2 *editor = memnew(EditorPropertyVector2);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::VECTOR2I: {
				EditorPropertyVector2i *editor = memnew(EditorPropertyVector2i);
				SETUP_MATH_RANGE(editor, it.value, int);
				prop = editor;

			} break;
			case Variant::RECT2: {
				EditorPropertyRect2 *editor = memnew(EditorPropertyRect2);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::RECT2I: {
				EditorPropertyRect2i *editor = memnew(EditorPropertyRect2i);
				SETUP_MATH_RANGE(editor, it.value, int);
				prop = editor;

			} break;
			case Variant::VECTOR3: {
				EditorPropertyVector3 *editor = memnew(EditorPropertyVector3);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::VECTOR3I: {
				EditorPropertyVector3i *editor = memnew(EditorPropertyVector3i);
				SETUP_MATH_RANGE(editor, it.value, int);
				prop = editor;

			} break;
			case Variant::TRANSFORM2D: {
				EditorPropertyTransform2D *editor = memnew(EditorPropertyTransform2D);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::PLANE: {
				EditorPropertyPlane *editor = memnew(EditorPropertyPlane);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::QUAT: {
				EditorPropertyQuat *editor = memnew(EditorPropertyQuat);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::AABB: {
				EditorPropertyAABB *editor = memnew(EditorPropertyAABB);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::BASIS: {
				EditorPropertyBasis *editor = memnew(EditorPropertyBasis);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;
			case Variant::TRANSFORM: {
				EditorPropertyTransform *editor = memnew(EditorPropertyTransform);
				SETUP_MATH_RANGE_WITH_STEP(editor, it.value, double);
				prop = editor;

			} break;

			// misc types
			case Variant::COLOR: {
				EditorPropertyColor *editor = memnew(EditorPropertyColor);
				editor->setup(it.value->hint != PROPERTY_HINT_COLOR_NO_ALPHA);
				prop = editor;

			} break;
			case Variant::STRING_NAME: {
				if (it.value->hint == PROPERTY_HINT_ENUM) {
					EditorPropertyTextEnum *editor = memnew(EditorPropertyTextEnum);
					Vector<String> options = it.value->hint_string.split(",");
					editor->setup(options, true);
					prop = editor;
				} else {
					EditorPropertyText *editor = memnew(EditorPropertyText);
					if (it.value->hint == PROPERTY_HINT_PLACEHOLDER_TEXT) {
						editor->set_placeholder(it.value->hint_string);
					}
					editor->set_string_name(true);
					prop = editor;
				}

			} break;
			case Variant::NODE_PATH: {
				EditorPropertyNodePath *editor = memnew(EditorPropertyNodePath);
				const int usage = 0; // TODO how to integrate this? check /godot/editor/editor_properties.cpp::parse_property
				if (it.value->hint == PROPERTY_HINT_NODE_PATH_TO_EDITED_NODE && it.value->hint_string != String()) {
					editor->setup(it.value->hint_string, Vector<StringName>(), (usage & PROPERTY_USAGE_NODE_PATH_FROM_SCENE_ROOT));
				}
				if (it.value->hint == PROPERTY_HINT_NODE_PATH_VALID_TYPES && it.value->hint_string != String()) {
					Vector<String> types = it.value->hint_string.split(",", false);
					Vector<StringName> sn = Variant(types); //convert via variant
					editor->setup(NodePath(), sn, (usage & PROPERTY_USAGE_NODE_PATH_FROM_SCENE_ROOT));
				}
				prop = editor;

			} break;
			case Variant::_RID: {
				prop = memnew(EditorPropertyRID);

			} break;
			case Variant::OBJECT: {
				EditorPropertyResource *editor = memnew(EditorPropertyResource);
				if (it.value->hint == PROPERTY_HINT_RESOURCE_TYPE) {
					editor->setup(it.value->hint_string);
					const String open_in_new = EDITOR_GET("interface/inspector/resources_to_open_in_new_inspector");
					for (int i = 0; i < open_in_new.get_slice_count(","); i++) {
						const String type = open_in_new.get_slicec(',', i).strip_edges();
						for (int j = 0; j < it.value->hint_string.get_slice_count(","); j++) {
							String inherits = it.value->hint_string.get_slicec(',', j);
							if (ClassDB::is_parent_class(inherits, type)) {
								editor->set_use_sub_inspector(false);
							}
						}
					}
				} else {
					editor->setup("Resource");
				}
				prop = editor;

			} break;
			case Variant::DICTIONARY: {
				prop = memnew(EditorPropertyDictionary);

			} break;
			case Variant::ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::ARRAY, it.value->hint_string);
				prop = editor;
			} break;

			// arrays
			case Variant::PACKED_BYTE_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_BYTE_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_INT32_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_INT32_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_FLOAT32_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_FLOAT32_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_INT64_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_INT64_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_FLOAT64_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_FLOAT64_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_STRING_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_STRING_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_VECTOR2_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_VECTOR2_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_VECTOR3_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_VECTOR3_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			case Variant::PACKED_COLOR_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_COLOR_ARRAY, it.value->hint_string);
				prop = editor;
			} break;
			default: {
			}
		}

		if (prop != nullptr) {
			prop->set_label(it.value->name.capitalize());
			p_container->add_child(prop);

			prop->set_object_and_property(entity, String(p_component_name) + "/" + it.value->name);
			prop->connect("property_changed", callable_mp(this, &EntityEditor::_property_changed));
			editor_properties.insert(it.value->name, prop);
		}
	}
	components_properties.insert(p_component_name, editor_properties);
}

void EntityEditor::update_component_inspector(StringName p_component_name) {
	OAHashMap<StringName, EditorProperty *> *props = components_properties.lookup_ptr(p_component_name);
	if (props == nullptr) {
		return;
	}

	for (OAHashMap<StringName, EditorProperty *>::Iterator it = props->iter(); it.valid; it = props->next_iter(it)) {
		if ((*it.value) != nullptr) {
			(*it.value)->update_property();
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

void EntityEditor::_property_changed(const String &p_path, const Variant &p_value, const String &p_name, bool p_changing) {
	if (p_changing) {
		// Nothing to do while chaning.
		return;
	}

	const Vector<String> names = p_path.split("/");
	ERR_FAIL_COND(names.size() < 2);
	const String component_name = names[0];
	const String property_name = names[1];

	entity->set_component_data_value(component_name, property_name, p_value);
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