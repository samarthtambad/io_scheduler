
class IOScheduler
{
    public:
        IOScheduler();
        ~IOScheduler();
        virtual void add_request(ioreq_t*) = 0;
        virtual ioreq_t* get_next_req() = 0;
        virtual bool is_request_pending() = 0;
};
IOScheduler::IOScheduler(){}



class FIFO : public IOScheduler
{
    private:
        std::deque<ioreq_t*> pending_ioreq;
    public:
        FIFO();
        ~FIFO();
        void add_request(ioreq_t*);
        ioreq_t* get_next_req();
        bool is_request_pending();
};
FIFO::FIFO(){}
void FIFO::add_request(ioreq_t* r){
    pending_ioreq.push_back(r);
}
ioreq_t* FIFO::get_next_req(){
    ioreq_t* req;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
    return req;
}
bool FIFO::is_request_pending(){
    return !pending_ioreq.empty();
}



class SSTF : public IOScheduler
{
    private:
        std::deque<ioreq_t*> pending_ioreq;
    public:
        SSTF();
        ~SSTF();
        void add_request(ioreq_t*);
        ioreq_t* get_next_req();
        bool is_request_pending();
};
void SSTF::add_request(ioreq_t* r){
    pending_ioreq.push_back(r);
}
SSTF::SSTF(){}
ioreq_t* SSTF::get_next_req(){
    ioreq_t* req;
    int min_seek = 10000;
    std::deque<ioreq_t*>::iterator min_seek_idx;
    
    for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
        int seek_time = abs(track - (*it)->track);
        if(seek_time < min_seek){
            min_seek = seek_time;
            min_seek_idx = it;
        }
    }
    
    req = *min_seek_idx;
    pending_ioreq.erase(min_seek_idx);
    return req;
}
bool SSTF::is_request_pending(){
    return !pending_ioreq.empty();
}



// like SCAN but doesn't go all the way to the end
class LOOK : public IOScheduler
{
    private:
        bool dir;
        std::deque<ioreq_t*> pending_ioreq;
    public:
        LOOK();
        ~LOOK();
        void add_request(ioreq_t*);
        ioreq_t* get_next_req();
        bool is_request_pending();
};
LOOK::LOOK(){
    dir = 1;    // 1 up, 0 down
}
void LOOK::add_request(ioreq_t* r){
    pending_ioreq.push_back(r);
}
ioreq_t* LOOK::get_next_req(){
    ioreq_t* req = nullptr;
    int closest_seek = 10000;
    std::deque<ioreq_t*>::iterator closest_seek_idx;

    if(opt_q) printf("\tGet: (");
    for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
        int seek_time = abs(track - (*it)->track);
        if(dir){    // up
            if(track <= (*it)->track){
                if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
                if(seek_time < closest_seek){
                    closest_seek = seek_time;
                    closest_seek_idx = it;
                    req = *it;
                }
            }
        } else {    // down
            if(track >= (*it)->track){
                if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
                if(seek_time < closest_seek){
                    closest_seek = seek_time;
                    closest_seek_idx = it;
                    req = *it;
                }
            }
        }
    }
    if(req == nullptr){
        if(opt_q) printf(") --> change direction to %d\n", dir?-1:1);
        if(dir) dir = 0;
        else dir = 1;

        closest_seek = 10000;

        if(opt_q) printf("\tGet: (");
        for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
            int seek_time = abs(track - (*it)->track);
            if(dir){    // up
                if(track <= (*it)->track){
                    if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
                    if(seek_time < closest_seek){
                        closest_seek = seek_time;
                        closest_seek_idx = it;
                        req = *it;
                    }
                }
            } else {    // down
                if(track >= (*it)->track){
                    if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
                    if(seek_time < closest_seek){
                        closest_seek = seek_time;
                        closest_seek_idx = it;
                        req = *it;
                    }
                }
            }
        }
    }
    if(opt_q) printf(") --> %d dir=%d\n", req->req_num, dir?1:-1);

    pending_ioreq.erase(closest_seek_idx);
    return req;
}
bool LOOK::is_request_pending(){
    return !pending_ioreq.empty();
}



class CLOOK : public IOScheduler
{
    private:
        std::deque<ioreq_t*> pending_ioreq;
    public:
        CLOOK();
        ~CLOOK();
        void add_request(ioreq_t*);
        ioreq_t* get_next_req();
        bool is_request_pending();
};
CLOOK::CLOOK(){}
void CLOOK::add_request(ioreq_t* r){
    pending_ioreq.push_back(r);
}
ioreq_t* CLOOK::get_next_req(){
    ioreq_t* req = nullptr;
    int closest_seek = 10000;
    int trk = track;
    std::deque<ioreq_t*>::iterator closest_seek_idx;

    if(opt_q) printf("\tGet: (");
    for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
        int seek_time = abs(trk - (*it)->track);
        if(trk <= (*it)->track){  // up direction only
            if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
            if(seek_time < closest_seek){
                closest_seek = seek_time;
                closest_seek_idx = it;
                req = *it;
            }
        }
    }
    if(req != nullptr){
        if(opt_q) printf(") --> %d\n", req->req_num);
    }
    if(req == nullptr){
        trk = 0;
        closest_seek = 10000;

        for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
            int seek_time = abs(trk - (*it)->track);
            if(trk <= (*it)->track){
                if(seek_time < closest_seek){
                    closest_seek = seek_time;
                    closest_seek_idx = it;
                    req = *it;
                }
            }
        }
        if(opt_q) printf(") --> go to bottom and pick %d\n", req->req_num);
    }

    pending_ioreq.erase(closest_seek_idx);
    return req;
}
bool CLOOK::is_request_pending(){
    return !pending_ioreq.empty();
}



