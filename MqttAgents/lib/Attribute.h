/* 
 * File:   Attribute.h
 * Author: attilio
 *
 * Created on November 22, 2012, 11:30 AM
 */

#ifndef ATTRIBUTE_H
#define	ATTRIBUTE_H

#include "hsrv.h"
#include "FileManager.h"

class TiXmlNode;

/** @brief Attributes 
 
 Utilizzata per creare istanze di attributi ognuno dei quali ha un nome e un valore associato
 @author Attilio Giordana
 @date December 2018
 */
class Attribute {
public:
	/**
	@var std::string
	@brief Nome dell'attributo 
	*/
       std::string name;
       	/**
	@var std::string
	@brief valore dell'attributo 
	*/
	std::string category;
	/**
	@var std::string
	@brief categoria dell'attributo di default = "generic"
	*/
	std::string value;
        /**
	Costruttore della classe. Crea un'istanza di un nuovo attributo inizializzando 
	il nome, la categoria e il valore
	@param a nome attributo
	@param b valore dell'attributo
	*/
	Attribute(std::string a, std::string c, std::string b) {name = a; category=c; value=b;};
	/**
	Costruttore della classe. Crea un'istanza di un nuovo attributo inizializzando 
	il nome e il valore
	@param a nome attributo
	@param b valore dell'attributo
	*/
	Attribute(std::string a, std::string b) {name = a; value=b; category="generic";};
	/**
	Costruttore della classe. Crea un'istanza di un nuovo attributo inizializzando 
	il nome.
	Il valore viene inzializzato con la stringa "ON"
	@param a nome attributo
	*/
	Attribute(std::string a) {name = a; category="generic"; value="ON";};
	/**
	Costruttore della classe. Crea un'istanza di un nuovo attributo inizializzando
	prendendo le informazioni da un file XML
	@param desc puntatore ad un nodo XML
	@see xmlDecode()
	*/
	Attribute(TiXmlNode* desc){xmlDecode(desc);};
	/**
     Crea una stringa che contiene le informazioni sull'attributo in formato XML. 
     La stringa viene poi appesa ad una std::ostringstream passato per riferimento
     @param out ostringstream alla quale viene appesa la stringa creata
     @param md se > 0 inserisce l'intestazione xml
     @return 1 se tutto ok
     */
	bool xmlPrint(std::ostringstream& out, int md=0) {
		if(md > 0) 
			out << "<?xml version=\"1.0\" ?>";
		out << "<attribute name=\"" << name << "\" value=\"" << value << "\"/>"; 
		return 1;
	};
	/**
     Codifica le informazioni sull'attributoin XML
     @param md se > 0 inserisce l'intestazione xml
     @return stringa XML contenente le informazioni sull'attributo 
     */
     std::string xmlEncode(int md=1);
	/**
     Estrae le informazioni sull'attributo da un nodo XML
     @param desc puntatore ad un nodo XML contenente le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	bool xmlDecode(TiXmlNode*);
	/**
     Estrae le informazioni sull'attributo da una stringa in formato XML
     @param xmldesc stringa XML con le informazioni sull'attributo
     @return 1 se tutto ok, 0 altrimenti
     @see hsrv::getAttrbyName()
     */
	bool xmlDecode(std::string&);
	/**
     Carica il file XML con il nome specificato
     @param name nome del file XML su disco
     @return 1 se xmlDecode viene eseguita correttamente, 0 altrimenti
     @see xmlDecode(), hsrv::isfile()
     */
	bool xmlLoad(std::string);
	/**
     Salva un file XML che contiene le informazioni sull'istanza di Attribute 
     codificate in XML
     @param name nome del file XML su disco
     @return 1 se tutto ok, 0 altrimenti
     @see xmlEncode()
     */
	bool xmlSave(std::string);
	/**
     Cerca se esiste un attributo con il nome specificato e se ha come valore la
     stringa "ON" o la stringa "true"
     @param n nome dell'attributo
     @return 1 è presente, 0 altrimenti
     */
	bool member(std::string n) {
		if(n == name && (value=="ON" || value=="true")) return 1;
		else return 0;
	};
};


#endif	/* MATTRIBUTE_H */
