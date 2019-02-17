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

bool BrainArea::_set(const StringName &p_name, const Variant &p_value) {
	String name(p_name);

	if (!name.begins_with("hidden_layer_"))
		return false;

	const int hidden_layer = name
									 .get_slicec('/', 0)
									 .get_slicec('_', 2)
									 .to_int();

	String what = name.get_slicec('/', 1);

	if (what == "size") {

		set_hidden_layer_size(hidden_layer, p_value);
	} else if (what == "activation") {

		set_hidden_layer_activation(
				hidden_layer,
				static_cast<Activation>(static_cast<int>(p_value)));
	} else {
		return false;
	}

	return true;
}

bool BrainArea::_get(const StringName &p_name, Variant &r_ret) const {

	String name(p_name);

	if (!name.begins_with("hidden_layer_"))
		return false;

	const int hidden_layer = name
									 .get_slicec('/', 0)
									 .get_slicec('_', 2)
									 .to_int();

	String what = name.get_slicec('/', 1);

	if (what == "size") {

		r_ret = get_hidden_layer_size(hidden_layer);
	} else if (what == "activation") {

		r_ret = get_hidden_layer_activation(hidden_layer);
	} else {
		return false;
	}

	return true;
}

void BrainArea::_get_property_list(List<PropertyInfo> *p_list) const {

	for (int i(0); i < get_hidden_layers_count(); ++i) {
		p_list->push_back(PropertyInfo(
				Variant::INT,
				"hidden_layer_" + itos(i) + "/size"));

		p_list->push_back(PropertyInfo(
				Variant::INT,
				"hidden_layer_" + itos(i) + "/activation",
				PROPERTY_HINT_ENUM,
				"Sigmoid"));
	}
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

	ClassDB::bind_method(D_METHOD("save_knowledge", "path", "overwrite"), &BrainArea::save_knowledge);
	ClassDB::bind_method(D_METHOD("load_knowledge", "path"), &BrainArea::load_knowledge);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "input_layer_size"), "set_input_layer_size", "get_input_layer_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hidden_layers_count"), "set_hidden_layers_count", "get_hidden_layers_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "output_layer_size"), "set_output_layer_size", "get_output_layer_size");

	BIND_ENUM_CONSTANT(ACTIVATION_SIGMOID);
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

	_change_notify();
}

int BrainArea::get_hidden_layers_count() const {
	return brain_area.get_hidden_layers_count();
}

void BrainArea::set_hidden_layer_size(int p_hidden_layer, int p_size) {
	ERR_FAIL_INDEX(p_hidden_layer, get_hidden_layers_count());
	brain_area.set_hidden_layer_size(p_hidden_layer, p_size);
}

int BrainArea::get_hidden_layer_size(int p_hidden_layer) const {
	ERR_FAIL_INDEX_V(p_hidden_layer, get_hidden_layers_count(), 0);
	return brain_area.get_hidden_layer_size(p_hidden_layer);
}

void BrainArea::set_hidden_layer_activation(int p_hidden_layer, Activation p_activation) {
	ERR_FAIL_INDEX(p_hidden_layer, get_hidden_layers_count());
	ERR_FAIL_INDEX(p_activation, ACTIVATION_MAX);
	brain_area.set_hidden_layer_activation(
			p_hidden_layer,
			static_cast<brain::BrainArea::Activation>(p_activation));
}

BrainArea::Activation BrainArea::get_hidden_layer_activation(int p_hidden_layer) const {
	ERR_FAIL_INDEX_V(p_hidden_layer, get_hidden_layers_count(), ACTIVATION_MAX);
	return static_cast<Activation>(brain_area.get_hidden_layer_activation(p_hidden_layer));
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

void BrainArea::save_knowledge(const String &p_path, bool p_overwrite) {

	ERR_FAIL_COND(p_overwrite == false && FileAccess::exists(p_path));
	Error e;
	FileAccess *f = FileAccess::open(p_path, FileAccess::WRITE, &e);

	if (e != OK) {
		ERR_EXPLAIN("Can't open file" + p_path + " because " + itos(e));
		ERR_FAIL();
	}

	std::vector<uint8_t> buffer;
	if (!brain_area.get_buffer(buffer)) {
		f->close();
		memdelete(f);
		ERR_EXPLAIN("Can't save knowledge. File: " + p_path);
		ERR_FAIL();
	}

	f->store_buffer(buffer.data(), buffer.size());

	f->close();
	memdelete(f);
}

void BrainArea::load_knowledge(const String &p_path) {

	ERR_FAIL_COND(!FileAccess::exists(p_path));

	Error e;
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ, &e);

	if (e != OK) {
		ERR_EXPLAIN("Can't open file because " + itos(e));
		ERR_FAIL();
	}

	std::vector<uint8_t> buffer;

	const uint32_t buffer_size = f->get_len();
	buffer.resize(buffer_size);

	const int readed = f->get_buffer(buffer.data(), buffer_size);
	if (readed != buffer_size) {
		f->close();
		memdelete(f);
		ERR_EXPLAIN("File corrupted: " + p_path);
		ERR_FAIL();
	}

	if (brain_area.is_buffer_corrupted(buffer)) {
		f->close();
		memdelete(f);
		ERR_EXPLAIN("File corrupted: " + p_path);
		ERR_FAIL();
	}

	if (!brain_area.is_buffer_compatible(buffer)) {
		f->close();
		memdelete(f);
		ERR_EXPLAIN("This knowledge has a different brain structure: " + p_path);
		ERR_FAIL();
	}

	if (!brain_area.set_buffer(buffer)) {
		f->close();
		memdelete(f);
		ERR_EXPLAIN("File corrupted: " + p_path);
		ERR_FAIL();
	}

	f->close();
	memdelete(f);
}
