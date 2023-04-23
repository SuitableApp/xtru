#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "../define.h"
#include "../cCryptOpenSSL.h"


int main(int argc, char *argv[]) {
    if (3 != argc) {
        tcerr << "Usage: " << argv[0] << " crdfile connectstr" << std::endl;
        return EXIT_FAILURE;
    }
    ps::app::mkcrd::cCryptOpenSSL oCrypt(TEXT("MKRCD"));
    tstring crdfile = argv[1];
    tstring sCryptedStr;

// Encode process
    tstring sConnStr = argv[2];
    tofstream ofs( crdfile.c_str() );
    tcout << "ConnectStr:" << sConnStr << std::endl;

    sCryptedStr = oCrypt.sEncryptStr( sConnStr );  // encode
    ofs << sCryptedStr << std::endl;

    return EXIT_SUCCESS;
}
