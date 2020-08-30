/*
 * arch/sh/boards/mach-fx6010/setup.c
 *
 * Modified for Fortis FX6010 board by Audioniek,
 * based on arch/sh/boards/mach-b2067/setup.c
 *
 * Original code:
 * Copyright (C) 2012 STMicroelectronics Limited
 * Author: Stuart Menefy (stuart.menefy@st.com)
 *
 * May be copied or modified under the terms of the GNU General Public
 * License.  See linux/COPYING for more information.
 *
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/stm/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/leds.h>
#if defined(INPUT_ET6226)
#include <linux/et6226.h>
#include <linux/i2c.h>
#endif
#include <linux/mtd/partitions.h>
#include <linux/bpa2.h>
#include <linux/mtd/nand.h>
#include <linux/stm/nand_devices.h>
#include <linux/stm/platform.h>
#include <linux/stm/stxh205.h>
#include <linux/stm/sysconf.h>
#include <asm/irq-ilc.h>

#define FX6010_GPIO_POWER_ON_ETH stm_gpio(3, 3)
#define FX6010_POWER_ON          stm_gpio(3, 7)
#define KEY_SUSPEND              1 //TODO: get correct value

// FX6010 specific changes
#define FX6010_HDMI              stm_gpio(2, 5)
#if defined(INPUT_ET6226)
#define ET6226_I2C_SCL           stm_gpio(2, 6)
#define ET6226_I2C_SDA           stm_gpio(2, 7)
#endif

const char *LMI_IO_partalias[] = { "v4l2-coded-video-buffers", "BPA2_Region1", "v4l2-video-buffers" ,
                                    "coredisplay-video", "gfx-memory", "BPA2_Region0", "LMI_VID", NULL };
 
#if 0  // #if defined(CONFIG_BPA2_DIRECTFBOPTIMIZED)
/*
0x40000000 - 0x403FFFFF - cocpu 1 ram (4mb)
0x40400000 - 0x407FFFFF - cocpu 2 ram (4mb)
0x40800000 - 0x47FFFFFF - linux   (120mb) 
0x47600000 - 0x483FFFFF - bigphys ( 14mb (0x00E00000))
0x48400000 - 0x4FFFFFFF - lmi_io  ( 124mb (0x07C00000))
*/
static struct bpa2_partition_desc bpa2_parts_table[] = {
	{
		.name  = "LMI_IO",
		.start = 0x47600000,
		.size  = 0x07C00000, /* 124 Mb */
		.flags = 0,
		.aka   = LMI_IO_partalias
	}, 
	{
		.name  = "bigphysarea",
		.start = 0x4F200000,
		.size  = 0x00E00000, /* 14 Mb */
		.flags = 0,
		.aka   = NULL
	},
};
#else
/*
0x40000000 - 0x403fffff - companion 0? (4mb) 
0x40400000 - 0x404fffff - companion 1? (4mb) 
0x40800000 - 0x40fd1fff - kernel   (120mb) 
0x40fd2000 - 0x42dd1fff - bigphys ( 30mb)
//0x4A000000 - 0x4FBFFFFF - lmi_io  ( 92mb)
*/
static struct bpa2_partition_desc bpa2_parts_table[] =
{
	{
		.name  = "bigphysarea",
		.start = 0x40fd2000,
		.size  = 0x01e00000,  /* 30 Mb */
		.flags = 0,
		.aka   = NULL
	},
	{
		.name  = "LMI_IO",
		.start = 0x4a000000,
		.size  = 0x20000000,  /* 92 Mb */
		.flags = 0,
		.aka   = LMI_IO_partalias
	},
};
#endif

static void __init fx6010_setup(char **cmdline_p)
{
	printk(KERN_INFO "Fortis FX6010 board initialisation\n");

	stxh205_early_device_init();

	/*
	 * UART10: On board DB9 connector CN701
	 */
	stxh205_configure_asc(STXH205_ASC(10), &(struct stxh205_asc_config)
	{
		.hw_flow_control = 0,
		.is_console = 1
	});

#if defined(CONFIG_LIRC_SUPPORT)
	/*
	 * UART1: For capturing LIRC message at irw, using uart serial loopback mode
	 */
	stxh205_configure_asc(STXH205_ASC(1), &(struct stxh205_asc_config)
	{
		.hw_flow_control = 0,
	});
#endif

	bpa2_init(bpa2_parts_table, ARRAY_SIZE(bpa2_parts_table));
}

#if defined(CONFIG_LEDS_GPIO) \
 || defined(CONFIG_LEDS_GPIO_MODULE)
