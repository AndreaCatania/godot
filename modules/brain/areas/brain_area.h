#ifndef BRAIN_AREA_H
#define BRAIN_AREA_H

#include "scene/main/node.h"

#include "thirdparty/brain/brain/areas/brain_area.h"

class SynapticTerminals : public Reference {
	GDCLASS(SynapticTerminals, Reference);

	friend class BrainArea;

	brain::Matrix matrix;

	static void _bind_methods();

public:
	int terminal_count() const;
	real_t get(int p_row) const;
};

class BrainArea : public Node {
	GDCLASS(BrainArea, Node);

public:
	enum Activation {
		ACTIVATION_SIGMOID,
		ACTIVATION_MAX
	};

private:
	brain::BrainArea brain_area;
	brain::BrainArea::LearningCache learning_cache;

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

public:
	BrainArea();

	void set_input_layer_size(int p_size);
	int get_input_layer_size();

	void set_hidden_layers_count(int p_count);
	int get_hidden_layers_count() const;

	void set_hidden_layer_size(int p_hidden_layer, int p_size);
	int get_hidden_layer_size(int p_hidden_layer) const;

	void set_hidden_layer_activation(int p_hidden_layer, Activation p_activation);
	Activation get_hidden_layer_activation(int p_hidden_layer) const;

	void set_output_layer_size(int p_size);
	int get_output_layer_size();

	void prepare_to_learn();
	real_t learn(const Vector<real_t> &p_input, const Vector<real_t> &p_expected, real_t p_learning_rate);
	Ref<SynapticTerminals> guess(const Vector<real_t> &p_input);

	void save_knowledge(const String &p_path, bool p_overwrite = false);
	void load_knowledge(const String &p_path);
};

VARIANT_ENUM_CAST(BrainArea::Activation);

#endif // BRAIN_AREA_H
