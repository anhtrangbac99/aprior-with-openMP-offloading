#include<fstream>
#include<iostream>
#include<string>
#include<map>

enum Attributes 
{
    invoiceNo, 
    stockCode, 
    description, 
    quantity, 
    invoiceDate, 
    unitPrice, 
    customerID, 
    country
};

void retrieve_data(std::string filename, 
                std::string _country, 
                std::map<std::string, std::map<std::string, int>>* metadata);

