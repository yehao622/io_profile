#include <iostream>
#include <string>
#include <cstring>
#include <regex>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <unordered_map>

#include "gnuplot-iostream.h"
using namespace std;

typedef unordered_map<string, float> umap;
typedef unordered_multimap<string, vector<float>> ummap;
 
void initialPhrase(umap& sig_call, ummap& all_call, string s, int& sig, int& start, float& rr_start){
	int cnt=0;

	regex re("^(-)+.*(-)+$");
	istringstream iss(s);
	if(start){
		float sec;
		string call;
		do{
			if(cnt == 1) iss >> sec;
			iss >> call;
			if(call[0]>='a' && call[0]<='z') sig_call[call] = sec;
			cnt++;
		}while(iss);
		cnt = 0;
		sig++;
	}else{
		float r_time, dura;
		string tmp_oper, oper, rec_call;
		int nostop=1; 
		vector<float> v(2);

		do{
			if(cnt == 1){
				iss >> r_time;
				rr_start += r_time;
			}
			else iss >> tmp_oper;
			if(cnt==2 && tmp_oper[0]>='a' && tmp_oper[0]<='z') oper = tmp_oper;
			if(cnt>2 && oper.size() && tmp_oper!="=") oper += tmp_oper;
			if(oper.size() && tmp_oper=="=") rec_call = oper;
			if(oper.size() && tmp_oper.size()>3 && tmp_oper[0]=='<' && \
				tmp_oper[1]>='0'&&tmp_oper[1]<='9' && tmp_oper[tmp_oper.size()-1]=='>'){
				istringstream iss_float(tmp_oper.substr(1,tmp_oper.size()-2));
				iss_float >> dura;
				v[0] = rr_start; v[1] = dura;
				all_call.insert(make_pair(rec_call, v));
				nostop = 0;
			}
			cnt++;
		}while(iss && nostop);
		cnt = 0; 
	}
	if(regex_match(s, re)) start = 1; // Begin to collect most time-consuming system calls -- the first 10 types
}

int main(int argc, char** argv){
	if(argc != 2){
		printf("Use './prof [*.prof]' to parse a [*.prof].file, and please make sure the file is under the same path.\n");
		exit(1);
	}

	ifstream input(argv[1]);
	if(!input.is_open()){
		cout << "Fail to open file: " << argv[1] << endl;
		exit(2);
	}
	
	umap sig_call; // Record top 10 time consuming calls
	ummap all_call; // Record all finished calls
	string s;
	int sig=0, start=0;
	float rr_start = 0.0;
	while(getline(input, s) && sig<10){ // Parsing line by line
		initialPhrase(sig_call, all_call, s, sig, start, rr_start);
	}

	umap::iterator sig_it;
	ummap::iterator all_it;
	Gnuplot gp;
	vector<pair<float, float> > pts;

	for(sig_it=sig_call.begin(); sig_it!=sig_call.end(); sig_it++){
		/*string fname = argv[1];
		fname += ("_" + sig_it->first + "_plot"); 
		ofstream fout(fname, ofstream::out);*/
		for(all_it=all_call.begin(); all_it!=all_call.end(); all_it++){
			size_t pos = all_it->first.find('(');
			if(pos != string::npos){
				if(all_it->first.substr(0, pos)==sig_it->first){
					//fout << sig_it->first << " " << all_it->second[0] << " " \
						 << all_it->second[1] << endl;
					pts.push_back(make_pair(all_it->second[0], all_it->second[1]));			
				}
			}
		}

		if(sig_it == sig_call.begin())
			gp << "plot " << gp.file1d(pts) << " with points title '" <<sig_it->first<<"',";
		else
			gp << gp.file1d(pts) << " with points title '" <<sig_it->first<<"',";
		//fout.close();
		pts.resize(0);
	}
	gp << endl;

	/*if(remove(fname.c_str()) != 0){
		cout << "Fail to delete file: " << fname << endl;
		exit(3);
	}*/
		
}
