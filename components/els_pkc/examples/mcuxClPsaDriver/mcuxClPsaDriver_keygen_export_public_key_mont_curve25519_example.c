/*--------------------------------------------------------------------------*/
/* Copyright 2022-2024 NXP                                                  */
/*                                                                          */
/* NXP Proprietary. This software is owned or controlled by NXP and may     */
/* only be used strictly in accordance with the applicable license terms.   */
/* By expressly accepting such terms or by downloading, installing,         */
/* activating and/or otherwise using the software, you are agreeing that    */
/* you have read, and that you agree to comply with and are bound by, such  */
/* license terms.  If you do not agree to be bound by the applicable        */
/* license terms, then you may not retain, install, activate or otherwise   */
/* use the software.                                                        */
/*--------------------------------------------------------------------------*/

/**
 * @file  mcuxClPsaDriver_keygen_export_public_key_mont_curve25519_example.c
 * @brief Example for montgomery curves 384bits key pairs generating and public exporting
 *
 * @example mcuxClPsaDriver_keygen_export_public_key_mont_curve25519_example.c
 * @brief Example for montgomery curves 384bits key pairs generating and public exporting
 */

#include "common.h"

#include <mcuxClEls.h> // Interface to the entire mcuxClEls component
#include <mcuxClSession.h> // Interface to the entire mcuxClSession component
#include <mcuxClKey.h> // Interface to the entire mcuxClKey component
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClToolchain.h> // memory segment definitions
#include <stdbool.h>  // bool type for the example's return code
#include <mcuxClPsaDriver.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClEcc.h>
#include <mcuxClPkc.h>
#include <mcuxClExample_ELS_Helper.h>

#define LIFETIME_INTERNAL PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_EXTERNAL_STORAGE)
#define LIFETIME_EXTERNAL PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE)

MCUXCLEXAMPLE_FUNCTION(mcuxClPsaDriver_keygen_export_public_key_mont_curve25519_example)
{
    /* Enable ELS */
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**********************************************************************************************/
    /******************************  Example  *****************************************************/
    /******************  Generate Curve25519 montgomery key pairs**********************************/
    /**********************************************************************************************/

    /* Set up PSA key attributes. */
    psa_key_attributes_t keygenAttr = {
      .core = {                                                               // Core attributes
        .type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY),         // Keypair family with curve montgomery
        .bits = 256U,                                                         // Key bits of a Curve25519
        .lifetime = LIFETIME_EXTERNAL,                                        // Volatile (RAM), S50 Temporary Storage for private key
        .id = 0U,                                                             // ID zero
        .policy = {
          .usage = PSA_ALG_NONE,                                              // Key may be used for sign message or hash
          .alg = PSA_ALG_ECDSA_ANY,
          .alg2 = PSA_ALG_NONE},
        .flags = 0U},                                                         // No flags
      .domain_parameters = NULL,
      .domain_parameters_size = 0U};

    /* Call generate_key operation */
    ALIGNED uint8_t key_buffer[MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIVATEKEY] = {0U};
    size_t key_buffer_size = MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIVATEKEY;
    size_t key_buffer_length = 0U;

    psa_status_t status = psa_driver_wrapper_generate_key(
                &keygenAttr,
                key_buffer, key_buffer_size, &key_buffer_length);

    /* Check the return value */
    if(status != PSA_SUCCESS)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the output length */
    if(key_buffer_length != MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIVATEKEY)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /**********************************************************************************************/
    /***************************  Example  ********************************************************/
    /***************Export Curve25519 montgomery public key****************************************/
    /**********************************************************************************************/
    /* Set up PSA key attributes. */
    psa_key_attributes_t exportAttr = {
      .core = {                                                               // Core attributes
        .type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY),         // Keypair family with curve montgomery
        .bits = 256U,                                                         // Key bits of a Curve25519
        .lifetime = LIFETIME_EXTERNAL,                                        // Volatile (RAM), S50 Temporary Storage for private key
        .id = 0U,                                                             // ID zero
        .policy = {
          .usage = PSA_KEY_USAGE_EXPORT,                                      // Key may be used for sign message or hash
          .alg = PSA_ALG_ECDSA_ANY,
          .alg2 = PSA_ALG_NONE},
        .flags = 0U},                                                         // No flags
      .domain_parameters = NULL,
      .domain_parameters_size = 0U};

    /* Call export_public_key operation */
    MCUX_CSSL_ANALYSIS_START_PATTERN_EXTERNAL_MACRO()
    ALIGNED uint8_t data[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY), 256u)] = {0U};
    size_t data_size = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_MONTGOMERY), 256u);
    MCUX_CSSL_ANALYSIS_STOP_PATTERN_EXTERNAL_MACRO()
    size_t data_length = 0U;

    status = psa_driver_wrapper_export_public_key(
                &exportAttr,
                key_buffer, MCUXCLECC_MONTDH_CURVE25519_SIZE_PRIVATEKEY,
                data, data_size, &data_length );

    /* Check the return value */
    if(status != PSA_SUCCESS)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the output length */
    if(data_length > data_size)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    MCUX_CSSL_FP_FUNCTION_CALL_BEGIN(result, token, mcuxClEls_WaitForOperation(MCUXCLELS_ERROR_FLAGS_CLEAR)); // Wait for the mcuxClEls_KeyDelete_Async operation to complete.
    // mcuxClEls_LimitedWaitForOperation is a flow-protected function: Check the protection token and the return value
    if((MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClEls_WaitForOperation) != token) || (MCUXCLELS_STATUS_OK != result))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }
    MCUX_CSSL_FP_FUNCTION_CALL_END();

    /* Disable the ELS */
    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Return */
    return MCUXCLEXAMPLE_STATUS_OK;
}
