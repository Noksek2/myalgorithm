// License i dont know but it doesnt mean no license.
// Schroedinger's License
//UNGOOd JSON PARSER
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include <Windows.h>
enum {
	JO_MAX = 10000,
};
enum JsonDataType {
	JDT_null,
	JDT_bool,
	JDT_int,
	JDT_num,
	JDT_str,
	JDT_arr,
	JDT_obj,
};
enum JsonTokType{
	JT_None,
	JT_Block,
	JT_Block_e,
	JT_Sq,
	JT_Sq_e,
	JT_Minus, //-
	JT_Str,
	JT_Int,
	JT_Num,
	JT_Ident,
	JT_Comma,
	JT_Colon,
	JT_true,
	JT_false,
	JT_null,
};
std::wstring g_wstr;
void PrintUtf8(const std::string& utf8)
{
	if (utf8.empty()) return;

	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), nullptr, 0);
	g_wstr.resize(utf8.size());
	//std::wstring wstr(wlen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), g_wstr.data(), wlen);

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD written = 0;
	WriteConsoleW(hOut, g_wstr.data(), (DWORD)g_wstr.size(), &written, nullptr);
}

struct MyJsonData;
typedef std::unordered_map<std::string, MyJsonData>* MyJsonObj;
typedef std::vector<MyJsonData>* MyJsonArr;
struct MyJsonData{
	//THIS STRUCT IS SHIT
public:
	JsonDataType type;
	union {
		long long i;
		double num;
		MyJsonObj obj;
		MyJsonArr arr;
		std::string* str;
		void* ptr;
	};
	void SetNull() {
		type = JDT_null;
		i = 0;
	}
	void SetBool(bool val) {
		type = JDT_bool;
		i = val;
	}
	void SetInt(long long val) {
		type = JDT_int;
		i = val;
	}
	void SetNum(double d) {
		type = JDT_num;
		num = d;
	}
	void SetStr(const std::string& _str) {
		Delete();
		type = JDT_str;
		//void* ptr = MyJsonObject_new();
		str = new std::string(_str);
	}
	void SetArr() {
		Delete();
		type = JDT_arr;
		//void* ptr = MyJsonObject_new();
		arr = new std::vector<MyJsonData>();
	}
	void SetArr(const std::vector<MyJsonData>& _arr) {
		Delete();
		type = JDT_arr;
		//void* ptr = MyJsonObject_new();
		arr = new std::vector<MyJsonData>(_arr);
	}
	void SetObj() {
		Delete();
		type = JDT_obj;
		//void* ptr = MyJsonObject_new();
		obj = new std::unordered_map<std::string, MyJsonData>();
	}
	void SetObj(const std::unordered_map<std::string, MyJsonData>& _obj) {
		Delete();
		type = JDT_obj;
		//void* ptr = MyJsonObject_new();
		obj = new std::unordered_map<std::string, MyJsonData>(_obj);
	}

	void Delete() {
		switch (type) {
		case JDT_str:
			DeleteStr();
			break;
		case JDT_arr:
			DeleteArr();
			break;
		case JDT_obj:
			DeleteObj();
			break;
		}
		type = JDT_null;
		i = 0;
	}
	void DeleteStr() {
		if (str != nullptr) {
			delete str;
			str = nullptr;
		}
	}
	void DeleteArr() {
		if (arr != nullptr) {
			for (auto& a : *arr) {
				a.Delete();
			}
			delete arr;
			arr = nullptr;
		}
	}

