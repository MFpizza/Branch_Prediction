#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <math.h>
#include <iomanip>
using namespace std;
map<string, int> label; //label
map<string, int> Reg; // Register �Ϊ�
const string state[] = { "SN","WN","WT","ST" }; //state�����A��

struct entries { //entries���O ��BC��history�٦�mis�X�� No�O�s���X
	int No;
	vector<int> BC;
	string history;
	int mis;
};

void resetReg() { //��Ҧ�Register�k0
	for (int i = 0; i < 32; i++) {
		stringstream ss; //��ss�ӧ�int�নstring�å[�WR �N�Oregister���r��ó]��0
		ss << i;
		string s;
		ss >> s;
		s.insert(s.begin(), 'R');
		Reg[s] = 0;
	}
}

void add(string arr[]) { //arr[2]�� rs2 arr[1]�� rs1 arr[0]�� rd
	stringstream ss;
	int b, c;
	c=Reg[arr[2]]; //Ū�� register ���r�� �ন int �Ӱ��B��
	b=Reg[arr[1]]; //Ū�� register ���r�� �ন int �Ӱ��B��
	Reg[arr[0]] = b + c;  //rd = rs2 + rs1
}

void addi(string arr[]) {
	stringstream ss;
	int b, c;
	ss << arr[2]; //arr[2] �O immediate ��string �ন int
	ss >> c;
	ss.clear();
	b = Reg[arr[1]]; //Ū�� register ���r�� �ন int �Ӱ��B��
	Reg[arr[0]] = b + c;  //rd = rs2 + rs1
}

bool beq(string arr[]) { //arr[0] arr[1] ���O�Oregisterԣ �b�hReg�̭���� �ݦ��S�� == 
	if (Reg[arr[1]] == Reg[arr[0]])
		return true;
	return false;
}

int historyToBc(string s,int bitNum) { //��history �ഫ�� int �ӧ�X�۹����� BC
	string a;
	for (int i = bitNum; i > 0; i--) //Ū���᭱bitNum�ƶq��history
		a.push_back(s[s.size() - i]);
	
	int num = 1, sum = 0; //a�O�G�i�쪺string  �ন �Q�i�� ��return
	for (int i = a.size() - 1; i >= 0; i--) {
		sum += num * (a[i] - '0'); 
		num *= 2;
	}
	return sum;
}

void outputEntries(entries* ptr, string s, int howManyBit) { //�Ψӿ�X�o��entry���������
	cout << "entry: " << ptr->No << "       " << s << endl << "("; //��X�ĴX��entry �L��code �Oԣ
	for (int i = howManyBit; i > 0; i--)
		cout << ptr->history[ptr->history.size() - i]; //��Xhistory�Oԣ
	for (int i = 0; i < ptr->BC.size(); i++)
		cout << ", " << state[ptr->BC[i]]; //��X�Ҧ���state���A
	cout << ") ";
}

