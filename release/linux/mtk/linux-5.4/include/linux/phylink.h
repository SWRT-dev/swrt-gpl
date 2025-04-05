#ifndef NETDEV_PCS_H
#define NETDEV_PCS_H

#include <linux/phy.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

struct device_node;
struct ethtool_cmd;
struct fwnode_handle;
struct net_device;

enum {
	MLO_PAUSE_NONE,
	MLO_PAUSE_ASYM = BIT(0),
	MLO_PAUSE_SYM = BIT(1),
	MLO_PAUSE_RX = BIT(2),
	MLO_PAUSE_TX = BIT(3),
	MLO_PAUSE_TXRX_MASK = MLO_PAUSE_TX | MLO_PAUSE_RX,
	MLO_PAUSE_AN = BIT(4),

	MLO_AN_PHY = 0,	/* Conventional PHY */
	MLO_AN_FIXED,	/* Fixed-link mode */
	MLO_AN_INBAND,	/* In-band protocol */
};

static inline bool phylink_autoneg_inband(unsigned int mode)
{
	return mode == MLO_AN_INBAND;
}

/**
 * struct phylink_link_state - link state structure
 * @advertising: ethtool bitmask containing advertised link modes
 * @lp_advertising: ethtool bitmask containing link partner advertised link
 *   modes
 * @interface: link &typedef phy_interface_t mode
 * @speed: link speed, one of the SPEED_* constants.
 * @duplex: link duplex mode, one of DUPLEX_* constants.
 * @pause: link pause state, described by MLO_PAUSE_* constants.
 * @rate_matching: rate matching being performed, one of the RATE_MATCH_*
 *   constants. If rate matching is taking place, then the speed/duplex of
 *   the medium link mode (@speed and @duplex) and the speed/duplex of the phy
 *   interface mode (@interface) are different.
 * @link: true if the link is up.
 * @an_enabled: true if autonegotiation is enabled/desired.
 * @an_complete: true if autonegotiation has completed.
 */
struct phylink_link_state {
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	phy_interface_t interface;
	int speed;
	int duplex;
	int pause;
	int rate_matching;
	unsigned int link:1;
	unsigned int an_enabled:1;
	unsigned int an_complete:1;
};

enum phylink_op_type {
	PHYLINK_NETDEV = 0,
	PHYLINK_DEV,
};

/**
 * struct phylink_config - PHYLINK configuration structure
 * @dev: a pointer to a struct device associated with the MAC
 * @type: operation type of PHYLINK instance
 * @poll_fixed_state: if true, starts link_poll,
 *		      if MAC link is at %MLO_AN_FIXED mode.
 */
struct phylink_config {
	struct device *dev;
	enum phylink_op_type type;
	bool poll_fixed_state;
};

/**
 * struct phylink_mac_ops - MAC operations structure.
 * @validate: Validate and update the link configuration.
 * @mac_select_pcs: Select a PCS for the interface mode.
 * @mac_link_state: Read the current link state from the hardware.
 * @mac_prepare: prepare for a major reconfiguration of the interface.
 * @mac_config: configure the MAC for the selected mode and state.
 * @mac_finish: finish a major reconfiguration of the interface.
 * @mac_an_restart: restart 802.3z BaseX autonegotiation.
 * @mac_link_down: take the link down.
 * @mac_link_up: allow the link to come up.
 *
 * The individual methods are described more fully below.
 */
struct phylink_mac_ops {
	void (*validate)(struct phylink_config *config,
			 unsigned long *supported,
			 struct phylink_link_state *state);
	struct phylink_pcs *(*mac_select_pcs)(struct phylink_config *config,
					      phy_interface_t interface);
	int (*mac_link_state)(struct phylink_config *config,
			      struct phylink_link_state *state);
	int (*mac_prepare)(struct phylink_config *config, unsigned int mode,
			   phy_interface_t iface);
	void (*mac_config)(struct phylink_config *config, unsigned int mode,
			   const struct phylink_link_state *state);
	int (*mac_finish)(struct phylink_config *config, unsigned int mode,
			  phy_interface_t iface);
	void (*mac_an_restart)(struct phylink_config *config);
	void (*mac_link_down)(struct phylink_config *config, unsigned int mode,
			      phy_interface_t interface);
	void (*mac_link_up)(struct phylink_config *config,
			    struct phy_device *phy, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex,
			    bool tx_pause, bool rx_pause);
};

