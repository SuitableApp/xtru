%define debug_package %{nil}
%define dist .el9
%define org_version 18.1.8
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
Source0:       https://github.com/llvm/llvm-project/releases/download/llvmorg-%{org_version}/llvm-project-%{org_version}.src.tar.xz
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
%setup -q -n llvm-project-%{org_version}.src

%build
mkdir build
cd build
cmake -G "Unix Makefiles" \
 -DCMAKE_BUILD_TYPE="Release" \
 -DCMAKE_INSTALL_PREFIX="/usr/local" \
 -DCMAKE_CXX_FLAGS="-w" \
 -DLLVM_PARALLEL_LINK_JOBS="1" \
 -DLLVM_INCLUDE_BENCHMARKS=OFF \
 -DLLVM_INCLUDE_TESTS=OFF \
 -DLLVM_INCLUDE_EXAMPLES=OFF \
 -DLLVM_TARGETS_TO_BUILD="X86;AArch64" \
 -DLLVM_ENABLE_PROJECTS="clang;lld" \
 ../llvm
make %{?_smp_mflags}

%install
cd build
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
