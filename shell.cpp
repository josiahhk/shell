#include "std_lib_facilities_4.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
 
 void trim(string& s){
	size_t p = s.find_first_not_of(" \t");
	s.erase(0, p);

	p = s.find_last_not_of(" \t");
		if (string::npos != p)
		s.erase(p+1);
}

//separate string by spaces
vector<string> parse(string in){
	string current = in;
	vector<string> tokens;
	//trim(current);
	int count = 0;
	int index = 0;
	int next = 0;
	int difference = 0;
	//counts the number of spaces
	for(int i = 0; i < in.size(); ++i){
		if(in[i] == ' ')
			count++;
	}
	//
	for(int j = 0; j <= count; j++)
		if(j == count){
			tokens.push_back(current.substr(index, current.size()-index));
		}
		else{
			next = in.find(" ", index);
			difference = in.find(" ", index)-index;
			tokens.push_back(current.substr(index, difference+1));
			index = next + 1;
			
		}
	return tokens;
}


//parse string based on symbols
vector<string> parseFurth(vector<string> v){
	string buff = "";
	bool curr; //curr stores whether the current buffer is a symbol or a char
	bool last;
	vector<string> rtnVec;
	for(int i = 0; i < v.size(); ++i)
		for(int j = 0; j < v[i].size(); ++j)
			if(v[i].size() == 1){
				buff = buff+ v[i][j];
				rtnVec.push_back(buff);
				buff = "";
			}
			else if(j == 0){ //first symbol
				if((v[i][j] == '"') || (v[i][j] =='\'') || (v[i][j]=='<') || (v[i][j] == '|')
				|| (v[i][j] == '&') || (v[i][j] == '>')){
					curr = true;
					last = curr;
					buff = buff+v[i][j];
				}
				else{
					curr = false;
					last = curr;
					buff = buff+v[i][j];
				}
			}
			else if(j == v[i].size()-1){ //last symbol
				if((v[i][j] == '"') || (v[i][j] =='\'') || (v[i][j]=='<') || (v[i][j] == '|')
				|| (v[i][j] == '&') || (v[i][j] == '>')){
					curr = true;
					if(curr != last){
						rtnVec.push_back(buff);
						buff = "";
						buff = buff + v[i][j];
						rtnVec.push_back(buff);
						buff = "";
					}
					else{
						buff = buff + v[i][j];
						rtnVec.push_back(buff);
						buff = "";
					}
				}
				else{					
					curr = false;
					if(curr != last){
						rtnVec.push_back(buff);
						buff = "";
						buff = buff + v[i][j];
						rtnVec.push_back(buff);
						buff = "";
					}
					else{
						buff = buff + v[i][j];
						rtnVec.push_back(buff);
						buff = "";
					}
				}
			}
			else{
				if((v[i][j] == '"') || (v[i][j] =='\'') || (v[i][j]=='<') || (v[i][j] == '|')
				|| (v[i][j] == '&') || (v[i][j] == '>')){
					curr = true;
					if(curr != last){
						rtnVec.push_back(buff);
						buff = "";
					}
					buff = buff + v[i][j];
					last = true;
				}
				else{
					curr = false;
					if(curr != last){
						rtnVec.push_back(buff);
						buff = "";
					}
					buff = buff + v[i][j];
					last = false;
				}
			}
			return rtnVec;
}


	
void trimr(string& s){
	if(s[s.size()-1] == '\r')
		s.erase(s.begin()+s.size()-1);
}

void push_together(vector<string> & t){
	int k, l =0;
	bool found = false;
	//push together things with quotes
	for(int i = 0; i < t.size(); i++)
		if(t[i][0] == '\"') //open quotes will always be at the front 
			for(int j = i+1; j < t.size(); j++)
				if(t[j][0] == '\"' || t[j][t[j].size()-1] == '\"'){ //c quotes could be " or >>>"
					k = i; l = j; found = true;
				}
	//merges everything together starting from the back
	if(found == true){
		for(int j = l; j > k; j--){ 
			t[j-1] = t[j-1] + t[j];
			t.erase(t.begin()+j);
		}
		//erase quotations (for echo w/ quotes)
		t[k].erase(t[k].begin());
		t[k].erase(t[k].end()-1);
		
	}
	//get rid of spaces at the end (add case for numerous spaces in a row)
	for(int i = t.size()-1; i >=0; i--)
		if(t[i][t[i].size()-1] == ' '){
			if(t[i].size() == 1)
				t.erase(t.begin()+i);
			else t[i].erase(t[i].begin()+t[i].size()-1);
		}
		
		
	//pushes together single quotes
	found = false;
	k, l = 0;
	for(int i = 0; i < t.size(); i++)
		if(t[i] == "\'")
			for(int j = i+1; j < t.size(); j++)
				if(t[j] == "\'"){
					k = i; l = j; found = true;
				}
	//merges everything together starting from the back
	if(found == true)
		for(int j = l; j > k; j--){ 
			t[j-1] = t[j-1] + t[j];
			t.erase(t.begin()+j);
		}
		

}


