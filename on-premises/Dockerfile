FROM oraclelinux:8

MAINTAINER s_hirayama <support@suitableapp.com>

ENV LD_LIBRARY_PATH=/usr/local/lib
ENV TNS_ADMIN=/root/tns_admin

RUN dnf -y update && \
    dnf -y install oracle-epel-release-el8 && \
    dnf -y install openssl libunwind-devel libaio sqlite-devel

WORKDIR /root

CMD ["/bin/bash","-x","/usr/local/bin/init_container.sh"]

# docker rmi ol8_custom && docker builder prune
# docker build --no-cache --progress=plain -t ol8_custom .
# docker run -it --rm -v $PWD/init_container.sh:/usr/local/bin/init_container.sh -v $HOME/rpmbuild/RPMS/$(uname -m):/root -v $TNS_ADMIN:/root/tns_admin -v $HOME/.wallet:/root/.wallet ol8_custom

