%define debug_package %{nil}
%define dist .el9
%define org_version 20.1.0
%undefine __arch_install_post


Summary:       LLVM %{org_version}
Name:          llvm_custom
License:       NCSA http://releases.llvm.org/4.0.0/LICENSE.TXT
Version:       5.3.0
Release:       2%{?dist}
Group:         Development/Tools
Packager:      XTRU product development department <support@suitableapp.com>
Vendor:        SuitableApp
URL:           https://llvm.org/
Source0:       https://github.com/llvm/llvm-project/releases/download/llvmorg-%{org_version}/llvm-%{org_version}.src.tar.xz
Source1:       https://github.com/llvm/llvm-project/releases/download/llvmorg-%{org_version}/clang-%{org_version}.src.tar.xz
Source2:       https://github.com/llvm/llvm-project/releases/download/llvmorg-%{org_version}/lld-%{org_version}.src.tar.xz
BuildRequires: cmake
BuildRequires: zlib-devel
BuildRequires: ncurses-devel
BuildRequires: python3-devel
AutoReqProv:   no
Requires:      glibc

%description
LLVM is a compiler infrastructure designed for compile-time, link-time,
runtime, and idle-time optimization of programs from arbitrary programming
languages. The compiler infrastructure includes mirror sets of programming
tools as well as libraries with equivalent functionality.

%prep
%setup -q -a 1 -a 2 -n %{name}-%{version} -c
mv clang-%{org_version}.src llvm-%{org_version}.src/tools/clang
mv lld-%{org_version}.src llvm-%{org_version}.src/tools/lld

%build
cmake -G "Unix Makefiles" -DCMAKE_CXX_FLAGS="-w" -DCMAKE_BUILD_TYPE="Release" \
 -DLLVM_PARALLEL_LINK_JOBS="1" \
 -DLLVM_INCLUDE_BENCHMARKS=OFF \
 -DLLVM_TARGETS_TO_BUILD="X86;AArch64" llvm-%{org_version}.src
make %{?_smp_mflags}

%install
%make_install DESTDIR=$RPM_BUILD_ROOT
%py3_shebang_fix \
  %{buildroot}/usr/local/share \
  %{buildroot}/usr/local/bin/git-clang-format \
  %{buildroot}/usr/local/bin/scan-view \
  %{buildroot}/usr/local/bin/hmaptool

%clean
[ "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
/usr/local/bin/*
/usr/local/include/*
/usr/local/lib/*
/usr/local/libexec/*
/usr/local/share/*

%changelog
