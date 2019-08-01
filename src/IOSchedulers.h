
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

