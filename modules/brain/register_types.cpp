
#include "register_types.h"

#include "areas/sharp_brain_area.h"
#include "areas/uniform_brain_area.h"
#include "neat/neat_population.h"
#include "thirdparty/brain/brain/error_handler.h"
#include "weights_raw_resource_format_loader.h"

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

static WeightsRawResourceFormatLoader *weight_raw_loader = NULL;
void register_brain_types() {
	ClassDB::register_class<SynapticTerminals>();
	ClassDB::register_virtual_class<BrainArea>();
	ClassDB::register_class<UniformBrainArea>();

	ClassDB::register_virtual_class<SharpBrainAreaStructure>();
	ClassDB::register_class<SharpBrainAreaStructureEditable>();
	ClassDB::register_class<SharpBrainAreaStructureAncestor>();
	ClassDB::register_class<SharpBrainAreaStructureRuntime>();
	ClassDB::register_class<SharpBrainAreaStructureFile>();
	ClassDB::register_class<SharpBrainArea>();

	ClassDB::register_virtual_class<Neat>();
	ClassDB::register_class<NeatPopulation>();

	error_handler = new brain::ErrorHandlerList;
	error_handler->errfunc = print_error_callback;
	brain::add_error_handler(error_handler);

	weight_raw_loader = memnew(WeightsRawResourceFormatLoader);
	ResourceLoader::add_resource_format_loader(weight_raw_loader);
	ClassDB::register_class<WeightsRawResourceFormatLoader>();
}

void unregister_brain_types() {
	delete error_handler;
	error_handler = NULL;

	memdelete(weight_raw_loader);
	weight_raw_loader = NULL;
}
