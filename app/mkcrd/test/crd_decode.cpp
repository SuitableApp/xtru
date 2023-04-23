#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "../define.h"
#include "../cCryptOpenSSL.h"


int main(int argc, char *argv[]) {
    if (2 != argc) {
        tcerr << "Usage: " << argv[0] << " crdfile " << std::endl;
        return EXIT_FAILURE;
    }
    ps::app::mkcrd::cCryptOpenSSL oCrypt(TEXT("MKRCD"));
    tstring crdfile = argv[1];
    tstring sCryptedStr;

// Decode process
    tifstream ifs( crdfile.c_str() );
    // Read the encrypted 4-byte integer character string one by one into the buffer.
    TCHAR buff[1024];
    std::memset(buff, 0, 1024);
    while (ifs)
    {
        ifs.read(buff, 1024);
        sCryptedStr.append(buff, (tstring::size_type) ifs.gcount());
    }
    tcout << "crd str:"<< std::endl << buff << std::endl;
    tstring sConnStr = oCrypt.sDecryptStr( sCryptedStr );  // decode
    tcout << TEXT("Coonect string is:") << std::endl;
    tcout << sConnStr << std::endl;

    return EXIT_SUCCESS;
}
