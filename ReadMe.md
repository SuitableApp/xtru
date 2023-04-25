# About XTRU

## Synopsis

- Extreme Unloader (XTRU) is an app for quick file exporting from Oracle Database.
- This app is written in Standard C++11 using Boost C++ libraries and Oracle C++ Call Interface (OCCI) functions.
- High utilization efficiency for network and memory buffer can expect since it can fetch multiple rows at a time.
- By allocating threads for each partition in the table, CPU time can be intensively input, so you can fully utilize system scalability and reduce the time required.
- Optionally, datagrams can be piped to subsequent processes (IPC), reducing the I/O workload to zero.
- XTRU works so smoothly that it helps to incarnate operational plans that have taken too long and given up.
- XTRU can output control files for "SQL*Loader" along with flat files, making it easier to transport data.

## An example of exporting a familiar table:

- Data file.
```plain
00000000587369,~~SMITH~~,~~CLERK~~,7902,~~19801217000000~~,800,,20,
00000000657499,~~ALLEN~~,~~SALESMAN~~,7698,~~19810220000000~~,1600,300,30,
00000000647521,~~WARD~~,~~SALESMAN~~,7698,~~19810222000000~~,1250,500,30,
00000000617566,~~JONES~~,~~MANAGER~~,7839,~~19810402000000~~,2975,,20,
00000000677654,~~MARTIN~~,~~SALESMAN~~,7698,~~19810928000000~~,1250,1400,30,
00000000617698,~~BLAKE~~,~~MANAGER~~,7839,~~19810501000000~~,2850,,30,
00000000617782,~~CLARK~~,~~MANAGER~~,7839,~~19810609000000~~,2450,,10,
00000000587839,~~KING~~,~~PRESIDENT~~,,~~19811117000000~~,5000,,10,
00000000647844,~~TURNER~~,~~SALESMAN~~,7698,~~19810908000000~~,1500,0,30,
00000000587900,~~JAMES~~,~~CLERK~~,7698,~~19811203000000~~,950,,30,
00000000607902,~~FORD~~,~~ANALYST~~,7566,~~19811203000000~~,3000,,20,
00000000607934,~~MILLER~~,~~CLERK~~,7782,~~19820123000000~~,1300,,10,
```
- Control file for using SQL*Loader.
```plain
LOAD
INFILE "SCOTT.EMP.dat" "VAR 10"
INTO TABLE "SCOTT"."EMP"
TRUNCATE REENABLE FIELDS TERMINATED BY ','
(  "EMPNO"      DECIMAL EXTERNAL(5)
,  "ENAME"      CHAR(10) ENCLOSED BY '~~'
,  "JOB"        CHAR(9) ENCLOSED BY '~~'
,  "MGR"        DECIMAL EXTERNAL(5)
,  "HIREDATE"   DATE(16) "YYYYMMDDHH24MISS" ENCLOSED BY '~~'
,  "SAL"        DECIMAL EXTERNAL(9)
,  "COMM"       DECIMAL EXTERNAL(9)
,  "DEPTNO"     DECIMAL EXTERNAL(3)
)
```
- The factory default settings are output in a representation format that maximizes performance when reloading.
- CSV, TSV, and fixed-length formats can also be selected.

## Data Sheet

  | Index                  | Measured            | Remarks       |
  | ---------------------- | -------------------:| -------------:|
  | Destination:           |                     |               |
  | - OCI File Storage     |            40 MiB/s |               |
  | - OCI Block Volume     |            46 MiB/s |        10 VPU |
  | Condition:             |                     |               |
  | - Segment size (TABLE) |            5,696 MB |               |
  | - Number of rows       |          50,000,000 |               |
  | - Average size per row |           165 bytes |               |
  | - Compute shape        | VM.Standard<br>.A1.Flex | 4 VCPU,<br>24 GB Memory |


# Getting started

## Prepare to builld.

- Clone the repository, and change directory.
  ```bash
  git clone git@github.com:SuitableApp/xtru.git ~/xtru
  cd ~/xtru
  ```
- Prepare dotfiles.tar.gz tns_admin
  ```bash
  ```
- Prepair a stage of building RPM packages, and a working directory for the application.
  ```bash
  mkdir -p ./{sa_home,rpmbuild,tns_admin}
  chmod 0777 ./{sa_home,rpmbuild,tns_admin}
  touch .gitconfig
  ```
- Refresh .ssh/known_hosts
  ```bash
  tar --delete -vf ctx/dotfiles.tar.gz ./.ssh/known_hosts
  tar -C ~/ -rvf ctx/dotfiles.tar.gz ./.ssh/known_hosts
  ```

## TODO
## How to build and run.

- Storage allocated by docker-compose build can be released.
  ```bash
  docker rmi xtru-builder
  docker builder prune
  docker image prune -a
  ```

- You can start a container with the following command. If the image has not been built, it will be prepared first.
  ```bash
  docker-compose run --rm builder
  ```
- About what to expect before you start building:
  - A docker image building (i.e. docker-compose build) depends on specs, but in most cases it takes a few minutes.
  - However, building a container takes longer than that. It may be over when you go out for a haircut and come back.
  - This is because it will start building the LLVM, Clang, and Boost C++ Libraries (if you haven't done so already).

- Depending on your platform, you may receive the following warning: and build fails.
  > WARN[0000] The "HOSTTYPE" variable is not set. Defaulting to a blank string.

- Please fill in a required values in the following command.
  ```bash
  echo "HOSTTYPE=$(uname -m)" > ./.env
  ```
- (1)
  ```bash
  time docker build --no-cache --progress plain --build-arg UNAME_M=$HOSTTYPE \
    -t xtru-builder ctx |& tee ~/docker_build_1.log
  docker run -it --rm --hostname xtru_host --name xtru \
    -v $PWD/init_container.sh:/usr/local/bin/init_container.sh:ro \
    -v $PWD/tns_admin:/opt/oracle/tns_admin:ro \
    -v $PWD:/home/xtru/xtru \
    -v $PWD/rpmbuild:/home/xtru/rpmbuild \
    -v $PWD/sa_home:/home/xtru/sa_home \
    -v $PWD/.gitconfig=/home/xtru/.gitconfig \
    xtru-builder
  ```
