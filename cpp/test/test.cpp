#include <mitn.h>

#include <iostream>
#include <fstream>

std::string readFile(const std::string& filename)
{
	std::string result;
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	file.seekg(0, std::ios::end);
	result.resize(static_cast<size_t>(file.tellg()));
	file.seekg(0, std::ios::beg);
	file.read(const_cast<char*>(result.data()), result.size());
	return result;
}

int main()
{
	const auto text_a = readFile("test/a.mitn");
	const auto nodes_a = mitn::read(text_a);
	const auto trip_a = nodes_a.write();
	std::cout << trip_a;
	std::cout << "a -> a: " << (trip_a == text_a ? "ok" : "fail!") << '\n';

	const auto text_b = readFile("test/b.mitn");
	const auto nodes_b = mitn::read(text_b);
	const auto trip_b = nodes_b.write();
	std::cout << trip_b;
	std::cout << "b -> a: " << (trip_b == text_a ? "ok" : "fail!") << '\n';

	return 0;
}