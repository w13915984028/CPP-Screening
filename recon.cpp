

#include <iostream>
#include <string>
#include <vector>

#include <array>

using namespace std;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    for (auto& el : vec)
    {
        os << el << ' ';
    }
    return os;
}

// enum for TRANS STATE
enum class ETRANS_STATE { 
    SUCCESS,
    FAILED,
    PENDING
};

// enum for DATA PROCESSING, it indicates the state of the data
enum class EPROC_STATE { 
    INVALID,  // invalid data from the input file, output as: unknown
    INITIAL,  // normal data, waiting for match
    MATCHED   // the matched data
};

// struct/class for transaction
// use struct for less get/set function, it makes fewer sense in a test programming
struct SATransaction
{
    // normal construction
    SATransaction(int tid=0, int mid=0, enum ETRANS_STATE mst=ETRANS_STATE::PENDING, double amt=0):
        m_transId(tid), m_merTransId(mid),m_state(mst),m_amount(amt)
    {
        m_proc = EPROC_STATE::INITIAL;
    }

    // construction of invalid/unknown data
    SATransaction(int tid, int mid, enum ETRANS_STATE mst, double amt, enum EPROC_STATE proc):
        m_transId(tid), m_merTransId(mid),m_state(mst),m_amount(amt), m_proc(proc)
    {
    }    
    
    
    //data corresponding to the transaction
    int m_transId =0 ;
    int m_merTransId =0 ;
    enum ETRANS_STATE m_state = ETRANS_STATE::PENDING;
    double m_amount = 0;
    enum EPROC_STATE m_proc = EPROC_STATE::INVALID;  // for internal process usage, if matched, mark it
};

// struct for storing the processing result
struct SReconRes
{
    int m_unknown = 0;
    double m_total = 0;
    double m_matched = 0;
    double m_missing = 0 ;
};

// compare transactions-harder with transactions-ppro
inline bool compare_harder(SATransaction& harder,   SATransaction& ppro)
{
    // skip invalid item
    if ((harder.m_proc == EPROC_STATE::INVALID) || (ppro.m_proc == EPROC_STATE::INVALID))
        return false;
    
    // all match, true
    if ((harder.m_state == ppro.m_state) && 
    (harder.m_amount == ppro.m_amount) &&
    (harder.m_merTransId == ppro.m_merTransId) &&
    (harder.m_transId == ppro.m_transId))
    {
        return true;
    }
    
    return false;
}

// compare transactions-simpler with transactions-ppro
inline bool compare_simpler(SATransaction& simpler,   SATransaction& ppro)
{
    //skip invalid item
    if ((simpler.m_proc == EPROC_STATE::INVALID) || (ppro.m_proc == EPROC_STATE::INVALID))
        return false;
    
    // all match, true
    if ((simpler.m_state == ppro.m_state) && 
    (simpler.m_amount == ppro.m_amount) &&
    (simpler.m_merTransId == ppro.m_merTransId))
    {
        return true;
    }
    
    return false;
}


// compare transactions-simple with transactions-ppro
inline bool compare_simple(SATransaction& simple,   SATransaction& ppro)
{
    //skip invalid item
    if ((simple.m_proc == EPROC_STATE::INVALID) || (ppro.m_proc == EPROC_STATE::INVALID))
        return false;
    
    // all match, true
    if ((simple.m_state == ppro.m_state) && 
    (simple.m_amount == ppro.m_amount) &&
    (simple.m_transId == ppro.m_transId))
    {
        return true;
    }
    
    return false;
}

void print_res(const char* filename, SReconRes& res)
{
    cout<<filename<<endl;
    if (res.m_unknown > 0)
        cout<<"The file has "<<res.m_unknown<<" uknown transactions."<<endl;
    
    cout<<"Total volumn:"<<res.m_total<<endl;
    cout<<"Matched volumn:"<<res.m_matched<<endl;
    cout<<"Missing volumn:"<<res.m_missing<<endl;
    cout<<endl;
}

