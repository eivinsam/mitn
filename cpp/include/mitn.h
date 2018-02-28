#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>

namespace mitn
{
	struct Node;

	class Edges
	{
		std::vector<Node> _data;
	public:

		template <class... Args>
		void push(Args&&... args) { _data.emplace_back(std::forward<Args>(args)...); }

		const auto size() const { return _data.size(); }

		auto&& back()       { return _data.back(); }
		auto&& back() const { return _data.back(); }

		auto begin()       { return _data.begin(); }
		auto begin() const { return _data.begin(); }
		auto end()       { return _data.end(); }
		auto end() const { return _data.end(); }

		void clear() { _data.clear(); }

		void write(std::string& out, int indent) const;
		std::string write() const { std::string out; write(out, 0); return out; }
	};

	struct Node
	{
		std::string name;
		Edges edges;

		Node() = default;
		Node(std::string name) : name(std::move(name)) { }

		void write(std::string& out, int indent) const;
		std::string write() const { std::string out; write(out, 0); return out; }

		template <class T>
		std::optional<T> as() const;
	};

	Edges read(const std::string& data);

	template <class Parent>
	struct Potential
	{
		Node* node;
		Parent parent;
		std::string_view edge;

		void realize();

		void operator=(std::string value)
		{
			if (!node)
				realize();
			node->edges.clear();
			node->edges.push(std::move(value));
		}
	};

	template <class Parent>
	inline void Potential<Parent>::realize()
	{
		if (!parent.node)
			parent.realize();
		parent.node->edges.push(std::string(edge));
		node = &parent.node->edges.back();
		parent.node = nullptr;
	}
	template <> 
	inline void Potential<Edges*>::realize()
	{
		parent->push(std::string(edge));
		node = &parent->back();
		parent = nullptr;
	}

	Potential<Edges*> operator/(Edges& edges, std::string_view edgename);
	inline auto operator/(Node& node, std::string_view edgename)
	{
		return node.edges / edgename;
	}
	template <class Parent>
	inline Potential<Potential<Parent>> operator/(Potential<Parent> pn,  std::string_view edge)
	{
		return { pn.node ? (*pn.node / edge).node : nullptr, pn, edge };
	}

	template <class T>
	struct Converter { };

	template <class T>
	static constexpr Converter<T> as = {};

	template <class Parent, class T>
	inline std::optional<T> operator/(Potential<Parent> p, Converter<T>)
	{
		if (!p.node)
			throw std::runtime_error("node does not exist");
		return p.node->as<T>();
	}

	template <>
	inline std::optional<std::string> Node::as<std::string>() const { return write(); }
}
