FROM oraclelinux:9

MAINTAINER s_hirayama <support@suitableapp.com>

ARG UNAME_M
ENV MACHINE          $UNAME_M
ENV VERS             5.3.0
ENV RNO              2
ENV ORA_RELEASE      23.8
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

RUN dnf -y update && dnf -y install oracle-epel-release-el9 oraclelinux-developer-release-el9 && \
    dnf -y install python3-oci-cli rsync hostname sudo wget rlwrap vim-enhanced jq mlocate \
      git rpmdevtools libcurl-devel ccache p7zip openssl-devel ncurses-devel python3-devel \
      sqlite-devel libunwind-devel autoconf automake libtool cmake gcc-c++ clang && \
    dnf clean all

# Avoiding obstacles in distributing files containing hyphens.
RUN sed -e "s/\(ispythonprog = re\.compile\)\(.\+\)/\1(r'^[-\\\w]+\\\.py$')/" -i /usr/bin/pathfix.py

# Change sudoers
RUN sed -i /etc/sudoers \
 -e "s/%wheel\tALL=(ALL)\tALL/# %wheel\tALL=(ALL)\tALL/g" \
 -e "s/# %wheel\tALL=(ALL)\tNOPASSWD: ALL/%wheel\tALL=(ALL)\tNOPASSWD: ALL/g"

# Prepare an environment to maintain development.
RUN groupadd $DEVELOPER
RUN useradd -g $DEVELOPER -G wheel $DEVELOPER
RUN mkdir -p /home/$DEVELOPER/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
RUN chown -R $DEVELOPER:$DEVELOPER /home/$DEVELOPER/rpmbuild

RUN ulimit -c unlimited

USER $DEVELOPER
WORKDIR /home/$DEVELOPER
ADD dotfiles.tar .
RUN for item in .{oci,ssh,wallet}/ .{bash,screen,vim}rc ; do if [ -e $item ]; then sudo chown -R $DEVELOPER: $item; fi; done
# Use "/home/$DEVELOPER/" instead of "~/"
RUN mkdir -p /home/$DEVELOPER/.vim/autoload/ /home/$DEVELOPER/vim/var/undo/ && \
    wget --directory-prefix=/home/$DEVELOPER/.vim/autoload https://raw.githubusercontent.com/tpope/vim-pathogen/master/autoload/pathogen.vim && \
    mkdir -p /home/$DEVELOPER/.vim/bundle && \
    git clone https://github.com/Shougo/neobundle.vim /home/$DEVELOPER/.vim/bundle/neobundle.vim

RUN sudo updatedb

CMD ["/bin/bash","-x","/usr/local/bin/init_container.sh"]
