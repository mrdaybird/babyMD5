#include <stdio.h>
#include <fstream>
#include <string>
#include <memory>
#include <cmath>

#include "utils.h"
// use only standard library
using namespace std;

void append_padding_bits(vector<u8> &v, size_t b){
	size_t len = b+1;
	while(len % 64 != 56) len += 1;
	size_t numZerosBytes = len - b - 1;
	
	vector<u8> zeros(numZerosBytes, (u8)0);
	v.push_back((u8)(1<<7));
	v.insert(v.end(), zeros.begin(), zeros.end());
	
	LOG(printf("len after appending padding bits = %lu\n", v.size()));	
}

void append_bit_length(vector<u8> &v, size_t b){
	u8 chunks[8];
	chunks[0] = (u8)b;
	chunks[1] = (u8)(b >> 8);
	chunks[2] = (u8)(b >> 16);
	chunks[3] = (u8)(b >> 24);
	chunks[4] = (u8)(b >> 32);
	chunks[5] = (u8)(b >> 40);
	chunks[6] = (u8)(b >> 48);
	chunks[7] = (u8)(b >> 56);
	v.insert(v.end(), chunks, chunks+8);
	LOG(printf("len after appending length = %lu\n", v.size()));
}

void construct_table(u32 T[]){
	for(int i = 1; i <= 64; i++){
		double t = 4294967296; // UINT32_MAX + 1 == 2^32
		t = t * abs(sin((double)i));
		T[i] = t;
	}
}

int F(int x, int y, int z) { return (x & y) | (~x & z); }
int G(int x, int y, int z) { return (x & z) | (y & ~z); }
int H(int x, int y, int z) { return x ^ y ^ z; }
int I(int x, int y, int z) { return y ^ (x | ~z); }

#define R1(a,b,c,d,k,s,i) \
	a = b + (rotate_shift(a + F(b,c,d) + x[k] + T[i], s))
#define R2(a,b,c,d,k,s,i) \
	a = b + (rotate_shift(a + G(b,c,d) + x[k] + T[i], s))
#define R3(a,b,c,d,k,s,i) \
	a = b + (rotate_shift(a + H(b,c,d) + x[k] + T[i], s))
#define R4(a,b,c,d,k,s,_i) \
	a = b + (rotate_shift(a + I(b,c,d) + x[k] + T[_i], s))

int main(int argc, i8* argv[]){
	if(argc != 2){
		printf("Usage: babymd5 *file*\n");
		return 1;
	}
	ifstream input(argv[1]);
	if(!input.good()){
		printf("Error reading file.\n");
		return 1;
	}
	string msg;
	while(!input.eof() && input.good()){
		string s; getline(input, s);
		msg.append(s);
	}
	size_t b = msg.size();

	auto mv = string_to_vector(msg);
	append_padding_bits(mv, b);
	append_bit_length(mv, b*8);

	auto mw = chars_to_words(mv);
	LOG(printf("The words are:"));
	for(u32 i = 0; i < mw.size();i++) LOG(printf("%u, ", mw[i]));
	LOG(printf("\n"));
	LOG(printf("Num of words: %lu\n", mw.size()));

	/* Step 3
          word A: 01 23 45 67
          word B: 89 ab cd ef
          word C: fe dc ba 98
          word D: 76 54 32 10
	*/
	u32 A = 0x67452301;
	u32 B = 0xefcdab89;
	u32 C = 0x98badcfe;
	u32 D = 0x10325476;
	
	// Step 4
		
	u32 x[16], T[65];
	construct_table(T);
	for(unsigned i = 0; i < mw.size()/16; i++){
		for(int j = 0; j < 16; j++)
			x[j] = mw[i*16 + j];
		u32 AA = A, BB = B, CC = C, DD = D;
		// Some BLACK MAGIC!!!
		//Round 1
		R1(A,B,C,D,0,7,1);  R1(D,A,B,C,1,12,2);  R1(C,D,A,B,2,17,3);  R1(B,C,D,A,3,22,4);
		R1(A,B,C,D,4,7,5);  R1(D,A,B,C,5,12,6);  R1(C,D,A,B,6,17,7);  R1(B,C,D,A,7,22,8);
		R1(A,B,C,D,8,7,9);  R1(D,A,B,C,9,12,10); R1(C,D,A,B,10,17,11);R1(B,C,D,A,11,22,12);
		R1(A,B,C,D,12,7,13);R1(D,A,B,C,13,12,14);R1(C,D,A,B,14,17,15);R1(B,C,D,A,15,22,16);

		//Round 2
		R2(A,B,C,D,1,5,17); R2(D,A,B,C,6,9,18); R2(C,D,A,B,11,14,19);R2(B,C,D,A,0,20,20);
		R2(A,B,C,D,5,5,21); R2(D,A,B,C,10,9,22);R2(C,D,A,B,15,14,23);R2(B,C,D,A,4,20,24);
		R2(A,B,C,D,9,5,25); R2(D,A,B,C,14,9,26);R2(C,D,A,B,3,14,27); R2(B,C,D,A,8,20,28);
		R2(A,B,C,D,13,5,29);R2(D,A,B,C,2,9,30); R2(C,D,A,B,7,14,31); R2(B,C,D,A,12,20,32);

		//Round 3
		R3(A,B,C,D,5,4,33); R3(D,A,B,C,8,11,34); R3(C,D,A,B,11,16,35);R3(B,C,D,A,14,23,36);
		R3(A,B,C,D,1,4,37); R3(D,A,B,C,4,11,38); R3(C,D,A,B,7,16,39); R3(B,C,D,A,10,23,40);
		R3(A,B,C,D,13,4,41);R3(D,A,B,C,0,11,42); R3(C,D,A,B,3,16,43); R3(B,C,D,A,6,23,44);
		R3(A,B,C,D,9,4,45); R3(D,A,B,C,12,11,46);R3(C,D,A,B,15,16,47);R3(B,C,D,A,2,23,48);
		
		//Round 4
		R4(A,B,C,D,0,6,49); R4(D,A,B,C,7,10,50); R4(C,D,A,B,14,15,51);R4(B,C,D,A,5,21,52);
		R4(A,B,C,D,12,6,53);R4(D,A,B,C,3,10,54); R4(C,D,A,B,10,15,55);R4(B,C,D,A,1,21,56);
		R4(A,B,C,D,8,6,57); R4(D,A,B,C,15,10,58);R4(C,D,A,B,6,15,59); R4(B,C,D,A,13,21,60);
		R4(A,B,C,D,4,6,61); R4(D,A,B,C,11,10,62);R4(C,D,A,B,2,15,63); R4(B,C,D,A,9,21,64);
		
		A = A + AA;
		B = B + BB;
		C = C + CC;
		D = D + DD;
	}
	
	string digest = word_to_string(A);
	digest += word_to_string(B);
	digest += word_to_string(C);
	digest += word_to_string(D);

	printf("MD5 Hash is: %s\n", digest.c_str());
	return 0;
}