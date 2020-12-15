#include "variant_component.h"

ScriptComponentInfo::ScriptComponentInfo() {
}

Storage *ScriptComponentInfo::create_storage() {
	switch (storage_type) {
		case StorageType::DENSE_VECTOR:
			//properties.size()
			return memnew(DenseVector<VariantComponent<0>>);
		case StorageType::NONE:
		default:
			return nullptr;
	}
}
