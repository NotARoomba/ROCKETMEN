/**
 * @file      llcc68_hal.h
 *
 * @brief     Hardware Abstraction Layer for LLCC68
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LLCC68_HAL_H
#define LLCC68_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/**
 * @brief Write this to SPI bus while reading data, or as a dummy/placeholder
 */
#define LLCC68_NOP ( 0x00 )

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

typedef enum llcc68_hal_status_e
{
    LLCC68_HAL_STATUS_OK    = 0,
    LLCC68_HAL_STATUS_ERROR = 3,
} llcc68_hal_status_t;

typedef int32_t (*llcc68_write_ptr)(void*, const uint8_t *, const uint16_t, const uint8_t *, const uint16_t);
typedef int32_t (*llcc68_read_ptr)(void*, const uint8_t *, const uint16_t, uint8_t *, const uint16_t);
typedef int32_t (*llcc68_reset_ptr)(void*);

typedef struct
{
  /** Component mandatory fields **/
  llcc68_write_ptr  write_reg;
  llcc68_read_ptr   read_reg;
  llcc68_reset_ptr reset;
  void *gpio;
  /** Customizable optional pointer **/
  void *handle;
} llcc68_ctx_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/**
 * Radio data transfer - write
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be transmitted
 * @param [in] data_length      Buffer size to be transmitted
 *
 * @returns Operation status
 */
llcc68_hal_status_t llcc68_hal_write( const llcc68_ctx_t *context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length );

/**
 * Radio data transfer - read
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context          Radio implementation parameters
 * @param [in] command          Pointer to the buffer to be transmitted
 * @param [in] command_length   Buffer size to be transmitted
 * @param [in] data             Pointer to the buffer to be received
 * @param [in] data_length      Buffer size to be received
 *
 * @returns Operation status
 */
llcc68_hal_status_t llcc68_hal_read( const llcc68_ctx_t *context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length );

/**
 * Reset the radio
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context Radio implementation parameters
 *
 * @returns Operation status
 */
llcc68_hal_status_t llcc68_hal_reset( const llcc68_ctx_t *context );

/**
 * Wake the radio up.
 *
 * @remark Shall be implemented by the user
 *
 * @param [in] context Radio implementation parameters
 *
 * @returns Operation status
 */
llcc68_hal_status_t llcc68_hal_wakeup( const llcc68_ctx_t *context );

#ifdef __cplusplus
}
#endif

#endif  // LLCC68_HAL_H

/* --- EOF ------------------------------------------------------------------ */
