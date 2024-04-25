include $(TOPDIR)/rules.mk

PKG_NAME:=test_pcm_capture
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)
SECTION:=allwinner
CATEGORY:=Allwinner
TITLE:=my-test 
DEPENDS:= +libstdcpp 
endef

define Package/$(PKG_NAME)/description
 test tool.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