static struct gpio_led fx6010_led[] =
{
	{
		.name = "LED_GREEN",
		.gpio = stm_gpio(13, 2)
	},
	{
		.name = "LED_LOGO",
		.gpio = stm_gpio(15, 7)
	}
};

static struct gpio_led_platform_data fx6010_led_data =
{
	.num_leds	= ARRAY_SIZE(fx6010_led),
	.leds		= fx6010_led
};

static struct platform_device fx6010_leds =
{
	.name = "leds-gpio",
	.id = -1,
	.dev =
	{
		.platform_data = &fx6010_led_data,
	}
};
#endif

#if defined(INPUT_ET6226)
static struct et6226 key fx6010 front_panel_keys[] =
{
	{ 0x00000001, KEY_UP,    "Channel Up" },
	{ 0x00000002, KEY_DOWN,  "Channel Down" }
};

static struct et6226_character fx6010_front_panel_characters[] =
{
	ET6226_7_SEG_ASCII
};

static struct platform_device fx6010_front_panel =
{
	.name = "et6226",
	.id   = -1,
	.dev.platform_data = &(struct et6226_platform_data)
	{
		.gpio_scl         = ET6226_I2C_SCL,
		.gpio_sda         = ET6226_I2C_SDA,
		.digits           = 4,

		.keys_num         = ARRAY_SIZE(fx6010_front_panel_keys),
		.keys             = fx6010_front_panel_keys,
		.keys_poll_period = DIV_ROUND_UP(HZ, 5),

		.brightness       = 8,
		.characters_num   = ARRAY_SIZE(fx6010_front_panel_characters),
		.characters       = fx6010_front_panel_characters
		.text             = "6010",
	},
};

static struct gpio_keys_button fx6010_fp_gpio_keys_button =
{
	.code = KEY_SUSPEND,
	.gpio = FX6010_POWER_ON,
	.desc = "Standby",
};

static struct platform_device fx6010_fp_gpio_keys =
{
	.name          = "gpio-keys",
	.id            = -1,
	.num_resources = 0,
	.dev           =
	{
		.platform_data = &(struct gpio_keys_platform_data)
		{
			.buttons  = &fx6010_fp_gpio_keys_button,
			.nbuttons = 1,
		}
	}
};
#endif

/* NAND Flash */
/* The FX6010 main board is equipped with a 256 Mbyte AMD S34ML02G1 NAND flash memory */
static struct stm_nand_bank_data fx6010_nand_flash =
{
	.csn		= 0,
	.options	= NAND_NO_AUTOINCR | NAND_USE_FLASH_BBT,
	.nr_partitions	= 9,
	.partitions	= (struct mtd_partition [])
	{
		{
			.name   = "boot",             // mtd0
			.offset = 0x00000000,
			.size   = 0x00100000          //   1 Mbyte
		},
		{
			.name   = "kernel",           // mtd1
			.offset = 0x00400000,         //   4 Mbyte
			.size   = 0x00400000          //   4 Mbyte
		},
		{
			.name   = "rootfs",           // mtd2 (UBI)
			.offset = 0x00800000,         //   8 Mbyte
			.size   = 0x0b600000          // 182 Mbyte
		},
		{
			.name   = "logo",             // mtd3
			.offset = 0x00180000,         // 1.5 Mbyte
			.size   = 0x00200000          //   2 Mbyte
		},
		{
			.name   = "eeprom",           // mtd4
			.offset = 0x00380000,         // 3.5 Mbyte
			.size   = 0x00080000          // 512 kbyte
		},
		{
			.name   = "config",           // mtd5 (UBI), seems to be here for compatibility reasons
			.offset = 0x08000000,         // 128 Mbyte,  as it is somewhere in the middle of rootfs
			.size   = 0x00100000          //   1 Mbyte
		},
		{
			.name   = "user",             // mtd6
			.offset = 0x0be00000,         // 190 Mbyte
			.size   = 0x04000000          //  64 Mbyte
		},
		{
			.name   = "ALL",              // mtd7
			.offset = 0x00000000,         //   0 Mbyte
			.size   = MTDPART_SIZ_FULL    // 256 Mbyte
		},
		{
			.name   = "config1",          // mtd8       This is the config actually used
			.offset = 0x0fe00000,         // 254 Mbyte  by the factory firmware
			.size   = 0x00200000          //   2 Mbyte
		}
	},
	.timing_spec            = &NAND_TSPEC_SPANSION_S34ML02G1
};

