
#include "register_types.h"

#include "areas/brain_area.h"
#include "extra/mnist_images.h"
#include "thirdparty/brain/brain/error_handler.h"

brain::ErrorHandlerList *error_handler;

void print_error_callback(
		void *p_user_data,
		const char *p_function,
		const char *p_file,
		int p_line,
		const char *p_error,
		const char *p_explain,
		brain::ErrorHandlerType p_type) {

	std::string msg =
			std::string() +
			(p_type == brain::ERR_HANDLER_ERROR ? "[ERROR] " : "[WARN]") +
			p_file +
			" Function: " + p_function +
			", line: " +
			brain::itos(p_line) +
			"\n\t" + p_error +
			" " + p_explain;

	ERR_PRINT(msg.c_str());
}

void register_brain_types() {
	ClassDB::register_class<BrainArea>();
	ClassDB::register_class<SynapticTerminals>();
	ClassDB::register_class<MnistImages>();

	error_handler = new brain::ErrorHandlerList;
	error_handler->errfunc = print_error_callback;
	brain::add_error_handler(error_handler);
}

void unregister_brain_types() {
	delete error_handler;
	error_handler = NULL;
}
