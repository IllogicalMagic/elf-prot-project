#ifndef ENCRYPT_COMMONS_H_DEFINED
#define ENCRYPT_COMMONS_H_DEFINED

#include "aes256.h"
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

namespace Cipher {
    namespace fs = std::experimental::filesystem;
    bool validate_filepath(const char * fp) {
        auto stat = fs::status(fp);
        return fs::is_regular_file(stat);
    }

    using ModeT = std::ios_base::openmode;
    using Mode = std::ios_base;
    //Mode::in is AND-ed automatically
    void loadfile_into_str(const char * fp, ModeT op_mode, std::string & dst) {
        if (!validate_filepath(fp)) {
            std::cout << "Filepath is incorrect\n";
            exit(-1);
        }
        std::ifstream file_stream(fp, op_mode & Mode::in);

        std::istreambuf_iterator<char> tmp;
        dst = std::string(std::istreambuf_iterator<char>(file_stream), tmp);
    }

    void loadfile_into_vector(char * fp, ModeT op_mode, std::vector<CryptoPP::byte> & dst) {
        if (!validate_filepath(fp)) {
            std::cout << "Filepath is incorrect\n";
            exit(-1);
        }
        std::ifstream file_stream(fp, op_mode & Mode::in);

        std::istreambuf_iterator<char> tmp;
        dst = std::vector<CryptoPP::byte>(std::istreambuf_iterator<char>(file_stream), tmp);
    }
}

#endif //ENCRYPT_COMMONS_H_DEFINED
