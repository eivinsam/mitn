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
	};

	struct Node
	{
		std::string name;
		Edges edges;

	};

	Edges from(const std::string& data);

}