void do_analysis()
{

    vector<SATransaction> vecPpro;
    vecPpro.push_back(SATransaction(101, 401, ETRANS_STATE::SUCCESS, 100));
    vecPpro.push_back(SATransaction(102, 0, ETRANS_STATE::SUCCESS, 3.99));    
    vecPpro.push_back(SATransaction(901, 901, ETRANS_STATE::SUCCESS, 100, EPROC_STATE::INVALID ));
    vecPpro.push_back(SATransaction(107, 501, ETRANS_STATE::SUCCESS, 300));    
    SReconRes pproRes;
    
    vector<SATransaction> vecSimple;
    
    SReconRes simpleRes;
    
    vector<SATransaction> vecSimpler;
    vecSimpler.push_back(SATransaction(0, 401,  ETRANS_STATE::SUCCESS, 100));      
    vecSimpler.push_back(SATransaction(0, 402,  ETRANS_STATE::FAILED, 88.44));   
    vecSimpler.push_back(SATransaction(0, 403,  ETRANS_STATE::PENDING, 333.22));   
    vecSimpler.push_back(SATransaction(0, 404,  ETRANS_STATE::FAILED, 77.12));     
    SReconRes simplerRes;
    
    vector<SATransaction> vecHarder;    
    vecHarder.push_back(SATransaction(107, 501, ETRANS_STATE::SUCCESS, 300));   
    vecHarder.push_back(SATransaction(-1, 502, ETRANS_STATE::SUCCESS, 200));      
    SReconRes harderRes;
    
    
    // process harder  
    //cout<<"process harder"<<endl;
    for (auto& x : vecHarder)
    {
        for (auto& y : vecPpro)
            if (compare_harder(x, y))
            {
                //cout<<"matched one item"<<endl;
                x.m_proc = EPROC_STATE::MATCHED;
                y.m_proc = EPROC_STATE::MATCHED;
            }
    }


    // process simpler   
    //cout<<"process simpler"<<endl;
    for (auto& x : vecSimpler)
    {
        for (auto& y : vecPpro)
            if (compare_simpler(x, y))
            {
                //cout<<"matched one item"<<endl;
                x.m_proc = EPROC_STATE::MATCHED;
                y.m_proc = EPROC_STATE::MATCHED;
            }
    }    
    
    
    // process simple 
    //cout<<"process simple"<<endl;
    for (auto& x : vecSimple)
    {
        for (auto& y : vecPpro)
            if (compare_simple(x, y))
            {
                x.m_proc = EPROC_STATE::MATCHED;
                y.m_proc = EPROC_STATE::MATCHED;
            }
    }    
    
    
    
    
    // calculate and print
    for (auto& x: vecPpro)
    {
        if (x.m_proc == EPROC_STATE::MATCHED)
        {
            pproRes.m_matched += x.m_amount;
            pproRes.m_total += x.m_amount;
        } else if (x.m_proc == EPROC_STATE::INITIAL)
        {
            pproRes.m_missing += x.m_amount;
            pproRes.m_total += x.m_amount;            
        } else if (x.m_proc == EPROC_STATE::INVALID)
        {
            pproRes.m_unknown ++;
        }
    }
    
    print_res("ppro-transactions.csv", pproRes);
    
    
    // simple, calculate and output
    for (auto& x: vecSimple)
    {
        if (x.m_proc == EPROC_STATE::MATCHED)
        {
            simpleRes.m_matched += x.m_amount;
            simpleRes.m_total += x.m_amount;
        } else if (x.m_proc == EPROC_STATE::INITIAL)
        {
            simpleRes.m_missing += x.m_amount;
            simpleRes.m_total += x.m_amount;            
        } else if (x.m_proc == EPROC_STATE::INVALID)
        {
            simpleRes.m_unknown ++;
        }
    }
    
    print_res("simple-transactions.csv", simpleRes);

    
    
    // simpler, calculate and output
    for (auto& x: vecSimpler)
    {
        if (x.m_proc == EPROC_STATE::MATCHED)
        {
            simplerRes.m_matched += x.m_amount;
            simplerRes.m_total += x.m_amount;
        } else if (x.m_proc == EPROC_STATE::INITIAL)
        {
            simplerRes.m_missing += x.m_amount;
            simplerRes.m_total += x.m_amount;            
        } else if (x.m_proc == EPROC_STATE::INVALID)
        {
            simplerRes.m_unknown ++;
        }
    }
    
    print_res("simpler-transactions.csv", simplerRes);   



    // Harder, calculate and output
    for (auto& x: vecHarder)
    {
        if (x.m_proc == EPROC_STATE::MATCHED)
        {
            harderRes.m_matched += x.m_amount;
            harderRes.m_total += x.m_amount;
        } else if (x.m_proc == EPROC_STATE::INITIAL)
        {
            harderRes.m_missing += x.m_amount;
            harderRes.m_total += x.m_amount;            
        } else if (x.m_proc == EPROC_STATE::INVALID)
        {
            harderRes.m_unknown ++;
        }
    }
    
    print_res("harder-transactions.csv", harderRes);    
        
    
}



int main()
{
    //std::vector<std::string> vec = {
    //    "Hello", "from", "GCC", __VERSION__, "!" 
    //};
    //std::cout << vec << std::endl;
    
    //test1();
    //cout<<"start process"<<endl;
    do_analysis();
    
}



// compile and run
// g++ -std=c++17 -O2 -Wall -pedantic -pthread main.cpp && ./a.out


