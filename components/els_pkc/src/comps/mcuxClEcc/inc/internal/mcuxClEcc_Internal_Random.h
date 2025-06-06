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
 * @file  mcuxClEcc_Internal_Random.h
 * @brief internal header for abstracting random access in mcuxClEcc
 */


#ifndef MCUXCLECC_INTERNAL_RANDOM_H_
#define MCUXCLECC_INTERNAL_RANDOM_H_

#include <mcuxClCore_Platform.h>
#include <mcuxClRandom.h>
#include <internal/mcuxClRandom_Internal_Functions.h>


/******************************************************************************/
/* Macro to generate high-quality random number in PKC workarea.              */
/******************************************************************************/
#if defined(MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND)
#include <internal/mcuxClSession_Internal.h>

#define MCUXCLECC_FP_RANDOM_HQRNG_PKCWA(callerID, pSession, pOutPKCWA, length, result)                        \
    do{                                                                                                      \
        /* Generate random number in CPU workarea. */                                                        \
        const uint32_t tempSizeWord = ((length) + (sizeof(uint32_t)) - 1u) / (sizeof(uint32_t));             \
        uint8_t * pTemp = (uint8_t*) mcuxClSession_allocateWords_cpuWa(pSession, tempSizeWord);               \
        if (NULL == pTemp)                                                                                   \
        {                                                                                                    \
            MCUX_CSSL_FP_FUNCTION_EXIT(callerID, MCUXCLECC_STATUS_FAULT_ATTACK);                               \
        }                                                                                                    \
        MCUXCLBUFFER_INIT(buffTemp, NULL, pTemp, length);                                                     \
        MCUX_CSSL_DI_RECORD(sumOfRandomGenerateParams, (uint32_t)(pSession) + (uint32_t)buffTemp + (length)); \
        MCUX_CSSL_FP_FUNCTION_CALL(ret_random, mcuxClRandom_generate_internal(pSession, buffTemp, length));    \
        if (MCUXCLRANDOM_STATUS_OK != ret_random)                                                             \
        {                                                                                                    \
            result = MCUXCLECC_STATUS_RNG_ERROR;                                                              \
            break;                                                                                           \
        }                                                                                                    \
        /* Copy generated random numbers to PKC workarea. */                                                 \
        MCUXCLMEMORY_FP_MEMORY_COPY(pOutPKCWA, pTemp, length);                                                \
        /* Release temporary buffer. */                                                                      \
        mcuxClSession_freeWords_cpuWa(pSession, tempSizeWord);                                                \
        result = MCUXCLECC_STATUS_OK;                                                                         \
    } while(false)

#define MCUXCLECC_FP_CALLED_RANDOM_HQRNG_PKCWA \
    MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate_internal), \
    MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClMemory_copy)
#else
#define MCUXCLECC_FP_RANDOM_HQRNG_PKCWA(callerID, pSession, pOutPKCWA, length, result)                            \
    do{                                                                                                          \
        MCUXCLBUFFER_INIT(buffOutPKCWA, NULL, pOutPKCWA, length);                                                 \
        MCUX_CSSL_DI_RECORD(sumOfRandomGenerateParams, (uint32_t)(pSession) + (uint32_t)buffOutPKCWA + (length)); \
        MCUX_CSSL_FP_FUNCTION_CALL(ret_random, mcuxClRandom_generate_internal(pSession, buffOutPKCWA, length));    \
        if (MCUXCLRANDOM_STATUS_OK != ret_random)                                                                 \
        {                                                                                                        \
            result = MCUXCLECC_STATUS_RNG_ERROR;                                                                  \
        }                                                                                                        \
        else                                                                                                     \
        {                                                                                                        \
            result = MCUXCLECC_STATUS_OK;                                                                         \
        }                                                                                                        \
    } while(false)

#define MCUXCLECC_FP_CALLED_RANDOM_HQRNG_PKCWA  MCUX_CSSL_FP_FUNCTION_CALLED(mcuxClRandom_generate_internal)
#endif /* MCUXCL_FEATURE_ELS_ACCESS_PKCRAM_WORKAROUND */


#endif /* MCUXCLECC_INTERNAL_RANDOM_H_ */
