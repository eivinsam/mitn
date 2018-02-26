#pragma once

#include <vector>
#include <string>
#include <memory>

namespace mitn
{
	struct Node;

	class Edges
	{
		std::vector<Node> _data;
	public:

		template <class... Args>
		void push(Args&&... args) { _data.emplace_back(std::forward<Args>(args)...); }

		auto&& back()       { return _data.back(); }
		auto&& back() const { return _data.back(); }

		auto&& begin()       { return _data.begin(); }
		auto&& begin() const { return _data.begin(); }
		auto&& end()       { return _data.end(); }
		auto&& end() const { return _data.end(); }

		void write(std::string& out, int indent) const;
		std::string write() const { std::string out; write(out, 0); return out; }
	};

	struct Node
	{
		std::string name;
		Edges edges;

		void write(std::string& out, int indent) const;
		std::string write() const { std::string out; write(out, 0); return out; }
	};

	inline void Edges::write(std::string& out, int indent) const
	{
		for (auto&& n : _data)
			n.write(out, indent);
	}


	Edges read(const std::string& data);

}
