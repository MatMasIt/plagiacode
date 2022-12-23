all:
	g++ main.cpp -o bin/plagiacheck -std=c++17 --static -g
	x86_64-w64-mingw32-g++ main.cpp -o bin/plagiacheck.exe -std=c++17 --static -g
	i686-w64-mingw32-g++  main.cpp -o bin/plagiacheck32.exe -std=c++17 --static -g
