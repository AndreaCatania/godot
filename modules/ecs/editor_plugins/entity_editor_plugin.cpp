/* Author: AndreaCatania */

#include "entity_editor_plugin.h"
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

	for (OAHashMap<StringName, PropertyInfo>::Iterator it = properties->iter(); it.valid; it = properties->next_iter(it)) {
		EditorProperty *prop = nullptr;

		switch (it.value->type) {
			case Variant::NIL: {
				prop = memnew(EditorPropertyNil);

			} break;

			// atomic types
			case Variant::BOOL: {
				prop = memnew(EditorPropertyCheck);

			} break;
			case Variant::INT: {
				EditorPropertyInteger *editor = memnew(EditorPropertyInteger);
				editor->setup(-100000, 100000, 1, true, true);
				prop = editor;

			} break;
			case Variant::FLOAT: {
				EditorPropertyFloat *editor = memnew(EditorPropertyFloat);

				editor->setup(-100000, 100000, 0.001, true, false, true, true);
				prop = editor;
			} break;
			case Variant::STRING: {
				prop = memnew(EditorPropertyText);

			} break;

			// math types
			case Variant::VECTOR2: {
				EditorPropertyVector2 *editor = memnew(EditorPropertyVector2);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::VECTOR2I: {
				EditorPropertyVector2i *editor = memnew(EditorPropertyVector2i);
				editor->setup(-100000, 100000, true);
				prop = editor;

			} break;
			case Variant::RECT2: {
				EditorPropertyRect2 *editor = memnew(EditorPropertyRect2);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::RECT2I: {
				EditorPropertyRect2i *editor = memnew(EditorPropertyRect2i);
				editor->setup(-100000, 100000, true);
				prop = editor;

			} break;
			case Variant::VECTOR3: {
				EditorPropertyVector3 *editor = memnew(EditorPropertyVector3);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::VECTOR3I: {
				EditorPropertyVector3i *editor = memnew(EditorPropertyVector3i);
				editor->setup(-100000, 100000, true);
				prop = editor;

			} break;
			case Variant::TRANSFORM2D: {
				EditorPropertyTransform2D *editor = memnew(EditorPropertyTransform2D);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::PLANE: {
				EditorPropertyPlane *editor = memnew(EditorPropertyPlane);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::QUAT: {
				EditorPropertyQuat *editor = memnew(EditorPropertyQuat);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::AABB: {
				EditorPropertyAABB *editor = memnew(EditorPropertyAABB);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::BASIS: {
				EditorPropertyBasis *editor = memnew(EditorPropertyBasis);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;
			case Variant::TRANSFORM: {
				EditorPropertyTransform *editor = memnew(EditorPropertyTransform);
				editor->setup(-100000, 100000, 0.001, true);
				prop = editor;

			} break;

			// misc types
			case Variant::COLOR: {
				prop = memnew(EditorPropertyColor);

			} break;
			case Variant::STRING_NAME: {
				EditorPropertyText *ept = memnew(EditorPropertyText);
				ept->set_string_name(true);
				prop = ept;

			} break;
			case Variant::NODE_PATH: {
				prop = memnew(EditorPropertyNodePath);

			} break;
			case Variant::_RID: {
				prop = memnew(EditorPropertyRID);

			} break;
			case Variant::OBJECT: {
				//if (Object::cast_to<EncodedObjectAsID>(value)) {
				//	EditorPropertyObjectID *editor = memnew(EditorPropertyObjectID);
				//	editor->setup("Object");
				//	prop = editor;

				//} else {
				//	EditorPropertyResource *editor = memnew(EditorPropertyResource);
				//	editor->setup("Resource");
				//	prop = editor;
				//}

			} break;
			case Variant::DICTIONARY: {
				prop = memnew(EditorPropertyDictionary);

			} break;
			case Variant::ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::ARRAY);
				prop = editor;
			} break;

			// arrays
			case Variant::PACKED_BYTE_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_BYTE_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_INT32_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_INT32_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_FLOAT32_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_FLOAT32_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_INT64_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_INT64_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_FLOAT64_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_FLOAT64_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_STRING_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_STRING_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_VECTOR2_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_VECTOR2_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_VECTOR3_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_VECTOR3_ARRAY);
				prop = editor;
			} break;
			case Variant::PACKED_COLOR_ARRAY: {
				EditorPropertyArray *editor = memnew(EditorPropertyArray);
				editor->setup(Variant::PACKED_COLOR_ARRAY);
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
			components_properties.insert(p_component_name, prop);
		}
	}
}

void EntityEditor::update_component_inspector(StringName p_component_name) {
	const OAHashMap<StringName, PropertyInfo> *properties = ECS::get_component_properties(p_component_name);
	if (properties == nullptr) {
		return;
	}

	for (OAHashMap<StringName, PropertyInfo>::Iterator it = properties->iter(); it.valid; it = properties->next_iter(it)) {
		EditorProperty *prop = *components_properties.lookup_ptr(*it.key);
		const Variant value = entity->get_component_data_value(p_component_name, *it.key);
		prop->set_object_and_property
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