static int fx6010_phy_reset(void *bus)
{
	/*
	 * IC+ IP101 datasheet specifies 10mS low period and device usable
	 * 2.5mS after rising edge. However experimentally it appears
	 * 10mS is required for reliable functioning.
	 */
	gpio_set_value(FX6010_GPIO_POWER_ON_ETH, 0);
	mdelay(10);
	gpio_set_value(FX6010_GPIO_POWER_ON_ETH, 1);
	mdelay(10);

	return 1;
}

static struct stmmac_mdio_bus_data stmmac_mdio_bus =
{
	.bus_id         = 0,
	.phy_reset      = fx6010_phy_reset,
	.phy_mask       = 0,
	.probed_phy_irq = ILC_IRQ(25),  /* MDINT */
};

static struct platform_device *fx6010_devices[] __initdata =
{
#if defined(CONFIG_LEDS_GPIO) \
 || defined(CONFIG_LEDS_GPIO_MODULE)
	&fx6010_leds,
#endif
#if defined(INPUT_ET6226)
	&fx6010_keys,
	&fx6010_front_panel,
 	&fx6010_fp_gpio_keys,
#endif
};

static int __init device_init(void)
{
#if 1
	/* Temporary: try and find the FP LED pio's */
#define SLEEP 500
	int i, j;

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 8; j++)
		{

			if (! ((i == 2 && j == 6) || (i == 2 && j == 7) || (i == 3 && j == 1) || (i == 3 && j == 2) || (i == 3 && j == 3) || (i == 3 && j == 7)))
			{
				stm_gpio_direction(stm_gpio(i, j), STM_GPIO_DIRECTION_OUT);
				gpio_set_value(stm_gpio(i, j), 0);
				printk("GPIO(%2d,%1d): ", i, j);
				printk("off ");
				mdelay(SLEEP);
				gpio_set_value(stm_gpio(i, j), 1);
				printk("on ");
				mdelay(SLEEP);
				gpio_set_value(stm_gpio(i, j), 0);
				printk("off ");
				mdelay(SLEEP);
				stm_gpio_direction(stm_gpio(i, j), STM_GPIO_DIRECTION_IN);
				printk("set to input\n");
				mdelay(SLEEP);
				mdelay(SLEEP);
				mdelay(SLEEP);
//				mdelay(SLEEP);
//				mdelay(SLEEP);
//				mdelay(SLEEP);
			}
		}
	}
#endif
	/* The "POWER_ON_ETH" line should be rather called "PHY_RESET",
	 * but it isn't... ;-) */
	gpio_request(FX6010_GPIO_POWER_ON_ETH, "POWER_ON_ETH");
	gpio_direction_output(FX6010_GPIO_POWER_ON_ETH, 0);

	gpio_request(FX6010_POWER_ON, "POWER_ON");
	gpio_direction_output(FX6010_POWER_ON, 0);

	gpio_request(FX6010_HDMI, "HDMI_HPD");  // stm_gpio(2.5)
	gpio_direction_input(FX6010_HDMI);
	
	stxh205_configure_ethernet(&(struct stxh205_ethernet_config)
	{
		.mode          = stxh205_ethernet_mode_mii,
		.ext_clk       = 1,
		.phy_bus       = 0,
		.phy_addr      = -1,
		.mdio_bus_data = &stmmac_mdio_bus,
	});

	/* PHY IRQ has to be triggered LOW */
	set_irq_type(ILC_IRQ(25), IRQ_TYPE_LEVEL_LOW);

#if 1  // SATA?
	stxh205_configure_miphy(&(struct stxh205_miphy_config)
	{
		.mode = SATA_MODE,
		.iface = UPORT_IF,
	});
