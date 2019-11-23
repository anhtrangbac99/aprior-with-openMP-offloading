#include "retrieve_data.h"
#include <vector>
#include <algorithm>

/*
* USAGE: ./main <min_support> <min_confidence> <csv_file> <country(ies)>
*
*
*
*

*/

#define METADATA std::map<std::string, std::map<std::string, int>>
#define DATA_L std::map<std::string, int>
#define RULES std::map<std::vector<std::string>,std::vector<std::string>>
enum args 
{
    EXEC_NAME,
    MIN_SUP,
    MIN_CONF,
    READ_FILE,
    FCOUNTRY
};

/* * * * * * * * * * /
* PROTOTYPE SECTION *
/ * * * * * * * * * */

void count_support(int min_sup, METADATA metadata, DATA_L* dataset, int i = 1);
void print_data(const DATA_L dataset);
void generate_permutations(const DATA_L base_data, DATA_L* dataset,METADATA metadata,int min_sup, int i = 1);
void str_split(std::string* spl_str, std::string* data_str, char separator, int i);
void generate_rule(DATA_L L,std::vector<DATA_L> list_L,int i,int min_conf,RULES& rules);
void subsetsUtil(std::vector<std::string>& A, std::vector<std::vector<std::string> >& res, std::vector<std::string>& subset, int index);
std::vector<std::vector<std::string>> subsets(std::vector<std::string>& A); 
std::vector<std::string> minus_subsets(std::vector<std::string> set,std::vector<std::string> subset);

/******************************************/
int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        std::cout << "USAGE: ./main <min_support> <min_confidence> <csv_file> <country(ies)>" << std::endl;
        return 1;
    }
    
    // Parse data from console arguments
    int min_support = std::stoi(argv[MIN_SUP]);
    float min_confidence = std::stof(argv[MIN_CONF]);
    std::string filename(argv[READ_FILE]);
    std::string country(argv[FCOUNTRY]);
    std::replace(country.begin(), country.end(), '_', ' ');
    
    METADATA metadata;

    // Retrieving data
    std::cout << "Reading data from file: " << 
                filename << 
                ", country: " << 
                country << std::endl;

    retrieve_data(filename, country, &metadata);
    std::cout << "Start counting from L1...." << std::endl;
    // Count L1
    DATA_L L1;

    count_support(min_support, metadata, &L1);
    std::cout << L1.size() << std::endl;
    DATA_L L2, L3;

    generate_permutations(L1, &L2,metadata,min_support);
    count_support(min_support,metadata,&L2,2);
    std::cout << L2.size() << std::endl;

    std::vector<DATA_L> list_L;
    list_L.push_back(L1);
    list_L.push_back(L2);

    RULES rules;
    generate_rule(L2,list_L,2,min_confidence,rules);
    std::cout << rules.size() << std::endl;
/*  
    count_support(min_support, metadata, &L2, 2);
    std::cout << L2.size() << std::endl;

    generate_permutations(L2, &L3, 2);
    count_support(min_support, metadata, &L3, 3);    
    
    std::cout << L3.size() << std::endl;*/
    //print_data(L2);
    return 0;
}

