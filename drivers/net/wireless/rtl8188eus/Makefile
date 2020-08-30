KBUILD_CFLAGS += -Idrivers/net/wireless/rtl8188eus/include -DCONFIG_LITTLE_ENDIAN

EXTRA_CFLAGS += -O1
#EXTRA_CFLAGS += -O3
#EXTRA_CFLAGS += -Wall
#EXTRA_CFLAGS += -Wextra
#EXTRA_CFLAGS += -Werror
#EXTRA_CFLAGS += -pedantic
#EXTRA_CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes

EXTRA_CFLAGS += -Wno-unused-variable
EXTRA_CFLAGS += -Wno-unused-value
EXTRA_CFLAGS += -Wno-unused-label
EXTRA_CFLAGS += -Wno-unused-parameter
EXTRA_CFLAGS += -Wno-unused-function
EXTRA_CFLAGS += -Wno-unused

EXTRA_CFLAGS += -Wno-uninitialized

EXTRA_CFLAGS += -Idrivers/net/wireless/rtl8188eus/include
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN

RTL871X = rtl8188e
HCI_NAME = usb
MODULE_NAME = 8188eu
OUTSRC_FILES := hal/OUTSRC/odm_debug.o	\
				hal/OUTSRC/odm_interface.o\
				hal/OUTSRC/odm_HWConfig.o\
				hal/OUTSRC/odm.o\
				hal/OUTSRC/HalPhyRf.o
				
rtk_core :=	core/rtw_cmd.o \
		core/rtw_security.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_ioctl_query.o \
		core/rtw_ioctl_set.o \
		core/rtw_ieee80211.o \
		core/rtw_mlme.o \
		core/rtw_mlme_ext.o \
		core/rtw_wlan_util.o \
		core/rtw_pwrctrl.o \
		core/rtw_rf.o \
		core/rtw_recv.o \
		core/rtw_sta_mgt.o \
		core/rtw_xmit.o	\
		core/rtw_p2p.o \
		core/rtw_br_ext.o \
		core/rtw_iol.o
HAL_COMM_FILES := hal/$(RTL871X)/$(RTL871X)_xmit.o\
									hal/$(RTL871X)/$(RTL871X)_sreset.o
OUTSRC_FILES += hal/OUTSRC/$(RTL871X)/HalHWImg8188E_MAC.o\
		hal/OUTSRC/$(RTL871X)/HalHWImg8188E_BB.o\
		hal/OUTSRC/$(RTL871X)/HalHWImg8188E_RF.o\
		hal/OUTSRC/$(RTL871X)/Hal8188EFWImg_CE.o\
		hal/OUTSRC/$(RTL871X)/HalPhyRf_8188e.o\
		hal/OUTSRC/$(RTL871X)/odm_RegConfig8188E.o\
		hal/OUTSRC/$(RTL871X)/Hal8188ERateAdaptive.o\
		hal/OUTSRC/$(RTL871X)/odm_RTL8188E.o
_OS_INTFS_FILES :=	os_dep/osdep_service.o \
			os_dep/linux/os_intfs.o \
			os_dep/linux/$(HCI_NAME)_intf.o \
			os_dep/linux/ioctl_linux.o \
			os_dep/linux/xmit_linux.o \
			os_dep/linux/mlme_linux.o \
			os_dep/linux/recv_linux.o \
			os_dep/linux/ioctl_cfg80211.o \
			os_dep/linux/rtw_android.o
PWRSEQ_FILES := hal/HalPwrSeqCmd.o \
				hal/$(RTL871X)/Hal8188EPwrSeq.o
_HAL_INTFS_FILES :=	hal/hal_init.o \
			hal/hal_com.o \
			hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o

OBJ := core/efuse/rtw_efuse.o

$(MODULE_NAME)-y += $(rtk_core)

$(MODULE_NAME)-y += $(_OS_INTFS_FILES)

$(MODULE_NAME)-y += $(HAL_COMM_FILES)

$(MODULE_NAME)-y += $(OUTSRC_FILES)

$(MODULE_NAME)-y += $(PWRSEQ_FILES)

$(MODULE_NAME)-y += $(OBJ)

$(MODULE_NAME)-y += $(_HAL_INTFS_FILES)

obj-$(CONFIG_RTL8188EUS) := $(MODULE_NAME).o

	
##CHIP_FILES += $(rtk_core) $(_OS_INTFS_FILES) $(HAL_COMM_FILES) $(OUTSRC_FILES) $(PWRSEQ_FILES) $(_HAL_INTFS_FILES) $(OBJ)

##obj-$(CONFIG_RTL8188EUS) += $(CHIP_FILES)