class FLOOK : public IOScheduler
{
    private:
        bool dir;
        int Q;
        int AQ;
        std::deque<ioreq_t*> ADDQ;
        std::deque<ioreq_t*> ACTVQ;
    public:
        FLOOK();
        ~FLOOK();
        void add_request(ioreq_t*);
        ioreq_t* get_next_req();
        bool is_request_pending();
};
FLOOK::FLOOK(){
    dir = 1;    // 1 up, 0 down
    Q = 0;
    AQ = 1;
}
void FLOOK::add_request(ioreq_t* r){
    ADDQ.push_back(r);
    if(opt_q){
        printf("   Q=%d ( ", Q);
        for (std::deque<ioreq_t*>::iterator it = ADDQ.begin(); it!=ADDQ.end(); ++it){
            printf("%d:%d ", (*it)->req_num, (*it)->track);
        }
        printf(")\n");
    }
}
ioreq_t* FLOOK::get_next_req(){
    
    ioreq_t* req = nullptr;
    int closest_seek = 10000;
    std::deque<ioreq_t*>::iterator closest_seek_idx;

    if(ACTVQ.empty()){  // active queue is empty, exchange
        if(Q == 0) Q = 1;
        else Q = 0;

        if(AQ == 0) AQ = 1;
        else AQ = 0;

        std::deque<ioreq_t*> temp;
        temp = ACTVQ;
        ACTVQ = ADDQ;
        ADDQ = temp;
    }

    if(opt_q){
        if(Q == 0){
            printf("AQ=%d dir=%d curtrack=%d:  Q[0] = ( ", AQ, dir?1:-1, track);
            for (std::deque<ioreq_t*>::iterator it = ADDQ.begin(); it!=ADDQ.end(); ++it){
                printf("%d:%d:%d ", (*it)->req_num, (*it)->track, ((*it)->track - track));
            }
            printf(")  Q[1] = ( ");
            for (std::deque<ioreq_t*>::iterator it = ACTVQ.begin(); it!=ACTVQ.end(); ++it){
                printf("%d:%d:%d ", (*it)->req_num, (*it)->track, ((*it)->track - track));
            }
            printf(") \n");
        } else {
            printf("AQ=%d dir=%d curtrack=%d:  Q[0] = ( ", AQ, dir?1:-1, track);
            for (std::deque<ioreq_t*>::iterator it = ACTVQ.begin(); it!=ACTVQ.end(); ++it){
                printf("%d:%d:%d ", (*it)->req_num, (*it)->track, ((*it)->track - track));
            }
            printf(")  Q[1] = ( ");
            for (std::deque<ioreq_t*>::iterator it = ADDQ.begin(); it!=ADDQ.end(); ++it){
                printf("%d:%d:%d ", (*it)->req_num, (*it)->track, ((*it)->track - track));
            }
            printf(") \n");
        }
        
    }

    if(opt_q) printf("\tGet: (");
    for (std::deque<ioreq_t*>::iterator it = ACTVQ.begin(); it!=ACTVQ.end(); ++it){
        int seek_time = abs(track - (*it)->track);
        if(dir){    // up
            if(track <= (*it)->track){
                if(opt_q) printf("%d:%d:%d ", (*it)->req_num, (*it)->track, seek_time);
                if(seek_time < closest_seek){
                    closest_seek = seek_time;
                    closest_seek_idx = it;
                    req = *it;
                }
            }
        } else {    // down
            if(track >= (*it)->track){
                if(opt_q) printf("%d:%d:%d ", (*it)->req_num, (*it)->track, seek_time);
                if(seek_time < closest_seek){
                    closest_seek = seek_time;
                    closest_seek_idx = it;
                    req = *it;
                }
            }
        }
    }
    if(req == nullptr){
        if(opt_q) printf(") --> change direction to %d\n", dir?-1:1);
        if(dir) dir = 0;
        else dir = 1;

        closest_seek = 10000;

        if(opt_q) printf("\tGet: (");
        for (std::deque<ioreq_t*>::iterator it = ACTVQ.begin(); it!=ACTVQ.end(); ++it){
            int seek_time = abs(track - (*it)->track);
            if(dir){    // up
                if(track <= (*it)->track){
                    if(opt_q) printf("%d:%d:%d ", (*it)->req_num, (*it)->track, seek_time);
                    if(seek_time < closest_seek){
                        closest_seek = seek_time;
                        closest_seek_idx = it;
                        req = *it;
                    }
                }
            } else {    // down
                if(track >= (*it)->track){
                    if(opt_q) printf("%d:%d:%d ", (*it)->req_num, (*it)->track, seek_time);
                    if(seek_time < closest_seek){
                        closest_seek = seek_time;
                        closest_seek_idx = it;
                        req = *it;
                    }
                }
            }
        }
    }
    if(opt_q) printf(") --> %d dir=%d\n", req->req_num, dir?1:-1);
    if(opt_f) printf("%d:       %d get Q=%d\n", sim_time, req->req_num, AQ);

    ACTVQ.erase(closest_seek_idx);
    return req;
}
bool FLOOK::is_request_pending(){
    return !ACTVQ.empty() || !ADDQ.empty();
}