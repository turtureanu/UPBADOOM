################################################################################
#
# sedutil-sha512
#
################################################################################

SEDUTIL_SHA512_VERSION = 1.15-5ad84d8
SEDUTIL_SHA512_SITE = $(call github,ChubbyAnt,sedutil,$(SEDUTIL_SHA512_VERSION))
SEDUTIL_SHA512_LICENSE = GPL-3.0+
SEDUTIL_SHA512_LICENSE_FILES = Common/LICENSE.txt
# Fetched from Github with no configure script
SEDUTIL_SHA512_AUTORECONF = YES

# Calls git to figure out version info
define SEDUTIL_SHA512_SET_VERSION
	echo '#define GIT_VERSION "$(SEDUTIL_SHA512_VERSION)"' > $(@D)/linux/Version.h
endef
SEDUTIL_SHA512_POST_CONFIGURE_HOOKS += SEDUTIL_SHA512_SET_VERSION

$(eval $(autotools-package))
