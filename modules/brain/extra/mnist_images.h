#pragma once

#include "scene/main/node.h"
#include "thirdparty/mnist/mnist_reader.hpp"

/**
 * @brief The MnistImages class
 *
 * http://yann.lecun.com/exdb/mnist/
 * This resource loads the mnist image database inside
 * To correctly use it you must pass the directory where you have extracted
 * all database downloaded from http://yann.lecun.com/exdb/mnist/
 *
 * The images are 28x28
 */
class MnistImages : public Node {

	GDCLASS(MnistImages, Node);

	String mnist_path;
	bool lock;
	mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> dataset;

	static void _bind_methods();

public:
	MnistImages();

	void set_mnist_path(String p_path);
	String get_mnist_path() const;

	void load();
	void unload();

	int training_image_count() const;
	const uint8_t *get_training_image(int p_index) const;
	uint8_t get_training_label(int p_index) const;

	int test_image_count() const;
	const uint8_t *get_test_image(int p_index) const;
	uint8_t get_test_label(int p_index) const;

	void print_image(int p_index, bool p_from_training = true) const;
};
