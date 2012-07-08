/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include <linux/module.h>
#include <mach/irqs.h>
#include <asm/mach-types.h>
#include "gpiomux.h"
#include "gpiomux-tenderloin.h"

#define GPIOMUX_CFG(f, d, p) {.func = f, .drv = d, .pull = p}
#define GPIOMUX_DCFG(f, d, p, r) {.func = f, .drv = d, .pull = p, .dir = r}
#define GPIOMUX_SUSP_DEF(pin, suspend)\
	{ .gpio = pin, .settings = {\
			[GPIOMUX_SUSPENDED] = suspend, },\
	}
#define GPIOMUX_ACT_SUSP_DEF(pin, active, suspend)\
	{ .gpio = pin, .settings = {\
			[GPIOMUX_ACTIVE] = active,\
			[GPIOMUX_SUSPENDED] = suspend, },\
	}
#define GPIO_IN_2M_PU GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP)
#define GPIO_IN_2M_PD GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_DOWN)
#define GPIO_IN_2M_PN GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define GPIO_IN_8M_PU GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define GPIO_IN_8M_PD GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_DOWN)
#define GPIO_IN_8M_PN GPIOMUX_CFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE)

#define GPIO_OUTL_2M_PN GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO,GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define GPIO_OUTL_8M_PN GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)

#define GPIO_OUTH_2M_PN GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_8M_PN GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_2M_PU GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_8M_PU GPIOMUX_DCFG(\
		GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP, GPIOMUX_OUT_HIGH)


#define GPIO_IN_2M_PK GPIOMUX_CFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_KEEPER)
#define GPIO_IN_8M_PK GPIOMUX_CFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_KEEPER)

#define GPIO_OUTL_2M_PK GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_KEEPER, GPIOMUX_OUT_LOW)
#define GPIO_OUTL_8M_PK GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_KEEPER, GPIOMUX_OUT_LOW)
#define GPIO_OUTH_2M_PK GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_KEEPER, GPIOMUX_OUT_HIGH)
#define GPIO_OUTH_8M_PK GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, GPIOMUX_PULL_KEEPER, GPIOMUX_OUT_HIGH)


#define CONSOLE_UART GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_8MA, 0)

#define GSBI1 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI3 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI4 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, 0)
#define GSBI7 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_12MA, 0)
#define GSBI8 GPIOMUX_CFG(GPIOMUX_FUNC_1, 0, 0)

#define PS_HOLD	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_12MA, 0)

#define USB_SWITCH_EN_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define USB_SWITCH_CNTL_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define USB_HUB_RESET_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define USB_SWITCH_EN_SUSP_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, GPIOMUX_PULL_DOWN)
#define USB_SWITCH_CNTL_SUSP_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, GPIOMUX_PULL_DOWN)
#define USB_HUB_RESET_SUSP_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, GPIOMUX_PULL_DOWN)

#define MSM_SNDDEV_ACTIVE_CONFIG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MSM_SNDDEV_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define WLAN_PWDN_N_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP)
#define WLAN_PWDN_N_SUSP_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, GPIOMUX_PULL_DOWN)

#define EBI2_A_D GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_OE  GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_WE	 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS2 GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS3 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_CS4 GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)
#define EBI2_ADV GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_UP)

#define USB_ISP1763_ACTV_CFG GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_8MA, \
					 GPIOMUX_PULL_NONE)
#define USB_ISP1763_SUSP_CFG GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, 0, \
					 GPIOMUX_PULL_DOWN)

#define SDCC1_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC1_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC1_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC1_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_UP)

#define SDCC2_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC2_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC2_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC2_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define SDCC5_DAT_0_3_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC5_DAT_4_7_CMD_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_10MA, GPIOMUX_PULL_UP)
#define SDCC5_CLK_ACTV_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_2, GPIOMUX_DRV_16MA, GPIOMUX_PULL_NONE)
#define SDCC5_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define AUX_PCM_ACTIVE_CONFIG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define AUX_PCM_SUSPEND_CONFIG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#define UART1DM_ACTIVE \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE)
#define UART1DM_OUT_ACTIVE GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)
#define UART1DM_SUSPENDED_OUT_HIGH \
	GPIOMUX_DCFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define UART1DM_SUSPENDED_IN_HIGH \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_UP)

#define MI2S_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MI2S_SUSPEND_CFG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define LCDC_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_14MA, GPIOMUX_PULL_NONE)
#define LCDC_ACTIVE_CFG_2M \
	GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)

#define LCDC_SUSPEND_CFG GPIO_IN_2M_PD

#define HDMI_SUSPEND_CFG GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN)

#define MDM2AP_STATUS_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MDM2AP_STATUS_SUSPEND_CFG \
	GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#define CAM_F1_OUTH_8M_PN	GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)
