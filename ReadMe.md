# About XTRU

## Synopsis

- **Extreme Unloader** (XTRU) is an app for quick file exporting from Oracle Database.
- This app is written in Standard C++11 using Boost C++ libraries and Oracle C++ Call Interface (OCCI) functions.
- High utilization efficiency for network and memory buffer can expect since it can fetch multiple rows at a time.
- By allocating threads for each partition in the table, CPU time can be intensively input, so you can fully utilize system scalability and reduce the time required.
- Optionally, datagrams can be piped to subsequent processes (IPC), reducing the I/O workload to zero.
- XTRU works so smoothly that it helps to incarnate operational plans that have taken too long and given up.
- XTRU can output control files for "SQL\*Loader" along with flat files, making it easier to transport data.


## An example of exporting a familiar table

- Data file

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

- Control file for using SQL\*Loader
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
- Default settings are tuned to produce output in a representation format that maximizes reload performance.
- CSV, TSV, and fixed-length format can also be selected.


## Data Sheet

  | Index                  | Measured            | Remarks       |
  | ---------------------- | -------------------:| -------------:|
  | Destination:           |                     |               |
  | - OCI File Storage     |            40 MiB/s |               |
  | - OCI Block Volume     |            46 MiB/s |        10 VPU |
  | Condition:             |                     |               |
  | - Segment size (TABLE) |            5,696 MB | Unpartitioned |
  | - Number of rows       |          50,000,000 |               |
  | - Average size per row |           165 bytes |               |
  | - Compute shape        | VM.Standard.A1.Flex | 4 OCPU, 24 GB Memory |


# Getting started

## Prepare for composition by Docker.

- Clone the repository and move to it.
  ```bash
  git clone git@github.com:SuitableApp/xtru.git ~/xtru
  cd ~/xtru
  ```

- If you would like to help us grow this project, please add an optional branch to enable Pull Requests.
  ```bash
  git checkout -b <YOUR_FAVORITE_BRANCH_NAME>
  ```

- Prepair a stage of building RPM packages and a working directory for this app.
  ```bash
  mkdir -p ~/{sa_home,rpmbuild,tns_admin}
  chmod a+w ~/{sa_home,rpmbuild}
  ```
- Save a copy of the Oracle Net configuration parameter files valid for your environment to **tns_admin**. For example:
  ```bash
  cp $TNS_ADMIN/{sqlnet,tnsnames}.ora ~/tns_admin/
  ```
  or
  ```bash
  cp $ORACLE_HOME/network/admin/{sqlnet,tnsnames}.ora ~/tns_admin/
  ```

- Gather copies of the dotfiles and dotfolders that are valid in your environment into **dotfiles.tar**. It already has some commonly used dotfiles (e.g. .vimrc, .screenrc).
- Anything saved it will be placed to the $HOME of the application user "**xtru**" when composing the image.
- ".ssh/" is probably necessary if you ever connect to GitHub.
- Include ".oci/" if you want to be connected to OCI (Oracle Cloud Infrastructure) form the container.
- Include ".wallet/" if you want to be connected to Autonomous Database under the mTLS authentication.
  ```bash
  cp dotfiles.tar ctx/
  tar -C ~/ -rvf ctx/dotfiles.tar .{oci,ssh,wallet}/
  tee ~/.gitconfig <<EOF
  [user]
     name = your_github_account
     email = your@email.address
  EOF
  ```

## Compose and run

- Storage allocated by docker-compose build can be released.
  ```bash
  docker rmi xtru-builder
  docker builder prune -f
  docker image prune -a -f
  ```

- You can start a container with the following command. If the image has not been built, it will be prepared first.
  ```bash
  docker-compose run --rm builder
  ```
  - Depending on your platform, you might receive a warning and build fails. Like a following:
    > WARN[0000] The "HOSTTYPE" variable is not set. Defaulting to a blank string.

  - Make up for a value what is missing.
    ```bash
    echo "HOSTTYPE=$(uname -m)" > ./.env
    ```
  - And rerun the `docker-compose run` command above.

- What you need to know before you start creating an image:
  - A docker image building (i.e. docker-compose build) depends on specs, but in most cases it takes a few minutes.
  - However, launching a container takes longer than that. It may be over when you go out for a haircut and come back.
  - And the container requires about 5GB of temporary storage. This can be removed before runtime.
  - This is because it will start building the LLVM, Clang, and Boost C++ Libraries (if these have not built so already).
  - Oracle Database Instant Client 19c will be also downloaded and set up automatically, as it is required for application build and runtime.


## After launched the container

- `docker-compose run` will eventually wait for the next input.
- Hit "`xtru`" and enter key. When the App is launched for the first time, it will stop immediately after some files and directories have been placed in the pre-created `~/sa_home` directory.
- `~/sa_home/xtru.conf` is one of the most important ones to be placed, and is used to specify the user name, password, and connection string for the Oracle Database.
- You should open this file with `vim` and replace the value of the keyword **`userid`** shown in the example with a real connection string. The factory default value for `userid` is set to "ADMIN/ChageToYourSetting@atp_high".
- The value of the keyword **`src_user`** should also be replaced to a real schema name. The default value is set to "SH".
- You close `~/sa_home/xtru.conf` and hit `xtru` again. If all goes well, each table in the specified schema will begin to be exported by XTRU.
- If you do not need to watch the process complete, you can always hit **`Ctrl-C`** to abort.
- The exported data will be output to the `~/sa_home/output` directory. This is specified by the value of the keyword **`output`** in `~/sa_home/xtru.conf`, or you can specify an absolute path.
- If you give the keyword **`listtable`** a CSV of table names, you can select a target from the tables in the schema. The table name is case sensitive.

