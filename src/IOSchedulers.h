
class IOScheduler
{
    private:
        
    public:
        IOScheduler();
        ~IOScheduler();
        virtual ioreq_t* get_next_req() = 0;
};
IOScheduler::IOScheduler(){}


class FIFO : public IOScheduler
{
    public:
        FIFO();
        ~FIFO();
        ioreq_t* get_next_req();
};
FIFO::FIFO(){}
ioreq_t* FIFO::get_next_req(){
    ioreq_t* req;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
    return req;
}


class SSTF : public IOScheduler
{
    public:
        SSTF();
        ~SSTF();
        ioreq_t* get_next_req();
};
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


// like SCAN but doesn't go all the way to the end
class LOOK : public IOScheduler
{
    private:
        bool dir;
    public:
        LOOK();
        ~LOOK();
        ioreq_t* get_next_req();
};
LOOK::LOOK(){
    dir = 1;    // 1 up, 0 down
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


class CLOOK : public IOScheduler
{
    public:
        CLOOK();
        ~CLOOK();
        ioreq_t* get_next_req();
};
CLOOK::CLOOK(){}
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

        // if(opt_q) printf("\tGet: (");
        for (std::deque<ioreq_t*>::iterator it = pending_ioreq.begin(); it!=pending_ioreq.end(); ++it){
            int seek_time = abs(trk - (*it)->track);
            if(trk <= (*it)->track){
                // if(opt_q) printf("%d:%d ", (*it)->req_num, seek_time);
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


class FLOOK : public IOScheduler
{
    public:
        FLOOK();
        ~FLOOK();
        ioreq_t* get_next_req();
};
FLOOK::FLOOK(){}
ioreq_t* FLOOK::get_next_req(){
    ioreq_t* req;
    if(pending_ioreq.empty()) return nullptr;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
    return req;
}