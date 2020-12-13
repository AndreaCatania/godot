#pragma once

#include "component.h"

template <int a>
class VariantComponent {
	VariantComponent(StringName p_component_name) {
	}
};

class Variant1Component_0 : public godex::Component {
	COMPONENT(Variant1Component_0, DenseVector)

	Variant var_1;

public:
	Variant1Component_0() {}
};

class Variant1Component_1 : public godex::Component {
	COMPONENT(Variant1Component_1, DenseVector)

	Variant var_1;

public:
	Variant1Component_1() {}
};

//void get_type() {
//	OAHashMap<StringName, uint32_t> script;
//	script.push_back("TransormComponent");
//
//	uint32_t index;
//	switch (index) {
//		case 0:
//			break;
//		default:
//			// TODO what to do in this case??
//	}
//}
