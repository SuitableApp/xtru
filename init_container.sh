#!/bin/bash

# Create /home/$DEVELOPER/rpmbuild and subdirectories.
rpmdev-setuptree 

# Install products of Oracle Corporation
# Newer packages are available, but These are chosen because they are available for both aarch64 and x86-64.

if [ -z $(rpm -qa oracle-*instantclient*.*-basic) ]; then
    rm -rf /home/$DEVELOPER/rpmbuild/RPMS/$MACHINE/oracle-instantclient$ORA_RELEASE-{basic,sqlplus,devel,tools}-$ORA_RELEASE.$ORA_PATCH_LV.$MACHINE.rpm
    wget --directory-prefix=/home/$DEVELOPER/rpmbuild/RPMS/$MACHINE https://download.oracle.com/otn_software/linux/instantclient/2380000/oracle-instantclient$ORA_RELEASE-{basic,sqlplus,devel,tools}-$ORA_RELEASE.$ORA_PATCH_LV.el9.$MACHINE.rpm
    sudo dnf install -y /home/$DEVELOPER/rpmbuild/RPMS/$MACHINE/oracle-instantclient$ORA_RELEASE-*-$ORA_RELEASE.$ORA_PATCH_LV.$MACHINE.rpm
    sudo ln -s /usr/lib/oracle/$ORA_RELEASE/client64/bin/sqlldr /usr/bin/sqlldr
    sudo ln -s /usr/lib/oracle/$ORA_RELEASE/client64/lib/libocci.so.23.1 /usr/lib/oracle/$ORA_RELEASE/client64/lib/libocci.so
fi

cd $WKDIR

# Build and Install dependencies.
PRJ_LIST='llvm libgperftools libboost'
for PRJ_NAME in $PRJ_LIST; do
    RPM_NAME=/home/$DEVELOPER/rpmbuild/RPMS/$MACHINE/${PRJ_NAME}_custom-$VERS-$RNO.el9.$MACHINE.rpm
    if [ ! -f $RPM_NAME ]; then
        spectool -g -R ${PRJ_NAME}_custom.spec
        if [ $? -ne 0 ]; then exit -1; fi
        rpmbuild -bb ${PRJ_NAME}_custom.spec  2>&1 | tee build_${PRJ_NAME}_custom.log
        if [ $? -ne 0 ]; then exit -2; fi
    fi

    rpm -qa | grep ${PRJ_NAME}_custom
    if [ $? -gt 0 ]; then
        sudo dnf -y install $RPM_NAME
    fi
done

# Build and Install a subject (Extreme Unloader).
PRJ_NAME=xtru
RPM_NAME=/home/$DEVELOPER/rpmbuild/RPMS/$MACHINE/${PRJ_NAME}-$VERS-$RNO.el9.$MACHINE.rpm
if [ ! -f $RPM_NAME ]; then
    git archive HEAD --output=../rpmbuild/SOURCES/${PRJ_NAME}-$VERS.tar.gz --prefix=${PRJ_NAME}-$VERS/
    rpmbuild -ba ${PRJ_NAME}.spec  2>&1 | tee build_${PRJ_NAME}.log
    if [ $? -ne 0 ]; then exit -3; fi
    sudo dnf -y install $RPM_NAME
fi

rpm -qa | grep ${PRJ_NAME}
if [ $? -gt 0 ]; then
    sudo dnf -y install $RPM_NAME
fi

exec /bin/bash
