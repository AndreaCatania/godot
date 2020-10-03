
/** @author AndreaCatania */

#pragma once

#include "modules/ecs/iterators/query.h"
#include "modules/ecs/resources/ecs_resource.h"
#include <type_traits>

// TODO put all this into a CPP or a namespace?

template <bool B>
struct bool_type {};

template <class R>
void extract_resource_info(bool_type<true>, SystemInfo &r_info) {
	if (std::is_const<R>()) {
		r_info.immutable_resources.push_back(R::get_resource_id());
	} else {
		r_info.mutable_resources.push_back(R::get_resource_id());
	}
}

template <class R>
void extract_resource_info(bool_type<false>, SystemInfo &r_info) {}

template <class Q>
void extract_query_info(bool_type<true>, SystemInfo &r_info) {
	Q::get_components(r_info.mutable_components, r_info.immutable_components);
}

template <class Q>
void extract_query_info(bool_type<false>, SystemInfo &r_info) {}

template <class... Cs>
struct InfoConstructor {
	InfoConstructor(SystemInfo &r_info) {
		// Nothing to do.
	}
};

template <typename Test, template <typename...> class Ref>
struct is_specialization : bool_type<false> {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : bool_type<true> {};

template <class C, class... Cs>
struct InfoConstructor<C, Cs...> : InfoConstructor<Cs...> {
	InfoConstructor(SystemInfo &r_info) :
			InfoConstructor<Cs...>(r_info) {
		extract_resource_info<std::remove_reference_t<C>>(bool_type<std::is_base_of<ECSResource, std::remove_reference_t<C>>::value>(), r_info);
		extract_query_info<std::remove_reference_t<C>>(is_specialization<std::remove_reference_t<C>, Query>(), r_info);
	}
};

/// Creates a SystemInfo, extracting the information from a system function.
template <class... RCs>
SystemInfo from_function() {
	SystemInfo si;
	InfoConstructor<RCs...> a(si);
	return si;
}
