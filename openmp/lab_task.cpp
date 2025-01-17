#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>
#include <chrono> 
#include <iomanip>

#ifdef OMP
#include <omp.h>
#endif


double lower_bound = -1000.0;
double upper_bound = 1000.0;
std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
std::default_random_engine re;


double random_e()
{
   return unif(re);
}


void generate(std::vector<double> * v,  double (*__gen)())
{
    for(int i =0; i < v->size(); i++){
        (*v)[i] = __gen();
    }
}



int main (int argc, char *argv[]) {

    re.seed(std::chrono::system_clock::now().time_since_epoch().count());

    int n;

    std::cin >> n;
    std::vector<double> B(n*n);

    // {
    //     double current;
    //     while(std::cin >> current) {
    //         B.push_back(current);
    //     }
    // }

    std::vector<double> C(n * n);
    std::vector<double> A(n * n);
    std::vector<double> x(n);
    std::vector<double> y(n);


    generate(&B, random_e);
    generate(&C, random_e);
    generate(&x, random_e);
    generate(&y, random_e);
    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();


    std::vector<double> CE(n); // single column of C*E
#pragma omp parallel
#pragma omp single
{

    for(int i = 0; i < n; i++){
        #pragma omp task firstprivate(i)
        {
            double tmp = 0.0;
            for(int k = 0; k < n; k++){
                tmp += C[i*n + k];
            }
            CE[i] = tmp;
        }
    }
}

    double trace = 0.0;
    double z1 = 0.0;
    double z2 = 0.0;
    double Ex = std::accumulate(x.begin(), x.end(), 0.0);
#pragma omp parallel
#pragma omp single
{

    for(int i = 0; i < n; i++){
        #pragma omp task shared(trace) firstprivate(i)
        {
            double trace_chunk = 0.0;
            for(int k = 0; k < n; k++){
                trace_chunk += B[i*n + k] * CE[k];
            }
            #pragma omp critical
            trace += trace_chunk;
        }
    }




    for(int i = 0; i < n; i++){ 
        #pragma omp task shared(z1) firstprivate(i)
        {
            double z1_chunk = 0.0;
            for(int j = 0; j < n; j++){
                z1_chunk += B[i*n + j] * Ex * y[i]; 
            }
            #pragma omp critical
            z1 += z1_chunk;
        // z1 += BEx_i * Ex * y[i];
        }
    }

    #pragma omp task shared(z2)
    {
        for(int i = 0; i < n; i++) {

            z2 += x[i] * y[i];
        }
    }

#pragma omp taskwait
}

    double z = z1 / z2;

#pragma omp parallel
#pragma omp single
{

    for(int i = 0; i < n; i++) {
        #pragma omp task firstprivate(i)
        {
            for(int j = 0; j < n; j++) {
   
                A[i*n + j] = trace * C[i*n +j] + (i == j ? 1 : 0) + z1 / z2;
            }
        }
    }
}
    std::chrono::milliseconds elapsed_milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
                                                                  start_time);

    std::cerr << "elapsed " << elapsed_milliseconds.count() << "ms" << std::endl ;

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            std::cout << std::setprecision(10) << A[i*n + j] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