#if 0 /* For kernel-doc purposes only. */
/**
 * validate - Validate and update the link configuration
 * @config: a pointer to a &struct phylink_config.
 * @supported: ethtool bitmask for supported link modes.
 * @state: a pointer to a &struct phylink_link_state.
 *
 * Clear bits in the @supported and @state->advertising masks that
 * are not supportable by the MAC.
 *
 * Note that the PHY may be able to transform from one connection
 * technology to another, so, eg, don't clear 1000BaseX just
 * because the MAC is unable to BaseX mode. This is more about
 * clearing unsupported speeds and duplex settings. The port modes
 * should not be cleared; phylink_set_port_modes() will help with this.
 *
 * If the @state->interface mode is %PHY_INTERFACE_MODE_1000BASEX
 * or %PHY_INTERFACE_MODE_2500BASEX, select the appropriate mode
 * based on @state->advertising and/or @state->speed and update
 * @state->interface accordingly. See phylink_helper_basex_speed().
 *
 * When @state->interface is %PHY_INTERFACE_MODE_NA, phylink expects the
 * MAC driver to return all supported link modes.
 *
 * If the @state->interface mode is not supported, then the @supported
 * mask must be cleared.
 */
void validate(struct phylink_config *config, unsigned long *supported,
	      struct phylink_link_state *state);
/**
 * mac_select_pcs: Select a PCS for the interface mode.
 * @config: a pointer to a &struct phylink_config.
 * @interface: PHY interface mode for PCS
 *
 * Return the &struct phylink_pcs for the specified interface mode, or
 * NULL if none is required, or an error pointer on error.
 *
 * This must not modify any state. It is used to query which PCS should
 * be used. Phylink will use this during validation to ensure that the
 * configuration is valid, and when setting a configuration to internally
 * set the PCS that will be used.
 */
struct phylink_pcs *mac_select_pcs(struct phylink_config *config,
				   phy_interface_t interface);

/**
 * mac_link_state() - Read the current link state from the hardware
 * @config: a pointer to a &struct phylink_config.
 * @state: a pointer to a &struct phylink_link_state.
 *
 * Read the current link state from the MAC, reporting the current
 * speed in @state->speed, duplex mode in @state->duplex, pause mode
 * in @state->pause using the %MLO_PAUSE_RX and %MLO_PAUSE_TX bits,
 * negotiation completion state in @state->an_complete, and link
 * up state in @state->link.
 */
int mac_link_state(struct phylink_config *config,
		   struct phylink_link_state *state);

/**
 * mac_prepare() - prepare to change the PHY interface mode
 * @config: a pointer to a &struct phylink_config.
 * @mode: one of %MLO_AN_FIXED, %MLO_AN_PHY, %MLO_AN_INBAND.
 * @iface: interface mode to switch to
 *
 * phylink will call this method at the beginning of a full initialisation
 * of the link, which includes changing the interface mode or at initial
 * startup time. It may be called for the current mode. The MAC driver
 * should perform whatever actions are required, e.g. disabling the
 * Serdes PHY.
 *
 * This will be the first call in the sequence:
 * - mac_prepare()
 * - mac_config()
 * - pcs_config()
 * - possible pcs_an_restart()
 * - mac_finish()
 *
 * Returns zero on success, or negative errno on failure which will be
 * reported to the kernel log.
 */
int mac_prepare(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);

