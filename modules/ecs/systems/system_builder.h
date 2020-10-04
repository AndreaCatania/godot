
/** @author AndreaCatania */

#pragma once

#include "modules/ecs/iterators/query.h"
#include "modules/ecs/resources/ecs_resource.h"
#include <type_traits>

// TODO put all this into a CPP or a namespace?

namespace SystemBuilder {

template <bool B>
struct bool_type {};

template <typename Test, template <typename...> class Ref>
struct is_specialization : bool_type<false> {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : bool_type<true> {};

template <class Q>
void extract_info(bool_type<true>, SystemInfo &r_info) {
	// This is a query.
	Q::get_components(r_info.mutable_components, r_info.immutable_components);
}

template <class R>
void extract_info(bool_type<false>, SystemInfo &r_info) {
	// This is a resource.
	if (std::is_const<R>()) {
		r_info.immutable_resources.push_back(R::get_resource_id());
	} else {
		r_info.mutable_resources.push_back(R::get_resource_id());
	}
}

template <class... Cs>
struct InfoConstructor {
	InfoConstructor(SystemInfo &r_info) {}
};

template <class C, class... Cs>
struct InfoConstructor<C, Cs...> : InfoConstructor<Cs...> {
	InfoConstructor(SystemInfo &r_info) :
			InfoConstructor<Cs...>(r_info) {
		extract_info<std::remove_reference_t<C>>(is_specialization<std::remove_reference_t<C>, Query>(), r_info);
	}
};

/// Creates a SystemInfo, extracting the information from a system function.
template <class... RCs>
SystemInfo get_system_info_from_function(void (*system_func)(RCs...)) {
	SystemInfo si;
	InfoConstructor<RCs...> a(si);
	return si;
}

// This is an utility used to convert the type to a reference.
// The keyword `auto` doesn't take into account the reference `&`, so it's
// necessary wrap the type to preserve the reference.
template <class C>
struct Container {
	C inner;

	template <class... Cs>
	Container(Query<Cs...> p_query) :
			inner(p_query) {}

	Container(C p_inner) :
			inner(p_inner) {}

	C &get_inner() {
		return inner;
	}
};

template <class C>
Container<C> obtain_query_or_resource(bool_type<true>, Pipeline *p_pipeline) {
	return Container<C>(C(p_pipeline));
}

template <class C>
Container<C &> obtain_query_or_resource(bool_type<false>, Pipeline *p_pipeline) {
	return Container<C &>(p_pipeline->get_resource<C>());
}

#define OBTAIN(name, T, pipeline) auto name = obtain_query_or_resource<std::remove_reference_t<T>>(is_specialization<std::remove_reference_t<T>, Query>(), pipeline);

// ~~~~ system_exec_func definition ~~~~ //

template <class A>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A)) {
	OBTAIN(a, A, p_pipeline);

	p_system(
			a.get_inner());
}

template <class A, class B>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner());
}

template <class A, class B, class C>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner());
}

template <class A, class B, class C, class D>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner());
}

template <class A, class B, class C, class D, class E>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner());
}

template <class A, class B, class C, class D, class E, class F>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U, class V>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);
	OBTAIN(v, V, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner(),
			v.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U, class V, class W>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);
	OBTAIN(v, V, p_pipeline);
	OBTAIN(w, W, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner(),
			v.get_inner(),
			w.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U, class V, class W, class X>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);
	OBTAIN(v, V, p_pipeline);
	OBTAIN(w, W, p_pipeline);
	OBTAIN(x, X, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner(),
			v.get_inner(),
			w.get_inner(),
			x.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U, class V, class W, class X, class Y>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);
	OBTAIN(v, V, p_pipeline);
	OBTAIN(w, W, p_pipeline);
	OBTAIN(x, X, p_pipeline);
	OBTAIN(y, Y, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner(),
			v.get_inner(),
			w.get_inner(),
			x.get_inner(),
			y.get_inner());
}

template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M, class N, class O, class P, class Q, class R, class S, class T, class U, class V, class W, class X, class Y, class Z>
void system_exec_func(Pipeline *p_pipeline, void (*p_system)(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z)) {
	OBTAIN(a, A, p_pipeline);
	OBTAIN(b, B, p_pipeline);
	OBTAIN(c, C, p_pipeline);
	OBTAIN(d, D, p_pipeline);
	OBTAIN(e, E, p_pipeline);
	OBTAIN(f, F, p_pipeline);
	OBTAIN(g, G, p_pipeline);
	OBTAIN(h, H, p_pipeline);
	OBTAIN(i, I, p_pipeline);
	OBTAIN(j, J, p_pipeline);
	OBTAIN(k, K, p_pipeline);
	OBTAIN(l, L, p_pipeline);
	OBTAIN(m, M, p_pipeline);
	OBTAIN(n, N, p_pipeline);
	OBTAIN(o, O, p_pipeline);
	OBTAIN(p, P, p_pipeline);
	OBTAIN(q, Q, p_pipeline);
	OBTAIN(r, R, p_pipeline);
	OBTAIN(s, S, p_pipeline);
	OBTAIN(t, T, p_pipeline);
	OBTAIN(u, U, p_pipeline);
	OBTAIN(v, V, p_pipeline);
	OBTAIN(w, W, p_pipeline);
	OBTAIN(x, X, p_pipeline);
	OBTAIN(y, Y, p_pipeline);
	OBTAIN(z, Z, p_pipeline);

	p_system(
			a.get_inner(),
			b.get_inner(),
			c.get_inner(),
			d.get_inner(),
			e.get_inner(),
			f.get_inner(),
			g.get_inner(),
			h.get_inner(),
			i.get_inner(),
			j.get_inner(),
			k.get_inner(),
			l.get_inner(),
			m.get_inner(),
			n.get_inner(),
			o.get_inner(),
			p.get_inner(),
			q.get_inner(),
			r.get_inner(),
			s.get_inner(),
			t.get_inner(),
			u.get_inner(),
			v.get_inner(),
			w.get_inner(),
			x.get_inner(),
			y.get_inner(),
			z.get_inner());
}

#undef OBTAIN

} // namespace SystemBuilder