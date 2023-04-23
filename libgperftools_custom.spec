%define debug_package %{nil}
%define dist .el8
%define org_version 2.10
%undefine __arch_install_post

Summary:       Google Performance Tools 2.6rc4
Name:          libgperftools_custom
License:       BSD
Version:       5.3.0
Release:       2%{?dist}
Group:         Development/Debuggers
Packager:      XTRU product development department <support@suitableapp.com>
Vendor:        SuitableApp
URL:           https://github.com/gperftools/gperftools
Source0:       https://github.com/gperftools/gperftools/archive/gperftools-%{org_version}.zip
BuildRequires: libunwind-devel
BuildRequires: autoconf
BuildRequires: automake
BuildRequires: libtool
BuildRequires: glibc
AutoReqProv:   no
Requires:      glibc

%description
Google Performance Tools
https://github.com/gperftools/gperftools

%prep
%setup -q -n gperftools-gperftools-%{org_version}
autoreconf -iv

%build
./configure
make %{?_smp_mflags}

%install
make install prefix=$RPM_BUILD_ROOT/usr/local

%clean
[ "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
/usr/local/bin/*
/usr/local/include/*
/usr/local/lib/*
%doc /usr/local/share/*

%changelog
* Wed Mar 1 2023 XTRU product development department <support@suitableapp.com> 5.3.0 1
- Initial release