/*
* COUNT_SUPPORT
* COUNT THE SUPPORT OF ANY TUPLES SIZE
* FROM METADATA
*
*
*
*/
void count_support(int min_sup, METADATA metadata, DATA_L* dataset, int i)
{
    /* 
    * METADATA: std::map<std::string, std::map<std::string, int>>
    */
    
    if (i == 1){
    // Getting count from all transactions
        for (auto e = metadata.begin(); e != metadata.end(); e++)
        {
            // e->second = The inner map
            // _e->first = e->second->first = The string of the inner map
            // _e->second = e->second->second = The number of the inner map
            for (auto _e = e->second.begin(); _e != e->second.end(); _e++)
            {
                if (dataset->find(_e->first) != dataset->end()) // If this product is in our dataset
                {
                    // Add one count to it!
                    (*dataset)[_e->first]++;
                }
                else // If there is no record of this product
                {
                    // Make a new one
                    dataset->insert({_e->first, 1});
                }
            }
        }    }
    else
    {
        // Start counting every tuple from the metadata
        for (auto e = dataset->begin(); e != dataset->end(); e++)
        {
            // Split the stock codes first
            std::string l_str = e->first;
            std::string* str = new std::string[i];
            
            str_split(&l_str, str, ';', i);

            // Then find those tuples in the metadata
            for (auto _e = metadata.begin(); _e != metadata.end(); _e++)
            {
                bool fail = false;
                for (int j = 0; j < i; j++)
                {
                    // If we cannot find the current stock code in this transaction, loop through another one
                    if (_e->second.find(str[j]) == _e->second.end())
                    {
                        fail = true;
                        break;
                    }                    
                }
                
                // If we can find the tuple
                if (!fail)
                {
                    // And add one to the tuple
                    e->second++;
                }
            }
            delete[] str;
        }

    }
    


    std::vector<std::string> vec;
    // Start removing transactions have count < min_support
    for (auto e = dataset->begin(); e != dataset->end(); e++)
    {
        if (e->second < min_sup)
        {
           vec.push_back(e->first);
        }
    }
    
    for(auto e = vec.begin(); e != vec.end(); e++)
        dataset->erase(*e);
}

/*
* PRINT DATA
* OUT FROM A DATASET.
* FORMAT: <STOCK CODES> : <QUANTITY>
*/
void print_data(const DATA_L dataset)
{
    for (auto e = dataset.begin(); e != dataset.end(); e++)
    {
        std::cout << e->first << " : " << e->second << std::endl;
    }
}


/*
*
* GENERATE PERMUTATIONS
* DESCRIPTION: GENERATE THE PERMUTATIONS OF TWO TUPLES IF THEY
* HAVE EXACTLY i - 1 MATCHES
* FROM BASE_DATA, STORED AT L_NEXT
* WITH i IS CURRENT SIZE OF A TUPLE
*/
void generate_permutations(const DATA_L base_data, DATA_L* L_next,METADATA metadata,int min_sup, int i)
{
    int point = 1;
    // Generate from the L(i - 1)
    for (auto e = base_data.begin(); e != base_data.end(); e++)
    {
        for (auto _e = base_data.begin(); _e != base_data.end(); _e++)
        {
            if (point > 0)
            {
                point--;
                continue;
            }

            if (i > 1)
            {
                // Getting temporary strings for comparison reason
                std::string l_str1 = e->first, l_str2 = _e->first;
                std::string* str1 = new std::string[i];
                std::string* str2 = new std::string[i];
                
                // Split strings by ';'
                str_split(&l_str1, str1, ';', i);
                str_split(&l_str2, str2, ';', i);
                
                // Compare those substrings, if they have exactly i - 1 matches, pair them
                int n = 0;
                std::string res = "";
                for (int j = 0; j < i; j++)
                {
                    res = res + str1[j] + ";";
                    for (int k = 0; k < i; k++)
                    {
                        if (str1[j].compare(str2[k]) == 0)
                        {
                            n++;
                            break;
                        }
                    }
                }
        
                for (int j = 0; j < i; j++)
                    if (res.find(str2[j]) == std::string::npos)
                    {
                        res += str2[j];
                    } 

                // After iteration, res will be StockCode1;StockCode2;...       

                if (n == i - 1)
                {
                    
                    L_next->insert({res, 0});
                }
                

                delete[] str1;
                delete[] str2;
            }
            else
            {
                if (e->first == _e->first) continue;
        
                std::string pair = e->first + ";" + _e->first;
                L_next->insert({pair, 0});
            }
        }

        point++;
    }

}