#define CAM_F1_OUTL_8M_PN	GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)

#define MDM2AP_SYNC_ACTIVE_CFG \
	GPIOMUX_CFG(GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE)
#define MDM2AP_SYNC_SUSPEND_CFG \
	GPIOMUX_CFG(0, 0, GPIOMUX_PULL_NONE)

#define BT_CFG_IN GPIO_IN_2M_PN

#define BT_CFG_OUT GPIOMUX_DCFG(\
	GPIOMUX_FUNC_GPIO, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_LOW)

static struct msm_gpiomux_config msm8x60_gsbi_configs[] __initdata = {
#if 0
	{
		.gpio      = 33,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI1,
		},
	},
	{
		.gpio      = 34,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI1,
		},
	},
	{
		.gpio      = 35,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI1,
		},
	},
	{
		.gpio      = 36,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI1,
		},
	},
#endif
	{
		.gpio      = 43,
		.settings = {
			[GPIOMUX_ACTIVE] = GSBI3,
			[GPIOMUX_SUSPENDED] = GSBI3,
		},
	},
	{
		.gpio      = 44,
		.settings = {
			[GPIOMUX_ACTIVE] = GSBI3,
			[GPIOMUX_SUSPENDED] = GSBI3,
		},
	},
#if 0
	{
		.gpio      = 47,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI4,
		},
	},
	{
		.gpio      = 48,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI4,
		},
	},
	{
		.gpio      = 59,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI7,
		},
	},
	{
		.gpio      = 60,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI7,
		},
	},
	{
		.gpio      = 64,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI8,
		},
	},
	{
		.gpio      = 65,
		.settings = {
			[GPIOMUX_SUSPENDED] = GSBI8,
		},
	},
#endif
};

static struct msm_gpiomux_config msm8x60_ebi2_configs[] __initdata = {
	{
		.gpio      = 40,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_CS2,
		},
	},
	{
		.gpio      = 92,
		.settings = {
			[GPIOMUX_SUSPENDED] = PS_HOLD,
		},
	},
	{
		.gpio      = 123,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 124,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 125,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 127,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 128,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 129,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 130,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
#ifdef CONFIG_USB_PEHCI_HCD
	/* ISP VDD_3V3_EN */
	{
		.gpio      = 132,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_CS4,
		},
	},
#endif
	{
		.gpio      = 133,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_CS3,
		},
	},
	{
		.gpio      = 135,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 136,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 137,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 138,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 139,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 140,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 141,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 142,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 143,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 144,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 145,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 146,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 147,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 148,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 149,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 150,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_A_D,
		},
	},
	{
		.gpio      = 151,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_OE,
		},
	},
	{
		.gpio      = 153,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_ADV,
		},
	},
	{
		.gpio      = 157,
		.settings = {
			[GPIOMUX_SUSPENDED] = EBI2_WE,
		},
	},
};

#ifdef CONFIG_USB_PEHCI_HCD
static struct msm_gpiomux_config msm8x60_isp_usb_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(ISP1763_INT_GPIO, GPIO_IN_2M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_DACK_GPIO, GPIO_OUTH_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_DREQ_GPIO, GPIO_IN_2M_PD, GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(ISP1763_RST_GPIO, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_3V3_EN, GPIO_OUTL_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_DISABLE_N, GPIO_OUTH_2M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_WAKE_N, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_3G_UIM_CD_N, GPIO_IN_2M_PU, GPIO_IN_2M_PU),
};
#endif

static struct msm_gpiomux_config msm8x60_uart_configs[] __initdata = {
	{ /* UARTDM_TX */
		.gpio      = 53,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_OUT_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_OUT_HIGH,
		},
	},
	{ /* UARTDM_RX */
		.gpio      = 54,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_IN_HIGH,
		},
	},
	{ /* UARTDM_CTS */
		.gpio      = 55,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_IN_HIGH,
		},
	},
	{ /* UARTDM_RFR */
		.gpio      = 56,
		.settings = {
			[GPIOMUX_ACTIVE]    = UART1DM_OUT_ACTIVE,
			[GPIOMUX_SUSPENDED] = UART1DM_SUSPENDED_OUT_HIGH,
		},
	},
	{ /* DEBUG_RX */
		.gpio      = 117,
		.settings = {
			[GPIOMUX_SUSPENDED] = CONSOLE_UART,
		},
	},
	{ /* DEBUG_TX */
		.gpio      = 118,
		.settings = {
			[GPIOMUX_SUSPENDED] = CONSOLE_UART,
		},
	},
};

