#pragma once

#include <vector>
#include <string>

using namespace std;

#ifdef DEBUG
#define LOG(x) do { printf("[LOG] ");x; } while(0)
#else
#define LOG(x)
#endif

#define i8 char
#define u8 unsigned char
#define u32 unsigned int
#define u64 unsigned long long

auto string_to_vector(string s){
	u8 *cstr = (u8 *)s.c_str();
	vector<u8> vc(cstr, cstr + s.size());
	return vc;
}	

auto chars_to_words(vector<u8> v){
	vector<u32> words;
	for(unsigned i = 0; i < v.size(); i+= 4){
		u32 w = (unsigned i8) v[i+3];
		w = (w << 8) | (unsigned i8) v[i+2];
		w = (w << 8) | (unsigned i8) v[i+1];
		w = (w << 8) | (unsigned i8) v[i];
		words.push_back(w);
	}
	return words;
}

u32 rotate_shift(u32 n, u32 s){
	return (n << s) | (n >> (32-s));
}

string word_to_string(u32 num){
	string s;
	for(int i = 0; i < 4; i++){
		i8 hx[4];
		u8 byte = (u8)num;
		snprintf(hx, 3, "%02x", byte);
		s += hx;
		num = num >> 8;
	}
	return s;
}