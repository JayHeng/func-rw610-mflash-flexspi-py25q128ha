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

#include "common.h"

#include <mcuxClToolchain.h>
#include <mcuxClCore_Examples.h> // Defines for example return codes and assertions
#include <mcuxClExample_ELS_Helper.h>
#include <mcuxClEls.h> // Interface to the entire mcuxClEls component
#include <mcuxClSession.h> // Interface to the entire mcuxClSession component
#include <mcuxClKey.h> // Interface to the entire mcuxClKey component
#include <mcuxCsslFlowProtection.h>
#include <mcuxClCore_FunctionIdentifiers.h> // Code flow protection
#include <mcuxClToolchain.h> // memory segment definitions
#include <stdbool.h>  // bool type for the example's return code
#include <mcuxClPsaDriver.h>
#include <mcuxClAes.h> // Interface to AES-related definitions and types

#define LIFETIME_INTERNAL PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_EXTERNAL_STORAGE)
#define LIFETIME_EXTERNAL PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE)
#define TAG_LEN ((uint32_t)6u)
#define MSG_PLAIN_LEN (MCUXCLAES_BLOCK_SIZE)

MCUXCLEXAMPLE_FUNCTION(mcuxClPsaDriver_aead_ccm_oneshot_example)
{
    /* NIST Special Publication 800-38C example 2 test vectors */

    /** Key for the AES encryption. */
    const ALIGNED uint8_t aes128_key[MCUXCLAES_BLOCK_SIZE] = {
        0x40u, 0x41u, 0x42u, 0x43u,
        0x44u, 0x45u, 0x46u, 0x47u,
        0x48u, 0x49u, 0x4au, 0x4bu,
        0x4cu, 0x4du, 0x4eu, 0x4fu
    };

    /** IV of the AES encryption. */
    const ALIGNED uint8_t aes128_iv[] = {
        0x10u, 0x11u, 0x12u, 0x13u,
        0x14u, 0x15u, 0x16u, 0x17u
    };

    /** Plaintext input for the AES encryption. */
    const ALIGNED uint8_t msg_plain[MSG_PLAIN_LEN] = {
        0x20u, 0x21u, 0x22u, 0x23u,
        0x24u, 0x25u, 0x26u, 0x27u,
        0x28u, 0x29u, 0x2au, 0x2bu,
        0x2cu, 0x2du, 0x2eu, 0x2fu
    };

    /** Additional authenticated data. */
    const ALIGNED uint8_t msg_adata[MCUXCLAES_BLOCK_SIZE] = {
        0x00u, 0x01u, 0x02u, 0x03u,
        0x04u, 0x05u, 0x06u, 0x07u,
        0x08u, 0x09u, 0x0au, 0x0bu,
        0x0cu, 0x0du, 0x0eu, 0x0fu
    };

    /** Expected ciphertext output of the AES-CCM encryption. */
    const ALIGNED uint8_t msg_enc_expected[MCUXCLAES_BLOCK_SIZE] = {
        0xd2u, 0xa1u, 0xf0u, 0xe0u,
        0x51u, 0xeau, 0x5fu, 0x62u,
        0x08u, 0x1au, 0x77u, 0x92u,
        0x07u, 0x3du, 0x59u, 0x3du
    };

    /** Expected authentication tag output. */
    const ALIGNED uint8_t msg_tag_expected[TAG_LEN] = {
        0x1fu, 0xc6u, 0x4fu, 0xbfu, 0xacu, 0xcdu
    };

    /** Initialize ELS, Enable the ELS **/
    if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /****************/
    /** Encryption **/
    /****************/

    /* Request an CCM algorithm with shorter tag */
    const psa_algorithm_t PSA_ALG_CCM_SHORTER_TAG = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, TAG_LEN);

    /* Set up PSA key attributes. */
    psa_key_attributes_t attributes = {
        .core = {                                // Core attributes
            .type = PSA_KEY_TYPE_AES,            // Key is for AES operations
            .bits = 0U,                          // No key bits
            .lifetime = LIFETIME_EXTERNAL,       // Volatile (RAM), Local Storage (plain) key
            .id = 0U,                            // ID zero
            .policy = {
                .usage = PSA_KEY_USAGE_ENCRYPT,  // Key may be used for encryption
                .alg = PSA_ALG_CCM_SHORTER_TAG,  // Key may be used for AEAD CCM mode only
                .alg2 = PSA_ALG_NONE
            },
            .flags = 0u},                        // No flags
        .domain_parameters = NULL,               // No domain parameters
        .domain_parameters_size = 0u
    };

    /* Variable for the output length of the encryption operation */
    size_t output_length = 0u;

    /* Output buffer for the CCM encryption */
    
    ALIGNED uint8_t output_enc[MCUXCLAES_BLOCK_SIZE + TAG_LEN];

    /* Call the AEAD encryption operation */
    psa_status_t result = psa_driver_wrapper_aead_encrypt(
        &attributes,              // const psa_key_attributes_t *attributes
        aes128_key,               // const uint8_t *key_buffer
        sizeof(aes128_key),       // size_t key_buffer_size
        PSA_ALG_CCM_SHORTER_TAG,  // psa_algorithm_t alg
        aes128_iv,                // const uint8_t *nonce
        sizeof(aes128_iv),        // size_t nonce_length
        msg_adata,                // const uint8_t *additional_data
        sizeof(msg_adata),        // size_t additional_data_length
        msg_plain,                // const uint8_t *plaintext
        MSG_PLAIN_LEN,        // size_t plaintext_length
        output_enc,               // uint8_t *ciphertext
        sizeof(output_enc),       // size_t ciphertext_size
        &output_length            // size_t *ciphertext_length
        );

    /* Check the return value */
    if(PSA_SUCCESS != result)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the output length */
