#include<iostream>
#include<fstream>

char buffer[32*1024*1024] = {0};
int main(int argc, char *argv[]) {
    std::cout<<"Opening "<<argv[1]<<std::endl;
    std::ifstream in(argv[1]);
    in.seekg(0, std::ios::end);
    int size = in.tellg();
    std::cout<<"size: "<<size<<" (";
    size /= (1024*1024);
    std::cout<<size<<" MB)"<<std::endl;
    in.seekg(0xc00, std::ios::beg);
    std::ofstream out("output.gba");
    in.read(buffer, size * 1024 * 1024);
    out.write(buffer, size * 1024 * 1024);
    in.close();
    out.close();
    return 0;
}
