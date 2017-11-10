#include <iostream>
#include <pthread.h>
#include <string>
#include <string.h>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>

#define THREAD_NUM 36

using namespace std;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int thread_stat[THREAD_NUM];    // -1 for sleep, else awake

set<string> words;
vector<set<string> > tosearch(THREAD_NUM);
map<string, size_t> searched;
string buf;
const char* query;

void* ThreadFunc(void* arg){
		size_t tid = (size_t)arg;
		pthread_mutex_lock(&mtx);
		thread_stat[tid] = -1;   //sleep
		//cout << "Thread entry " << tid << endl;
		pthread_cond_wait(&cond, &mtx);
		pthread_mutex_unlock(&mtx);

		// Search algorithm here
		while(1){
				for(set<string>::iterator it = tosearch[tid].begin(); it != tosearch[tid].end(); it++)
				{
						//consit char* query = buf.c_str();
						const char* pattern = (*it).c_str();
						size_t pos = (size_t)(strstr(query, pattern)-query +1 );
						if(pos <= buf.size() && pos >= 0){
								pthread_mutex_lock(&mtx);
								searched.insert(make_pair(*it, pos));
								pthread_mutex_unlock(&mtx);
						}
				}

				// Work done
				pthread_mutex_lock(&mtx);
				thread_stat[tid] = 1;
				pthread_cond_wait(&cond, &mtx);
				pthread_mutex_unlock(&mtx);
		}
		return NULL;
}

int main(){
		int N;
		pthread_t threads[THREAD_NUM];
		thread_stat[0] = 0;
		// Create all threads
		for(int i=0;i<THREAD_NUM;i++){
				if(pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0){
						cout << "Error from pthread_create()" << endl;
						return 0;
				}

				if(thread_stat[i] != -1){
						pthread_yield();
				}
		}

		std::ios::sync_with_stdio(false);
		cin >> N;
		for(int i=0;i<N;i++){
				cin >> buf;
				words.insert(buf);
		}
		cout << "R" << endl;

		char cmd;
		while(cin >> cmd){
				cin.get();
				getline(cin, buf);
				switch(cmd){
						case 'Q':{
												 //  Initialize
												 for(int i=0;i<THREAD_NUM;i++){
														 thread_stat[i] = -1;
												 }
												 query = buf.c_str();
												 set<string>::iterator wit = words.begin();
												 int word_idx=0;
												 while(wit != words.end()){
														 if(word_idx == THREAD_NUM){
																 word_idx = 0;
														 }
														 tosearch[word_idx++].insert(*wit);
														 wit++;
												 }

												 //cout << "All threads are ready to search" << endl;
												 pthread_cond_broadcast(&cond);

												 // Wait until all threads have finished work.
												 while(1){
														 bool all_done = true;
														 for(int i=0;i<THREAD_NUM;i++){
																 if(thread_stat[i] == -1){
																		 all_done = false;
																		 break;
																 }
														 }
														 if(all_done){
																 break;
														 }
														 pthread_yield();
												 }
												 //cout << "All threads are done" << endl;

												 //  Re-ordering
												 vector<pair<size_t, string> > vec;
												 for(map<string, size_t>::iterator it = searched.begin(); it != searched.end(); it++){
														 vec.push_back(make_pair(it->second, it->first));
												 }
												 sort(vec.begin(), vec.end());
												 int cnt = vec.size();
												 if(cnt == 0){
														 cout << "-1" << endl;
														 break;
												 }
												 vector<pair<size_t, string> >::iterator it = vec.begin();
												 for (; cnt != 0; cnt--, it++){
														 cout << it->second;
														 if (cnt != 1){
																 cout << "|";
														 }
												 }
												 for(int i=0;i<THREAD_NUM;i++)
														 tosearch[i].clear();
												 searched.clear();
												 vec.clear();
												 cout << endl;
										 }
										 break;
						case 'A':
										 words.insert(buf);
										 break;
						case 'D':
										 words.erase(buf);
										 break;
				}
		}

		return 0;
}