/**
 * mac_config() - configure the MAC for the selected mode and state
 * @config: a pointer to a &struct phylink_config.
 * @mode: one of %MLO_AN_FIXED, %MLO_AN_PHY, %MLO_AN_INBAND.
 * @state: a pointer to a &struct phylink_link_state.
 *
 * Note - not all members of @state are valid.  In particular,
 * @state->lp_advertising, @state->link, @state->an_complete are never
 * guaranteed to be correct, and so any mac_config() implementation must
 * never reference these fields.
 *
 * The action performed depends on the currently selected mode:
 *
 * %MLO_AN_FIXED, %MLO_AN_PHY:
 *   Configure the specified @state->speed, @state->duplex and
 *   @state->pause (%MLO_PAUSE_TX / %MLO_PAUSE_RX) modes over a link
 *   specified by @state->interface.  @state->advertising may be used,
 *   but is not required.  Other members of @state must be ignored.
 *
 *   Valid state members: interface, speed, duplex, pause, advertising.
 *
 * %MLO_AN_INBAND:
 *   place the link in an inband negotiation mode (such as 802.3z
 *   1000base-X or Cisco SGMII mode depending on the @state->interface
 *   mode). In both cases, link state management (whether the link
 *   is up or not) is performed by the MAC, and reported via the
 *   mac_link_state() callback. Changes in link state must be made
 *   by calling phylink_mac_change().
 *
 *   If in 802.3z mode, the link speed is fixed, dependent on the
 *   @state->interface. Duplex is negotiated, and pause is advertised
 *   according to @state->an_enabled, @state->pause and
 *   @state->advertising flags. Beware of MACs which only support full
 *   duplex at gigabit and higher speeds.
 *
 *   If in Cisco SGMII mode, the link speed and duplex mode are passed
 *   in the serial bitstream 16-bit configuration word, and the MAC
 *   should be configured to read these bits and acknowledge the
 *   configuration word. Nothing is advertised by the MAC. The MAC is
 *   responsible for reading the configuration word and configuring
 *   itself accordingly.
 *
 *   Valid state members: interface, an_enabled, pause, advertising.
 *
 * Implementations are expected to update the MAC to reflect the
 * requested settings - i.o.w., if nothing has changed between two
 * calls, no action is expected.  If only flow control settings have
 * changed, flow control should be updated *without* taking the link
 * down.  This "update" behaviour is critical to avoid bouncing the
 * link up status.
 */
void mac_config(struct phylink_config *config, unsigned int mode,
		const struct phylink_link_state *state);

/**
 * mac_finish() - finish a to change the PHY interface mode
 * @config: a pointer to a &struct phylink_config.
 * @mode: one of %MLO_AN_FIXED, %MLO_AN_PHY, %MLO_AN_INBAND.
 * @iface: interface mode to switch to
 *
 * phylink will call this if it called mac_prepare() to allow the MAC to
 * complete any necessary steps after the MAC and PCS have been configured
 * for the @mode and @iface. E.g. a MAC driver may wish to re-enable the
 * Serdes PHY here if it was previously disabled by mac_prepare().
 *
 * Returns zero on success, or negative errno on failure which will be
 * reported to the kernel log.
 */
int mac_finish(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);

/**
 * mac_an_restart() - restart 802.3z BaseX autonegotiation
 * @config: a pointer to a &struct phylink_config.
 */
void mac_an_restart(struct phylink_config *config);

/**
 * mac_link_down() - take the link down
 * @config: a pointer to a &struct phylink_config.
 * @mode: link autonegotiation mode
 * @interface: link &typedef phy_interface_t mode
 *
 * If @mode is not an in-band negotiation mode (as defined by
 * phylink_autoneg_inband()), force the link down and disable any
 * Energy Efficient Ethernet MAC configuration. Interface type
 * selection must be done in mac_config().
 */
void mac_link_down(struct phylink_config *config, unsigned int mode,
		   phy_interface_t interface);

/**
 * mac_link_up() - allow the link to come up
 * @config: a pointer to a &struct phylink_config.
 * @phy: any attached phy
 * @mode: link autonegotiation mode
 * @interface: link &typedef phy_interface_t mode
 * @speed: link speed
 * @duplex: link duplex
 * @tx_pause: link transmit pause enablement status
 * @rx_pause: link receive pause enablement status
 *
 * Configure the MAC for an established link.
 *
 * @speed, @duplex, @tx_pause and @rx_pause indicate the finalised link
 * settings, and should be used to configure the MAC block appropriately
 * where these settings are not automatically conveyed from the PCS block,
 * or if in-band negotiation (as defined by phylink_autoneg_inband(@mode))
 * is disabled.
 *
 * Note that when 802.3z in-band negotiation is in use, it is possible
 * that the user wishes to override the pause settings, and this should
 * be allowed when considering the implementation of this method.
 *
 * If in-band negotiation mode is disabled, allow the link to come up. If
 * @phy is non-%NULL, configure Energy Efficient Ethernet by calling
 * phy_init_eee() and perform appropriate MAC configuration for EEE.
 * Interface type selection must be done in mac_config().
 */
void mac_link_up(struct phylink_config *config, struct phy_device *phy,
		 unsigned int mode, phy_interface_t interface,
		 int speed, int duplex, bool tx_pause, bool rx_pause);
