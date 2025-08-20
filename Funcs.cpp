#include "Funcs.hpp"
#define NOMINMAX
#include <Windows.h>
#include <memory>
#include <vector>

std::string ws2s(const std::wstring& wstr)
{
	int wLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), nullptr, 0, nullptr, nullptr);

	std::string result(wLen, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &result[0], wLen, nullptr, nullptr);

	return result;
}

int EditDistance(const std::string a, const std::string b)
{
	if(a.empty()) return b.length();
	if(b.empty()) return a.length();

	auto m = a.length();
	auto n = b.length();

	std::vector<std::vector<int>> mat(m + 1, std::vector<int>(n + 1));

	for(int i = 0; i <= m; ++i)
	{
		mat[i][0] = i;
	}
	for(int j = 0; j <= n; ++j)
	{
		mat[0][j] = j;
	}

	for(int i = 1; i <= m; ++i)
	{
		for(int j = 1; j <= n; ++j)
		{
			if(a[i] == b[j])
				mat[i][j] = mat[i - 1][j - 1];
			else
				mat[i][j] = 1 + std::min({
					mat[i - 1][j],
					mat[i][j - 1], 
					mat[i - 1][ j - 1] 
				});
		}
	}

	return mat[m][n];
}
