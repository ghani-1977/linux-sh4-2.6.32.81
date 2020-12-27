/*
 * drivers/net/phy/t78q2123.c
 *
 * Driver for Teridian 78Q2123/78Q2133 PHY
 *
 * Author: Tomasz Szkutkowski <tomasz.szkutkowski@adp-technologies.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
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

#define T78Q2123_PHY_ID       0x000E7230
#define T78Q2123_PHY_ID_MASK  0xFFFFFFF0
#define T78Q2123_REG_INT      17
#define T78Q2123_REG_INT_MASK 0x0500

static int t78q21x3_config_init(struct phy_device *phydev)
{
	int value, err;

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
	}
	while (value & BMCR_RESET);
	return 0;
}

static int t78q21x3_config_intr(struct phy_device *phydev)
{
	int value = phy_read(phydev, T78Q2123_REG_INT);

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) 
	{
		value |= T78Q2123_REG_INT_MASK;
	}
	else
	{
		value &= ~(T78Q2123_REG_INT_MASK);
	}
	return phy_write(phydev, T78Q2123_REG_INT, value);
}

static int t78q21x3_ack_interrupt(struct phy_device *phydev)
{
	int err = phy_read(phydev, T78Q2123_REG_INT);

	if (err < 0)
	{
		return err;
	}
	return 0;
}


static struct phy_driver t78q21x3_pdriver =
{
	.phy_id        = T78Q2123_PHY_ID,
	.phy_id_mask   = T78Q2123_PHY_ID_MASK,
	.name          = "Teridan 78Q21x3 PHY",
	.features      = PHY_BASIC_FEATURES,
	.flags         = PHY_HAS_INTERRUPT,
	.config_init   = t78q21x3_config_init,
	.config_aneg   = genphy_config_aneg,
	.read_status   = genphy_read_status,
	.ack_interrupt = t78q21x3_ack_interrupt,
	.config_intr   = t78q21x3_config_intr,
	.suspend       = genphy_suspend,
	.resume        = genphy_resume,
	.driver        =
		{
			.owner = THIS_MODULE,
		}
};

static int __init t78q21x3_init(void)
{
	printk("Teridian 78Q21x3 PHY driver init\n");
	return phy_driver_register(&t78q21x3_pdriver);
}

static void __exit t78q21x3_exit(void)
{
	phy_driver_unregister(&t78q21x3_pdriver);
}

module_init(t78q21x3_init);
module_exit(t78q21x3_exit);

MODULE_DESCRIPTION("Teridian 78Q21x3 PHY driver");
MODULE_AUTHOR("ADPT <contact@adp-technologies.com>");
MODULE_LICENSE("GPL");
// vim:ts=4
