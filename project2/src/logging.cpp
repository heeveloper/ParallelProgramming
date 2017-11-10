#include "logging.h"

extern int64_t* records;

void makeLog(size_t tid){
	stringstream sstm;
	sstm << "thread" << tid+1 << ".txt";
	ofstream out(sstm.str().c_str(), ios::app);
	out.close();
	return;
}

void printLog(size_t tid, size_t commit_id, size_t i, size_t j, size_t k){
	stringstream sstm;
	sstm << "thread" << tid+1 << ".txt";
	ofstream out(sstm.str().c_str(), ios::app);
	if(out.is_open()){
		out << commit_id << " " << i << " " << j << " " << k << " " << records[i] << " " << records[j] << " " << records[k] << endl;
	}
	out.close();
	return;
}