#endif
	stxh205_configure_usb(0);

	stxh205_configure_usb(1);

	/*
	 * Assignment of i2c pio's (order equals Fortis stock firmware setup)
	 *
	 * SSC1: FE/DEMOD
	 * Becomes /dev/i2c-0
	 * i2c addresses: 0x38 (STV6111 tuner), 0x68 (STV0913 demod)
	 */
	stxh205_configure_ssc_i2c(STXH205_SSC(1), &(struct stxh205_ssc_config)
	{
		.routing.ssc1.sclk = stxh205_ssc1_sclk_pio12_0,
		.routing.ssc1.mtsr = stxh205_ssc1_mtsr_pio12_1
	});
	/*
	 * SSC3: SYS
	 * Becomes /dev/i2c-1
	 * i2c addresses: none detected by i2cdetect
	 */
	stxh205_configure_ssc_i2c(STXH205_SSC(3), &(struct stxh205_ssc_config)
	{
		.routing.ssc3.sclk = stxh205_ssc3_sclk_pio15_5,
		.routing.ssc3.mtsr = stxh205_ssc3_mtsr_pio15_6
	});
	/*
	 * SSC0: HDMI
	 * Becomes /dev/i2c-2
	 * i2c addresses: 0x3a, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57 (NAND emulation EEPROM)
	 */
	stxh205_configure_ssc_i2c(STXH205_SSC(0), &(struct stxh205_ssc_config)
	{  // HDMI PIO SCDL:6.2, SDA:6.3
		.routing.ssc0.sclk = stxh205_ssc0_sclk_pio6_2,
		.routing.ssc0.mtsr = stxh205_ssc0_mtsr_pio6_3
	});
	/*
	 * SSC2: LNBH25PQR LNB power driver
	 * Becomes /dev/i2c-3
	 * i2c address: 0x08
	 */
	stxh205_configure_ssc_i2c(STXH205_SSC(2), &(struct stxh205_ssc_config)
	{ // LNB PIO SCDL:9.4, SDA:9.5

		.routing.ssc2.sclk = stxh205_ssc2_sclk_pio9_4,
		.routing.ssc2.mtsr = stxh205_ssc2_mtsr_pio9_5
	});
#if 1 // defined(INPUT_ET6226)
	/*
	 * SSC5: ET6226 front panel driver
	 * Becomes /dev/i2c-4
	 * i2c addresses: 0x24(CTL), 0x27(KEYS), 0x34(DSP0), 0x35(DSP1), 0x36(DSP2), 0x37(DSP3)
	 */
	stxh205_configure_ssc_i2c(STXH205_SSC(5), &(struct stxh205_ssc_config)
	{  // ET6226 PIO SCDL:2.6, SDA:2.7
//TODO:		.routing.ssc5.sclk = stxh205_ssc5_sclk_pio2_6,
//		.routing.ssc5.mtsr = stxh205_ssc5_mtsr_pio2_7
	});
#endif

#if defined(CONFIG_LIRC_SUPPORT)
	stxh205_configure_lirc(&(struct stxh205_lirc_config)
	{
#if defined(CONFIG_LIRC_STM_UHF)
		.rx_mode       = stxh205_lirc_rx_mode_uhf,
#else
		.rx_mode       = stxh205_lirc_rx_mode_ir,
#endif
		.tx_enabled    = 0,
		.tx_od_enabled = 0,
	});
#endif  // CONFIG_LIRC_SUPPORT

	stxh205_configure_pwm(&(struct stxh205_pwm_config)
	{
		/*
		 * PWM10 is connected to 12V->1.2V power supply
		 * for "debug purposes". Enable at your own risk!
		 */
		.out10_enabled = 0
	});

	stxh205_configure_mmc(&(struct stxh205_mmc_config)
	{
		.emmc                   = 0,
		.no_mmc_boot_data_error = 1
	});

	stxh205_configure_nand(&(struct stm_nand_config)
	{
		.driver             = stm_nand_flex,
		.nr_banks           = 1,
		.banks              = &fx6010_nand_flash,
		.rbn.flex_connected = 1,
//		.bch_ecc_cfg        = BCH_ECC_CFG_NOECC
	});

	return platform_add_devices(fx6010_devices, ARRAY_SIZE(fx6010_devices));
}
arch_initcall(device_init);

static void __iomem *fx6010_ioport_map(unsigned long port, unsigned int size)
{
	/* If we have PCI then this should never be called because we
	 * are using the generic iomap implementation. If we don't
	 * have PCI then there are no IO mapped devices, so it still
	 * shouldn't be called. */
	BUG();
	return NULL;
}

struct sh_machine_vector mv_b2067 __initmv =
{
	.mv_name       = "fx6010",
	.mv_setup      = fx6010_setup,
	.mv_nr_irqs    = NR_IRQS,
	.mv_ioport_map = fx6010_ioport_map,
};

#if defined(CONFIG_HIBERNATION_ON_MEMORY)

#include "../../kernel/cpu/sh4/stm_hom.h"

static int b2067_board_freeze(void)
{
	gpio_set_value(FX6010_GPIO_POWER_ON_ETH, 0);
	return 0;
}

static int b2067_board_defrost(void)
{
	fx6010_phy_reset(NULL);
	return 0;
}

static struct stm_hom_board b2067_hom =
{
	.freeze = b2067_board_freeze,
	.restore = b2067_board_defrost,
};

static int __init b2067_hom_register(void)
{
	return stm_hom_board_register(&b2067_hom);
}

module_init(b2067_hom_register);
#endif
// vim:ts=4