MCUX_CSSL_ANALYSIS_START_PATTERN_EXTERNAL_MACRO()
    if(output_length != PSA_AEAD_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CCM_SHORTER_TAG, MSG_PLAIN_LEN))
MCUX_CSSL_ANALYSIS_STOP_PATTERN_EXTERNAL_MACRO()
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the content of the encrypted output data */
    if(!mcuxClCore_assertEqual(output_enc, msg_enc_expected, MSG_PLAIN_LEN))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the content of the tag */
    if(!mcuxClCore_assertEqual(output_enc + sizeof(msg_enc_expected), msg_tag_expected, TAG_LEN))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /****************/
    /** Decryption **/
    /****************/

    /* Update key usage */
    attributes.core.policy.usage = PSA_KEY_USAGE_DECRYPT;  // Key may be used for decryption

    /* reset output length */
    output_length = 0u;

    /* Output buffer for the CCM decryption */
    ALIGNED uint8_t output_dec[MSG_PLAIN_LEN];

    /* Call the AEAD decryption operation */
    result = psa_driver_wrapper_aead_decrypt(
        &attributes,              // const psa_key_attributes_t *attributes
        aes128_key,               // const uint8_t *key_buffer
        sizeof(aes128_key),       // size_t key_buffer_size
        PSA_ALG_CCM_SHORTER_TAG,  // psa_algorithm_t alg
        aes128_iv,                // const uint8_t *nonce
        sizeof(aes128_iv),        // size_t nonce_length
        msg_adata,                // const uint8_t *additional_data
        sizeof(msg_adata),        // size_t additional_data_length
        output_enc,               // const uint8_t *ciphertext
        sizeof(output_enc),       // size_t ciphertext_length
        output_dec,               // uint8_t *plaintext
        sizeof(output_dec),       // size_t plaintext_size
        &output_length            // size_t *plaintext_length
        );

    /* Check the return value */
    if(PSA_SUCCESS != result)
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the output length */
MCUX_CSSL_ANALYSIS_START_PATTERN_EXTERNAL_MACRO()
    if(output_length != PSA_AEAD_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CCM_SHORTER_TAG, sizeof(output_enc)))
MCUX_CSSL_ANALYSIS_STOP_PATTERN_EXTERNAL_MACRO()
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Check the content of the plain output data */
    if(!mcuxClCore_assertEqual(output_dec, msg_plain, MSG_PLAIN_LEN))
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Disable the ELS */
    if(!mcuxClExample_Els_Disable())
    {
        return MCUXCLEXAMPLE_STATUS_ERROR;
    }

    /* Success */
    return MCUXCLEXAMPLE_STATUS_OK;
}
