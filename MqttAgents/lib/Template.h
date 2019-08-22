//
//  templates.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef _TEMPLATES_
#define _TEMPLATES_

#include "tinyxml.h"
#include "tinystr.h"
#include "FileManager.h"

/** @brief Template per insieme di oggetti
 
 Template per un insieme di oggetti di classe T. Eredita Gli attributi e i metodi dalla classe
 standard vector e dalla classe di servizio hsrv.
 @author Attilio Giordana
 @date Novembre 2011
 */
template <class T> class setof: public std::vector<T> {
public:
	setof() {ty="";};
	setof(std::string t) {ty = t;};
	std::string ty;
    /**
     Aggiunge alla ostringstream passata come parametro la stringa che contiene le informazioni 
     sul tipo di oggetto contenuto 
     @param out ostringstream che contiene la stringa XML con le informazioni sul messaggio
     @param md se > 0 stampa l'intestazione XML
     @return true se tutto ok
     */
	bool xmlPrint(std::ostringstream& out, int md = 0) {
		if(md > 0) 
            out << "<?xml version=\"1.0\" ?>" << std::endl;
		if(ty == "") 
            out<<"<set>"<<std::endl;
        else 
            out << "<set type=\"" << ty << "\">" << std::endl;
		for(unsigned i=0; i<(*this).size(); i++) {
			(*this)[i].xmlPrint(out,0);
			out << std::endl;
		}
		out << "</set>" << std::endl;
		return true;
	};
    /**
     Ritorna una stringa con le informazioni in formato XML
     @param md se > 0 stampa l'intestazione XML
     @return stringa contenente le informazioni in formato XML
     */
	std::string xmlEncode(int md=0) {
		std::string r = "";
		if(md > 0) 
            r = "<?xml version=\"1.0\" ?>";
		if(ty == "") 
            r += "<set>";
        else 
            r += "<set type=\"" + ty + "\">";
        
		for(unsigned i = 0; i < (*this).size(); i++)
			r += (*this)[i].xmlEncode(0);
		r += "</set>";
		return r;
	};
    /**
     Estrae le informazioni sull'attributo da una stringa in formato XML
     @param xmldesc stringa XML con le informazioni sull'attributo
     @return true se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName(), hsrv::charExplode(), hsrv::charPrintf()
     */
	bool xmlDecode(std::string& xmldesc) {
		TiXmlDocument doc;
		TiXmlNode* prg;
		TiXmlNode* pChild;
        
		doc.Parse(xmldesc.c_str(), 0 , TIXML_ENCODING_UTF8);
		prg = doc.FirstChild();
		prg = prg->NextSibling();
		
        if(prg == NULL) 
            return false;
		
        ty = hsrv::getAttrbyName(prg->ToElement(), "type");
		(*this).clear();
		
        for ( pChild = prg->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			this->push_back(T(pChild));
		
        return true;
	};
    /**
     Estrae le informazioni sull'attributo da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni
     @return true se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	bool xmlDecode(TiXmlNode* desc) {
		TiXmlNode* pChild;
		
        if(desc == NULL) 
            return false;
		
        ty = hsrv::getAttrbyName(desc->ToElement(), "type");
		(*this).clear();
		
        for ( pChild = desc->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
			this->push_back(T(pChild));
		return true;
	};	
    /**
     Carica le informazioni dal file XML con il nome passato come parametro
     @param name nome del file XML contenente le informazioni da caricare
     @return true se xmlDecode termina correttamente, 0 altrimenti
     */
	bool xmlLoad(std::string name) {
		bool res;
		TiXmlDocument* doc;
		
        if(!FileManager::isfile(name, "xml")) 
            return false;
		
        doc = new TiXmlDocument(name.c_str());
		res = doc->LoadFile();
		
        if(!res) 
            return false;
		
        return xmlDecode(doc->FirstChild()->NextSibling());	
	};
    /**
     Salva un file XML che contiene le informazioni sull'istanza dell'oggetto 
     codificate in XML
     @param name nome del file XML su disco
     @return true se tutto ok, 0 altrimenti
     @see xmlEncode()
     */
	bool xmlSave(std::string name) {
        std::ofstream out;
		std::string r;
		out.open(name.c_str());
        
		if(!out.is_open()) 
            return false;
		
        r = xmlEncode(1);
		out << r << std::endl;
		out.close();
        
		return true;
	};
    /**
     Cerca se esiste un attributo con il nome specificato e se ha come valore la
     stringa "ON" o la stringa "true"
     @param n nome dell'attributo
     @return true se è presente, 0 altrimenti
     */
	bool member(std::string n) {
		for(unsigned i = 0; i < (*this).size(); i++) 
            if((*this)[i].member(n)) 
                return true;
		return false;
	};
    
};


/** @brief Template per coda di messaggi
 
 Template per una coda di messaggi con priorità. Viene gestita anche la mutua esclusione attraverso 
 l'utilizzo del meccanismo dei semafori.
 @author Attilio Giordana
 @date Novembre 2011
 */
template<class Q> class MsgQueue: public std::vector<Q> {
public:
    /**
     Crea una nuova istanza di una coda di messaggi di oggetti di tipo Q. Inizializza la variabile
     di condizione e quella di mutua esclusione usate dalle varie thread
     */
	MsgQueue(){ 
		pthread_cond_init(&bfull, NULL);
		pthread_mutex_init(&mtx, NULL);
		maxlength = 0;
		
	};
	
	MsgQueue(int n){ 
		pthread_cond_init(&bfull, NULL);
		pthread_mutex_init(&mtx, NULL);
		if(n>0) maxlength = n;
		else maxlength = 0;
		
	};
	
    /**
     Elimina la coda di messaggi e le variabili di condizione e di mutua esclusione usate dalle 
     thread
     */
	~MsgQueue() {
		pthread_cond_destroy(&bfull);
		pthread_mutex_destroy(&mtx);
	};
    /**
     Aggiunge un'istanza della classe Q al vettore in mutua esclusione
     @param m istanza della classe Q da inserire nel vettore
     @return true se tutto ok
     */
	bool send(Q& m) {
		pthread_mutex_lock(&mtx);
		this->push_back(m);
		purge();
		pthread_cond_signal(&bfull);
		pthread_mutex_unlock(&mtx);
		return true;
	};
    /**
     Elimina tutti gli elementi dal vettore
     @return true se tutto ok
     */
	bool reset() {
		while((*this).size()>0) erase((*this).begin());
		return true;
	};
	/**
     Estrae un'istanza di un oggetto Q in mutua esclusione e lo elimina dal vettore.
     Gli oggetti vengono estratti in base alla loro priorità
     @return oggetto con priorità più alta
     */
	Q receive() {
		pthread_mutex_lock(&mtx);
		if((*this).size() == 0) pthread_cond_wait(&bfull, &mtx);
		Q v;
		unsigned i;
		unsigned r = 0;
		for(i=0; i<(*this).size(); i++) 
            if((*this)[i].priority > (*this)[r].priority) 
                r = i;
		v = (*this)[r];
		this->erase((*this).begin()+r);
		pthread_mutex_unlock(&mtx);
		return v;
	};
    /**
     Ritorna il valore della priorità più alta cercando tra tutti gli oggetti presenti nella coda
     @return valore priorità più alta
     */
	int priority() {
		int c = -1;
		pthread_mutex_lock(&mtx);
		for(unsigned i=0; i<(*this).size(); i++) if((*this)[i].priority>c) c = (*this)[i].priority;
		pthread_mutex_unlock(&mtx);
		return c;
	};
    /**
     Inizializza la variabile di condizione e quella di mutua esclusione usate dalle varie thread
     @return true se tutto ok
     */
	bool init() {
		pthread_cond_init(&bfull, NULL);
		pthread_mutex_init(&mtx, NULL);
		return true;
	};
    
private:
    /**
     @var pthread_mutex_t 
     @brief variabile utilizzata per la mutua esclusione 
     */
	pthread_mutex_t mtx;
	/**
     @var pthread_mutex_t 
     @brief variabile di condizione 
     */
    pthread_cond_t bfull;
	int maxlength;
	
	bool purge() {
		unsigned n = 0;
		if (maxlength==0 || maxlength >= (*this).size()) return false;
		for(unsigned i=0; i<(*this).size(); i++) if((*this)[i].priority<(*this)[n].priority) n = i;
		(*this).erase((*this).begin()+n);
		return true;
	};
};


#endif
