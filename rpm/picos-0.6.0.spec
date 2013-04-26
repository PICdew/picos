Name:picos	
Version:0.6.0	
Release:	1%{?dist}
Summary: Multitasking, programmable operating system for the PIC microcontroller.

Group:Development/System	
License:GPLv3	
URL:http://wiki.davecoss.com/Picos
Source0: picos-0.6.0.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: fuse-devel
Requires: fuse

%description
Multitasking, programmable operating system for the PIC microcontroller.

%prep
%setup -q


%build
cd picos/tools
mkdir BUILD
cd BUILD
cmake ..

%install
rm -rf %{buildroot}
cd picos/tools/BUILD
make install DESTDIR=%{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc
/usr



%changelog

