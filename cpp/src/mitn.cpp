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

	inline static bool skipComment(const byte*& src)
	{
		if (src[0] == '#' && (src[1] == ' ' || src[1] == '\t'))
		{
			while (*src != '\n')
				++src;
			return true;
		}
		return false;
	}

	static int readIndent(const byte*& src)
	{
		int result = 0;
		for (;; ++src) switch (*src)
		{
		case CR: continue;
		case LF: result = 0; continue;
		case TAB: result = (result+8)&(~7); continue;
		case SPACE: result += 1; continue;
		default: 
			if (*src < SPACE)
				return -1;
			if (src[0] == '#' && (src[1] == ' ' || src[1] == '\t'))
			{
				skipComment(src);
				continue;
			}
				
			return result;
		}
	}

	static void readQuote(const byte*& src, std::string& dst)
	{
		++src;
		for (;; ++src) switch (*src)
		{
		case CR: continue;
		case LF: readIndent(src); --src; dst.push_back(' '); continue;
		case TAB: dst.push_back('\t'); continue;
		case '"':
			++src; 
			if (*src <= ' ')
				return;
			else if (*src >= '0' && *src <= '9')
				dst.push_back(*src - '0');
			else if (*src >= 'A' && *src < 'A' + 22)
				dst.push_back(*src - ('A' - 10));
			else if (*src >= 'a' && *src < 'a' + 22)
				dst.push_back(*src - ('a' - 10));
			else
				dst.push_back(*src);
			break;
		default:
			if (*src < SPACE)
				return;
			dst.push_back(*src);
			continue;
		}
	}

	static int readEdges(const byte*& src, int indent, Edges& dst);
	static int readNode(const byte*& src, int indent, Node& dst)
	{
		if (*src == '"')
			readQuote(src, dst.name);
		else for (; *src > SPACE; ++src)
			dst.name.push_back(*src);

		for (;; ++src) switch (*src)
		{
		case SPACE: case TAB: case CR: continue;
		case LF: 
			return readEdges(src, indent+1, dst.edges); 
		default:
			if (*src < SPACE)
				return -1;
			if (skipComment(src))
			{
				--src;
				continue;
			}
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

	static void writeQuote(const std::string& in, std::string& out)
	{
		static constexpr const char* digit = 
			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		out.push_back('"');
		for (auto ch : in)
			if (ch >= 0 && ch < ' ')
			{
				out.push_back('"');
				out.push_back(digit[ch]);
			}
			else if (ch == '"')
				out.append("\"\"");
			else
				out.push_back(ch);
		out.push_back('"');
	}

	static inline bool needs_sanitizing(const std::string& text)
	{
		for (auto ch : text)
			if (ch == '"' || (ch >= 0 && ch <= ' '))
				return true;
		return false;
	}

	void Node::write(std::string& out, int indent) const
	{
		if (needs_sanitizing(name))
			writeQuote(name, out);
		else
			out.append(name);
		if (edges.size() == 1)
		{
			out.push_back(' ');
			edges.back().write(out, indent);
		}
		else
		{
			out.push_back('\n');
			edges.write(out, indent + 1);
		}
	}
	void Edges::write(std::string & out, int indent) const
	{
		for (auto&& n : _data)
		{
			out.resize(out.size() + indent * 2, ' ');
			n.write(out, indent);
		}
	}

	Potential<Edges*> operator/(Edges& edges, std::string_view edgename)
	{
		for (auto&& n : edges)
			if (n.name == edgename)
				return { &n, nullptr, edgename };
		return { nullptr, &edges, edgename };
	}

}
