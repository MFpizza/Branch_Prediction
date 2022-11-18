#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <math.h>
#include <iomanip>
using namespace std;
map<string, int> label; //label
map<string, int> Reg; // Register 用的
const string state[] = { "SN","WN","WT","ST" }; //state的狀態表

struct entries { //entries型別 裝BC跟history還有mis幾次 No是編號幾
	int No;
	vector<int> BC;
	string history;
	int mis;
};

void resetReg() { //把所有Register歸0
	for (int i = 0; i < 32; i++) {
		stringstream ss; //用ss來把int轉成string並加上R 就是register的字串並設為0
		ss << i;
		string s;
		ss >> s;
		s.insert(s.begin(), 'R');
		Reg[s] = 0;
	}
}

void add(string arr[]) { //arr[2]放 rs2 arr[1]放 rs1 arr[0]放 rd
	stringstream ss;
	int b, c;
	c=Reg[arr[2]]; //讀取 register 的字串 轉成 int 來做運算
	b=Reg[arr[1]]; //讀取 register 的字串 轉成 int 來做運算
	Reg[arr[0]] = b + c;  //rd = rs2 + rs1
}

void addi(string arr[]) {
	stringstream ss;
	int b, c;
	ss << arr[2]; //arr[2] 是 immediate 的string 轉成 int
	ss >> c;
	ss.clear();
	b = Reg[arr[1]]; //讀取 register 的字串 轉成 int 來做運算
	Reg[arr[0]] = b + c;  //rd = rs2 + rs1
}

bool beq(string arr[]) { //arr[0] arr[1] 分別是register啥 在去Reg裡面找值 看有沒有 == 
	if (Reg[arr[1]] == Reg[arr[0]])
		return true;
	return false;
}

int historyToBc(string s,int bitNum) { //把history 轉換成 int 來找出相對應的 BC
	string a;
	for (int i = bitNum; i > 0; i--) //讀取後面bitNum數量的history
		a.push_back(s[s.size() - i]);
	
	int num = 1, sum = 0; //a是二進位的string  轉成 十進位 來return
	for (int i = a.size() - 1; i >= 0; i--) {
		sum += num * (a[i] - '0'); 
		num *= 2;
	}
	return sum;
}

void outputEntries(entries* ptr, string s, int howManyBit) { //用來輸出這格entry的部分資料
	cout << "entry: " << ptr->No << "       " << s << endl << "("; //輸出第幾格entry 他的code 是啥
	for (int i = howManyBit; i > 0; i--)
		cout << ptr->history[ptr->history.size() - i]; //輸出history是啥
	for (int i = 0; i < ptr->BC.size(); i++)
		cout << ", " << state[ptr->BC[i]]; //輸出所有的state狀態
	cout << ") ";
}

void BitHistory(vector<string> code) {
	cout << "請輸入要多少bit的predictor:"; //可以決定要幾個bit 的 predictor
	int bit;
	cin >> bit;
	int total = pow(2, bit); //total 代表總共需要幾個 BC
	cout << "請輸入要多少entries:"; //可以決定要幾個 entries
	int entriesNum;
	cin >> entriesNum;
	vector<entries*> entry; //代替用來放entries 的表格
	for (int i = 0; i < entriesNum; i++) { //創建entriesNum數量的entries 並且預設 mis=0 history=bit數量的0 跟BC有幾個且狀態都是0(SN)
		entries* en = new entries;
		en->No = i;
		en->mis = 0;
		en->history.resize(bit, '0');
		for (int j = 0; j < total; j++)
			en->BC.push_back(0);
		entry.push_back(en);
	}
	string s, s2;
	for (int k = 0; k < code.size(); k++) { //開始分析程式碼
		if (code[k][code[k].size() - 1] == ':') //代表是label 不會運算 跳過
			continue;
		stringstream ss;
		ss << code[k];
		ss >> s >> s2; //分割code
		string arr[3]; //存放分割空白後再次用,分割的array
		int j = 0;
		for (int i = 0; i < s2.size(); i++) { //add addi beq 都是用兩個逗號分隔三個東西 型態都類似 a,b,c 所以分別存入相對應位置的arr
			if (s2[i] == ',') {
				j++; i++;
			}
			arr[j].push_back(s2[i]);
		}

		if (s == "add") //如果是add呼叫add function
			add(arr);
		else if (s == "addi") //如果是addi 就呼叫addi function
			addi(arr);
		else if (s == "beq") { //beq 重頭戲 
			arr[2].push_back(':'); //先幫label加上 : 避免之後在 label(map)裡面找不到相對應的
			outputEntries(entry[k % entriesNum], code[k], bit); //先把這格entry的部分資料顯示在cmd上  且 k % entriesNum 代表他相對應的entry在第幾格
			string History = entry[k % entriesNum]->history; //獲取History
			int whichBc = historyToBc(History,bit); //獲取現在指向第幾個BC
			bool takenOrNot = (entry[k % entriesNum]->BC[whichBc] > 1) ? true : false; //得知現在預測是 taken or not taken
			if (takenOrNot) cout << " T"; //顯示預測的結果
			else cout << " N";
			if (beq(arr)) {  //如果實際做出來是taken
				cout << " T";
				if (!takenOrNot) //如果實際taken 但預測 not taken 代表miss了一次 所以miss次數++
					entry[k % entriesNum]->mis++;

				entry[k % entriesNum]->BC[whichBc]++; //讓現在指向的那個BC生一格狀態 因為現在是taken
				if (entry[k % entriesNum]->BC[whichBc] > 3) //如果超過3代表狀態在 3(ST) 就會保持在 3(ST)的狀態
					entry[k % entriesNum]->BC[whichBc] = 3;
				entry[k % entriesNum]->history.push_back('1'); //紀錄這次的branch結果是taken
				cout << setw(20) << "misprediction: " << entry[k % entriesNum]->mis << endl; //輸出這個entries的predict mis了幾次
				k = label[arr[2]];
			}
			else {  //如果實際做出來是not taken
				cout << " N";
				if (takenOrNot) //如果實際 not taken 但預測 taken 代表miss了一次 所以miss次數++
					entry[k % entriesNum]->mis++;

				entry[k % entriesNum]->BC[whichBc]--;  //讓現在指向的那個BC降一格狀態 因為現在是 not taken
				if (entry[k % entriesNum]->BC[whichBc] < 0) //如果小於0代表本來就在 0(SN) 的狀態 就會保持在 0(SN) 的狀態
					entry[k % entriesNum]->BC[whichBc] = 0;
				entry[k % entriesNum]->history.push_back('0'); //紀錄這次的branch結果是not taken
				cout << setw(20)<<"misprediction: " << entry[k % entriesNum]->mis << endl; //輸出這個entries的predict mis了幾次
			}
		}
	}
}

int main() {
	string s;
	vector<string> code; //存放所有code
	cout << "請開始輸入所有code，label請隔一行輸出且最後一個字元是:\n最後一行輸入-1結束\nEx:\nloop:\nadd R0,R1,R3\nbeq R0,R0,loop\n-1\n";
	cout << "code 開始:\n";
	int i = 0;
	while (getline(cin, s)) { //用來讀取所有code 並且判斷label在第幾行
		if (s == "-1")
			break;
		if (s[s.size() - 1] == ':') {
			label[s] = i;
		}
		i++;
		code.push_back(s);
	}
	resetReg(); //重製register
	BitHistory(code); //主體
}