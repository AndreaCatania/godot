#include "brain_area.h"

#include "core/os/os.h"
#include "thirdparty/brain/brain/math/math_funcs.h"

void SynapticTerminals::_bind_methods() {

	ClassDB::bind_method(D_METHOD("terminal_count"), &SynapticTerminals::terminal_count);
	ClassDB::bind_method(D_METHOD("get_value", "index"), &SynapticTerminals::get);
}

int SynapticTerminals::terminal_count() const {
	return matrix.get_row_count();
}

real_t SynapticTerminals::get(int p_index) const {
	return matrix.get(p_index, 0);
}

void BrainArea::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_input_layer_size", "size"), &BrainArea::set_input_layer_size);
	ClassDB::bind_method(D_METHOD("get_input_layer_size"), &BrainArea::get_input_layer_size);

	ClassDB::bind_method(D_METHOD("set_hidden_layers_count", "count"), &BrainArea::set_hidden_layers_count);
	ClassDB::bind_method(D_METHOD("get_hidden_layers_count"), &BrainArea::get_hidden_layers_count);

	ClassDB::bind_method(D_METHOD("set_output_layer_size", "size"), &BrainArea::set_output_layer_size);
	ClassDB::bind_method(D_METHOD("get_output_layer_size"), &BrainArea::get_output_layer_size);

	ClassDB::bind_method(D_METHOD("prepare_to_learn"), &BrainArea::prepare_to_learn);
	ClassDB::bind_method(D_METHOD("learn", "input", "expected", "learning_rate"), &BrainArea::learn);
	ClassDB::bind_method(D_METHOD("guess", "input"), &BrainArea::guess);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "input_layer_size"), "set_input_layer_size", "get_input_layer_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hidden_layers_count"), "set_hidden_layers_count", "get_hidden_layers_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "output_layer_size"), "set_output_layer_size", "get_output_layer_size");
}

BrainArea::BrainArea() {}

void BrainArea::set_input_layer_size(int p_size) {
	brain_area.set_input_layer_size(p_size);
}

int BrainArea::get_input_layer_size() {
	return brain_area.get_input_layer_size();
}

void BrainArea::set_hidden_layers_count(int p_count) {

	p_count = MAX(0, p_count);

	const int prev = brain_area.get_hidden_layers_count();
	brain_area.set_hidden_layers_count(p_count);

	// Auto initialization
	if (prev < p_count) {
		const int size(brain_area.get_input_layer_size());

		for (int i(prev); i < p_count; ++i) {
			brain_area.set_hidden_layer(
					i,
					size,
					brain::BrainArea::ACTIVATION_SIGMOID);
		}
	}
}

int BrainArea::get_hidden_layers_count() {
	return brain_area.get_hidden_layers_count();
}

void BrainArea::set_output_layer_size(int p_size) {
	brain_area.set_output_layer_size(p_size);
}

int BrainArea::get_output_layer_size() {
	return brain_area.get_output_layer_size();
}

void BrainArea::prepare_to_learn() {
	brain::Math::seed(OS::get_singleton()->get_unix_time());
	brain_area.randomize_weights(1);
	brain_area.randomize_biases(1);
}

real_t BrainArea::learn(
		const Vector<real_t> &p_input,
		const Vector<real_t> &p_expected,
		real_t learning_rate) {

	brain::Matrix input(p_input.size(), 1, p_input.ptr());
	brain::Matrix expected(p_expected.size(), 1, p_expected.ptr());

	return brain_area.learn(input, expected, learning_rate, &learning_cache);
}

Ref<SynapticTerminals> BrainArea::guess(const Vector<real_t> &p_input) {

	brain::Matrix input(p_input.size(), 1, p_input.ptr());

	Ref<SynapticTerminals> output;
	output.instance();
	brain_area.guess(input, output->matrix);
	return output;
}
