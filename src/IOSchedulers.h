
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
    if(pending_ioreq.empty()) return nullptr;
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
    if(pending_ioreq.empty()) return nullptr;
    
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


class LOOK : public IOScheduler
{
    public:
        LOOK();
        ~LOOK();
        ioreq_t* get_next_req();
};
LOOK::LOOK(){}
ioreq_t* LOOK::get_next_req(){
    ioreq_t* req;
    if(pending_ioreq.empty()) return nullptr;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
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
    ioreq_t* req;
    if(pending_ioreq.empty()) return nullptr;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
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