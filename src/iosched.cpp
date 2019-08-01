#include <string>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

bool testing = false;
bool opt_v = false, opt_q = false, opt_f = false;

int total_time = 0, tot_movement = 0, max_waittime = 0;
double avg_turnaround = 0, avg_waittime = 0;

#define stime_t int
typedef struct ioreq_t {
    int req_num;
    int track;
    stime_t arrival_time;
    stime_t start_time;
    stime_t end_time;
    ioreq_t(): req_num(0),arrival_time(0), track(0), start_time(0), end_time(0){}
} ioreq_t;
vector<ioreq_t*> io_requests;
deque<ioreq_t*> pending_ioreq;

// Pager *pager = = (Pager *) new FIFO();

void parse_args(int argc, char *argv[], string &input_file){
    int c;
    if(testing) printf("----------args:---------\n");
    while ((c = getopt (argc, argv, "s:vqf")) != -1){
        switch(c){
            case 's':
                char algo;
                algo = optarg[0];
                if(testing) printf("algo %c\n", algo);
                switch(algo){
                    case 'i':   // FIFO

                        break;
                    case 'j':   // SSTF

                        break;
                    case 's':   // LOOK

                        break;
                    case 'c':   // CLOOK

                        break;
                    case 'f':   // FLOOK

                        break;
                    default:
                        break;
                }
                break;
            case 'v':
                opt_v = true;
                if(testing) printf("opt_v: %d\n", opt_v);
                break;
            case 'q':
                opt_q = true;
                if(testing) printf("opt_q: %d\n", opt_q);
                break;
            case 'f':
                opt_f = true;
                if(testing) printf("opt_f: %d\n", opt_f);
                break;
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                abort();
        }
    }
    int idx = optind;
    if(idx < argc) input_file = argv[idx];
    if(testing){
        printf("input file: %s\n", input_file.c_str());
        printf("------------------------\n");
    }
}

void parse_input(string input_file){
    
    // read and parse input file
    ifstream in;
    in.open(input_file.c_str());
    if(!in){
        printf("Error: Can't open the file named %s\n", input_file.c_str());
        exit(1);
    } 

    // parse instructions
    int ts = 0, trk = 0, req_num = 0;
    ioreq_t* req;
    string line = "";
    while(in){
        getline(in, line);
        if(line[0] == '#') continue;
        if(line.length() <= 2) break;
        // printf("%s\n", line.c_str());
        ts = stoi(line.substr(0, line.find_first_of(" ")));
        trk = stoi(line.substr(line.find_first_of(" ") + 1));
        req = new ioreq_t();
        req->arrival_time = ts;
        req->track = trk;
        req->req_num = req_num;
        req_num++;
        io_requests.push_back(req);
        if(testing) printf("%d %d\n", ts, trk);
    }
    req = nullptr;

    if(testing) printf("------------------------\n");

}

ioreq_t* get_next_req(){
    ioreq_t* req;
    if(pending_ioreq.empty()) return nullptr;
    req = pending_ioreq.front();
    pending_ioreq.pop_front();
    return req;
}

void simulation(){
    stime_t sim_time = 0;
    int track = 0;
    int next_ioreq_num = 0;
    int num_processed_req = 0;
    ioreq_t* cur_ioreq = nullptr;
    
    while(num_processed_req <= io_requests.size()){
        if(next_ioreq_num < io_requests.size() && io_requests[next_ioreq_num]->arrival_time == sim_time){
            pending_ioreq.push_back(io_requests[next_ioreq_num]);
            if(opt_v) printf("%d: %d add %d\n", sim_time, io_requests[next_ioreq_num]->req_num, io_requests[next_ioreq_num]->track);
            next_ioreq_num++;
        }

        if(cur_ioreq != nullptr){  // io is active
            if(track == cur_ioreq->track){  // completed at this time
                cur_ioreq->end_time = sim_time;
                if(opt_v) printf("%d: %d finish %d\n", sim_time, cur_ioreq->req_num, (cur_ioreq->end_time - cur_ioreq->arrival_time));
                cur_ioreq = nullptr;
                num_processed_req++;
            } else {   // not yet complete 
                if(track < cur_ioreq->track) track++;
                else track--;
            }
        } else {    // no IO req active
            if(!pending_ioreq.empty()){   // req pending, fetch next req & start io
                cur_ioreq = get_next_req();
                cur_ioreq->start_time = sim_time;
                if(opt_v) printf("%d: %d issue %d %d\n", sim_time, cur_ioreq->req_num, cur_ioreq->track, track);
            }
        }
        sim_time++;
    }
}

void print_result(){
    for(int i = 0; i < io_requests.size(); i++){
        printf("%5d: %5d %5d %5d\n",i, io_requests[i]->arrival_time, io_requests[i]->start_time, io_requests[i]->end_time);
    }
    printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);
}

int main(int argc, char *argv[]){

    // parse the args & input
    string input_file, rand_file;
    parse_args(argc, argv, input_file);
    parse_input(input_file);

    simulation();

    print_result();

    return 0;
}