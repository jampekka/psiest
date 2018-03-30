psijs.js: psijs.cpp psi.hpp
	em++ -std=c++1z -Ixtensor-git/include -Ixtl-git/include -s TOTAL_MEMORY=33554432 -DNDEBUG -O3 --memory-init-file 0 --bind psijs.cpp -o psijs.js