void BitHistory(vector<string> code) {
	cout << "�п�J�n�h��bit��predictor:"; //�i�H�M�w�n�X��bit �� predictor
	int bit;
	cin >> bit;
	int total = pow(2, bit); //total �N���`�@�ݭn�X�� BC
	cout << "�п�J�n�h��entries:"; //�i�H�M�w�n�X�� entries
	int entriesNum;
	cin >> entriesNum;
	vector<entries*> entry; //�N���Ψө�entries �����
	for (int i = 0; i < entriesNum; i++) { //�Ы�entriesNum�ƶq��entries �åB�w�] mis=0 history=bit�ƶq��0 ��BC���X�ӥB���A���O0(SN)
		entries* en = new entries;
		en->No = i;
		en->mis = 0;
		en->history.resize(bit, '0');
		for (int j = 0; j < total; j++)
			en->BC.push_back(0);
		entry.push_back(en);
	}
	string s, s2;
	for (int k = 0; k < code.size(); k++) { //�}�l���R�{���X
		if (code[k][code[k].size() - 1] == ':') //�N��Olabel ���|�B�� ���L
			continue;
		stringstream ss;
		ss << code[k];
		ss >> s >> s2; //����code
		string arr[3]; //�s����Ϊťի�A����,���Ϊ�array
		int j = 0;
		for (int i = 0; i < s2.size(); i++) { //add addi beq ���O�Ψ�ӳr�����j�T�ӪF�� ���A������ a,b,c �ҥH���O�s�J�۹�����m��arr
			if (s2[i] == ',') {
				j++; i++;
			}
			arr[j].push_back(s2[i]);
		}

		if (s == "add") //�p�G�Oadd�I�sadd function
			add(arr);
		else if (s == "addi") //�p�G�Oaddi �N�I�saddi function
			addi(arr);
		else if (s == "beq") { //beq ���Y�� 
			arr[2].push_back(':'); //����label�[�W : �קK����b label(map)�̭��䤣��۹�����
			outputEntries(entry[k % entriesNum], code[k], bit); //����o��entry�����������ܦbcmd�W  �B k % entriesNum �N��L�۹�����entry�b�ĴX��
			string History = entry[k % entriesNum]->history; //���History
			int whichBc = historyToBc(History,bit); //����{�b���V�ĴX��BC
			bool takenOrNot = (entry[k % entriesNum]->BC[whichBc] > 1) ? true : false; //�o���{�b�w���O taken or not taken
			if (takenOrNot) cout << " T"; //��ܹw�������G
			else cout << " N";
			if (beq(arr)) {  //�p�G��ڰ��X�ӬOtaken
				cout << " T";
				if (!takenOrNot) //�p�G���taken ���w�� not taken �N��miss�F�@�� �ҥHmiss����++
					entry[k % entriesNum]->mis++;

				entry[k % entriesNum]->BC[whichBc]++; //���{�b���V������BC�ͤ@�檬�A �]���{�b�Otaken
				if (entry[k % entriesNum]->BC[whichBc] > 3) //�p�G�W�L3�N���A�b 3(ST) �N�|�O���b 3(ST)�����A
					entry[k % entriesNum]->BC[whichBc] = 3;
				entry[k % entriesNum]->history.push_back('1'); //�����o����branch���G�Otaken
				cout << setw(20) << "misprediction: " << entry[k % entriesNum]->mis << endl; //��X�o��entries��predict mis�F�X��
				k = label[arr[2]];
			}
			else {  //�p�G��ڰ��X�ӬOnot taken
				cout << " N";
				if (takenOrNot) //�p�G��� not taken ���w�� taken �N��miss�F�@�� �ҥHmiss����++
					entry[k % entriesNum]->mis++;

				entry[k % entriesNum]->BC[whichBc]--;  //���{�b���V������BC���@�檬�A �]���{�b�O not taken
				if (entry[k % entriesNum]->BC[whichBc] < 0) //�p�G�p��0�N���ӴN�b 0(SN) �����A �N�|�O���b 0(SN) �����A
					entry[k % entriesNum]->BC[whichBc] = 0;
				entry[k % entriesNum]->history.push_back('0'); //�����o����branch���G�Onot taken
				cout << setw(20)<<"misprediction: " << entry[k % entriesNum]->mis << endl; //��X�o��entries��predict mis�F�X��
			}
		}
	}
}

int main() {
	string s;
	vector<string> code; //�s��Ҧ�code
	cout << "�ж}�l��J�Ҧ�code�Alabel�йj�@���X�B�̫�@�Ӧr���O:\n�̫�@���J-1����\nEx:\nloop:\nadd R0,R1,R3\nbeq R0,R0,loop\n-1\n";
	cout << "code �}�l:\n";
	int i = 0;
	while (getline(cin, s)) { //�Ψ�Ū���Ҧ�code �åB�P�_label�b�ĴX��
		if (s == "-1")
			break;
		if (s[s.size() - 1] == ':') {
			label[s] = i;
		}
		i++;
		code.push_back(s);
	}
	resetReg(); //���sregister
	BitHistory(code); //�D��
}