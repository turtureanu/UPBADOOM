################################################################################
#
# fbdoom
#
################################################################################

FBDOOM_VERSION = 912c51c4d0e5aa899bb534e8c77227a556ff2377
FBDOOM_SITE = $(call github,maximevince,fbDOOM,$(FBDOOM_VERSION))
FBDOOM_LICENSE = GPLv2+
FBDOOM_LICENSE_FILES = README.TXT

define FBDOOM_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/fbdoom NOSDL=1 all
endef

define FBDOOM_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/fbdoom/fbdoom $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
