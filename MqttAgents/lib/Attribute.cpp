/* 
 * File:   Mattribute.cpp
 * Author: attilio
 * 
 * Created on November 22, 2012, 11:30 AM
 */


using namespace std;

#include "Attribute.h"

//------------------------- Class Attribute --------------------------------

string Attribute::xmlEncode(int md) {
	string r="";
	if(md > 0) r = "<?xml version=\"1.0\" ?>";
	r = r+"<attribute name=\""+name+"\" category=\""+category+"\" value=\""+value+"\"/>"; 
	return r;
}

bool Attribute::xmlDecode(TiXmlNode* desc) {
	TiXmlElement* pElement;
	
	if(desc == NULL) 
		return false;
	pElement = desc->ToElement();
	
	if(pElement == NULL) 
		return false;
		
	name = hsrv::getAttrbyName(pElement, "name");
        category = hsrv::getAttrbyName(pElement, "category");
        if(category=="*") category = "generic";
	value = hsrv::getAttrbyName(pElement, "value");
	
	return true;
}

bool Attribute::xmlDecode(string& xmldesc) {
	TiXmlDocument doc;
	TiXmlNode* prg;
	TiXmlElement* pElement;
	const char* pTest = doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
	
	if(pTest != NULL) 
		return false;
	
	prg = doc.FirstChild();
	
	if(prg == NULL) 
		return false;
	
	prg = prg->NextSibling();
	
	if(prg == NULL) 
		return false;
	
	pElement = prg->ToElement();
	
	if(pElement == NULL) 
		return false;
	
	name = hsrv::getAttrbyName(pElement, "name");
        value = hsrv::getAttrbyName(pElement, "category");
        if(category=="*") category = "generic";
	value = hsrv::getAttrbyName(pElement, "value");
	
	return true;
}

bool Attribute::xmlLoad(string name) {
	bool res;
	TiXmlDocument* doc;
	
	if(!FileManager::isfile(name, "xml")) 
		return false;
	
	doc = new TiXmlDocument(name.c_str());
	res = doc->LoadFile();
	
	if(!res) 
		return false;
	
	return xmlDecode(doc->FirstChild()->NextSibling());
}


bool Attribute::xmlSave(string name) {
	ofstream out;
	string r;
	
	out.open(name.c_str());
	r = xmlEncode(1);
	out << r << endl;
	out.close();
	
	return true;
}