	void Dump() {
		switch (type) {
		case JDT_num:
			printf("%lf", num);
			break;
		case JDT_int:
			printf("%lld", i);
			break;
		case JDT_null:
			printf("null");
			break;
		case JDT_bool:
			printf("%s", (i ? "true" : "false"));
			break;
		case JDT_str:
			printf("\"%s\"", str->c_str());
			break;
		case JDT_arr:
			putchar('[');
			for (auto a : *arr) {
				a.Dump();
				putchar(',');
				putchar(' ');
			}
			putchar(']');
			break;
		case JDT_obj:
			putchar('{');
			for (auto a : *obj) {
				printf("%s : ", a.first.c_str());
				a.second.Dump();
				putchar(',');
				putchar(' ');
			}
			putchar('}');
			break;
		}
	}
	void DumpString(std::string& strbuf) {
		char buf[128];
		switch (type) {
		case JDT_num:
			sprintf_s(buf, 128, "%.16lf", num);
			for (int i = (int)strlen(buf) - 1; i >= 0; i--) {
				if (buf[i] == '0') continue;
				else if (buf[i] == '.') {
					//buf[i + 1] = '0';
					buf[i + 2] = '\0';
					break;
				}
				else {
					buf[i + 1] = '\0';
					break;
				}
			}
			strbuf += buf;
			break;
		case JDT_int:
			sprintf_s(buf, 128, "%lld", i);
			strbuf += buf;
			break;
		case JDT_null:
			strbuf += "null";
			break;
		case JDT_bool:
			if (i) {
				strbuf += "true";
			}
			else {
				strbuf += "false";
			}
			break;
		case JDT_str:
			strbuf += '"';
			strbuf += str->c_str();
			strbuf += '"';
			break;
		case JDT_arr:
			strbuf += '[';
			for (int i = 0; i < (int)arr->size() - 1; i++) {
				(*arr)[i].DumpString(strbuf);
				strbuf += ',';
			}
			if (arr->size() > 1) {
				(*arr)[arr->size() - 1].DumpString(strbuf);
			}
			strbuf += ']';
			break;
		case JDT_obj: {
			int cnt = 0;
			strbuf += '{';
			for (auto a : *obj) {
				strbuf += '"';
				strbuf += a.first.c_str();
				strbuf += '"';
				strbuf += ':';
				a.second.DumpString(strbuf);
				cnt++;
				if(cnt < obj->size()) strbuf += ',';
			}
			strbuf += '}';
			break;
		}
		}
	}
private:
	void DeleteObj() {
		if (obj != nullptr) {
			for (auto& a : *obj) {
				a.second.Delete();
			}
			delete obj;
			obj = nullptr;
		}
	}
};
typedef std::unordered_map<std::string, MyJsonData> MyJsonObjData;
typedef std::vector<MyJsonData> MyJsonArrData;
class JsonParser {
	std::string tokbuf;
	char* code; 
	MyJsonData tree;
	int code_len;

	int code_idx;
	
	JsonTokType toktype;
public:
	static void InitJsonParser() {
		//if (g_JO == nullptr) {
		//	g_JO = new void* [JO_MAX];
		//}
	}	//
	static void DeleteJsonParser() {
		//if (g_JO != nullptr) {
		//	delete[] g_JO;
		//	g_JO = nullptr;
		//}
	}
	bool New(const char* filename) {
		tokbuf.reserve(1024);
		code = nullptr;
		code_len = 0;
		code_idx = 0;

		FILE* fp;
		fopen_s(&fp, filename, "rb");
		if (fp == NULL) return false;
		long f_sz;
		fseek(fp, 0, SEEK_SET);
		fseek(fp, 0, SEEK_END);
		f_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		code = new char[f_sz + 1];
		if (code == nullptr) return false;
		fread(code, sizeof(char), f_sz, fp);
		code[f_sz] = '\0';
		code_len = f_sz;
		//puts(code);
		fclose(fp);
		return true;
	}
	void Delete() {
		if (code != nullptr) {
			delete[] code;
			code = nullptr;
		}
		tree.Delete();
	}

