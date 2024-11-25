#include <vector>
#include <iostream>
#include <iomanip>


void print_vec(std::vector<double> & m, int n){
    std::cout << "{";
    for(int j = 0; j < n; j++){
        std::cout << std::setprecision (15) << m[j];
        if(j != n-1) std::cout << ",";
    }    
    std::cout << "}";
}

void print_matrix(std::vector<double> & m, int n){
    std::cout << "{";
    for(int i = 0; i < n; i++){
        std::cout << "{";
        for(int j = 0; j < n; j++){
            std::cout << std::setprecision (15) << m[i*n + j];
            if(j != n-1) std::cout << ",";
        }    
        std::cout << "}";
        if(i != n-1) std::cout << ",";
    }
    std::cout << "}" << std::endl;
}