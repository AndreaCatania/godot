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

	brain::BrainArea brain_area;
	brain::BrainArea::LearningCache learning_cache;

	static void _bind_methods();

public:
	BrainArea();

	void set_input_layer_size(int p_size);
	int get_input_layer_size();

	void set_hidden_layers_count(int p_count);
	int get_hidden_layers_count();

	void set_output_layer_size(int p_size);
	int get_output_layer_size();

	void prepare_to_learn();
	real_t learn(const Vector<real_t> &p_input, const Vector<real_t> &p_expected, real_t p_learning_rate);
	Ref<SynapticTerminals> guess(const Vector<real_t> &p_input);

	void save_knowledge(const String &p_path, bool p_overwrite = false);
	void load_knowledge(const String &p_path);
};

#endif // BRAIN_AREA_H
