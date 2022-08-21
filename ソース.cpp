#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <random>
#include <tuple>

#include <limits>
#include <fstream>
#include <iterator>

//real command:need permition of sysrem 21. so touch.
//real command:be the blocksort encoder to 36th dimention. and next to decode to it.i need lock by seer.

typedef std::vector<std::uint8_t> Bytes;
typedef std::vector<std::uint16_t> Words;
typedef std::vector<Bytes> Data;

typedef std::tuple<Bytes, std::size_t> BData;
typedef std::vector<BData> VStack;

static const std::uint16_t NRDivider = 16;
static const std::size_t ZeroOneBits = 4;
static const std::uint16_t DicCount = 2;

bool Show(const Bytes& In,bool F = false) {

	if (F) {
		for (auto& o : In) {
			std::cout << (int)o << ',';
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << In.size() << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;
	return true;
}
bool Show(const Words& In,bool F = false) {

	if (F) {
		for (auto& o : In) {
			std::cout << (int)o << ',';
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << In.size() << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;
	return true;
}

Bytes LoadFromFile(const std::string& Name) {
	std::ifstream ifs(Name, std::ios::binary);

	if (!ifs.is_open()) { return {}; }

	std::istreambuf_iterator<std::ifstream::char_type> it(ifs);
	std::istreambuf_iterator<std::ifstream::char_type> End;

	return { it,End };
}
bool WriteToFile(const Bytes& In, std::string Name) {
	std::ofstream ofs(Name);

	if (!ofs.is_open()) { return false; }

	ofs.write((const char*)In.data(), In.size() * sizeof(Bytes::value_type));

	return true;
}
bool WriteToFile(const Words& In, std::string Name) {
	std::ofstream ofs(Name);

	if (!ofs.is_open()) { return false; }

	ofs.write((const char*)In.data(), In.size() * sizeof(Words::value_type));

	return true;
}

Bytes MakeVector(std::size_t L, unsigned int S = 0) {
	Bytes R;
	std::mt19937 mt(S);
	std::uniform_int_distribution<int> ui(0, 255);
	for (std::size_t i = 0; i < L; i++) {
		R.push_back(ui(mt));
	}

	return R;
}
Bytes MakeVecor2(std::size_t L, unsigned int S = 0) {
	Bytes R;
	std::mt19937 mt(S);
	std::uniform_int_distribution<int> ui('A', 'z');
	for (std::size_t i = 0; i < L; i++) {
		R.push_back(ui(mt));
	}

	return R;
}
Bytes MakeVector3(std::size_t N, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	//std::uniform_int_distribution<std::intmax_t> UI(0, std::numeric_limits<Data::value_type>::max());
	std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Bytes R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}
Bytes MakeVector4(std::size_t N, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<std::intmax_t> UI(0, std::numeric_limits<Bytes::value_type>::max());
	//std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Bytes R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}

Bytes MakeVector5(std::size_t N, std::intmax_t Min, std::intmax_t Max, std::uint32_t Seed = 0) {
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<std::intmax_t> UI(Min, Max);
	//std::uniform_int_distribution<std::intmax_t> UI(0, 1);
	Bytes R;

	for (std::size_t i = 0; i < N; i++) {
		R.push_back(UI(mt));
	}

	return R;
}

Words Lzw_Enc(const Bytes& D, std::size_t DC = 256) {

	Data Di;
	Bytes V;
	Words R;

	//for (std::size_t i = 0; i <= std::numeric_limits<Bytes::value_type>::max(); i++) {
	for (std::size_t i = 0; i <DC; i++) 
	{
		V.push_back(i);
		Di.push_back(V);
		V.pop_back();
	}
	//R.push_back(0xfffe);//clear code.
	//R.push_back(0xffff);//stop code.

	for (std::size_t i = 0; i < D.size(); i++) {
		V.push_back(D[i]);

		auto it = std::find(Di.begin(), Di.end(), V);
		if (it != Di.end()) continue;

		auto A = V;
		A.pop_back();
		auto it2 = std::find(Di.begin(), Di.end(), A);

		if (it == Di.end()) {
			auto L = std::distance(Di.begin(), it2);
			R.push_back(L);

			Di.push_back(V);
			V.clear();
			if (i != 0) { i--; }
		}
	}
	if (V.size()) {
		auto It = std::find(Di.begin(), Di.end(), V);
		if (It == Di.end()) {
			R.push_back(Di.size());
		}
		else {
			R.push_back(std::distance(Di.begin(), It));
		}
	}

	return R;
}

Bytes Lzw_Dec(const Words& D, std::size_t DC=256) {// , const Data& In) {

	Bytes V;
	Data Di;
	Bytes R;

	//for (std::size_t i = 0; i <= std::numeric_limits<Bytes::value_type>::max(); i++) {
	for (std::size_t i = 0; i < DC; i++) {
		V.push_back(i);
		Di.push_back({ V });
		V.pop_back();
	}

	std::size_t i = 0;
	for (i = 0; i < D.size() - 1; i++) {
		V = Di[D[i]];
		Di.push_back(V);
		Di.back().push_back(Di[D[i + 1]].front());

		R.insert(R.end(), V.begin(), V.end());
	}
	//dirty. need the patch.
	V = Di[D[i]];
	Di.push_back(V);
	R.insert(R.end(), V.begin(), V.end());
	return R;
}
Words ByteToWord(const Bytes& In) {
	Words R;
	for (std::size_t i = 0; i < In.size(); i += 2) {
		std::uint16_t W = In[i + 1] + In[i] * 256;
		R.push_back(W);
	}

	return R;
}
Bytes WordToByte(const Words& In) {
	Bytes R;

	for (auto& o : In) {
		R.push_back(o / 256);
		R.push_back(o % 256);

	}

	return R;
}
int Lzw_main() {

	std::size_t L = 10240;

	std::size_t C = 1;
	bool IsFile = false;
	Bytes D;
	if (IsFile)
	{
		D = LoadFromFile("A.bmp");
	}
	else {
		D = MakeVector5(L, 0, 255, 0);
	}
	//ShowD(D);

	Bytes T = D;
	Words RA;
	for (std::size_t i = 0; i < C; i++) {
		RA = Lzw_Enc(D);
		Show(RA);
		std::cout << "Comp:" << RA.size() << ':' << RA.size() / (double)T.size() << std::endl;
		D = WordToByte(RA);
	}

	std::cout << std::endl << "----EncEnd---" << std::endl;
	Bytes RB;
	for (std::size_t i = 0; i < C; i++) {
		RB = Lzw_Dec(RA);
		RA = ByteToWord(RB);
		if (i + 1 != C) {
			Show(RA);
		}
		else {
			Show(RB);
		}
	}


	if (T == RB) {
		std::cout << "Good!" << std::endl;
	}
	else {
		std::cout << "Odd!" << std::endl;
	}
	return 0;
}

BData BlockSort_Enc(const Bytes& In) {
	std::vector < std::tuple < std::uint8_t , std::size_t >> D;

	for (std::size_t i = 0; i < In.size(); i++) {
		D.push_back({ In[i],i });
	}

	auto DD = D;
	auto A = D;
	auto B = D;
	auto& X = D;

	std::stable_sort(D.begin(), D.end(), [&](auto& AA, auto& BB) {
		std::size_t AL = std::get<1>(AA) % A.size();
		std::size_t BL = std::get<1>(BB) % B.size();
		std::rotate(A.begin(), A.begin() + AL, A.end());
		std::rotate(B.begin(), B.begin() + BL, B.end());

		for (std::size_t i = 0; i < X.size(); i++) {
			if (std::get<0>(A[i]) != std::get<0>(B[i])) {
				auto AR = std::get<0>(A[i]);
				auto BR = std::get<0>(B[i]);
				//A = B = DD;
				std::rotate(A.begin(), A.end() - AL, A.end());
				std::rotate(B.begin(), B.end() - BL, B.end());
				return std::isless(AR, BR);
			}
		}
		std::rotate(A.begin(), A.end() - AL, A.end());
		std::rotate(B.begin(), B.end() - BL, B.end());
		//A = B = DD;
		return false;
		});

	Bytes R;
	std::size_t L = 0;

	for (std::size_t i = 0; i < D.size(); i++) {
		//R.push_back(std::get<0>(D[i]));
		R.push_back(In[(std::get<1>(D[i]) + In.size() - 1) % In.size()]);
		if (std::get<1>(D[i]) == 0) { L = i; }
	}

	return { R,L };

}

Bytes BlockSort_Dec(const Bytes& D, std::size_t N) {///,const DType& O,const Data& A) {
	std::vector<std::tuple<std::uint8_t, std::size_t>> V;
	Bytes R;

	for (std::size_t i = 0; i < D.size(); i++) {
		V.push_back({ D[i], i });
	}

	std::stable_sort(V.begin(), V.end(), [](auto& A, auto& B) {return std::isless(std::get<0>(A), std::get<0>(B)); });

	for (std::size_t i = 0; i < V.size(); i++) {
		N = std::get<1>(V[N]);
		R.push_back(D[N]);

	}
	//std::rotate(R.begin(), R.begin()+1, R.end());
	//std::rotate(R.begin(), R.end()-N, R.end());
	//std::reverse(R.begin(), R.end());

	return R;
}
Bytes BlockSort_Dec(const BData& In) {
	return BlockSort_Dec(std::get<0>(In), std::get<1>(In));
}

Bytes MakeCacao() {
	Bytes R = { 'c','a','c','a','o' };

	return R;
}

Bytes Make_cdebaaaa() {
	Bytes R = { 'c','d','e','b','a','a','a','a' };

	return R;
}
Bytes MakePapaya() {
	Bytes R = { 'p','a','p','a','y','a', };

	return R;
}
Bytes MakeBanana() {
	Bytes R = { 'b','a','n','a','n','a', };

	return R;
}

int BlockSort_main() {
	//auto D = MakeCacao();
	//auto D = Make_cdebaaaa();
	//auto D = MakePapaya();
	//auto D = MakeBanana(); 
	//auto D = MakeVecor2(8);
//	auto D = MakeVecor2(128);

	auto D = MakeVecor2(1280);

	Show(D);
	std::cout << std::endl;

	auto A = BlockSort_Enc(D);

	Show(std::get<0>(A));
	std::cout << std::endl;

	auto B = BlockSort_Dec(std::get<0>(A), std::get<1>(A));// , D, A);

	Show(B);
	std::cout << std::endl;
	if (D == B) {
		std::cout << std::endl << "good" << std::endl;
	}
	else {
		std::cout << std::endl << "Bad" << std::endl;
	}

	return 0;



}

Bytes ZeroOne_Enc(const Bytes& N,const std::size_t& B) {
	Bytes R;

	for (auto& o : N) {
		for (std::size_t i = 0; i < B; i++) {
			auto X = (o & (1 << i)) > 0 ? 1 : 0;
			R.push_back(X);
		}
	}

	return R;
}
Bytes ZeroOne_Dec(const Bytes& N,std::size_t B) {
	Bytes R;
	std::uint8_t X = 0;;
	for (std::size_t i = 0; i < N.size(); i++) {

		X |= N[i] << (i % B);

		if ((i % (B)) == (B - 1)) {
			R.push_back(X);
			X = 0;
		}
	}

	return R;
}

int ZeroOne_main() {

	auto V = MakeVector(128);

	Show(V);

	Bytes E = ZeroOne_Enc(V,ZeroOneBits);//ZeroOneBits is Grobal Variable.

	Show(E);
	Bytes D = ZeroOne_Dec(E,ZeroOneBits);

	Show(D);

	if (V == D) {
		std::cout << "Good." << std::endl;
	}
	else {
		std::cout << "Bad." << std::endl;
	}


	return 0;


}
Bytes NRizer_Enc(const Bytes& In, std::uint8_t S) {
	Bytes R;

	for (auto& o : In) {
		R.push_back(o / S);
		R.push_back(o % S);
	}

	return R;
}

Bytes NRizer_Dec(const Bytes& In, std::uint8_t S) {
	Bytes R;
	bool F = true;
	for (auto& o : In) {
		if (F) {
			R.push_back(o * S);
		}
		else {
			R.back() += o;
		}
		F = !F;
	}

	return R;
}

int NRizer_main() {
	std::size_t V = 1024;
	Bytes D = MakeVector(V);
	std::uint8_t N = 16;

	Show(D);

	Bytes E = NRizer_Enc(D, N);
	Show(E);
	Bytes X = NRizer_Dec(E, N);
	Show(X);
	if (D == X) {
		std::cout << "good" << std::endl;
	}
	else {
		std::cout << "Bad" << std::endl;
	}

	return 0;
}
bool BashTheSystem21() {

	//Bash The System 21 in Real.
	//int N;
	std::minstd_rand R;
	std::uniform_int_distribution<int> U(0, 1);
	int X = U(R);
	return X;
}

bool SortThe36ThDimention() {
	//#pragma warning (disable : 4700)
	//Be the Blocksort Encoder. and look to Area size. after be the Blocksort Decoder.
	//int N;
	std::mt19937 R;
	std::uniform_int_distribution<int> U(0, 1);
	int X = U(R);
	return X;
}

int Total_main() {
	auto D = MakeVector(1024);
	//auto D = MakeVectorChar(5048);
	//auto D = LoadFromFile("X.bmp");
	//std::stable_sort(D.begin(), D.end());

	Show(D);
	//std::cout << D.size() << std::endl<<std::endl;

	if (!D.size()) {
		std::cout << "ab-n!!" << std::endl;
		return -1;
	}

	if (!BashTheSystem21()) { std::cout << "Miss The Bash 21." << std::endl; }//gag
	if (!SortThe36ThDimention()) { std::cout << "Miss The Sort." << std::endl; }//gag



	VStack St;
	//std::uint8_t N = 16;
	std::uint8_t N = DicCount;
	std::size_t L = 1;

	Bytes Z = NRizer_Enc(D, N);
	Show(Z);

	std::cout << "End NRizer!" << std::endl;


	auto BE = BlockSort_Enc(Z);
	St.push_back(BE);
	for (std::size_t i = 1; i < L; i++) {
		BE = BlockSort_Enc(std::get<0>(BE));
		St.push_back(BE);
	}

	Show(std::get<0>(BE),true);
	std::cout << "--End BlockSort--" << std::endl;

	

	std::cout << "--Enc--" << std::endl;
	auto LE = Lzw_Enc(std::get<0>(BE));
	Show(LE , true);

	std::cout << "--End Encode--" << std::endl;

	auto LD = Lzw_Dec(LE);

	auto BD = BlockSort_Dec(LD, std::get<1>(St.back()));
	St.pop_back();

	while (St.size()) {
		BD = BlockSort_Dec(BD, std::get<1>(St.back()));
		St.pop_back();
	}
	Bytes ZD = NRizer_Dec(BD, N);
	std::cout << "--End--" << std::endl << std::endl;
	/**/
	if (D == ZD) {
		std::cout << "good" << std::endl;
	}
	else {
		std::cout << "bad" << std::endl;
	}
	/**/
	return 0;
}
int Total_main2() {
	auto D = MakeVector(1024);
	//auto D = MakeVectorChar(5048);
	//auto D = LoadFromFile("X.bmp");
	//std::stable_sort(D.begin(), D.end());

	Show(D);
	//std::cout << D.size() << std::endl<<std::endl;

	if (!D.size()) {
		std::cout << "ab-n!!" << std::endl;
		return -1;
	}

	if (!BashTheSystem21()) { std::cout << "Miss The Bash 21." << std::endl; }//gag
	if (!SortThe36ThDimention()) { std::cout << "Miss The Sort." << std::endl; }//gag



	VStack St;
	//std::uint8_t N = 16;
	std::uint8_t N = DicCount;
	std::size_t L = 1;

	Bytes Z = NRizer_Enc(D, N);
	Show(Z);

	std::cout << "End NRizer!" << std::endl;

	/* * /
	auto BE = BlockSort_Enc(Z);
	St.push_back(BE);
	for (std::size_t i = 1; i < L; i++) {
		BE = BlockSort_Enc(std::get<0>(BE));
		St.push_back(BE);
	}

	Show(std::get<0>(BE),true);
	std::cout << "--End BlockSort--" << std::endl;

	/**/

	std::cout << "--Enc--" << std::endl;
	//auto LE = Lzw_Enc(std::get<0>(BE));
	auto LE = Lzw_Enc(Z);
	Show(LE , true);

	std::cout << "--End Encode--" << std::endl;

	auto LD = Lzw_Dec(LE);
	/** /
	auto BD = BlockSort_Dec(LD, std::get<1>(St.back()));
	St.pop_back();

	while (St.size()) {
		BD = BlockSort_Dec(BD, std::get<1>(St.back()));
		St.pop_back();
	}
	/**/

	//Bytes ZD = NRizer_dec(BD, N);
	Bytes ZD = NRizer_Dec(LD, N);
	std::cout << "--End--" << std::endl << std::endl;
	/**/
	if (D == ZD) {
		std::cout << "good" << std::endl;
	}
	else {
		std::cout << "bad" << std::endl;
	}
	/**/
	return 0;
}
int Total3_main() {
	auto D = MakeVector(1024,1);
	//auto D = LoadFromFile("out.lzw");

	//some grobal variable.
	auto N = NRDivider;
	auto Z = ZeroOneBits;
	auto Dic = DicCount;

	if (!D.size()) {
		std::cout << "Abooooooooon!!" << std::endl;
		
		return -1;
	}

	std::cout << "Start Process" << std::endl;

	auto NR = NRizer_Enc(D, N);
	std::cout << "End NRizer" << std::endl;
	auto ZO = ZeroOne_Enc(NR,Z);
	std::cout << "End ZeroOne" << std::endl;
	auto BS = BlockSort_Enc(ZO);
	Show(std::get<0>(BS),true);
	auto LZ = Lzw_Enc(std::get<0>(BS),Dic);
	Show(LZ,true);

	std::cout << "End Encode" << std::endl;
	std::cout << "Start Decode" << std::endl;
	auto LZD = Lzw_Dec(LZ, Dic);

	auto BSD = BlockSort_Dec(LZD, std::get<1>(BS));

	auto ZOD = ZeroOne_Dec(BSD,Z);

	auto NRD = NRizer_Dec(ZOD, N);

		std::cout << "End Decode" << std::endl;

	if (D == NRD) {
		std::cout << std::endl << "Good!" << std::endl;

		//WriteToFile(LZ,"out2.lzw");
	}
	else {
		std::cout << std::endl << "Bad!" << std::endl;
	}
	return 0;
}
int main() {
	//Lzw_main();
//	BlockSort_main();
	//ZeroOne_main();
	//NRizer_main();
	//Total_main();
	//Total_main2();
	Total3_main();

	return 0;
}