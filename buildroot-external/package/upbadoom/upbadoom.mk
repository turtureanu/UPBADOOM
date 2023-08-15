################################################################################
#
# upbadoom package
#
################################################################################

UPBADOOM_VERSION = 1.0.0
UPBADOOM_SITE = $(BR2_EXTERNAL_UPBADOOM_PATH)/../upbadoom
UPBADOOM_LICENSE = GPL-3.0+
UPBADOOM_SITE_METHOD = local

define UPBADOOM_BUILD_CMDS
    $(MAKE) -C $(@D)
endef

define UPBADOOM_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/upbadoom $(TARGET_DIR)/usr/sbin/upbadoom
endef

$(eval $(generic-package))
