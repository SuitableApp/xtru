%define debug_package %{nil}
%define dist .el8

Summary:       Extreme Unloader
Name:          xtru
License:       The customer is granted only the non-exclusive right to use based on the contract
Version:       5.3.0
Release:       2%{?dist}
Group:         Applications/Databases
Packager:      XTRU product development department <support@suitableapp.com>
Vendor:        SuitableApp
URL:           https://www.suitableapp.com/
Source:        %{name}-%{version}.tar.gz
BuildRequires: libboost_custom >= %{version}
BuildRequires: libgperftools_custom >= %{version}
BuildRequires: llvm_custom >= %{version}
BuildRequires: libstdc++ >= 4.4
BuildRequires: glibc >= 2.12
BuildRequires: openssl-devel >= 1.0.1e
BuildRequires: sqlite-devel >= 3.6
BuildRequires: ccache >= 3.7.0
AutoReqProv:   no
Requires:      libboost_custom >= %{version}
Requires:      libgperftools_custom >= %{version}
Requires:      openssl >= 1.0.1e
Requires:      sqlite >= 3.6
%description
Extreme Unloader (XTRU) is an utility softwre for quick file exporting from Oracle Database.
This software is written in Standard C++11 using Boost C++ libraries and Oracle C++ Call Interface (OCCI).
High utilization efficiency for network and memory buffer can be expected, because it able to fetch multiple rows at a time.
By allocating threads for each partition in the table, CPU time can be intensively pour, so you can fully utilize system scalability and reduce the time required.
XTRU would help to incarnate your operational plan that has been taken too match time and given up, because it outperformed than the conventional.
XTRU is very easy to move large amounts of data because control file for "SQL*Loader" along with the flat file is also output.

%prep
%setup -q

%build
make CONF=Release %{?_smp_mflags}

%install
make install CONF=Release prefix=$RPM_BUILD_ROOT/usr/local

%clean
[ "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%check

%post
#install -m775 -d $RPM_BUILD_ROOT/var/log/suitableapp

%postun
#[ "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT/var/log/suitableapp

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS ReadMe.md
/usr/local/bin/*
/usr/local/share/%{name}-%{version}/conf.tar

%changelog
* Wed Mar 1 2023 XTRU product development department <support@suitableapp.com> 5.3.0 1
- Initial release
