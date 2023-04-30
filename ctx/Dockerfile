FROM oraclelinux:8

MAINTAINER s_hirayama <support@suitableapp.com>

ARG UNAME_M
ENV MACHINE          $UNAME_M
ENV VERS             5.3.0
ENV RNO              2
ENV ORA_RELEASE      19.10
ENV ORA_PATCH_LV     0.0.0-1
ENV OCCI_INC_PATH    /usr/include/oracle/$ORA_RELEASE/client64
ENV OCCI_LIB_PATH    /usr/lib/oracle/$ORA_RELEASE/client64/lib
ENV LD_LIBRARY_PATH  /usr/local/lib:$OCCI_LIB_PATH
ENV NLS_LANG         japanese_japan.al32utf8
ENV TNS_ADMIN        /opt/oracle/tns_admin
ENV DEVELOPER        xtru
ENV WKDIR            xtru

# which alias is configured in the .ssh/config.
ENV REPOS            gitlab

RUN dnf -y update && dnf -y install oracle-epel-release-el8 oraclelinux-developer-release-el8 && \
    dnf config-manager --add-repo http://yum.oracle.com/repo/OracleLinux/OL8/developer/EPEL/$MACHINE && \
    dnf -y install python36-oci-cli dstat rsync screen hostname sudo wget rlwrap vim-enhanced jq mlocate \
      git rpmdevtools libcurl-devel ccache p7zip openssl-devel ncurses-devel python3-devel \
      sqlite-devel libunwind-devel rr-testsuite autoconf automake libtool cmake gcc-c++ && \
    dnf clean all

# Avoiding obstacles in distributing files containing hyphens.
RUN sed -e "s/\(ispythonprog = re\.compile\)\(.\+\)/\1(r'^[-\\\w]+\\\.py$')/" -i /usr/bin/pathfix.py

# Change sudoers
RUN sed -i /etc/sudoers \
 -e "s/%wheel\tALL=(ALL)\tALL/# %wheel\tALL=(ALL)\tALL/g" \
 -e "s/# %wheel\tALL=(ALL)\tNOPASSWD: ALL/%wheel\tALL=(ALL)\tNOPASSWD: ALL/g"

# Install products of Oracle Corporation
# Newer packages are available, but These are chosen because they are available for both aarch64 and x86-64.
RUN wget --directory-prefix=/tmp https://download.oracle.com/otn_software/linux/instantclient/191000/oracle-instantclient$ORA_RELEASE-{basic,sqlplus,devel,tools}-$ORA_RELEASE.$ORA_PATCH_LV.$MACHINE.rpm && \
    dnf install -y /tmp/oracle-instantclient19.*-*-19*.$MACHINE.rpm && \
    ln -s /usr/lib/oracle/$ORA_RELEASE/client64/bin/sqlldr /usr/bin/sqlldr && \
    ln -s /usr/lib/oracle/$ORA_RELEASE/client64/lib/libocci.so.19.1 /usr/lib/oracle/$ORA_RELEASE/client64/lib/libocci.so && \
    rm -rf /tmp/oracle-instantclient19.*-*-19*.$MACHINE.rpm

# Prepare an environment to maintain development.
RUN groupadd $DEVELOPER
RUN useradd -g $DEVELOPER -G wheel $DEVELOPER

RUN ulimit -c unlimited

USER $DEVELOPER
WORKDIR /home/$DEVELOPER
ADD dotfiles.tar .
RUN for item in .{oci,ssh,wallet}/ .{bash,screen,vim}rc ; do if [ -e $item ]; then sudo chown -R $DEVELOPER: $item; fi; done
RUN mkdir -p ~/.vim/autoload/ ~/vim/var/undo/ && \
    wget --directory-prefix=~/.vim/autoload https://raw.githubusercontent.com/tpope/vim-pathogen/master/autoload/pathogen.vim && \
    mkdir -p ~/.vim/bundle && \
    git clone https://github.com/Shougo/neobundle.vim ~/.vim/bundle/neobundle.vim

RUN sudo updatedb

CMD ["/bin/bash","-x","/usr/local/bin/init_container.sh"]