#endif

struct phylink_pcs_ops;

/**
 * struct phylink_pcs - PHYLINK PCS instance
 * @ops: a pointer to the &struct phylink_pcs_ops structure
 * @poll: poll the PCS for link changes
 *
 * This structure is designed to be embedded within the PCS private data,
 * and will be passed between phylink and the PCS.
 */
struct phylink_pcs {
	const struct phylink_pcs_ops *ops;
	bool poll;
};

/**
 * struct phylink_pcs_ops - MAC PCS operations structure.
 * @pcs_validate: validate the link configuration.
 * @pcs_get_state: read the current MAC PCS link state from the hardware.
 * @pcs_config: configure the MAC PCS for the selected mode and state.
 * @pcs_an_restart: restart 802.3z BaseX autonegotiation.
 * @pcs_link_up: program the PCS for the resolved link configuration
 *               (where necessary).
 */
struct phylink_pcs_ops {
	int (*pcs_validate)(struct phylink_pcs *pcs, unsigned long *supported,
			    const struct phylink_link_state *state);
	int (*pcs_enable)(struct phylink_pcs *pcs);
	void (*pcs_disable)(struct phylink_pcs *pcs);
	void (*pcs_get_state)(struct phylink_pcs *pcs,
			      struct phylink_link_state *state);
	int (*pcs_config)(struct phylink_pcs *pcs, unsigned int mode,
			  phy_interface_t interface,
			  const unsigned long *advertising,
			  bool permit_pause_to_mac);
	void (*pcs_an_restart)(struct phylink_pcs *pcs);
	void (*pcs_link_up)(struct phylink_pcs *pcs, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex);
};

#if 0 /* For kernel-doc purposes only. */
/**
 * pcs_validate() - validate the link configuration.
 * @pcs: a pointer to a &struct phylink_pcs.
 * @supported: ethtool bitmask for supported link modes.
 * @state: a const pointer to a &struct phylink_link_state.
 *
 * Validate the interface mode, and advertising's autoneg bit, removing any
 * media ethtool link modes that would not be supportable from the supported
 * mask. Phylink will propagate the changes to the advertising mask. See the
 * &struct phylink_mac_ops validate() method.
 *
 * Returns -EINVAL if the interface mode/autoneg mode is not supported.
 * Returns non-zero positive if the link state can be supported.
 */
int pcs_validate(struct phylink_pcs *pcs, unsigned long *supported,
		 const struct phylink_link_state *state);

/**
 * pcs_get_state() - Read the current inband link state from the hardware
 * @pcs: a pointer to a &struct phylink_pcs.
 * @state: a pointer to a &struct phylink_link_state.
 *
 * Read the current inband link state from the MAC PCS, reporting the
 * current speed in @state->speed, duplex mode in @state->duplex, pause
 * mode in @state->pause using the %MLO_PAUSE_RX and %MLO_PAUSE_TX bits,
 * negotiation completion state in @state->an_complete, and link up state
 * in @state->link. If possible, @state->lp_advertising should also be
 * populated.
 *
 * When present, this overrides mac_pcs_get_state() in &struct
 * phylink_mac_ops.
 */
void pcs_get_state(struct phylink_pcs *pcs,
		   struct phylink_link_state *state);

/**
 * pcs_config() - Configure the PCS mode and advertisement
 * @pcs: a pointer to a &struct phylink_pcs.
 * @mode: one of %MLO_AN_FIXED, %MLO_AN_PHY, %MLO_AN_INBAND.
 * @interface: interface mode to be used
 * @advertising: adertisement ethtool link mode mask
 * @permit_pause_to_mac: permit forwarding pause resolution to MAC
 *
 * Configure the PCS for the operating mode, the interface mode, and set
 * the advertisement mask. @permit_pause_to_mac indicates whether the
 * hardware may forward the pause mode resolution to the MAC.
 *
 * When operating in %MLO_AN_INBAND, inband should always be enabled,
 * otherwise inband should be disabled.
 *
 * For SGMII, there is no advertisement from the MAC side, the PCS should
 * be programmed to acknowledge the inband word from the PHY.
 *
 * For 1000BASE-X, the advertisement should be programmed into the PCS.
 *
 * For most 10GBASE-R, there is no advertisement.
 */
