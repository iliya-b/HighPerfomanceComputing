#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <random>
#include <chrono> 
#include <iomanip>

#include <mpi.h>


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


void print_matrix(std::vector<double> M, int n)
{
    std::cout << "[";
    for(int i = 0; i < n; i++) {
        std::cout << "[";
        for(int j = 0; j < n; j++) {
            std::cout << std::setprecision(10) << M[i*n + j] << ", ";
        }
        std::cout << "],";
        std::cout << std::endl;
    }
    std::cout << "]";
}


void print_vector(std::vector<double> M, int n)
{
    std::cout << "[";

        for(int i = 0; i < n; i++) {
            std::cout << std::setprecision(10) << M[i] << ", ";
        }

    std::cout << "]";
}



int main(int argc, char** argv) {

  MPI_Init(&argc, &argv);

  int world_rank;
  int comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


    re.seed(std::chrono::system_clock::now().time_since_epoch().count());

    int n = atoi(argv[1]);

    // std::cin >> n;


    if (n%comm_size!=0) {
        if (world_rank==0) printf("N is not divisible by number of processors\n");
        MPI_Finalize();
        exit(-1);
    }
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

    

    if(world_rank == 0){
        generate(&B, random_e);
        generate(&C, random_e);
        generate(&x, random_e);
        generate(&y, random_e);

        // print_matrix(B, n);
        // std::cout << std::endl<< std::endl;
        // print_matrix(C, n);
        // std::cout << std::endl<< std::endl;
        // print_vector(x, n);
        // std::cout << std::endl<< std::endl;
        // print_vector(y, n);
        // std::cout << std::endl<< std::endl;

    }
    // MPI_Finalize();
    // exit(-1);

    MPI_Bcast(&B[0], B.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&C[0], C.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&x[0], x.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y[0], y.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);


    int chunk_size = n / comm_size;
    int offset = chunk_size * world_rank;
    int end = chunk_size * (world_rank + 1);

    MPI_Barrier(MPI_COMM_WORLD);


    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();


    std::vector<double> CE(n); // single column of C*E

{
    std::vector<double> CE_chunk(chunk_size);
    for(int i = offset; i < end; i++){
        double tmp = 0.0;
        for(int k = 0; k < n; k++){
            tmp += C[i*n + k];
        }
        CE_chunk[i - offset] = tmp;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(CE_chunk.data(), CE_chunk.size(), MPI_DOUBLE, CE.data(), CE_chunk.size(), MPI_DOUBLE, MPI_COMM_WORLD);
}


    double trace = 0.0;
    double z1 = 0.0;
    double z2 = 0.0;
    double Ex = std::accumulate(x.begin(), x.end(), 0.0);

{
    double trace_chunk = 0.0;
    for(int i = offset; i < end; i++){
        for(int k = 0; k < n; k++){
            trace_chunk += B[i*n + k] * CE[k];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(&trace_chunk, &trace, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}


{
    double z1_chunk = 0.0;
    for(int i = offset; i < end; i++){ 
        for(int j = 0; j < n; j++){
            z1_chunk += B[i*n + j] * Ex * y[i]; 
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(&z1_chunk, &z1, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}

{
    double z2_chunk = 0.0;
    for(int i = offset; i < end; i++) {
        z2_chunk += x[i] * y[i];
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(&z2_chunk, &z2, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}


    double z = z1 / z2;

{
    std::vector<double> A_chunk(chunk_size*n);

    for(int i = offset; i < end; i++) {
        for(int j = 0; j < n; j++) {
            A_chunk[(i-offset)*n + j] = trace * C[i*n +j] + (i == j ? 1 : 0) + z1 / z2;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allgather(A_chunk.data(), A_chunk.size(), MPI_DOUBLE, A.data(), A_chunk.size(), MPI_DOUBLE, MPI_COMM_WORLD);
}
    std::chrono::milliseconds elapsed_milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
                                                                  start_time);

    if(world_rank == 0){
        std::cerr << "elapsed " << elapsed_milliseconds.count() << "ms" << std::endl ;
        print_matrix(A, n);

    }
    MPI_Finalize();
}

