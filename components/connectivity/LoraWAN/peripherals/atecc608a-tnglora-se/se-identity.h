/*!
 * \file      se-identity.h
 *
 * \brief     Secure Element identity and keys
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 */
#ifndef __SOFT_SE_IDENTITY_H__
#define __SOFT_SE_IDENTITY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
 ******************************************************************************
 ********************************** WARNING ***********************************
 ******************************************************************************
  The secure-element implementation supports both 1.0.x and 1.1.x LoRaWAN
  versions of the specification.
  Thus it has been decided to use the 1.1.x keys and EUI name definitions.
  The below table shows the names equivalence between versions:
               +---------------------+-------------------------+
               |       1.0.x         |          1.1.x          |
               +=====================+=========================+
               | LORAWAN_DEVICE_EUI  | LORAWAN_DEVICE_EUI      |
               +---------------------+-------------------------+
               | LORAWAN_APP_EUI     | LORAWAN_JOIN_EUI        |
               +---------------------+-------------------------+
               | LORAWAN_GEN_APP_KEY | LORAWAN_APP_KEY         |
               +---------------------+-------------------------+
               | LORAWAN_APP_KEY     | LORAWAN_NWK_KEY         |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_F_NWK_S_INT_KEY |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_S_NWK_S_INT_KEY |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_NWK_S_ENC_KEY   |
               +---------------------+-------------------------+
               | LORAWAN_APP_S_KEY   | LORAWAN_APP_S_KEY       |
               +---------------------+-------------------------+
 ******************************************************************************
 ******************************************************************************
 ******************************************************************************
 */

/*!
 * Secure-element pin
 */
#define SECURE_ELEMENT_PIN     \
    {                          \
        0x00, 0x00, 0x00, 0x00 \
    }

/*!
 * When set to 1 DevAdd is LORAWAN_DEVICE_ADDRESS
 * When set to 0 DevAdd is automatically generated using a pseudo random generator
 */
#define STATIC_DEVICE_ADDRESS 0

/*!
 * Device address on the network (big endian)
 *
 * \remark In this application the value is automatically generated using
 *         a pseudo random generator seeded with a value derived from
 *         BoardUniqueId value if LORAWAN_DEVICE_ADDRESS is set to 0
 */
#define LORAWAN_DEVICE_ADDRESS ( uint32_t ) 0x00000000

/*
 * =============================================================================
 * BELOW LINES SHOULDN'T BE MODIFIED
 * =============================================================================
 */

/*!
 * ATECC608A predefined key slots setup
 */
#define TNGLORA_DEV_EUI_SLOT 10U
#define TNGLORA_JOIN_EUI_SLOT 9U
#define TNGLORA_APP_KEY_SLOT 0U
#define TNGLORA_NWK_KEY_SLOT 0U
#define TNGLORA_S_NWK_S_INT_KEY_SLOT 4U
#define TNGLORA_F_NWK_S_INT_KEY_SLOT 5U
#define TNGLORA_J_S_INT_KEY_SLOT 6U
#define TNGLORA_J_S_ENC_KEY_SLOT 7U
#define TNGLORA_APP_S_KEY_SLOT 2U
#define TNGLORA_NWK_S_ENC_KEY_SLOT 3U
#define TNGLORA_MC_APP_S_KEY_0_SLOT 11U
#define TNGLORA_MC_NWK_S_KEY_0_SLOT 12U
#define TNGLORA_APP_KEY_BLOCK_INDEX 1U
#define TNGLORA_REMAINING_KEYS_BLOCK_INDEX 0U

