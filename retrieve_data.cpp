#include "retrieve_data.h"
/*
*   DOCUMENTATION
*   
*   USAGE: ./read_csv <file_name>
*
*   map elements:
*       metadata: map: {InvoiceNo, map: {StockCode, Quantity}}
*
*
*/


void retrieve_data(std::string filename, std::string _country, std::map<std::string, std::map<std::string, int>>* metadata)
{   
    std::fstream raw;
    raw.open(filename);
    if (raw.is_open())
    {
        std::string line;
        std::getline(raw, line);
        while(!raw.eof())
        {   
            
            std::getline(raw, line);
            //std::cout << line << std::endl;
            
            //invoiceNo, stockCode, description, quantity, invoiceDate, unitPrice, customerID, country;
            std::string data[8]; // Each temp is relatively the field above

            int i = 0;
            if (line.size() > 0) // Handle the case where we headed to the last empty line
            {
                while (line.find(';') != std::string::npos)
                {
                    data[i++] = line.substr(0, line.find(';'));
                    line = line.substr(line.find(';') + 1);
                    if (i == 7)
                    {
                        data[i] = line.substr(0, line.size() - 1); // Remove the \0 character
                        i = 0;
                    }
                }
                if(data[country].compare(_country) < 0)
                { // Get data based on desired country only
                    if (data[invoiceNo].find('C') == std::string::npos && data[invoiceNo].find('A') == std::string::npos) // If this transaction is not cancelled and not "Adjust bad debt"
                    {
                        if (metadata->find(data[invoiceNo]) != metadata->end()) // Find the invoice in metadata
                        {
                            // Insert the tuple {STOCKCODE, QUANTITY} to the inner map
                            (*metadata)[data[invoiceNo]].insert({data[stockCode], std::stoi(data[quantity], nullptr, 10)});
                        }
                        else // If there is no record of this invoice
                        {
                            // Make a new one
                            std::map<std::string, int> product;
                            product.insert({data[stockCode], std::stoi(data[quantity], nullptr, 10)});
                            metadata->insert({data[invoiceNo], product});
                            
                        }
                    }
                }
            }
        }

        raw.close();  
    }
    
}
