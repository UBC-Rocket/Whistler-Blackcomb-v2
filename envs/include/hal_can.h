#ifndef _HAL_CAN_H_
#define _HAL_CAN_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"

#ifdef COMPILE_BOARD
#include "fsl_flexcan.h"

#elif defined(COMPILE_x86)
#include "MK66F18.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * Helper union to keep track of CAN frame packets in different data types
 */
typedef union _hal_can_packet_t {
	uint8_t c[8];
	uint32_t i[2];
} hal_can_packet_t;

typedef enum {
	can_packet_id_pt,
	can_packet_id_tc,
} hal_can_packet_id_t;

typedef enum {
	can_pt_1,
} hal_can_pt_id_t;

typedef enum {
	can_tc_1,
} hal_can_tc_id_t;

#ifdef COMPILE_BOARD

typedef struct _hal_can_handle_t {
	CAN_Type *base;
	flexcan_config_t config;
	flexcan_rx_fifo_config_t fifo_config;
	flexcan_handle_t transfer_handle;
	flexcan_fifo_transfer_t fifo_transfer;
	flexcan_mb_transfer_t txXfer;
	SemaphoreHandle_t rxSem;
	SemaphoreHandle_t txSem;
} hal_can_handle_t;

#elif defined(COMPILE_x86)

typedef struct _hal_can_handle_t {
	CAN_Type *base;
	SemaphoreHandle_t rxSem;
	SemaphoreHandle_t txSem;
} hal_can_handle_t;


/* FlexCAN message frame structure. This is copy pasted from fsl_flexcan.h,
 * since although it's a generic frame structure that's nice to use we can't
 * include the rest of the hardware specific stuff from that file */
typedef struct _flexcan_frame
{
    struct
    {
        uint32_t timestamp : 16; /*!< FlexCAN internal Free-Running Counter Time Stamp. */
        uint32_t length : 4;     /*!< CAN frame payload length in bytes(Range: 0~8). */
        uint32_t type : 1;       /*!< CAN Frame Type(DATA or REMOTE). */
        uint32_t format : 1;     /*!< CAN Frame Identifier(STD or EXT format). */
        uint32_t : 1;            /*!< Reserved. */
        uint32_t idhit : 9;      /*!< CAN Rx FIFO filter hit id(This value is only used in Rx FIFO receive mode). */
    };
    struct
    {
        uint32_t id : 29; /*!< CAN Frame Identifier, should be set using FLEXCAN_ID_EXT() or FLEXCAN_ID_STD() macro. */
        uint32_t : 3;     /*!< Reserved. */
    };
    union
    {
        struct
        {
            uint32_t dataWord0; /*!< CAN Frame payload word0. */
            uint32_t dataWord1; /*!< CAN Frame payload word1. */
        };
        struct
        {
            uint8_t dataByte3; /*!< CAN Frame payload byte3. */
            uint8_t dataByte2; /*!< CAN Frame payload byte2. */
            uint8_t dataByte1; /*!< CAN Frame payload byte1. */
            uint8_t dataByte0; /*!< CAN Frame payload byte0. */
            uint8_t dataByte7; /*!< CAN Frame payload byte7. */
            uint8_t dataByte6; /*!< CAN Frame payload byte6. */
            uint8_t dataByte5; /*!< CAN Frame payload byte5. */
            uint8_t dataByte4; /*!< CAN Frame payload byte4. */
        };
    };
} flexcan_frame_t;

#endif

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes the CAN bus (which I realize is a bit ambiguous with querying
 * whether it "can" as in able to be initialized, but whatever)
 * @param handle the handle of the can bus instance
 * @param base the base of the CAN controller
 */
void canInit(hal_can_handle_t *handle, CAN_Type *base);

/**
 * Blocks until FIFO buffer is non-empty, then retrieves latest message. If
 * already messages will return immediately
 * @param handle the handle of the can bus instance
 * @param rxFrame the frame of the received message
 */
void canReceive(hal_can_handle_t *handle, flexcan_frame_t *rxFrame);

/**
 * Sends message over can bus
 * @param handle the handle of the can bus instance
 * @param id the id of the packet to send, must be 11 bits or less
 * @param buffer array of 8 bytes for message to send. If length < 8 rest of
 * buffer can be anything
 * @param length length of buffer, must be 8 or less
 */
void canSend(hal_can_handle_t *handle, uint32_t id, hal_can_packet_t packet,
		uint32_t length);

/**
 * Extracts id from can packet, i.e. first byte
 * @param rxFrame the frame to extract id from
 * @return the id of the packet in rxFrame
 */
hal_can_packet_id_t canGetId(flexcan_frame_t *rxFrame);


/**
 * Sets id of  a packet
 * @param packet a pointer to the packet to set data for
 * @param id the id to set
 */
void canSetId(hal_can_packet_t *packet, hal_can_packet_id_t id);

/**
 * Gets sensor id from a frame, works for tcs and pts
 * @param rxFrame the frame to extract from, must have id of sensor read type
 * @return the id of the sensor, depending on the sensor type either refers to
 * hal_can_pt_id_t or hal_can_tc_id_t
 */
int canGetSensorId(flexcan_frame_t *rxFrame);

/**
 * Sets sensor id for a frame, works for tcs and pts
 * @param rxFrame the frame to set id for
 * @param id the id of the sensor, depending on the sensor type either refers to
 * hal_can_pt_id_t or hal_can_tc_id_t
 */
void canSetSensorId(hal_can_packet_t *packet, int id);

/**
 * Gets command id from a frame
 * @param rxFrame the frame to extract from, must have id of command read type
 * @return the id of the command
 */
int canGetCommandId(flexcan_frame_t *rxFrame);

/**
 * Sets command id for a frame
 * @param rxFrame the frame to set id for
 * @param id the id of the command
 */
void canSetCommandId(hal_can_packet_t *packet, int id);

/**
 * Gets value of sensor from a frame
 * @param rxFrame the frame to extract from, must have id of a sensor read type
 * @return the value of the sensor in the units of the sensor, either pressure
 * or temperature
 */
float canGetFloatValue(flexcan_frame_t *rxFrame);

/**
 * Sets sensor value for a packet
 * @param packet the packet to set, must have id of a sensor read type
 * @param value the value to set for the packet
 */
void canSetFloatValue(hal_can_packet_t *packet, float value);

/**
 * Gets value of command from a frame
 * @param rxFrame the frame to extract from, must have id of a command read type
 * @return the value of the command
 */
int canGetIntValue(flexcan_frame_t *rxFrame);

/**
 * Sets command value for a packet
 * @param packet the packet to set, must have id of a command read type
 * @param value the value to set for the packet
 */
void canSetIntValue(hal_can_packet_t *packet, int value);

#endif