static struct msm_gpiomux_config msm8x60_ts_configs[] __initdata = {
	{
		/* TS_ATTN */
		.gpio = 58,
		.settings = {
			[GPIOMUX_SUSPENDED] =
				GPIOMUX_CFG(0, 0, GPIOMUX_PULL_DOWN),
		},
	},
};

static struct msm_gpiomux_config msm8x60_tmg200_configs[] __initdata = {
	{
		.gpio = 61,
		.settings = {
			[GPIOMUX_ACTIVE]    = GPIOMUX_CFG(GPIOMUX_FUNC_GPIO,
				GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE),
			[GPIOMUX_SUSPENDED] = GPIOMUX_CFG(0, 0,
				GPIOMUX_PULL_NONE),
		},
	},
};

static struct msm_gpiomux_config msm8x60_tma300_configs[] __initdata = {
	{
		.gpio = 61,
		.settings = {
			[GPIOMUX_ACTIVE]    = GPIOMUX_CFG(GPIOMUX_FUNC_GPIO,
				GPIOMUX_DRV_6MA, GPIOMUX_PULL_UP),
			[GPIOMUX_SUSPENDED] = GPIOMUX_CFG(0, 0,
				GPIOMUX_PULL_NONE),
		},
	},
};

static struct msm_gpiomux_config msm8x60_aux_pcm_configs[] __initdata = {
	{
		.gpio = 111,
		.settings = {
			[GPIOMUX_ACTIVE]    = AUX_PCM_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = AUX_PCM_SUSPEND_CONFIG,
		},
	},
	{
		.gpio = 112,
		.settings = {
			[GPIOMUX_ACTIVE]    = AUX_PCM_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = AUX_PCM_SUSPEND_CONFIG,
		},
	},
	{
		.gpio = 113,
		.settings = {
			[GPIOMUX_ACTIVE]    = AUX_PCM_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = AUX_PCM_SUSPEND_CONFIG,
		},
	},
	{
		.gpio = 114,
		.settings = {
			[GPIOMUX_ACTIVE]    = AUX_PCM_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = AUX_PCM_SUSPEND_CONFIG,
		},
	},
};