#define ATECC608A_SE_KEY_LIST                                                                                          \
    {                                                                                                                  \
        {                                                                                                              \
            /*!                                                                                                        \
             * Application root key                                                                                    \
             * WARNING: FOR 1.0.x DEVICES IT IS THE \ref LORAWAN_GEN_APP_KEY                                           \
             */                                                                                                        \
            .KeyID         = APP_KEY,                                                                                  \
            .KeySlotNumber = TNGLORA_APP_KEY_SLOT,                                                                     \
            .KeyBlockIndex = TNGLORA_APP_KEY_BLOCK_INDEX,                                                              \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Network root key                                                                                        \
             * WARNING: FOR 1.0.x DEVICES IT IS THE \ref LORAWAN_APP_KEY                                               \
             */                                                                                                        \
            .KeyID         = NWK_KEY,                                                                                  \
            .KeySlotNumber = TNGLORA_APP_KEY_SLOT,                                                                     \
            .KeyBlockIndex = TNGLORA_APP_KEY_BLOCK_INDEX,                                                              \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Join session integrity key (Dynamically updated)                                                        \
             * WARNING: NOT USED FOR 1.0.x DEVICES                                                                     \
             */                                                                                                        \
            .KeyID         = J_S_INT_KEY,                                                                              \
            .KeySlotNumber = TNGLORA_J_S_INT_KEY_SLOT,                                                                 \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Join session encryption key (Dynamically updated)                                                       \
             * WARNING: NOT USED FOR 1.0.x DEVICES                                                                     \
             */                                                                                                        \
            .KeyID         = J_S_ENC_KEY,                                                                              \
            .KeySlotNumber = TNGLORA_J_S_ENC_KEY_SLOT,                                                                 \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Forwarding Network session integrity key                                                                \
             * WARNING: NWK_S_KEY FOR 1.0.x DEVICES                                                                    \
             */                                                                                                        \
            .KeyID         = F_NWK_S_INT_KEY,                                                                          \
            .KeySlotNumber = TNGLORA_F_NWK_S_INT_KEY_SLOT,                                                             \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Serving Network session integrity key                                                                   \
             * WARNING: NOT USED FOR 1.0.x DEVICES. MUST BE THE SAME AS \ref LORAWAN_F_NWK_S_INT_KEY                   \
             */                                                                                                        \
            .KeyID         = S_NWK_S_INT_KEY,                                                                          \
            .KeySlotNumber = TNGLORA_S_NWK_S_INT_KEY_SLOT,                                                             \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Network session encryption key                                                                          \
             * WARNING: NOT USED FOR 1.0.x DEVICES. MUST BE THE SAME AS \ref LORAWAN_F_NWK_S_INT_KEY                   \
             */                                                                                                        \
            .KeyID         = NWK_S_ENC_KEY,                                                                            \
            .KeySlotNumber = TNGLORA_NWK_S_ENC_KEY_SLOT,                                                               \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Application session key                                                                                 \
             */                                                                                                        \
            .KeyID         = APP_S_KEY,                                                                                \
            .KeySlotNumber = TNGLORA_APP_S_KEY_SLOT,                                                                   \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Multicast root key (Dynamically updated)                                                                \
             * WARNING: NOT CURRENTLY SUPPORTED BY ATECC608A                                                           \
             * TODO: Add support                                                                                       \
             */                                                                                                        \
            .KeyID         = MC_ROOT_KEY,                                                                              \
            .KeySlotNumber = 0,                                                                                        \
            .KeyBlockIndex = 0,                                                                                        \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Multicast key encryption key (Dynamically updated)                                                      \
             * WARNING: NOT CURRENTLY SUPPORTED BY ATECC608A                                                           \
             * TODO: Add support                                                                                       \
             */                                                                                                        \
            .KeyID         = MC_KE_KEY,                                                                                \
            .KeySlotNumber = 0,                                                                                        \
            .KeyBlockIndex = 0,                                                                                        \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Multicast group #0 root key (Dynamically updated)                                                       \
             * WARNING: NOT CURRENTLY SUPPORTED BY ATECC608A                                                           \
             * TODO: Add support                                                                                       \
             */                                                                                                        \
            .KeyID         = MC_KEY_0,                                                                                 \
            .KeySlotNumber = 0,                                                                                        \
            .KeyBlockIndex = 0,                                                                                        \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Multicast group #0 application session key (Dynamically updated)                                        \
             */                                                                                                        \
            .KeyID         = MC_APP_S_KEY_0,                                                                           \
            .KeySlotNumber = TNGLORA_MC_APP_S_KEY_0_SLOT,                                                              \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * Multicast group #0 network session key (Dynamically updated)                                            \
             */                                                                                                        \
            .KeyID         = MC_NWK_S_KEY_0,                                                                           \
            .KeySlotNumber = TNGLORA_MC_NWK_S_KEY_0_SLOT,                                                              \
            .KeyBlockIndex = TNGLORA_REMAINING_KEYS_BLOCK_INDEX,                                                       \
        },                                                                                                             \
        {                                                                                                              \
            /*!                                                                                                        \
             * All zeros key. (ClassB usage)(constant)                                                                 \
             * WARNING: NOT CURRENTLY SUPPORTED BY ATECC608A                                                           \
             * TODO: Add support                                                                                       \
             *       SE should provide a slot for SLOT_RAND_ZERO_KEY.                                                  \
             *       .KeyValue = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
             *                     0x00, 0x00 }                                                                                            \
             */                                                                                                        \
            .KeyID         = SLOT_RAND_ZERO_KEY,                                                                       \
            .KeySlotNumber = 0,                                                                                        \
            .KeyBlockIndex = 0,                                                                                        \
        },                                                                                                             \
    },

#ifdef __cplusplus
}
#endif

#endif  //  __SOFT_SE_IDENTITY_H__
