#include "mnist_images.h"

#include "core/error_macros.h"
#include "core/print_string.h"
#include "thirdparty/mnist/mnist_reader.hpp"
#include <vector>

void MnistImages::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mnist_path", "path"), &MnistImages::set_mnist_path);
	ClassDB::bind_method(D_METHOD("get_mnist_path"), &MnistImages::get_mnist_path);

	ClassDB::bind_method(D_METHOD("load"), &MnistImages::load);
	ClassDB::bind_method(D_METHOD("unload"), &MnistImages::unload);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "mnist_path"), "set_mnist_path", "get_mnist_path");
}

MnistImages::MnistImages() :
		mnist_path(""),
		lock(false) {
	unload();
}

void MnistImages::set_mnist_path(String p_path) {
	ERR_FAIL_COND(lock);
	mnist_path = p_path;
}

String MnistImages::get_mnist_path() const {
	return mnist_path;
}

void MnistImages::load() {
	lock = true;
	dataset = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(std::string(mnist_path.utf8().get_data()));
}

void MnistImages::unload() {
	lock = false;
	dataset.training_images.resize(0);
	dataset.training_labels.resize(0);
	dataset.test_images.resize(0);
	dataset.test_labels.resize(0);
}

int MnistImages::training_image_count() const {
	ERR_FAIL_COND_V(!lock, 0);
	return dataset.training_images.size();
}

const uint8_t *MnistImages::get_training_image(int p_index) const {
	ERR_FAIL_COND_V(!lock, nullptr);
	ERR_FAIL_COND_V(training_image_count() < p_index, nullptr);
	return dataset.training_images[p_index].data();
}

uint8_t MnistImages::get_training_label(int p_index) const {
	ERR_FAIL_COND_V(!lock, 0);
	ERR_FAIL_COND_V(training_image_count() < p_index, 0);
	return dataset.training_labels[p_index];
}

int MnistImages::test_image_count() const {
	ERR_FAIL_COND_V(!lock, 0);
	return dataset.test_images.size();
}

const uint8_t *MnistImages::get_test_image(int p_index) const {
	ERR_FAIL_COND_V(!lock, nullptr);
	ERR_FAIL_COND_V(test_image_count() < p_index, nullptr);
	return dataset.test_images[p_index].data();
}

uint8_t MnistImages::get_test_label(int p_index) const {
	ERR_FAIL_COND_V(!lock, 0);
	ERR_FAIL_COND_V(test_image_count() < p_index, 0);
	return dataset.test_labels[p_index];
}

void MnistImages::print_image(int p_index, bool p_from_training) const {
	const uint8_t *image = p_from_training ? get_training_image(p_index) : get_test_image(p_index);
	String s("\n");
	for (int v = 0; v < 28; ++v) {
		for (int h = 0; h < 28; ++h) {
			uint8_t pixel = image[v * 28 + h];
			s += pixel == 0 ? " " : pixel < 125 ? "+" : "X";
		}
		s += "\n";
	}
	print_line(s);
	print_line("Number: " + itos(p_from_training ? get_training_label(p_index) : get_test_label(p_index)));
}
