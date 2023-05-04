#!/bin/bash

rpm -ivh \
  oracle-instantclient19.10-basic-19.10.0.0.0-1.aarch64.rpm \
  libboost_custom-5.3.0-2.el8.aarch64.rpm \
  libgperftools_custom-5.3.0-2.el8.aarch64.rpm \
  xtru-5.3.0-2.el8.aarch64.rpm

exec /bin/bash