static struct msm_gpiomux_config msm8x60_sdc_configs[] __initdata = {
	/* SDCC1 data[0] */
	{
		.gpio = 159,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[1] */
	{
		.gpio = 160,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[2] */
	{
		.gpio = 161,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[3] */
	{
		.gpio = 162,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[4] */
	{
		.gpio = 163,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[5] */
	{
		.gpio = 164,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[6] */
	{
		.gpio = 165,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 data[7] */
	{
		.gpio = 166,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 CLK */
	{
		.gpio = 167,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_CLK_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
	/* SDCC1 CMD */
	{
		.gpio = 168,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC1_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC1_SUSPEND_CONFIG,
		},
	},
};

static struct msm_gpiomux_config msm_qrdc_sdc_configs[] __initdata = {
	{
		.gpio      = 118,
		.settings = {
			[GPIOMUX_ACTIVE]    = WLAN_PWDN_N_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = WLAN_PWDN_N_SUSP_CFG,
		},
	},
};


static struct msm_gpiomux_config msm8x60_charm_sdc_configs[] __initdata = {
	/* SDCC5 cmd */
	{
		.gpio = 95,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* SDCC5 data[3]*/
	{
		.gpio = 96,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* SDCC5 clk */
	{
		.gpio = 97,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_CLK_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* SDCC5 data[2]*/
	{
		.gpio = 98,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* SDCC5 data[1]*/
	{
		.gpio = 99,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* SDCC5 data[0]*/
	{
		.gpio = 100,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC5_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC5_SUSPEND_CONFIG,
		},
	},
	/* MDM2AP_SYNC */
	{
		.gpio = 129,
		.settings = {
			[GPIOMUX_ACTIVE]    = MDM2AP_SYNC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MDM2AP_SYNC_SUSPEND_CFG,
		},
	},
	/* SDCC2 data[0] */
	{
		.gpio = 143,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[1] */
	{
		.gpio = 144,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[2] */
	{
		.gpio = 145,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[3] */
	{
		.gpio = 146,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[4] */
	{
		.gpio = 147,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[5] */
	{
		.gpio = 148,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[6] */
	{
		.gpio = 149,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 data[7] */
	{
		.gpio = 150,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_4_7_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
	/* SDCC2 CMD */
	{
		.gpio = 151,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_DAT_0_3_CMD_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},

	/* SDCC2 CLK */
	{
		.gpio = 152,
		.settings = {
			[GPIOMUX_ACTIVE]    = SDCC2_CLK_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = SDCC2_SUSPEND_CONFIG,
		},
	},
};

static struct msm_gpiomux_config msm_qrdc_usb_configs[] __initdata = {
	{
		.gpio      = 34,
		.settings = {
			[GPIOMUX_ACTIVE]    = USB_HUB_RESET_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = USB_HUB_RESET_SUSP_CFG,
		},
	},
	{
		.gpio      = 131,
		.settings = {
			[GPIOMUX_ACTIVE]    = USB_SWITCH_CNTL_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = USB_SWITCH_CNTL_SUSP_CFG,
		},
	},
	{
		.gpio      = 132,
		.settings = {
			[GPIOMUX_ACTIVE]    = USB_SWITCH_EN_ACTV_CFG,
			[GPIOMUX_SUSPENDED] = USB_SWITCH_EN_SUSP_CFG,
		},
	},
};

static struct msm_gpiomux_config msm8x60_snd_configs[] __initdata = {
	{
		.gpio = 108,
		.settings = {
			[GPIOMUX_ACTIVE]    = MSM_SNDDEV_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = MSM_SNDDEV_SUSPEND_CONFIG,
		},
	},
	{
		.gpio = 109,
		.settings = {
			[GPIOMUX_ACTIVE]    = MSM_SNDDEV_ACTIVE_CONFIG,
			[GPIOMUX_SUSPENDED] = MSM_SNDDEV_SUSPEND_CONFIG,
		},
	},
};

static struct msm_gpiomux_config msm8x60_mi2s_configs[] __initdata = {
	/* MI2S WS */
	{
		.gpio = 101,
		.settings = {
			[GPIOMUX_ACTIVE]    = MI2S_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MI2S_SUSPEND_CFG,
		},
	},
	/* MI2S SCLK */
	{
		.gpio = 102,
		.settings = {
			[GPIOMUX_ACTIVE]    = MI2S_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MI2S_SUSPEND_CFG,
		},
	},
	/* MI2S MCLK */
	{
		.gpio = 103,
		.settings = {
			[GPIOMUX_ACTIVE]    = MI2S_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MI2S_SUSPEND_CFG,
		},
	},
	/* MI2S SD3 */
	{
		.gpio = 107,
		.settings = {
			[GPIOMUX_ACTIVE]    = MI2S_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MI2S_SUSPEND_CFG,
		},
	},
};

/* boot configuration: pins are already muxed */
/* TBD: review as gpiomux evolves */
static struct msm_gpiomux_config msm8x60_lcdc_configs[] __initdata = {
	/* lcdc_pclk */
	{
		.gpio = 0,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_hsync */
	{
		.gpio = 1,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_vsync */
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_den */
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red7 */
	{
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_red6 */
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_red5 */
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red4 */
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red3 */
	{
		.gpio = 8,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red2 */
	{
		.gpio = 9,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red1 */
	{
		.gpio = 10,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_red0 */
	{
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn7 */
	{
		.gpio = 12,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_grn6 */
	{
		.gpio = 13,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_grn5 */
	{
		.gpio = 14,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn4 */
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn3 */
	{
		.gpio = 16,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn2 */
	{
		.gpio = 17,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn1 */
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_grn0 */
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu7 */
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_blu6 */
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG_2M,
		},
	},
	/* lcdc_blu5 */
	{
		.gpio = 22,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu4 */
	{
		.gpio = 23,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu3 */
	{
		.gpio = 24,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu2 */
	{
		.gpio = 25,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu1 */
	{
		.gpio = 26,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
	/* lcdc_blu0 */
	{
		.gpio = 27,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_ACTIVE_CFG,
		},
	},
};

/* steady configuration */
/* TBD: review as gpiomux evolves */
static struct msm_gpiomux_config msm8x60_lcdc_steady_configs[] = {
	/* lcdc_pclk */
	{
		.gpio = 0,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_hsync */
	{
		.gpio = 1,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_vsync */
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_den */
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red7 */
	{
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red6 */
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red5 */
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red4 */
	{
		.gpio = 7,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red3 */
	{
		.gpio = 8,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red2 */
	{
		.gpio = 9,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red1 */
	{
		.gpio = 10,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_red0 */
	{
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn7 */
	{
		.gpio = 12,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn6 */
	{
		.gpio = 13,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn5 */
	{
		.gpio = 14,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn4 */
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn3 */
	{
		.gpio = 16,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn2 */
	{
		.gpio = 17,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn1 */
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_grn0 */
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu7 */
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu6 */
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG_2M,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu5 */
	{
		.gpio = 22,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu4 */
	{
		.gpio = 23,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu3 */
	{
		.gpio = 24,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu2 */
	{
		.gpio = 25,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu1 */
	{
		.gpio = 26,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
	/* lcdc_blu0 */
	{
		.gpio = 27,
		.settings = {
			[GPIOMUX_ACTIVE]    = LCDC_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = LCDC_SUSPEND_CFG,
		},
	},
};

/* Because PMIC drivers do not use gpio-management routines and PMIC
 * gpios must never sleep, a "good enough" config is obtained by placing
 * the active config in the 'suspended' slot and leaving the active
 * config invalid: the suspended config will be installed at boot
 * and never replaced.
 */
/*
 * Set the pmic gpio configuration as hw team recommended
 */
static struct msm_gpiomux_config msm8x60_pmic_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(PMIC1_APC_USR_IRQ_N, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(PMIC2_APC_USR_IRQ_N, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};


static struct msm_gpiomux_config msm8x60_common_configs[] __initdata = {
	/* MDM2AP_STATUS */
	{
		.gpio = 77,
		.settings = {
			[GPIOMUX_ACTIVE]    = MDM2AP_STATUS_ACTIVE_CFG,
			[GPIOMUX_SUSPENDED] = MDM2AP_STATUS_SUSPEND_CFG,
		},
	},
};


#ifdef CONFIG_WEBCAM_MT9M113
static struct msm_gpiomux_config msm8x60_cam_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAM_I2C_DATA,  CAM_F1_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAM_I2C_CLK, CAM_F1_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_CAMIF_MCLK,  CAM_F1_OUTL_8M_PN, GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_WEBCAM_RST, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_WEBCAM_PWDN, GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
};
#endif


#define BT_RST_N_ACTIVE_CFG GPIO_OUTH_8M_PN
#define BT_RST_N_SUSPENDED_CFG GPIO_OUTH_2M_PN

#define BT_POWER_ACTIVE_CFG GPIO_OUTL_8M_PN
#define BT_POWER_SUSPENDED_CFG GPIO_IN_2M_PK

#define BT_WAKE_ACTIVE_CFG GPIO_OUTL_8M_PN
#define BT_WAKE_SUSPENDED_CFG GPIO_OUTL_2M_PN

#define BT_HOST_WAKE_ACTIVE_CFG GPIO_IN_8M_PU
#define BT_HOST_WAKE_SUSPENDED_CFG GPIO_IN_8M_PU

static struct msm_gpiomux_config msm8x60_bt_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(BT_RST_N, \
			BT_RST_N_ACTIVE_CFG, BT_RST_N_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_POWER, \
			BT_POWER_ACTIVE_CFG, BT_POWER_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_WAKE, \
			BT_WAKE_ACTIVE_CFG, BT_WAKE_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_HOST_WAKE, \
			BT_HOST_WAKE_ACTIVE_CFG, BT_HOST_WAKE_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_bt_configs_3g[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(BT_RST_N_3G, \
			BT_RST_N_ACTIVE_CFG, BT_RST_N_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_POWER_3G, \
			BT_POWER_ACTIVE_CFG, BT_POWER_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_WAKE_3G, \
			BT_WAKE_ACTIVE_CFG, BT_WAKE_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(BT_HOST_WAKE_3G, \
			BT_HOST_WAKE_ACTIVE_CFG, BT_HOST_WAKE_SUSPENDED_CFG),

};

#ifdef CONFIG_KEYBOARD_GPIO_PE
static struct msm_gpiomux_config msm8x60_kbdgpio_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(VOL_DN_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(VOL_UP_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(CORE_NAVI_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(JACK_DET_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};

static struct msm_gpiomux_config msm8x60_kbdgpio_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(CORE_NAVI_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};
#endif

#ifdef CONFIG_MAX8903B_CHARGER

static struct msm_gpiomux_config msm8x60_charger_cfgs[] __initdata = {
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_CHG_MODE,  GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_MODE, GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_SUS,  GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_1, GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_2, GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_EN,       GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_OK,        GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_STATUS_N,     GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_FAULT_N,      GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
};

static struct msm_gpiomux_config msm8x60_charger_cfgs_3g[] __initdata = {
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_CHG_MODE,  	GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_MODE_3G,	GPIO_OUTH_8M_PN, GPIO_OUTH_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_USB_CHG_SUS,  	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_1, 	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_D_ISET_2, 	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	//GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_CHG_EN,       	GPIO_OUTL_8M_PN, GPIO_OUTL_8M_PN),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_DC_OK_3G,    	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_STATUS_N,     	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
	GPIOMUX_ACT_SUSP_DEF(MAX8903B_GPIO_FAULT_N,      	GPIO_IN_8M_PU,   GPIO_IN_8M_PU),
};
#endif

#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
#define TOUCHSCREEN_GPIOCFG GPIOMUX_DCFG(\
	GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE, GPIOMUX_OUT_HIGH)

static struct msm_gpiomux_config msm8x60_touchscreen_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(72, TOUCHSCREEN_GPIOCFG, GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(73, TOUCHSCREEN_GPIOCFG, GPIO_IN_2M_PN),
};
#endif

static struct msm_gpiomux_config msm8x60_lighting_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_INT_IRQ_GPIO, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_EN_GPIO, GPIO_OUTL_8M_PN, GPIO_OUTL_2M_PN)
};

static struct msm_gpiomux_config msm8x60_lighting_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_INT_IRQ_GPIO_3G, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(LM8502_LIGHTING_EN_GPIO, GPIO_OUTL_8M_PN, GPIO_OUTL_2M_PN)
};


static struct msm_gpiomux_config msm8x60_sensor_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GYRO_INT,  GPIO_IN_8M_PU, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GSENS_INT, GPIO_IN_2M_PN, GPIO_IN_2M_PN),
};

static struct msm_gpiomux_config msm8x60_sensor_cfgs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GYRO_INT_3G, GPIO_IN_8M_PU, GPIO_IN_2M_PU),
};

/* wifi */
static struct msm_gpiomux_config msm8x60_wlan_configs[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WL_HOST_WAKE, \
			GPIO_IN_8M_PD, GPIO_IN_8M_PD),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL, \
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WLAN_RST_N, \
			GPIO_OUTL_8M_PN, GPIO_IN_2M_PK),
};


static struct msm_gpiomux_config msm8x60_wlan_configs_3g[] __initdata = {

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_WL_HOST_WAKE, \
			GPIO_IN_8M_PD, GPIO_IN_8M_PD),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL_3G, \
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_GPIO_HOST_WAKE_WL_3G, \
			GPIO_OUTL_8M_PN, GPIO_IN_2M_PK),

};

static struct msm_gpiomux_config msm8x60_system_gpio_cfgs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_FORCE_BOOT_DIS, GPIO_OUTL_2M_PN, GPIO_OUTL_2M_PN),
};

#ifdef CONFIG_A6

// Currently the suspend configuration for A6 is not used
#define TENDERLOIN_A6_TCK_ACTIVE_CFG GPIO_OUTH_8M_PN
#define TENDERLOIN_A6_TCK_SUSPENDED_CFG GPIO_IN_2M_PU

#define TENDERLOIN_A6_WAKEUP_ACTIVE GPIO_OUTL_8M_PN
#define TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG GPIO_OUTL_2M_PN

#define TENDERLOIN_A6_TDIO_ACTIVE_CFG GPIO_OUTH_8M_PU
#define TENDERLOIN_A6_TDIO_SUSPENDED_CFG GPIO_IN_2M_PU

#define TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG GPIO_IN_8M_PU
#define TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG GPIO_IN_2M_PU

/* a6 */
static struct msm_gpiomux_config msm8x60_a6_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_a6_configs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_3G, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_3G, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

};
static struct msm_gpiomux_config msm8x60_a6_configs_dvt [] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),
};


static struct msm_gpiomux_config msm8x60_a6_configs_3g_dvt [] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TCK_3G_DVT, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_0_MSM_IRQ_3G_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TCK_3G, \
			TENDERLOIN_A6_TCK_ACTIVE_CFG, TENDERLOIN_A6_TCK_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_WAKEUP_3G, \
			TENDERLOIN_A6_WAKEUP_ACTIVE, TENDERLOIN_A6_WAKEUP_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_TDIO_3G, \
			TENDERLOIN_A6_TDIO_ACTIVE_CFG, TENDERLOIN_A6_TDIO_SUSPENDED_CFG),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_A6_1_MSM_IRQ_3G_DVT, \
			TENDERLOIN_A6_MSM_IRQ_ACTIVE_CFG, TENDERLOIN_A6_MSM_IRQ_SUSPENDED_CFG),

};
#endif

#ifdef CONFIG_MSM8X60_AUDIO
static struct msm_gpiomux_config msm8x60_aud_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_AUD_LDO1_EN, \
			GPIO_OUTL_8M_PN, GPIO_OUTH_2M_PN),

	GPIOMUX_ACT_SUSP_DEF(TENDERLOIN_AUD_LDO2_EN, \
			GPIO_OUTL_8M_PN, GPIO_OUTH_2M_PN),
};
#endif

#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
static struct msm_gpiomux_config ctp_configs[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_RX,
		GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE),
		GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CY8CTMA395_XRES,
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_WAKE,
			GPIO_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SCL,
			GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE),
			GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SDA,
			GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE),
			GPIO_IN_2M_PN),
};
static struct msm_gpiomux_config ctp_configs_3g[] __initdata = {
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_RX,
		GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_2MA, GPIOMUX_PULL_NONE),
		GPIO_IN_2M_PD),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CY8CTMA395_XRES,
			GPIO_OUTH_8M_PN, GPIO_OUTH_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_WAKE_3G,
			GPIO_OUTH_8M_PN, GPIO_IN_2M_PU),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SCL,
			GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE),
			GPIO_IN_2M_PN),
	GPIOMUX_ACT_SUSP_DEF(GPIO_CTP_SDA,
			GPIOMUX_CFG(GPIOMUX_FUNC_1, GPIOMUX_DRV_8MA, GPIOMUX_PULL_NONE),
			GPIO_IN_2M_PN),
};
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

/*
 *
 */
struct msm_gpiomux_configs
tenderloin_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs, ARRAY_SIZE(msm8x60_bt_configs)},
	{msm8x60_wlan_configs, ARRAY_SIZE(msm8x60_wlan_configs)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	{msm8x60_sensor_cfgs, ARRAY_SIZE(msm8x60_sensor_cfgs)},
	{msm8x60_lighting_cfgs, ARRAY_SIZE(msm8x60_lighting_cfgs)},
#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs, ARRAY_SIZE(msm8x60_charger_cfgs)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs, ARRAY_SIZE(msm8x60_a6_configs)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif

	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},


#if 0
	{msm8x60_ts_configs, ARRAY_SIZE(msm8x60_ts_configs)},
	{msm8x60_sdc_configs, ARRAY_SIZE(msm8x60_sdc_configs)},
	{msm8x60_snd_configs, ARRAY_SIZE(msm8x60_snd_configs)},
	{msm8x60_mi2s_configs, ARRAY_SIZE(msm8x60_mi2s_configs)},

	{msm8x60_common_configs, ARRAY_SIZE(msm8x60_common_configs)},
	{msm8x60_tmg200_configs, ARRAY_SIZE(msm8x60_tmg200_configs)},
	{msm8x60_charm_sdc_configs, ARRAY_SIZE(msm8x60_charm_sdc_configs)},
#endif
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs, ARRAY_SIZE(ctp_configs)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_3g_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs_3g, ARRAY_SIZE(msm8x60_bt_configs_3g)},
	{msm8x60_wlan_configs_3g, ARRAY_SIZE(msm8x60_wlan_configs_3g)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	{msm8x60_sensor_cfgs_3g, ARRAY_SIZE(msm8x60_sensor_cfgs_3g)},
	{msm8x60_lighting_cfgs_3g, ARRAY_SIZE(msm8x60_lighting_cfgs_3g)},

#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs_3g, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs_3g, ARRAY_SIZE(msm8x60_charger_cfgs_3g)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_3g, ARRAY_SIZE(msm8x60_a6_configs)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
#ifdef CONFIG_USB_PEHCI_HCD
	{msm8x60_isp_usb_configs, ARRAY_SIZE(msm8x60_isp_usb_configs)},
	{msm8x60_ebi2_configs, ARRAY_SIZE(msm8x60_ebi2_configs)},
#endif
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if 0
	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},

	{msm8x60_ts_configs, ARRAY_SIZE(msm8x60_ts_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
	{msm8x60_sdc_configs, ARRAY_SIZE(msm8x60_sdc_configs)},
	{msm8x60_snd_configs, ARRAY_SIZE(msm8x60_snd_configs)},
	{msm8x60_mi2s_configs, ARRAY_SIZE(msm8x60_mi2s_configs)},

	{msm8x60_common_configs, ARRAY_SIZE(msm8x60_common_configs)},
	{msm8x60_tmg200_configs, ARRAY_SIZE(msm8x60_tmg200_configs)},
	{msm8x60_charm_sdc_configs, ARRAY_SIZE(msm8x60_charm_sdc_configs)},
#endif
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs_3g, ARRAY_SIZE(ctp_configs_3g)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_dvt_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs, ARRAY_SIZE(msm8x60_bt_configs)},
	{msm8x60_wlan_configs, ARRAY_SIZE(msm8x60_wlan_configs)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	{msm8x60_sensor_cfgs, ARRAY_SIZE(msm8x60_sensor_cfgs)},
	{msm8x60_lighting_cfgs, ARRAY_SIZE(msm8x60_lighting_cfgs)},
#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs, ARRAY_SIZE(msm8x60_charger_cfgs)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_dvt, ARRAY_SIZE(msm8x60_a6_configs_dvt)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if 0
	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},

	{msm8x60_ts_configs, ARRAY_SIZE(msm8x60_ts_configs)},
	{msm8x60_sdc_configs, ARRAY_SIZE(msm8x60_sdc_configs)},
	{msm8x60_snd_configs, ARRAY_SIZE(msm8x60_snd_configs)},
	{msm8x60_mi2s_configs, ARRAY_SIZE(msm8x60_mi2s_configs)},

	{msm8x60_common_configs, ARRAY_SIZE(msm8x60_common_configs)},
	{msm8x60_tmg200_configs, ARRAY_SIZE(msm8x60_tmg200_configs)},
	{msm8x60_charm_sdc_configs, ARRAY_SIZE(msm8x60_charm_sdc_configs)},
#endif
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs, ARRAY_SIZE(ctp_configs)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */
	{NULL, 0},
};

struct msm_gpiomux_configs
tenderloin_3g_dvt_gpiomux_cfgs[] __initdata = {
	{msm8x60_pmic_configs, ARRAY_SIZE(msm8x60_pmic_configs)},
	{msm8x60_uart_configs, ARRAY_SIZE(msm8x60_uart_configs)},
	{msm8x60_bt_configs_3g, ARRAY_SIZE(msm8x60_bt_configs_3g)},
	{msm8x60_wlan_configs_3g, ARRAY_SIZE(msm8x60_wlan_configs_3g)},
	{msm8x60_lcdc_configs, ARRAY_SIZE(msm8x60_lcdc_configs)},
	{msm8x60_sensor_cfgs_3g, ARRAY_SIZE(msm8x60_sensor_cfgs_3g)},
	{msm8x60_lighting_cfgs_3g, ARRAY_SIZE(msm8x60_lighting_cfgs_3g)},

#ifdef CONFIG_KEYBOARD_GPIO_PE
	{msm8x60_kbdgpio_cfgs_3g, ARRAY_SIZE(msm8x60_kbdgpio_cfgs)},
#endif
#ifdef CONFIG_MAX8903B_CHARGER
	{msm8x60_charger_cfgs_3g, ARRAY_SIZE(msm8x60_charger_cfgs_3g)},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT1386_I2C
	{msm8x60_touchscreen_cfgs, ARRAY_SIZE(msm8x60_touchscreen_cfgs)},
#endif
#ifdef CONFIG_A6
	{msm8x60_a6_configs_3g_dvt, ARRAY_SIZE(msm8x60_a6_configs_3g_dvt)},
#endif
#ifdef CONFIG_WEBCAM_MT9M113
	{msm8x60_cam_configs, ARRAY_SIZE(msm8x60_cam_configs)},
#endif
#ifdef CONFIG_MSM8X60_AUDIO
	{msm8x60_aud_configs, ARRAY_SIZE(msm8x60_aud_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
#endif
#ifdef CONFIG_USB_PEHCI_HCD
	{msm8x60_isp_usb_configs, ARRAY_SIZE(msm8x60_isp_usb_configs)},
	{msm8x60_ebi2_configs, ARRAY_SIZE(msm8x60_ebi2_configs)},
#endif

	{msm8x60_gsbi_configs, ARRAY_SIZE(msm8x60_gsbi_configs)},
	{msm8x60_system_gpio_cfgs, ARRAY_SIZE(msm8x60_system_gpio_cfgs)},

#if 0
	{msm8x60_ts_configs, ARRAY_SIZE(msm8x60_ts_configs)},
	{msm8x60_aux_pcm_configs, ARRAY_SIZE(msm8x60_aux_pcm_configs)},
	{msm8x60_sdc_configs, ARRAY_SIZE(msm8x60_sdc_configs)},
	{msm8x60_snd_configs, ARRAY_SIZE(msm8x60_snd_configs)},
	{msm8x60_mi2s_configs, ARRAY_SIZE(msm8x60_mi2s_configs)},

	{msm8x60_common_configs, ARRAY_SIZE(msm8x60_common_configs)},
	{msm8x60_tmg200_configs, ARRAY_SIZE(msm8x60_tmg200_configs)},
	{msm8x60_charm_sdc_configs, ARRAY_SIZE(msm8x60_charm_sdc_configs)},
#endif
#if defined (CONFIG_TOUCHSCREEN_CY8CTMA395) \
	|| defined (CONFIG_TOUCHSCREEN_CY8CTMA395_MODULE)
	{ctp_configs_3g, ARRAY_SIZE(ctp_configs_3g)},
#endif /* CONFIG_TOUCHSCREEN_CY8CTMA395[_MODULE] */

	{NULL, 0},
};


void __init msm8x60_init_gpiomux(struct msm_gpiomux_configs *cfgs)
{
	int rc;

	rc = msm_gpiomux_init(NR_GPIO_IRQS);
	if (rc) {
		pr_err("%s failure: %d\n", __func__, rc);
		return;
	}

	while (cfgs->cfg) {
		msm_gpiomux_install(cfgs->cfg, cfgs->ncfg);
		++cfgs;
	}
}

void msm8x60_gpiomux_lcdc_steadycfg(void)
{
	msm_gpiomux_install(msm8x60_lcdc_steady_configs,
			ARRAY_SIZE(msm8x60_lcdc_steady_configs));
}