int pcs_config(struct phylink_pcs *pcs, unsigned int mode,
	       phy_interface_t interface, const unsigned long *advertising,
	       bool permit_pause_to_mac);

/**
 * pcs_an_restart() - restart 802.3z BaseX autonegotiation
 * @pcs: a pointer to a &struct phylink_pcs.
 *
 * When PCS ops are present, this overrides mac_an_restart() in &struct
 * phylink_mac_ops.
 */
void pcs_an_restart(struct phylink_pcs *pcs);

/**
 * pcs_link_up() - program the PCS for the resolved link configuration
 * @pcs: a pointer to a &struct phylink_pcs.
 * @mode: link autonegotiation mode
 * @interface: link &typedef phy_interface_t mode
 * @speed: link speed
 * @duplex: link duplex
 *
 * This call will be made just before mac_link_up() to inform the PCS of
 * the resolved link parameters. For example, a PCS operating in SGMII
 * mode without in-band AN needs to be manually configured for the link
 * and duplex setting. Otherwise, this should be a no-op.
 */
void pcs_link_up(struct phylink_pcs *pcs, unsigned int mode,
		 phy_interface_t interface, int speed, int duplex);
#endif

struct phylink *phylink_create(struct phylink_config *, struct fwnode_handle *,
			       phy_interface_t iface,
			       const struct phylink_mac_ops *ops);
void phylink_destroy(struct phylink *);

int phylink_connect_phy(struct phylink *, struct phy_device *);
int phylink_of_phy_connect(struct phylink *, struct device_node *, u32 flags);
void phylink_disconnect_phy(struct phylink *);
int phylink_fixed_state_cb(struct phylink *,
			   void (*cb)(struct net_device *dev,
				      struct phylink_link_state *));

void phylink_mac_change(struct phylink *, bool up);

void phylink_start(struct phylink *);
void phylink_stop(struct phylink *);

void phylink_ethtool_get_wol(struct phylink *, struct ethtool_wolinfo *);
int phylink_ethtool_set_wol(struct phylink *, struct ethtool_wolinfo *);

int phylink_ethtool_ksettings_get(struct phylink *,
				  struct ethtool_link_ksettings *);
int phylink_ethtool_ksettings_set(struct phylink *,
				  const struct ethtool_link_ksettings *);
int phylink_ethtool_nway_reset(struct phylink *);
void phylink_ethtool_get_pauseparam(struct phylink *,
				    struct ethtool_pauseparam *);
int phylink_ethtool_set_pauseparam(struct phylink *,
				   struct ethtool_pauseparam *);
int phylink_get_eee_err(struct phylink *);
int phylink_init_eee(struct phylink *, bool);
int phylink_ethtool_get_eee(struct phylink *, struct ethtool_eee *);
int phylink_ethtool_set_eee(struct phylink *, struct ethtool_eee *);
int phylink_mii_ioctl(struct phylink *, struct ifreq *, int);

#define phylink_zero(bm) \
	bitmap_zero(bm, __ETHTOOL_LINK_MODE_MASK_NBITS)
#define __phylink_do_bit(op, bm, mode) \
	op(ETHTOOL_LINK_MODE_ ## mode ## _BIT, bm)

#define phylink_set(bm, mode)	__phylink_do_bit(__set_bit, bm, mode)
#define phylink_clear(bm, mode)	__phylink_do_bit(__clear_bit, bm, mode)
#define phylink_test(bm, mode)	__phylink_do_bit(test_bit, bm, mode)

void phylink_set_port_modes(unsigned long *bits);
void phylink_helper_basex_speed(struct phylink_link_state *state);

/**
 * phylink_get_link_timer_ns - return the PCS link timer value
 * @interface: link &typedef phy_interface_t mode
 *
 * Return the PCS link timer setting in nanoseconds for the PHY @interface
 * mode, or -EINVAL if not appropriate.
 */
static inline int phylink_get_link_timer_ns(phy_interface_t interface)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_USXGMII:
		return 1600000;

	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		return 10000000;

	default:
		return -EINVAL;
	}
}

void phylink_mii_c22_pcs_decode_state(struct phylink_link_state *state,
				      u16 bmsr, u16 lpa);
int phylink_mii_c22_pcs_encode_advertisement(phy_interface_t interface,
					     const unsigned long *advertising);
void phylink_decode_usxgmii_word(struct phylink_link_state *state,
				 uint16_t lpa);
#endif
