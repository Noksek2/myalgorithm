//NOT FINISHED YET
//SUCK JSON PARSER
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

enum {
	JO_MAX = 10000,
};
enum JsonDataType {
	JDT_null,
	JDT_bool,
	//JDT_int,
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
			if (c != '.') goto l_end;
			tokbuf += c; c = Next();
			while (isdigit(c)) {
				tokbuf += c;
				c = Next();
			}
		l_end:
			tokbuf += '\0';
			toktype = JT_Num;
		}
		else if (c == '"') {
			c = Next();
			while (c != '"' && c != '\0') {
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
			else toktype = JT_None;
			tokbuf += c;
			tokbuf += '\0';
			Next();
		}
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
		case JT_Str:case JT_Ident:
			ptree->SetStr(tokbuf);
			break;
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
};
//template <class T>
//class MyJsonAlloc {
//	using value_type = T;
//	MyJsonAlloc() noexcept {}
//	template <class U> MyJsonAlloc(const MyJsonAlloc<U>&) noexcept {}
//
//};


void myjson_test() {
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
	json.Dump();
	json.Delete();
	JsonParser::DeleteJsonParser();
}
