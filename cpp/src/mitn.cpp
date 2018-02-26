#include <mitn.h>

#include <cstdio>

namespace mitn
{
	using byte = unsigned char;

	static constexpr byte LF  = '\n';
	static constexpr byte CR  = '\r';
	static constexpr byte TAB = '\t';
	static constexpr byte SPACE  = ' ';

	static inline int isValid(byte b)
	{
		switch (b)
		{
		case LF: case CR: case TAB: return true;
		default: return b >= SPACE;
		}
	}

	static int readIndent(const byte*& src)
	{
		int result = 0;
		for (;; ++src) switch (*src)
		{
		case CR: continue;
		case LF: result = 0; continue;
		case TAB: result += 1; continue;
		default: return result;
		}
	}

	static int readEdges(const byte*& src, int indent, Edges& dst);
	static int readNode(const byte*& src, int indent, Node& dst)
	{
		for (; *src > SPACE; ++src)
			dst.name.push_back(*src);

		for (;; ++src) switch (*src)
		{
		case SPACE: case TAB: case CR: continue;
		case LF: 
			return readEdges(src, indent+1, dst.edges); 
		default:
			if (*src < SPACE)
				return -1;
			dst.edges.push();
			return readNode(src, indent, dst.edges.back());
		}

	}

	static int readEdges(const byte*& src, int indent, Edges& dst)
	{
		int next_indent = readIndent(src);
		while (next_indent >= indent && isValid(*src))
		{
			dst.push();
			next_indent = readNode(src, next_indent, dst.back());
		}
		return next_indent;
	}

	Edges read(const std::string& data)
	{
		auto src = reinterpret_cast<const byte*>(data.c_str());
		Edges result;
		readEdges(src, 0, result);
		return result;
	}


	void Node::write(std::string& out, int indent) const
	{
		out.resize(out.size() + indent, '\t');
		out.append(name);
		out.push_back('\n');
		edges.write(out, indent + 1);
	}
}