vector<vector<string>> numPipes(vector<string> tokens){
	//part 1: finds all the pipes and returns a 2D vector of commands
	int count = 0;
	vector<vector<string>> commands;
	vector<string> c1;
	vector<char*> final_vec;
	for(int i = 0; i < tokens.size(); ++i){
		if(tokens[i] == "|"){
			count++;
			commands.push_back(c1);
			c1.clear();
		}
		else c1.push_back(tokens[i]);
	}
	commands.push_back(c1);

	return commands;
}


//add a case for exit + stuff
int main(){
	
	bool exit = false;
	string input;
	vector<vector<string>> commands;
	int stdinbkp = dup(0);
	int stdoutbkp = dup(1);
	
	//begin shell
	while(exit != true){
		vector<string> tokens;
		vector<string> tokens2;
		vector<int> all_pid;
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		string wd(cwd);
		string wd1 = wd.substr(wd.find_last_of("/")+1);
		int pid;
		bool bgp = false;
		time_t now = time(0);
		string dt = ctime(&now);
		//cout<<"INPUT SOMETHING, NERD =========="<<endl;
		dt.erase(dt.end()-1);
		cout<<"["<<dt<<" "<<wd1<<"]";
		getline(cin,input);
		if(input.size()==0)
			;
		else{
			trimr(input);
			if(input == "exit"){
				exit = true;
				break;
			}
		
			tokens = parse(input);
			tokens2 = parseFurth(tokens);
			push_together(tokens2);
			commands = numPipes(tokens2);
			
		
		/*		
				for(int i = 0; i < commands.size(); i++)
					for(int j = 0; j < commands[i].size(); j++)
						cout<<"token at "<<i<<j<<": "<<commands[i][j]<<endl;
		*/	
				
			if(commands[0][0] == "cd"){
				if(commands[0].size() > 1){
					if(commands[0][1] == "-")
						chdir("/home/ec2-user");
					else
						chdir(commands[0][1].c_str());
				}
			}
			
			else{
			
				for(int i =0; i < commands.size(); i++){
					int fd[2];
					pipe(fd);
					int ird = -1;
					int ord = -1;
					
					for(int j = 0; j < commands[i].size(); j++){
						if(commands[i][commands[i].size()-1] == "&")
							bgp = true;
						if(commands[i][j] == ">"){
							ord = j;
							break;
						}
						if(commands[i][j] == "<"){
							ird = j;
							break;
						}	
					}						
					//cout<<"fd[0]: "<<fd[0]<<endl<<"fd[1]: "<<fd[1]<<endl;
					
					//Formatting strings to char* for use in exec
					//=====================================================
					char* cmd = (char*) commands[i][0].c_str();
					char* argv[50];
					if(bgp == true)
						commands[i].erase(commands[i].end()-1);
					if(ird != -1){ //sets up input redirection
						for(int j = 0; j < ird; j++)
							argv[j] = (char*)commands[i][j].c_str();
					}
					else if (ord != -1){ //sets up arguments for output redirection
						for(int j = 0; j < ord; j++)
							argv[j] = (char*)commands[i][j].c_str();
					}
					else{ //no redirection
						for(int j = 0; j < commands[i].size(); j++)
						argv[j] = (char*)commands[i][j].c_str();
					}
					argv[0] = cmd;
					argv[commands[i].size()] = NULL;
					//======================================================
					int status;
					int pid = fork();
						if(pid == 0){
							if(ord != -1){
								int fd2 = open (commands[i][ord+1].c_str(), O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
								dup2(fd2, 1);
							}
							else if (ird != -1){
								int fd2 = open(commands[i][ird+1].c_str(), O_RDONLY, 0);
								dup2(fd2, 0);
							}
							else if(i ==  commands.size()-1)
								dup2(stdoutbkp, 1); //redirects output to screen if last
							else
								dup2(fd[1], 1);
							close(fd[0]);
							execvp(cmd, argv);
						}
						else{
							
							
							dup2(fd[0], 0);	
							close (fd[1]);
							if(bgp == true){
								all_pid.push_back(pid);
							}
							else
								wait(0);
							for(int i = 0; i < all_pid.size(); i++)
								if(waitpid(all_pid[i], &status, WNOHANG) > 0)
									all_pid.erase(all_pid.begin()+i);
						}
							
				}
							dup2(stdinbkp, 0);
							dup2(stdoutbkp, 1);
			}
		}
	}
}