	void AddUniToUtf8(uint16_t ch) {
		if (ch <= 0x007f) {
			tokbuf += (char)ch;
		}
		else if (ch <= 0x07ff) {
			tokbuf += (char)(ch & 0b011110000000);
			tokbuf += (char)(ch & 0b000001111111);
		}
		else if (ch <= 0x07ff) {
			tokbuf += (char)(((ch & 0b011110000000) >> 7) | 0b11000000);
			tokbuf += (char)((ch & 0b000001111111) | 0b10000000);
		}
		else {
			//yyyy|yxxxxx|xxxxxx
			tokbuf += (char)((ch >> 12) | 0b11100000);
			tokbuf += (char)(((ch >> 6) & 0b000111111) | 0b10000000);
			tokbuf += (char)((ch & 0b00111111) | 0b10000000);
		}
	}
	char Get() {
		return code[code_idx];
	}
	char Next() {
		if (code_idx == code_len)return '\0';
		code_idx++;
		return code[code_idx];
	}
	bool GetToken() {
		tokbuf.clear();
		char c = Get();
		while (isspace(c)) { c=Next(); }
		if (c == '\0') return false;
		if (isdigit(c)) {
			tokbuf += c;
			c=Next();
			while (isdigit(c)) {
				tokbuf += c;
				c = Next();
			}
			if (c != '.') {
				tokbuf += '\0';
				toktype = JT_Int;
				
			}
			else {
				tokbuf += c; c = Next();
				while (isdigit(c)) {
					tokbuf += c;
					c = Next();
				}
				tokbuf += '\0';
				toktype = JT_Num;
			}
		}
		else if (c == '"') {
			c = Next();
			while (c != '"' && c != '\0') {
				if (c == '\\') {
					c = Next();
					if (c == 'u') {
						uint16_t wc=0u;
						c = Next();
						for (int i = 0; i < 4; i++) {
							wc *= 16u;
							if (isdigit(c)) { wc += c - '0'; }
							else if (c >= 'a' || c <= 'f') { wc += c - 'a' + 10; }
							else if (c >= 'A' || c <= 'F') { wc += c - 'A' + 10; }
							else return false;
							c = Next();
						}
						AddUniToUtf8(wc);
						continue;
					}
					else if (c == 'n') {
						c = '\n';
					}
					else if (c == 't') {
						c = '\t';
					}
					else if ((c == '\\')||
						(c=='"')||
						(c=='\'')) {

					}
					else {
						tokbuf += '\\';
					}
				}
				tokbuf += c;
				c = Next();
			}
			c = Next();
			tokbuf += '\0';
			toktype = JT_Str;
		}
		else if (c == '_' || isalpha(c)) {
			tokbuf += c;
			c = Next();
			while (c == '_' || isalpha(c) || isdigit(c)) {
				tokbuf += c;
				c = Next();
			}
			//c = Next();
			tokbuf += '\0';
			if (tokbuf == "true") toktype = JT_true;
			else if (tokbuf == "false") toktype = JT_false;
			else if (tokbuf == "null") toktype = JT_null;
			else toktype = JT_Ident;
		}
		else {
			if (c == '{') toktype = JT_Block;
			else if (c == '}') toktype = JT_Block_e;
			else if (c == '[') toktype = JT_Sq;
			else if (c == ']') toktype = JT_Sq_e;
			else if (c == ':') toktype = JT_Colon;
			else if (c == ',') toktype = JT_Comma;
			else if (c == '-') toktype = JT_Minus;
			else toktype = JT_None;
			tokbuf += c;
			tokbuf += '\0';
			Next();
		}
		//std::cout << code_idx << ':';
		//PrintUtf8(tokbuf);
		//std::cout << '\t';
		return true;
	}
	bool State(MyJsonData* ptree) {
		MyJsonData jd_buf;
		jd_buf.type = JDT_null;
		switch (toktype) {
		case JT_Block://{<str>:<factor>, ... }
			GetToken();
			ptree->SetObj();
			while (toktype != JT_Block_e && toktype != JT_None) {
				//GetToken();
				if (toktype != JT_Str && toktype != JT_Ident) return false;
				std::string key = tokbuf;
				GetToken();//:
				if (toktype != JT_Colon) return false;
				GetToken();
				ptree->obj->emplace(key, jd_buf);
				if (!State(&(*ptree->obj)[key])) return false;
				if (toktype != JT_Comma) break;
				GetToken();
			}
			if (toktype != JT_Block_e)return false;
			
			break;
		case JT_Sq:
			GetToken();
			ptree->SetArr();
			while (toktype != JT_Sq_e && toktype != JT_None) {
				ptree->arr->emplace_back(jd_buf);
				if (!State(&ptree->arr->back())) return false;
				if (toktype != JT_Comma) break;
				GetToken();
			}
			if (toktype != JT_Sq_e)return false;
			break;
		case JT_Minus:
			GetToken();
			if (toktype == JT_Num) {
				ptree->SetNum(-atof(tokbuf.c_str()));
			}
			else if(toktype == JT_Int) 
			{
				ptree->SetInt(-atoll(tokbuf.c_str()));
			}
			else return false;
			break;
		case JT_Str:case JT_Ident:
			ptree->SetStr(tokbuf);
			break;
		case JT_Int: {
			ptree->SetInt(atoll(tokbuf.c_str()));
			break;
		}
		case JT_Num: {
			ptree->SetNum(atof(tokbuf.c_str()));
			break;
		}
		case JT_null:
			ptree->SetNull();
			break;
		case JT_true:
			ptree->SetBool(true);
			break;
		case JT_false:
			ptree->SetBool(false);
			break;
		default:
			return false;
		}
		GetToken();
		return true;
	}
	bool Parse() {
		code_idx = 0;
		GetToken();
		if (!State(&tree)) return false;
		return true;
	}
	void Dump() {
		puts("json dump");
		tree.Dump();
	}
	void DumpString(std::string& str) {
		str.reserve(code_len);
		tree.DumpString(str);
	}
};
//template <class T>
//class MyJsonAlloc {
//	using value_type = T;
//	MyJsonAlloc() noexcept {}
//	template <class U> MyJsonAlloc(const MyJsonAlloc<U>&) noexcept {}
//
//};


void myjson_test() {
#ifdef _WIN32
	//SetConsoleOutputCP(CP_UTF8);
	//SetConsoleCP(65001);
#endif
	JsonParser::InitJsonParser();

	JsonParser json;
	if (!json.New("test.json")) {
		puts("Failed to open");
		return;
	}
	if (!json.Parse()) {
		puts("Failed to Parse");
		json.Dump();
		json.Delete();
		return;
	}
	std::string str;
	json.DumpString(str);
	//PrintUtf8(str);
	{
		FILE* fp;
		fopen_s(&fp, "export.json", "wb");
		fwrite(&str[0], 1, str.size(), fp);
		fclose(fp);
	}

	json.Delete();
	JsonParser::DeleteJsonParser();
}
