//
//  hsrv.cpp
//
//  Copyright 2018  Attilio Giordana
//

using namespace std;

#include "hsrv.h"
#define NUM_INDENTS_PER_SPACE 2


// ----------- Class hsrv ---------------------


int hsrv::isXmlstring(string& s) {
	size_t found;   
	string xmlsignature = "xml version=";
	
    found = s.find(xmlsignature);
	
    if(found != string::npos) 
        return 1;
	else 
        return 0;
}

string hsrv::tailof(string& path) {
	unsigned i = path.size();
    
	while(i > 0 && path[i] != '/') 
        i--;
	
    if(path[i] == '/') 
        return path.substr(i+1);
	else 
        return path;
}

double hsrv::gettime() {
	timeval tp;
	double mtime;
	if(gettimeofday(&tp, NULL)<0) return 0;
	mtime = mtime = tp.tv_sec+(double)tp.tv_usec/1000000;
	return mtime;
}


vector<int> hsrv::CurrentTime(int d) {
	vector<int> t;
	struct tm* ptm;
	time_t ptime;
    
	ptime = time(NULL) + d*24*3600;
	ptm = localtime(&ptime);
	
    t.insert(t.end(), ptm->tm_sec);
	t.insert(t.end(), ptm->tm_min);
	t.insert(t.end(), ptm->tm_hour);
	t.insert(t.end(), ptm->tm_mday);
	t.insert(t.end(), ptm->tm_mon+1);
	t.insert(t.end(), ptm->tm_year+1900);
	t.insert(t.end(), (ptm->tm_wday==0 ? 7 : ptm->tm_wday));
	t.insert(t.end(), ptm->tm_yday+1);
	t.insert(t.end(), ptm->tm_isdst);
	
    return t;
}

int hsrv::daysecond(vector<int>& tm) {
	return tm[2]*3600+tm[1]*60+tm[0];
}


