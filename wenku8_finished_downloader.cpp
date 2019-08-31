#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<map>
using namespace std;
map<int,bool> lib;
char una[]={'/','\\',':','*','|','<','>','?','\"'};
bool unavailable(char a){
	for (int i=0;i<9;i++) if (a==una[i]) return 1;
	return 0;
}
int downloadbook(string booknum){
	string cmd="wget -t 16 \"http://dl.wenku8.com/down.php?type=utf8&id="+booknum+"\" -O "+booknum+".txt";
	return system(cmd.data());
}
void renamefile(string booknum){
	string filename=booknum+".txt";
	ifstream workingfile (filename.data());
	string nameline;
	getline(workingfile,nameline);
	getline(workingfile,nameline);
	getline(workingfile,nameline);
	string newname;
	for (int i=1;i<nameline.size()-1;i++){
		if (unavailable(nameline[i])) continue;
		newname+=nameline[i];
	}
	newname+=".txt";
	workingfile.close();
	cout<<filename<<"	"<<newname<<endl;
	string cmd="mv \""+filename+"\" \""+newname+"\"";
	system(cmd.data());
}
void read_lib(){
	cout<<"Reading Library..."<<endl;
	ifstream libfile (".lib");
	int tmp1;
	while (libfile>>tmp1) lib[tmp1]=1;
}
void write_lib(string newbook){
	ofstream libfile (".lib",ios::app);
	libfile<<newbook<<endl;
}
int main(){
	//renamefile("2333");
	ofstream logerr ("wenku8_downlaoder.log");
	int errcount=0;
	read_lib();
	string content;
	int page=1;
	while (1){
		system("rm .cache");
		char num[3];
		sprintf(num,"%d",page);
		string cmd;
		cout<<num<<endl;
		cmd+="curl -s 'https://www.wenku8.net/modules/article/articlelist.php?fullflag=1&page=";
		cmd+=num;
		cmd+="' -H 'authority: www.wenku8.net' -H 'pragma: no-cache' -H 'cache-control: no-cache' -H 'upgrade-insecure-requests: 1' -H 'user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.132 Safari/537.36' -H 'sec-fetch-mode: navigate' -H 'sec-fetch-user: ?1' -H 'dnt: 1' -H 'accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3' -H 'sec-fetch-site: none' -H 'accept-encoding: gzip, deflate, br' -H 'accept-language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7' -H 'cookie: PHPSESSID=fm4aoqth0uuke07uucgt5qqtdhft59am; jieqiUserInfo=jieqiUserId%3D254439%2CjieqiUserName%3Dytoworld%2CjieqiUserGroup%3D3%2CjieqiUserVip%3D0%2CjieqiUserPassword%3Dc1b577aadd06e0657d43b1221527848a%2CjieqiUserName_un%3Dytoworld%2CjieqiUserHonor_un%3D%26%23x65B0%3B%26%23x624B%3B%26%23x4E0A%3B%26%23x8DEF%3B%2CjieqiUserGroupName_un%3D%26%23x666E%3B%26%23x901A%3B%26%23x4F1A%3B%26%23x5458%3B%2CjieqiUserLogin%3D1567140027; jieqiVisitInfo=jieqiUserLogin%3D1567140027%2CjieqiUserId%3D254439; jieqiVisitTime=jieqiArticlesearchTime%3D1567142886; jieqiVisitId=article_articleviews%3D1508%7C597%7C2306%7C133%7C2610%7C2080%7C1592%7C1367%7C5%7C2605%7C2277%2Cjieqi_article_reviews_views%3D192952%7C193038%7C192928%7C192537' --compressed -o .cache               ";
		cout<<cmd<<endl;
		system(cmd.data());
		FILE* cache=fopen(".cache","r");
		char tmp1=getc(cache);
		while (tmp1!=-1){
			content+=tmp1;
			tmp1=getc(cache);
		}
		fclose(cache);
		int find1=content.find(".htm\" target=\"_blank\"",0);
		if (find1==-1) break;
		while (find1!=-1){
			cout<<find1<<endl;
			string booknum;
			for (int i=find1-4;i<find1;i++) if (content[i]>47&&content[i]<58) booknum+=content[i];
			find1=content.find(".htm\" target=\"_blank\"",find1+20);
			if (lib[atoi(booknum.data())]) continue;
			if (downloadbook(booknum)){
				logerr<<"下载失败:"<<booknum<<endl;
				errcount++;
				continue;
			}
			//downloaded.push_back(booknum);
			renamefile(booknum);
			write_lib(booknum);
		}
		//content.find("class=\"last\">");
		cout<<"Newpage::::::::::::::::::::::::"<<endl;
		page++;
		content="";
	}
	if (errcount) cout<<"出现"<<errcount<<"个错误。请查看wenku8_downlaoder.log并尝试手动下载。"<<endl;
	else cout<<"下载完毕。未出现错误。"<<endl;
	return 0;
}