/*
* SPLIT STRING 
* BY DESIRED SEPARATOR
* SPL_STR: STRING NEEDS TO BE SPLITTED
* DATA_STR: ARRAY OF STRING TO STORE THE SPLITTED STRINGS
*/
void str_split(std::string* spl_str, std::string* data_str, char separator, int i)
{
    int idx = 0;
    while (idx < i)
    {
        data_str[idx++] = (idx == i - 1) ? *spl_str : spl_str->substr(0, spl_str->find(separator));
        *spl_str = spl_str->substr(spl_str->find(separator) + 1);
    }
}

/*
*GENERATE RULE
*DESCRIPTION: GENERATE ALL ASSOCIATION RULES : K => (Li - K) 
*WITH K IS A SUBSET OF ONE ELEMENT OF Li
*List_l: A VECTOR OF lj (j<i) 
*/
void generate_rule(DATA_L L,std::vector<DATA_L> list_L,int i,int min_conf,RULES& rules)
{
    // Traverse all element of Li
    for (auto e = L.begin();e != L.end();e++)
    {   
        
        std::string * itemset = new std::string[i];
        std::vector<std::string> list_itemset;
        std::vector<std::vector<std::string>> list_sub_itemset;
        std::string str = e->first;

        str_split(&str,itemset,';',i);

        for (int j =0;j<i;j++){
            list_itemset.push_back(itemset[j]);
        }

        // Generate all subsets of the element
        list_sub_itemset = subsets(list_itemset);

        // Check the confident of each subsets
        for (int j=0;j<list_sub_itemset.size();j++)
        {
            if(list_sub_itemset[j].size() == 0) continue;
            if (list_sub_itemset[j].size() == list_itemset.size()) continue;
            int count_subsets;
            int count_set = e->second;

            for (auto e1 = list_L[list_sub_itemset[j].size()-1].begin();e1!=list_L[list_sub_itemset[j].size()-1].end();e1++)
            {
                if (e1->first == list_sub_itemset[j][0])
                {
                    count_subsets = e1->second;
                    break;
                } 

            }
            if (count_set/count_subsets < min_conf) continue;
            rules.insert({list_sub_itemset[j],minus_subsets(list_itemset,list_sub_itemset[j])});
        }
    }
}

/*
*SUBSETS AND SUBSESTUNTIL
*GENERATE ALL SUBSETS OF A VECTOR<STRING>
*A: VECTOR OF STRING NEEDS TO BE GENERATED.
*res: RESULT OF THE FUNCTION - A VECTOR OF SUBSETS
*/
void subsetsUtil(std::vector<std::string>& A, std::vector<std::vector<std::string> >& res, std::vector<std::string>& subset, int index) 
{ 
    res.push_back(subset); 
    for (int i = index; i < A.size(); i++) { 
  
        // Include the A[i] in subset
        subset.push_back(A[i]); 
  
        // Move onto the next element
        subsetsUtil(A, res, subset, i + 1); 
  
        // Exclude the A[i] from subset and triggers 
        // Backtracking
        subset.pop_back(); 
    } 
  
    return; 
} 

std::vector<std::vector<std::string>> subsets(std::vector<std::string>& A) 
{ 
    std::vector<std::string> subset; 
    std::vector<std::vector<std::string> > res; 
  
    // Keep track of current element in vector A 
    int index = 0; 
    subsetsUtil(A, res, subset, index); 
  
    return res; 
}

/*
*MINUS SUBSETS
*GENERATE Li - K
*set: li.
*subset: K*/
std::vector<std::string> minus_subsets(std::vector<std::string> set,std::vector<std::string> subset)
{
    std::vector<std::string> result;
    for (int i =0;i<set.size();i++)
    {
        int count = 0;
        for (int j =0;j<subset.size();j++){
            if(set[i]==subset[j]){
                count ++;
                break;
            }
        }

        if (count==0){
            result.push_back(set[i]);
        }
    }
    return result;
}