int hsrv::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent) {
	if ( !pElement ) return 0;
	
	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent = getIndent(indent);
	printf("\n");
	while (pAttrib) {
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}

void hsrv::dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 ) {
	if ( !pParent ) return;
	
	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;
	
	switch ( t ) {
		case TiXmlNode::TINYXML_DOCUMENT:
			printf( "Document" );
			break;
			
        case TiXmlNode::TINYXML_ELEMENT:
			printf( "Element [%s]", pParent->Value() );
			num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
			switch(num) {
				case 0:  printf( " (No attributes)"); break;
				case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
				default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
			}
			break;
			
        case TiXmlNode::TINYXML_COMMENT:
			printf( "Comment: [%s]", pParent->Value());
			break;
			
        case TiXmlNode::TINYXML_UNKNOWN:
			printf( "Unknown" );
			break;
        case TiXmlNode::TINYXML_TEXT:
			pText = pParent->ToText();
			printf( "Text: [%s]", pText->Value() );
			break;
			
        case TiXmlNode::TINYXML_DECLARATION:
			printf( "Declaration" );
			break;
        default:
			break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

string hsrv::getAttrbyName(TiXmlElement* pElement, string name) {
	
    if(!pElement) 
        return "*";
	
    TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	
    while (pAttrib && pAttrib->Name() != name) 
        pAttrib = pAttrib->Next();
	
    if(pAttrib) 
        return pAttrib->Value();
	else 
        return "*";
}

string hsrv::getParambyName(TiXmlNode* prg, string name) {
	TiXmlNode* pChild;
	string tmp = "*";
	
    if(!prg) 
        return tmp;
	for ( pChild = prg->FirstChild(); pChild != 0 && tmp=="*"; pChild = pChild->NextSibling())
		tmp = getAttrbyName(pChild->ToElement(), name);
	return tmp;
}

int hsrv::string2second(string& s) {
	string hh, min, ss;
	int sec = 0;
	unsigned i;
	
    hh="";
	for(i=0; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        hh += s[i];
	
    if(hh!="") 
        sec += (atoi(hh.c_str()))*3600;
	if(i==s.size()) 
        return sec;
	i++;
    
	min = "";
	for(; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        min += s[i];
	if(min!="") 
        sec += (atoi(min.c_str()))*60;
	if(i==s.size()) 
        return sec;
	i++;
    
	ss = "";
	for(; i<s.size() && s[i]!=':' && s[i]!='.'; i++) 
        ss += s[i];
	if(ss!="") 
        sec += (atoi(ss.c_str()));
	return sec;
}

string hsrv::getasciitimeday() {
	timeval tp;
	string tmp;
    
	if(gettimeofday(&tp, NULL) <0) 
        return "";
	
    tmp = string(ctime(&tp.tv_sec));
	
    return tmp.substr(11,8);
}

string hsrv::getasciitimefull() {
	timeval tp;
	string tmp;
    
	if(gettimeofday(&tp, NULL) <0) 
        return "";
	
    tmp = string(ctime(&tp.tv_sec));
	
    for(unsigned i=0; i<tmp.size(); i++) 
        if(tmp[i]==' '||tmp[i]=='\t') 
            tmp[i]='_';
	
    return tmp;
}

string hsrv::getasciitimecompact() {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
	if(gettimeofday(&tp, NULL) <0) return "";
	tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%02d%02d%02d%02d%02d%02d", (tc->tm_year+1900)%100, tc->tm_mon+1, tc->tm_mday, tc->tm_hour, tc->tm_min, tc->tm_sec);
	return string(tmp);
}

string hsrv::mySQLdate(string date) {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
    
    if(date != "") {
		while(hsrv::strReplace(date, "_", "-") == 1);
		return date;
	}
	
	if(gettimeofday(&tp, NULL) < 0) 
        return "";
	
    tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%04d-%02d-%02d", (tc->tm_year+1900), tc->tm_mon+1, tc->tm_mday);
	
    return string(tmp);
}

int hsrv::strReplace(string& s, string dst, string src) {
	size_t pos;
	
    if((pos = s.find(dst))!=string::npos) {
        s.replace(pos, dst.size(), src);
        return 1;
	}
	else 
        return 0;
}

int hsrv::upcase(string& a) {
	for(unsigned i=0; i<a.size(); i++) 
        if(a[i]<='z' && a[i]>='a') 
            a[i] = 'A'+a[i]-'a';
	return 1;
}

string hsrv::double2a(double n) {
	stringstream tmp;
    tmp.precision(5);
    tmp.setf(ios::fixed,ios::floatfield);
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::long2a(long n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::int2a(int n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

string hsrv::unsigned2a(unsigned n) {
	stringstream tmp;
	tmp << n;
    string s(tmp.str());
    
    return s;
}

double hsrv::a2double(string& n) {
	double d;
    sscanf(n.c_str(), "%lf", &d);
	
    return d;
}

long hsrv::a2long(string& n) {
	long d;
	
    sscanf(n.c_str(), "%ld", &d);
	
    return d;
}

int hsrv::a2int(string& n) {
	int d;
	
    sscanf(n.c_str(), "%d", &d);
	
    return d;
}

unsigned hsrv::a2unsigned(string& n) {
	unsigned d;
	
    sscanf(n.c_str(), "%u", &d);
	
    return d;
} 

int hsrv::isInString(std::string& s, std::string signature) {
        size_t found = s.find(signature);
        return (found != string::npos);
}


string hsrv::getday(time_t delta) {
	char tmp[MAXB];
	timeval tp;
	struct tm* tc;
	
	if(gettimeofday(&tp, NULL) <0) return "";
	tp.tv_sec += delta;

	tc = localtime(&tp.tv_sec);
	sprintf(tmp,"%04d_%02d_%02d", (tc->tm_year+1900), tc->tm_mon+1, tc->tm_mday);
	
	return string(tmp);
}

std::string hsrv::cmdExec(string command) {
    FILE* in = NULL;
    char rbuf[MAXB];
    size_t result = 0;
    int n = 0, i;

    in = popen(command.c_str(), "r");
    
    if(in != NULL){
        result = fread(rbuf, 1, (sizeof rbuf)-1, in);       
        n = pclose(in);
    }
    // pulisce la stringa da eventuali caratteri non necessari
    for(i = (int)result;i < MAXB; i++) {
        rbuf[i] = '\0';
    }

	string output(rbuf);
    
    return output;
}

pid_t hsrv::getprocessid(string& cmd) {
	size_t pos;
	size_t j;
	pid_t pid;
	string res = cmdExec("ps ax");
	pos = res.find(cmd);
	if(pos == string::npos) return 0;
	for(j = pos; res[j]!='\n' && j>0; j--);
	sscanf(res.substr(j, pos-j).c_str(), "%d", &pid);
	return pid;
}

int hsrv::convertSpecialChar(string& s) {
	string tmp="";
	for (unsigned i=0; i<s.length(); i++) {
		if (i<s.length()-2&&s[i]=='\\'&&s[i+1]=='r') {
			tmp += '\r';
			i++;
		}
		else if(s[i]=='\\'&&s[i+1]=='n') {
			tmp += '\n';
			i++;
		}
		else tmp+=s[i];
	}
	s = tmp;
	return 1;
}

vector<string> hsrv::split(string s, char separator, int limit) {
    int starting_char = 0;
    int split_number = 0;
    vector<string> splitted;
    
    for(int i = 0;i < s.size();i++) {
        if((s[i]) == separator ){
            splitted.push_back(string(s.substr(starting_char, (i - starting_char))));
            starting_char = i+1;
            split_number++;
            
            if(split_number == limit) {
                return splitted;
            }
        }
        
    }
    
    splitted.push_back(string(s.substr(starting_char)));
    
    return splitted;
}

string hsrv::getHostName() {
  string hostname = hsrv::cmdExec("/bin/hostname");
  string answer = "";
  for(size_t i=0; i<hostname.size(); i++)
    if(hostname[i] != '\n' && hostname[i] != '\r') answer += hostname[i];
  return answer;
}

const char * hsrv::getIndent( unsigned int numIndents ) {
     static const char * pINDENT = "                                      + ";
     static const unsigned int LENGTH = strlen( pINDENT );
     unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;

     if ( n > LENGTH ) n = LENGTH;
     return &pINDENT[ LENGTH-n ];
}

const char * hsrv::getIndentAlt( unsigned int numIndents ) {
     static const char * pINDENT = "                                        ";
     static const unsigned int LENGTH = strlen( pINDENT );
     unsigned int n = numIndents*NUM_INDENTS_PER_SPACE;

     if ( n > LENGTH ) n = LENGTH;

     return &pINDENT[ LENGTH-n ];
}


