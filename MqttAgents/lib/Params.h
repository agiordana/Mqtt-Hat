/*
 *  agentlib.h
 *  phoneagent
 *
 *  Created by Attilio Giordana on 10/27/11.
 *  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _PARAMS_
#define _PARAMS_

#include <bitset>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include "Template.h"
#include "hsrv.h"
#include "Attribute.h"
#include "FileManager.h"
#include "Conf.h"

const unsigned int NUM_INDENTS_PER_SPACE=2;

class Params: public setof<Attribute> {
public:
    
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
     @param name stringa che indica il tipo del parametro
     @see Conf::configdir
     */
	Params(std::string name){ty = name; path = Conf::configdir+"/"+name+".xml"; pathh = Conf::homedir+"/"+name+".xml";};
	
    /**
     Costruttore della classe. Inizializza le variabili d'istanza.
	 @param dir sotto-directory di Conf::configdir dove risiede la descrizione dell'istanza
     @param name stringa che indica il tipo del parametro
     @see Conf::configdir
     */	
	Params(std::string dir, std::string name){
		ty = name; path = Conf::configdir+"/"+dir+"/"+name+".xml"; pathh = Conf::homedir+"/"+dir;
		FileManager::makeDir(pathh, false);
		pathh = pathh + "/"+name+".xml";
	};
    
	Params() {ty="nonname"; path="/tmp"; pathh = "/tmp"; };
    
    /**
     Carica un insieme di attributi da un file XML
     @param path del file XML che contiene gi attributi da caricare
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlLoad()
     */
    int load() {if(FileManager::isfile(pathh)) return xmlLoad(pathh); else return xmlLoad(path); };
	
    /**
     Salva su file in "home" gli attributi in formato XML
     @param path del file XML su cui salvare gli attributi
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlSave()
     */
    int save() { return xmlSave(pathh); };
    /**
     Salva su file in "config" gli attributi in formato XML
     @param path del file XML su cui salvare gli attributi
     @return 1 se tutto ok, 0 altrimenti
     @see setof::xmlSave()
     */
    int config() {return xmlSave(path);};
    /**
     Cancella i files su cui è salvato l'isieme di parametri nella home e nella config dir
     @param path del file nella config dir
     @param pathh del file nella home dir
     @return 1
    */
    int remove() { FileManager::deleteFile(path); FileManager::deleteFile(pathh); return 1; };
    /**
      Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string get(std::string name, std::string category) {
        std::string r = "";
	for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) 
                r = (*this)[i].value;
	return r;
    };
    
    /**
     Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string get(std::string name) {
	std::string r = "";
	for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") 
                r = (*this)[i].value;
		return r;
	}
	
       /**
     Recupera il valore dell'attributo con il nome passato.
     @param name nome dell'attributo da cercare
     @return valore dell'attributo se esiste, stringa vuota altrimenti
     */
    std::string categoryof(std::string name) {
		std::string r = "";
		for (unsigned i = 0; i < size() && r == ""; i++) 
            if((*this)[i].name == name) 
                r = (*this)[i].category;
		return r;
    }

    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti non fa nulla.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 se l'attributo esiste, 0 altrimenti
     */
    int set(std::string name, std::string category, std::string value) {
	int r = 0;
	for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) {
				(*this)[i].value=value;
				r = 1;
			}
	return r;
    }
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti non fa nulla.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 se l'attributo esiste, 0 altrimenti
     */
    int set(std::string name, std::string value) {
	int r = 0;
	for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") {
				(*this)[i].value=value;
				r = 1;
			}
	return r;
     }	
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti lo aggiunge.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 
     */
    int add(std::string name, std::string category, std::string value) {
        int r = 0;
        for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == category) {
				(*this)[i].value=value;
				r = 1;
			}
        if(r==0) {
            push_back(Attribute(name,category,value));
        }
        return 1;
    };
    /**
     Assegna il valore all'attributo con il nome passato se l'attributo esiste. Altrimenti lo aggiunge.
     @param name nome dell'attributo da cercare
	 @param value valore da assegnare all'attributo name
     @return 1 
     */
    int add(std::string name, std::string value) {
        int r = 0;
        for (unsigned i = 0; i < size() && r == 0; i++) 
            if((*this)[i].name == name && (*this)[i].category == "generic") {
				(*this)[i].value=value;
				r = 1;
			}
        if(r==0) {
            push_back(Attribute(name,"generic",value));
        }
        return 1;
    };	
     /**
     Controlla se esiste un attributo della categoria e del valore passati come parametro 
     @param value valore dell'attributo da cercare
     @return 1 se è presente nell'insieme, 0 altrimenti
     */
    int checkValue(std::string value, std::string category){
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].value == value) 
                return 1;
        return 0;
    };
    /**
     Controlla se esiste un attributo con il valore passato come parametro 
     @param value valore dell'attributo da cercare
     @return 1 se è presente nell'insieme, 0 altrimenti
     */
    int checkValue(std::string value){
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].value == value) 
                return 1;
        return 0;
    };
    /**
     * ritorna l'insieme di tutti gli attributi che hanno il nome passato come parametro
     * @param name
     * @return 
     */
     setof<Attribute> extractNamed(std::string name) {
        setof<Attribute> r;
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].name == name) r.push_back((*this)[i]);
        return r;  
    };
    /**
     * ritorna l'insieme di tutti gli attributi che hanno la categoria passata come parametro
     * @param category
     * @return 
     */

    setof<Attribute> extractCategory(std::string category) {
        setof<Attribute> r;
        for(unsigned i = 0; i < this->size(); i++) 
            if((*this)[i].category == category) r.push_back((*this)[i]);
        return r;
    };
    
    int reset() {return xmlLoad(path);};
private:
    
    /** 
     @var std::string
     @brief path del file di configurazione in formato XML
     */
	std::string path;
	std::string pathh;
	
};

#endif
