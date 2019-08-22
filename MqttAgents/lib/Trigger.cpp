/* 
 * File:   Trigger.cpp
 * Author: attilio
 * 
 * Created on November 21, 2012, 11:19 PM
 */

#include "Trigger.h"

using namespace std;


Trigger::Trigger(string file): Params(file) {
    load();
    initSubject();
}

void Trigger::start() {
   if(get("active")=="ON") tr = thread(&Trigger::exec, this);
}


bool Trigger::initSubject() {
    setof<Attribute> tr;
    tr = extractCategory("trigger");
    
    for(unsigned i=0; i<tr.size(); i++) {
        string v = tr[i].value;
        tm[tr[i].name] = hsrv::a2double(v);
        // name = tipo | value = sottotipo
    }
    
    return true;
}

void Trigger::exec() {
    //cout << "[Trigger] exec thread started" << endl;
    long MCD = (long)(tm.begin()->second * TPRECISION);
    long m;
    int res;
    
    map<string, double>::iterator it;
    for(it = tm.begin(); it != tm.end(); it++) { 
        m = (long)(it->second * TPRECISION);
        MCD = Euclide(MCD, m);
    }
    
    cycle = MCD / TPRECISION;
    rqtp.tv_sec = (int) cycle;
    rqtp.tv_nsec = (int)(NANOPERSECOND*(cycle - rqtp.tv_sec));
    while( true ) {
        for(it = tm.begin(); it != tm.end(); it++) {
            it->second-= cycle;
            if(it->second <=0) {
                string v = get(it->first,"trigger");
                it->second = hsrv::a2double(v);
		string content = "{request:"+it->first+"}";
                Message m("local/trigger", content );
		Conf::mbx.Send("cmdexecutor", m);
		//cout<<"Trigger: "<<m.theme<<" "<<m.content<<endl;
            }
        }
        res = nanosleep(&rqtp, &rmtp);
    }
    return;
}

long Trigger::Euclide(long a, long b)  { // prototipo della funzione Euclide //
    long r;
    r = a % b;             // operazione modulo //
    while(r != 0) {         // ciclo di iterazione //
        a = b;
        b = r;
        r = a % b;
    }
    return b;
}
