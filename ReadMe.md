
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
  mkdir -p rpmbuild sa_home
  sudo chmod 0777 rpmbuild sa_home tns_admin
  ```

## TODO
## How to build and run.

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

```bash
time docker build --no-cache --progress plain --build-arg UNAME_M=$(uname -m) \
  -t xtru-builder ctx |& tee ~/docker_build_1.log
mkdir -p ./{sa_home,rpmbuild} && chmod 0777 ./{sa_home,rpmbuild} && touch .gitconfig
docker run -it --rm --hostname xtru_host --name xtru \
  -v $PWD/init_container.sh:/usr/local/bin/init_container.sh:ro \
  -v $PWD/tns_admin:/opt/oracle/tns_admin:ro \
  -v $PWD:/home/xtru/xtru \
  -v $PWD/rpmbuild:/home/xtru/rpmbuild \
  -v $PWD/sa_home:/home/xtru/sa_home \
  -v $PWD/.gitconfig=$HOME/.gitconfig \
  xtru-builder
```

- Storage allocated by docker-compose build can be released.
  ```bash
  docker rmi xtru-builder
  docker builder prune
  ```
