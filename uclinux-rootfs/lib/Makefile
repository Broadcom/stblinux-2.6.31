#
#	Makefile -- Master makefile for all libraries.
#

.EXPORT_ALL_VARIABLES:

#
# Include architecture specific build rules.
#

ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_LIB=1
include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)
include $(ARCH_CONFIG)

#
# always build these dir_
#
dir_y  = $(LIBCDIR) libnet
#dir_y  = libnet
dir_n  =
dir_   =

ifeq ($(LIBCDIR),libc)
dir_y += libm
endif

#
# and build these if someone needs them
#
dir_$(CONFIG_LIB_LIBDES)         += libdes
dir_$(CONFIG_LIB_LIBDES_FORCE)   += libdes
dir_$(CONFIG_LIB_LIBSSL)         += openssl
dir_$(CONFIG_LIB_LIBSSL_FORCE)   += openssl
dir_$(CONFIG_LIB_LIBGMP)         += libgmp
dir_$(CONFIG_LIB_LIBGMP_FORCE)   += libgmp
dir_$(CONFIG_LIB_LIBG)           += libg
dir_$(CONFIG_LIB_LIBG_FORCE)     += libg
dir_$(CONFIG_LIB_LIBPAM)         += libpam
dir_$(CONFIG_LIB_LIBPAM_FORCE)   += libpam
dir_$(CONFIG_LIB_LIBPCAP)        += libpcap
dir_$(CONFIG_LIB_LIBPCAP_FORCE)  += libpcap
dir_$(CONFIG_LIB_LIBATM)         += libatm
dir_$(CONFIG_LIB_LIBATM_FORCE)   += libatm
dir_$(CONFIG_LIB_FREETYPE)       += freetype
dir_$(CONFIG_LIB_FREETYPE_FORCE) += freetype
dir_$(CONFIG_LIB_ZLIB) 		 += zlib
dir_$(CONFIG_LIB_ZLIB_FORCE)     += zlib
dir_$(CONFIG_LIB_LIBUSB)         += libusb
dir_$(CONFIG_LIB_LIBUSB_FORCE)   += libusb
dir_$(CONFIG_LIB_NCURSES)        += ncurses
dir_$(CONFIG_LIB_NCURSES_FORCE)  += ncurses

# THT libtermcap.so is delivered as part of bash
# dir_$(CONFIG_LIB_LIBTERMCAP)	 += $(ROOTDIR)/user/bash
# dir_$(CONFIG_LIB_LIBTERMCAP_FORCE)	 += $(ROOTDIR)/user/bash

# JD - independent libtermcap
dir_$(CONFIG_LIB_LIBTERMCAP)     += libtermcap
dir_$(CONFIG_LIB_GNU_LIBTERMCAP) += libtermcap

dir_$(CONFIG_LIB_POPT)           += popt
dir_$(CONFIG_LIB_LIBBFD)         += binutils
dir_$(CONFIG_LIB_LIBIBERTY)      += binutils
dir_$(CONFIG_LIB_BINUTILS_OBJDUMP) += binutils

dir_$(CONFIG_LIB_SNAPGEAR)       += libsnapgear libconfigdd

ifneq ($(INITRD_SUBDIR),) 
ifneq ($(shell echo $(filter lib%,$(INITRD_SUBDIR))|sed -e 's/lib[\/]/lib/'),lib)
dir_y:=$(patsubst lib/%,%,$(filter lib/%,$(INITRD_SUBDIR)))
endif
endif

all:
ifeq ($(LIBCDIR),glibc/build)
	[ -d glibc/build ] || mkdir glibc/build
	if [ ! -f glibc/build/config.cache ]; then \
		cd glibc/build; \
		export CFLAGS="$(VENDOR_CFLAGS) -DEMBED -O2"; \
		unset LDFLAGS; \
		../configure  --prefix= \
			$(GLIBC_TARGET_OPTS) --build=i386-redhat-linux \
			--with-headers=$(ROOTDIR)/$(LINUXDIR)/include \
			--without-cvs --without-gd \
			--disable-profile --disable-force-install \
			--enable-add-ons ; \
	fi
endif
# Force a rebuild of the Makefiles in the directfb tree.
	if [ -d directfb ]; then $(RM) directfb/Makefile directfb/configure directfb/config.cache; fi
	@echo '$$$$$$$$$$$$$$$$$ Building all libraries $$$$$$$$$$$$$$$$$$$$$$$$'
	@echo 'dir_y =' "$(dir_y)"
	for i in $(dir_y) ; do [ ! -d $$i ] || $(MAKEARCH) -C $$i || exit $? ; done

romfs:
	for i in $(dir_y) ; do \
		[ ! -d $$i ] || $(MAKEARCH) -C $$i romfs || exit $$? ; \
	done
ifeq ($(INITRD_SUBDIR),) 
ifeq ($(findstring y,$(CONFIG_LIB_LIBSTDCXX_FORCE)$(CONFIG_LIB_LIBSTDCXX)),y)
	-$(RM) -f $(ROMFSDIR)/lib/libstdc++.*
	cp -af $(GCCLIB)/libstdc++.so* $(ROMFSDIR)/lib/
	$(STRIP) $(ROMFSDIR)/lib/libstdc++.so
endif
ifeq ($(findstring y,$(CONFIG_LIB_LIBSUPCXX_FORCE)$(CONFIG_LIB_LIBSUPCXX)),y)
	-$(RM) -f $(ROMFSDIR)/lib/libsupc++.*
	cp -af $(GCCLIB)/libsupc++.so* $(ROMFSDIR)/lib/
	$(STRIP) $(ROMFSDIR)/lib/libsupc++.so
endif
ifeq ($(findstring y,$(CONFIG_LIB_LIBGCC_FORCE)$(CONFIG_LIB_LIBGCC)),y)
	# THT: Copy libgcc_s.so to romfs dir (see: PR45130, PR44678)
	-$(RM) -f $(ROMFSDIR)/lib/libgcc_s.so*
	cp -af $(GCCLIB)/libgcc_s.* $(ROMFSDIR)/lib/
	$(STRIP) $(ROMFSDIR)/lib/libgcc_s.so
endif
endif

clean:
ifeq ($(LIBCDIR),glibc/build)
	rm -rf glibc/build
endif
	-for i in $(dir_y) $(dir_n) $(dir_); do \
		[ ! -d $$i ] || $(MAKEARCH) -C $$i clean ; \
	done
ifeq ($(LIBCDIR),uClibc)
	rm -f $(LIBCDIR)/Config
endif

distclean:
	-for i in $(dir_y) $(dir_n) $(dir_); do \
		[ ! -d $$i ] || $(MAKEARCH) -C $$i distclean ; \
	done
ifeq ($(LIBCDIR),uClibc)
	rm -f $(LIBCDIR)/Config
endif
