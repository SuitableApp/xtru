#!/bin/sh 

DB_NAME=$HOME/sa_home/xtru.db

rm -f /tmp/dump_*.txt

sub_proc ()
{
    git checkout $1 > /tmp/$2.txt
    make clean clean_gch ; make -j $NUM_CPU_CORES
#   rm -f $DB_NAME
    build/xtru unload
    sqlite3 $DB_NAME .dump >> /tmp/$2.txt
}

sub_proc d3946e7 dump_a
sub_proc develop dump_b

diff -u /tmp/dump_a.txt /tmp/dump_b.txt
echo Enter following command, if you want to chek difference.
echo vim /tmp/dump_a.txt -c \"vertical diffsplit /tmp/dump_b.txt\"
