%define debug_package %{nil}
%define dist .el9
%define org_version 1.88.0
%define org_versstr 1_88_0

Summary:       Boost C++ Libraries %{org_version}
Name:          libboost_custom
License:       http://www.boost.org/LICENSE_1_0.txt
Version:       5.3.0
Release:       2%{?dist}
Group:         Development/Libraries
Packager:      XTRU product development department <support@suitableapp.com>
Vendor:        SuitableApp
URL:           https://www.suitableapp.com/
Source0:       https://archives.boost.io/release/%{org_version}/source/boost_%{org_versstr}.tar.bz2
BuildRequires: gcc-c++
BuildRequires: libstdc++
BuildRequires: glibc
AutoReqProv:   no
Requires:      libstdc++

%description
Boost C++ Libraries %{org_version}
http://www.boost.org/


%prep
%setup -q -n boost_%{org_versstr}
./bootstrap.sh --with-toolset=gcc --with-libraries=date_time,iostreams,regex,serialization,system,thread,program_options,filesystem
sed -i "s/using gcc/using gcc : : : <cxxflags>\"-Wno-unused-variable -Wno-deprecated-declarations -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++17\"/g" project-config.jam
# sed -i "s/using gcc/using clang : : : <cxxflags>\"-O3 -D_GLIBCXX_USE_CXX11_ABI=0\"/g" project-config.jam

%build
./b2 -a -q %{?_smp_mflags} --layout=tagged address-model=64 stage

%install
%{__install} -m755 -d $RPM_BUILD_ROOT/usr/local/lib $RPM_BUILD_ROOT/usr/local/include/boost
%{__install} -m755 -t $RPM_BUILD_ROOT/usr/local/lib stage/lib/*
chmod a-x $RPM_BUILD_ROOT/usr/local/lib/libboost_*.a
cp -R boost/* $RPM_BUILD_ROOT/usr/local/include/boost

%clean
[ "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
/usr/local/lib/libboost_*.*
/usr/local/include/boost/*
%doc

%changelog
* Wed Mar 1 2023 XTRU product development department <support@suitableapp.com> 5.3.0 1
- Initial release
