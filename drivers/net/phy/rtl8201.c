/* ============================================================================
 * This is a driver for the RTL8201 PHY controller.
 *  (based on STE10XP driver)
 *
 *	soon@dgstation.co.kr
 *
 * ----------------------------------------------------------------------------
 * Changelog:
 *	first release for cuberevo
 *	second release for opticum/orton 9600 HD
 *
 * ===========================================================================*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/phy.h>

#undef PHYDEBUG
#define DEFAULT_PHY_ID       0
#define RESOURCE_NAME        "rtl8201"

#define MII_XCIIS            0x11   /* Configuration Info IRQ & Status Reg*/
#define MII_XIE              0x12   /* Interrupt Enable Register*/
#define MII_XIE_DEFAULT_MASK 0x0070 /* ANE complete, Remote Fault, Link Down */

/* RTL8201 phy identifier values */
#define RTL8201_PHY_ID		 0x00008201  // 0x00061c51

static int rtl8201_config_init(struct phy_device *phydev)
{
	int value, err;

	printk("%s >\n", __func__);

	/* Software Reset PHY */
	value = phy_read(phydev, MII_BMCR);
	if (value < 0)
	{
		return value;
	}

	value |= BMCR_RESET;
	err = phy_write(phydev, MII_BMCR, value);
	if (err < 0)
	{
		return err;
	}

	do
	{
		value = phy_read(phydev, MII_BMCR);
	} while (value & BMCR_RESET);

	return 0;
}

/* RTL8201 don't have interrupt */
static int rtl8201_config_intr(struct phy_device *phydev)
{
	int err, value;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
	{
		/* Enable all STe101P interrupts (PR12) */
		err = phy_write(phydev, MII_XIE, MII_XIE_DEFAULT_MASK);
		/* clear any pending interrupts */
		if (err == 0)
		{
			value = phy_read(phydev, MII_XCIIS);
			if (value < 0)
			{
				err = value;
			}
		}
	}
	else
	{
		err = phy_write(phydev, MII_XIE, 0);
	}
	return err;
}

static int rtl8201_ack_interrupt(struct phy_device *phydev)
{
	int err = phy_read(phydev, MII_XCIIS);

	if (err < 0)
	{
		return err;
	}

	return 0;
}

static struct phy_driver rtl8201_pdriver =
{
	.phy_id         = RTL8201_PHY_ID,
	.phy_id_mask    = 0xfffffffe,
	.name           = "RTL8201",
	.features       = PHY_BASIC_FEATURES,
	.flags          = 0,  // PHY_HAS_INTERRUPT,
	.config_init    = rtl8201_config_init,
	.config_aneg    = genphy_config_aneg,
	.read_status    = genphy_read_status,
	.ack_interrupt  = rtl8201_ack_interrupt,
	.config_intr    = rtl8201_config_intr,
	.suspend        = genphy_suspend,
	.resume         = genphy_resume,
	.driver         =
	{
		.owner      = THIS_MODULE
	}
};

static int __init rtl8201_init(void)
{
	int retval;

	return phy_driver_register(&rtl8201_pdriver);
}

static void __exit rtl8201_exit(void)
{
	phy_driver_unregister(&rtl8201_pdriver);
}

module_init(rtl8201_init);
module_exit(rtl8201_exit);

MODULE_DESCRIPTION("RTL8201 PHY driver");
MODULE_LICENSE("GPL");
//vim:ts=4
